/*
 * CPointerClass.hpp
 *
 *  Created on: Jan 15, 2014
 *      Author: Billy
 */

#ifndef CPOINTERCLASS_HPP_
#define CPOINTERCLASS_HPP_

#include "../AbstractClass.hpp"
#include "../ScopeClass.hpp"
class CPointerClass: public AbstractClass{
public:
	inline CPointerClass(bool b):
		AbstractClass(& NS_LANG_C.staticVariables,"pointer", NULL,PRIMITIVE_LAYOUT,CLASS_CPOINTER,true,C_POINTERTYPE){
		NS_LANG_C.staticVariables.addClass(PositionID(0,0,"#pointer"),this);

		LANG_M.addFunction(PositionID(0,0,"#str"),"print")->add(
			new BuiltinInlineFunction(
					new FunctionProto("print",{AbstractDeclaration(this)},&voidClass),
			[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==1);
			r.printf("0x%" PRIXPTR, args[0]->evalV(r, id));
			return &VOID_DATA;
		}), PositionID(0,0,"#int"));
		LANG_M.addFunction(PositionID(0,0,"#str"),"println")->add(
			new BuiltinInlineFunction(
					new FunctionProto("println",{AbstractDeclaration(this)},&voidClass),
			[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==1);
			r.printf("0x%" PRIXPTR "\n", args[0]->evalV(r, id));
			return &VOID_DATA;
		}), PositionID(0,0,"#int"));
	}

	const AbstractClass* getLocalReturnClass(PositionID id, String s) const override final{
		illegalLocal(id,s);
		exit(1);
	}

	bool hasLocalData(String s) const override final{
		return false;
	}
	const Data* getLocalData(RData& r, PositionID id, String s, const Data* instance) const override final{
		illegalLocal(id,s);
		exit(1);
	}
	/*
	std::pair<AbstractClass*,unsigned int> getLocalVariable(PositionID id, String s) override final{
			illegalLocal(id,s);
			return std::pair<AbstractClass*,unsigned int>(this,0);
		}*/
	bool noopCast(const AbstractClass* const toCast) const override{
		return toCast->classType==CLASS_CPOINTER || toCast->classType==CLASS_VOID;
	}
	bool hasCast(const AbstractClass* const toCast) const override{
		return toCast->classType==CLASS_CPOINTER || toCast->classType==CLASS_VOID;
	}
	int compare(const AbstractClass* const a, const AbstractClass* const b) const override final{
			assert(hasCast(a));
			assert(hasCast(b));
			if(a->classType==CLASS_VOID && b->classType==CLASS_VOID) return 0;
			else if(a->classType==CLASS_VOID) return 1;
			else if(b->classType==CLASS_VOID) return -1;
			return 0;
	}
	/**
	 * Will error with id if this.hasCast(toCast)==false
	 */
	llvm::Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, llvm::Value* valueToCast) const override{
		if(toCast->classType!=CLASS_CPOINTER) id.error("Cannot cast "+getName()+" type to "+toCast->getName());
		return valueToCast;
	}
};

const CPointerClass c_pointerClass(true);

#endif /* CPOINTERCLASS_HPP_ */
