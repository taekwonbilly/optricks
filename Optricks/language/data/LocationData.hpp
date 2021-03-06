/*
 * LocationData.hpp
 *
 *  Created on: Dec 29, 2013
 *      Author: Billy
 */

#ifndef LOCATIONDATA_HPP_
#define LOCATIONDATA_HPP_
#include "LLVMData.hpp"

#define LOCATIONDATA_C_
class LocationData:public LLVMData{
public:
	Location* const value;
	LocationData(Location* const val, const AbstractClass* cp):LLVMData(R_LOC, cp),value(val){
		assert(value); assert(cp);
		assert(value->getType()==cp->type);
		//assert(cp->classType!=CLASS_CLASS);
		//assert(cp->classType!=CLASS_FUNC);
		//assert(cp->classType!=CLASS_GEN);
		assert(cp->layout!=LITERAL_LAYOUT);
	}
	inline llvm::Value* getValue(RData& r, PositionID id) const override final{
		return value->getValue(r,id);
	}
	inline void setValue(RData& r, llvm::Value* v) const{
		value->setValue(v, r);
	}
	inline Location* getMyLocation() const{
		return value;
	}
	const ConstantData* toValue(RData& r,PositionID id) const override final;
	inline const LocationData* toLocation(RData& r, String name) const override final{
		return this;
	}
	inline const Data* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final;
	inline llvm::Value* castToV(RData& r, const AbstractClass* const right, const PositionID id) const override final;
	const AbstractClass* getMyClass(PositionID id) const override final{
		if(type->classType!=CLASS_CLASS) id.error("Cannot use non-class type as a class");
		id.error("Cannot use non-constant class type");
		exit(1);
	}
};



#endif /* LOCATIONDATA_HPP_ */
