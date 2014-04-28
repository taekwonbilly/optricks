/*
 * Deconstructor.hpp
 *
 *  Created on: Apr 14, 2014
 *      Author: Billy
 */

#ifndef DECONSTRUCTOR_HPP_
#define DECONSTRUCTOR_HPP_

#include "../language/includes.hpp"
#include "../language/data/TupleData.hpp"
#include "../language/data/LocationData.hpp"
#include "../language/data/ConstantData.hpp"
#include "../language/class/builtin/TupleClass.hpp"
#include "../language/class/builtin/IntClass.hpp"
#include "../language/class/builtin/FloatClass.hpp"
void incrementCount(RData& r, PositionID filePos, const Data* D){
	const AbstractClass* C = D->getReturnType();
	switch(C->classType){
	case CLASS_FUNC:
	case CLASS_CPOINTER:
	case CLASS_GEN:
	case CLASS_BOOL:
	case CLASS_FLOAT:
	case CLASS_NULL:
	case CLASS_MATHLITERAL:
	case CLASS_INT:
	case CLASS_INTLITERAL:
	case CLASS_STRLITERAL:
	case CLASS_CHAR:
	case CLASS_COMPLEX:
	case CLASS_RATIONAL:
	case CLASS_ARRAY:
	case CLASS_MAP:
	case CLASS_SET:
	case CLASS_VECTOR:
	case CLASS_VOID:
	case CLASS_REF:
	case CLASS_CLASS:
	case CLASS_LAZY:
	case CLASS_FLOATLITERAL:
		/*
		 * Tuple tracking done during constructor / deconstructor
		 */
	case CLASS_TUPLE:
	case CLASS_NAMED_TUPLE:
	{
		filePos.warning("Incrementing count for (literal) class "+C->getName()+" ");
		return;
	}
	case CLASS_STR:{
		filePos.error("Cannot increment count for class "+C->getName()+" ");
		return;
	}
	case CLASS_USER:
		if(C->layout==POINTER_LAYOUT){
			filePos.error("Cannot increment count for pointer user class "+C->getName()+" ");
			return;
		} else{
			filePos.warning("Incrementing count for primitive user class "+C->getName()+" ");
			return;
		}
	}
}

void decrementCount(RData& r, PositionID filePos, const Data* D){
	const AbstractClass* C = D->getReturnType();
	switch(C->classType){
	case CLASS_FUNC:
	case CLASS_CPOINTER:
	case CLASS_GEN:
	case CLASS_BOOL:
	case CLASS_FLOAT:
	case CLASS_NULL:
	case CLASS_MATHLITERAL:
	case CLASS_INT:
	case CLASS_INTLITERAL:
	case CLASS_STRLITERAL:
	case CLASS_CHAR:
	case CLASS_COMPLEX:
	case CLASS_RATIONAL:
	case CLASS_ARRAY:
	case CLASS_MAP:
	case CLASS_SET:
	case CLASS_VECTOR:
	case CLASS_VOID:
	case CLASS_REF:
	case CLASS_CLASS:
	case CLASS_LAZY:
	case CLASS_FLOATLITERAL:
	{
		llvm::SmallVector<Type*,1> t_args(1);
		t_args[0] = CSTRINGTYPE;
		auto CU = r.getExtern("puts", FunctionType::get(c_intClass.type, t_args,true));
		r.builder.CreateCall(CU, r.getConstantCString("Decrementing count for class "+C->getName()+" "));
		//filePos.warning("Decrementing count for class "+C->getName()+" ");
		return;
	}
		/*
		 * Tuple tracking done during constructor / deconstructor
		 */
	case CLASS_TUPLE:
	case CLASS_NAMED_TUPLE:
	{
		const TupleClass* TC = (const TupleClass*)C;
		for(unsigned i=0; i<TC->innerTypes.size(); i++){
			if(D->type==R_TUPLE){
				TupleData* td = (TupleData*)D;
				decrementCount(r, filePos, td->inner[i]->castTo(r,TC->innerTypes[i],filePos));
				return;
			}
			if(D->type==R_LOC){
				auto LD = ((const LocationData*)D)->value;
				if(TC->innerTypes.size()==1){
					LocationData LOCD(LD, TC->innerTypes[0]);
					decrementCount(r, filePos, &LOCD);
					return;
				}
				else{
					LocationData LOCD(LD->getInner(r, filePos, 0, i), TC->innerTypes[i]);
					decrementCount(r, filePos, &LOCD);

					return;
				}
			} else{
				assert(D->type==R_CONST);
				llvm::Value* v = ((ConstantData*)D)->value;
				if(TC->innerTypes.size()==1){
					ConstantData CD(v, TC->innerTypes[0]);
					decrementCount(r, filePos, &CD);
					return;
				} else {
					ConstantData CD(r.builder.CreateExtractValue(v, i), TC->innerTypes[i]);
					decrementCount(r, filePos, &CD);
					return;
				}
			}
		}
		filePos.warning("Deconstructed literal class "+C->getName()+" ");
		return;
	}
	case CLASS_STR:{
		filePos.error("Cannot decrement count for class "+C->getName()+" ");
		return;
	}
	case CLASS_USER:
		if(C->layout==POINTER_LAYOUT){
			filePos.error("Cannot decrement count for pointer user class "+C->getName()+" ");
			return;
		} else{
			filePos.warning("Decrementing count for primitive user class "+C->getName()+" ");
			return;
		}
	}
}



#endif /* DECONSTRUCTOR_HPP_ */