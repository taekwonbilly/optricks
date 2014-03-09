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
inline Value* FunctionClass::castTo(const AbstractClass* const toCast, RData& r, PositionID id, Value* valueToCast) const{
		switch(toCast->classType){
		case CLASS_CPOINTER: return r.builder.CreatePointerCast(valueToCast, C_POINTERTYPE);
		case CLASS_FUNC: {
			if(noopCast(toCast)) return valueToCast;
			else {
				id.error("Cannot cast value of function type "+getName()+" to "+toCast->getName());
				exit(1);
			}
	/*
			if(toCast==this) return true;
			//TODO
			assert(0 & "Function class autocasting has yet to be determined in the Optricks spec");
			cerr << "Function class autocasting has yet to be determined in the Optricks spec" << endl << flush;
			exit(1);*/
		}
		default:
			id.error("Cannot cast value of type "+getName()+" to "+toCast->getName());
			exit(1);
		}
	}



#endif /* FUNCTIONCLASSP_HPP_ */