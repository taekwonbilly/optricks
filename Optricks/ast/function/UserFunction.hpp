/*
 * UserFunction.hpp
 *
 *  Created on: Feb 9, 2014
 *      Author: Billy
 */

#ifndef USERFUNCTION_HPP_
#define USERFUNCTION_HPP_
#include "./E_FUNCTION.hpp"
#include "../../language/class/ClassLib.hpp"
class UserFunction : public E_FUNCTION{
private:
	E_VAR* self;
	Statement* returnV;
	Statement* body;
	mutable bool built;
public:
	UserFunction(PositionID id, std::vector<Declaration*> dec, E_VAR* s, Statement* r, Statement* b):
		E_FUNCTION(id,dec),self(s),returnV(r),body(b),built(false){
		assert(s);
		assert(r);
	}
	void registerClasses() const override final{
		body->registerClasses();
	}
	void buildFunction(RData& a) const override final{
		if(built) return;
		built = true;
		registerFunctionPrototype(a);

		BasicBlock *Parent = a.builder.GetInsertBlock();
		llvm::Function* F = myFunction->getSingleFunc();
		a.builder.SetInsertPoint(& (F->getEntryBlock()));
		body->evaluate(a);
		if(! a.hadBreak()){
			if(myFunction->getSingleProto()->returnType->classType==CLASS_VOID)
				a.builder.CreateRetVoid();
			else error("Could not find return statement");
		}
		a.FinalizeFunction(F);
		if(Parent) a.builder.SetInsertPoint( Parent );
		body->buildFunction(a);
	}
	void registerFunctionPrototype(RData& a) const override final{
		if(myFunction) return;
		BasicBlock *Parent = a.builder.GetInsertBlock();
		std::vector<Type*> args;
		std::vector<AbstractDeclaration> ad;
		for(auto & b: declaration){
			const AbstractClass* ac = b->getClass(filePos);
			if(ac->classType==CLASS_AUTO) error("Cannot have auto-class in function declaration");
			ad.push_back(AbstractDeclaration(ac, b->variable->pointer.name, b->value));
			Type* cl = ac->type;
			if(cl==NULL) error("Type argument "+ac->getName()+" is null");
			args.push_back(cl);
		}
		const AbstractClass* returnType = (returnType)?(returnV->getSelfClass(filePos)):(nullptr);

		if(returnType==nullptr){
			std::vector<const AbstractClass*> yields;
			body->collectReturns(yields,returnType);
			if(yields.size()==0) returnType = voidClass;
			else {
				returnType = getMin(yields,filePos);
				if(returnType->classType==CLASS_AUTO)
					filePos.compilerError("Cannot deduce return type of function "+self->getFullName());
			}
		}
		assert(returnType);
		llvm::Type* r = returnType->type;
		FunctionType *FT = FunctionType::get(r, ArrayRef<Type*>(args), false);
		String nam = "!"+((self)?(self->getShortName()):("anon"));
		llvm::Function *F = a.CreateFunctionD(nam,FT, LOCAL_FUNC);
		myFunction = new CompiledFunction(new FunctionProto(self->getFullName(), ad, returnType), F);
		self->getMetadata().addFunction(myFunction);

		BasicBlock *BB = a.CreateBlockD("entry", F);
		a.builder.SetInsertPoint(BB);

		unsigned Idx = 0;
		for (Function::arg_iterator AI = F->arg_begin(); Idx != F->arg_size(); ++AI, ++Idx) {
			((Value*)AI)->setName(Twine(myFunction->getSingleProto()->declarations[Idx].declarationVariable));
			declaration[Idx]->variable->getMetadata().setObject(
				(new ConstantData(AI,myFunction->getSingleProto()->declarations[Idx].declarationType))->toLocation(a)
			);
		}

		if(Parent) a.builder.SetInsertPoint( Parent );
		body->registerFunctionPrototype(a);
	}
};




#endif /* USERFUNCTION_HPP_ */
