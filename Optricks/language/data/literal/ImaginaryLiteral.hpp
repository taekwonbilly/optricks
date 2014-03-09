/*
 * ComplexLiteral.hpp
 *
 *  Created on: Jan 2, 2014
 *      Author: Billy
 */

#ifndef IMAGINARYLITERAL_HPP_
#define IMAGINARYLITERAL_HPP_


#include "Literal.hpp"
#include "../VoidData.hpp"
//TODO implement methods
class ImaginaryLiteral:public Literal{
public:
	const Data* const imag;
	virtual ~ImaginaryLiteral(){};
	const Data* callFunction(RData& r, PositionID id, const std::vector<const Evaluatable*>& args) const override{
		id.error("Cannot use complex literal as function");
		return VOID_DATA;
	}
	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const override{
		id.error("Complex literal cannot act as function");
		exit(1);
	}
	const AbstractClass* getMyClass(RData& r, PositionID id) const override final{
		id.error("Cannot use complex literal as class");
		exit(1);
	}
	bool hasCastValue(const AbstractClass* const a) const override final;
	int compareValue(const AbstractClass* const a, const AbstractClass* const b) const override final;
	ImaginaryLiteral(const Data* const i):Literal(R_IMAG),imag(i){
		assert(i);
		assert(i->type==R_INT || i->type==R_FLOAT);

	}
	const AbstractClass* getReturnType() const override final;
	ConstantVector* getValue(RData& r, PositionID id) const override final;
	const Data* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final;
	Value* castToV(RData& r, const AbstractClass* const right, const PositionID id) const override final;
};


#endif /* COMPLEXLITERAL_HPP_ */