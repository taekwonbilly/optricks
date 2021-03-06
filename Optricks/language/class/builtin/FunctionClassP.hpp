/*
 * FunctionClassP.hpp
 *
 *  Created on: Jan 17, 2014
 *      Author: Billy
 */

#ifndef FUNCTIONCLASSP_HPP_
#define FUNCTIONCLASSP_HPP_
#include "FunctionClass.hpp"
#include "../../RData.hpp"
inline llvm::Value* FunctionClass::castTo(const AbstractClass* const toCast, RData& r, PositionID id, llvm::Value* valueToCast) const{
		switch(toCast->classType){
		case CLASS_CPOINTER: return r.pointerCast(valueToCast, C_POINTERTYPE);
		case CLASS_FUNC: {
			if(noopCast(toCast)){
				assert(toCast->type->isPointerTy());
				if(toCast->type==valueToCast->getType()) return valueToCast;
				else return r.pointerCast(valueToCast, (llvm::PointerType*) toCast->type);
			}
			else {
				id.error("Cannot cast value of function type "+getName()+" to "+toCast->getName());
				exit(1);
			}
		}
		default:
			id.error("Cannot cast value of type "+getName()+" to "+toCast->getName());
			exit(1);
		}
	}



#endif /* FUNCTIONCLASSP_HPP_ */
