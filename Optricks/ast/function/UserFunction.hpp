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
		auto tmp = a.functionReturn;
		a.functionReturn = myFunction->getSingleProto()->returnType;
		body->evaluate(a);
		if(! a.hadBreak()){
			if(myFunction->getSingleProto()->returnType->classType==CLASS_VOID)
				a.builder.CreateRetVoid();
			else error("Could not find return statement");
		}
		assert(F);
		a.FinalizeFunction(F);
		if(Parent) a.builder.SetInsertPoint( Parent );
		assert(a.functionReturn == myFunction->getSingleProto()->returnType);
		a.functionReturn = tmp;
		body->buildFunction(a);
	}
	void registerFunctionPrototype(RData& a) const override final{
		if(myFunction) return;
		BasicBlock *Parent = a.builder.GetInsertBlock();
		llvm::SmallVector<Type*,0> args(declaration.size());
		std::vector<AbstractDeclaration> ad;
		for(unsigned i=0; i<declaration.size(); i++){
			const auto& b = declaration[i];
			const AbstractClass* ac = b->getClass(filePos);
			ad.push_back(AbstractDeclaration(ac, b->variable.pointer.name, b->value));
			Type* cl = ac->type;
			if(cl==NULL) error("Type argument "+ac->getName()+" is null");
			args[i] = cl;
		}
		for (unsigned Idx = 0; Idx < declaration.size(); Idx++) {
			if(ad[Idx].declarationType->classType==CLASS_REF){
				auto ic = ((ReferenceClass*)ad[Idx].declarationType)->innerType;
				declaration[Idx]->variable.getMetadata().setObject(
					(new ConstantData(UndefValue::get(ic->type),ic))
				);
			} else{
				declaration[Idx]->variable.getMetadata().setObject(
					(new ConstantData(UndefValue::get(ad[Idx].declarationType->type),ad[Idx].declarationType))
				);
			}
		}
		const AbstractClass* returnType = (returnV)?(returnV->getSelfClass(filePos)):(nullptr);

		if(returnType==nullptr){
			std::vector<const AbstractClass*> yields;
			body->collectReturns(yields,nullptr);
			if(yields.size()==0) returnType = &voidClass;
			else {
				returnType = yields[0];
				for(unsigned i=1;i<yields.size();i++)
					returnType = getMin(returnType,yields[i],filePos);
			}
		}
		assert(returnType);
		llvm::Type* r = returnType->type;
		FunctionType *FT = FunctionType::get(r, args, false);
		String nam = "!"+((self)?(self->getShortName()):("anon"));
		llvm::Function *F = a.CreateFunction(nam,FT, LOCAL_FUNC);
		myFunction = new CompiledFunction(new FunctionProto(self->getFullName(), ad, returnType), F);
		self->getMetadata().addFunction(myFunction);

		BasicBlock *BB = a.CreateBlockD("entry", F);
		a.builder.SetInsertPoint(BB);

		unsigned Idx = 0;
		for (Function::arg_iterator AI = F->arg_begin(); Idx != F->arg_size(); ++AI, ++Idx) {
			((Value*)AI)->setName(Twine(myFunction->getSingleProto()->declarations[Idx].declarationVariable));
			if(ad[Idx].declarationType->classType==CLASS_REF){
				declaration[Idx]->variable.getMetadata().setObject(
					new LocationData(new StandardLocation(AI),((ReferenceClass*) ad[Idx].declarationType)->innerType)
				);
			} else {
				declaration[Idx]->variable.getMetadata().setObject(
					(new ConstantData(AI,ad[Idx].declarationType))->toLocation(a)
				);
			}
		}

		if(Parent) a.builder.SetInsertPoint( Parent );
		body->registerFunctionPrototype(a);
	}
};




#endif /* USERFUNCTION_HPP_ */
