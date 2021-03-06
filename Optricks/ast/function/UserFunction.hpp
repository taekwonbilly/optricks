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
	mutable bool built;
	mutable std::pair<std::map<llvm::Value*,llvm::PHINode*>, llvm::BasicBlock*> closureInfo;
public:
	UserFunction(PositionID id, OModule* superScope,String n):
		E_FUNCTION(id,OModule(superScope),n),built(false){
		closureInfo.second=nullptr;
		module.closureInfo = &closureInfo;
	}
	void registerClasses() const override final{
		methodBody->registerClasses();
	}
	void reset() const override final{
		built = false;
		myFunction=nullptr;
		closureInfo.second = nullptr;
		closureInfo.first.clear();
	}
	void buildFunction(RData& a) const override final{
		if(built) return;
		built = true;
		registerFunctionPrototype(a);

		closureInfo.second = a.builder.GetInsertBlock();
		auto F = (llvm::Function*)myFunction->getSingleFunc();
		a.builder.SetInsertPoint(& (F->getEntryBlock()));

		Jumpable j(name, FUNC, &module, nullptr,nullptr,myFunction->getSingleProto()->returnType);
		a.addJump(&j);

		methodBody->evaluate(a);
		if(! a.hadBreak()){
			for(const auto& dat: module.vars){
				decrementCount(a, filePos, dat);
			}
			if(myFunction->getSingleProto()->returnType->classType==CLASS_VOID)
				a.builder.CreateRetVoid();
			else error("Could not find return statement");
		}

		if(closureInfo.first.size() > 0){
			filePos.error("Function closures not currently supported: "+str(closureInfo.first.size())+" values replaced with 0 or null");
			for(auto& ac: closureInfo.first){
				ac.second->replaceAllUsesWith(a.getGlobal(ac.first->getType(), false));
			}
		}

		for(auto& d: declaration) d->buildFunction(a);
		methodBody->buildFunction(a);

		assert(F);
		a.FinalizeFunction(F);
		if(closureInfo.second) a.builder.SetInsertPoint( closureInfo.second );
		auto tmp = a.popJump();
		assert(tmp== &j);
	}
	void registerFunctionPrototype(RData& a) const override final{
		if(myFunction) return;
		llvm::BasicBlock* Parent = a.builder.GetInsertBlock();
		llvm::SmallVector<llvm::Type*,0> args(declaration.size());
		std::vector<AbstractDeclaration> ad;
		for(unsigned i=0; i<declaration.size(); i++){
			const auto& b = declaration[i];
			const AbstractClass* ac = b->getClass(filePos);
			assert(ac);
			ad.push_back(AbstractDeclaration(ac, b->variable.pointer.name, b->value));
			if(ac->type==nullptr) error("Type argument "+ac->getName()+" is null");
			args[i] = ac->type;
		}
		for (unsigned Idx = 0; Idx < declaration.size(); Idx++) {
			if(ad[Idx].declarationType->classType==CLASS_REF){
				auto ic = ((ReferenceClass*)ad[Idx].declarationType)->innerType;
				declaration[Idx]->variable.getMetadata().setObject(
					(new ConstantData(getUndef(ic->type),ic))
				);
			} else{
				declaration[Idx]->variable.getMetadata().setObject(
					(new ConstantData(getUndef(ad[Idx].declarationType->type),ad[Idx].declarationType))
				);
			}
		}
		const AbstractClass* returnType = (returnV)?(returnV->getMyClass(filePos)):(nullptr);

		if(returnType==nullptr){
			std::vector<const AbstractClass*> yields;
			methodBody->collectReturns(yields,nullptr);
			if(yields.size()==0) returnType = &voidClass;
			else {
				returnType = yields[0];
				for(unsigned i=1;i<yields.size();i++)
					returnType = getMin(returnType,yields[i],filePos);
			}
		}
		assert(returnType);
		assert(returnType->type);
		auto FT = llvm::FunctionType::get(returnType->type, args, false);
		String nam = "_opt"+((name.length()!=0)?name:"anon");
		llvm::Function *F = a.CreateFunction(nam,FT, LOCAL_FUNC);
		//TODO replace with long name
		myFunction = new CompiledFunction(new FunctionProto(name, ad, returnType), F);
		module.surroundingScope->addFunction(filePos, name)->add(myFunction, filePos);

		llvm::BasicBlock* BB = a.CreateBlockD("entry", F);
		a.builder.SetInsertPoint(BB);

		unsigned Idx = 0;
		for (llvm::Function::arg_iterator AI = F->arg_begin(); Idx != F->arg_size(); ++AI, ++Idx) {
			((llvm::Value*)AI)->setName(llvm::Twine(myFunction->getSingleProto()->declarations[Idx].declarationVariable));
			if(ad[Idx].declarationType->classType==CLASS_REF){
				declaration[Idx]->variable.getMetadata().setObject(
					new LocationData(new StandardLocation(false,AI),((ReferenceClass*) ad[Idx].declarationType)->innerType)
				);
			} else {
				declaration[Idx]->variable.getMetadata().setObject(
					(new ConstantData(AI,ad[Idx].declarationType))->toLocation(a,ad[Idx].declarationVariable)
				);
			}
		}

		if(Parent) a.builder.SetInsertPoint( Parent );
		for(auto& d: declaration) d->registerFunctionPrototype(a);
		methodBody->registerFunctionPrototype(a);
	}
};




#endif /* USERFUNCTION_HPP_ */
