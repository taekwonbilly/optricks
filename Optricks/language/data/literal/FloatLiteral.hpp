/*
 * FloatLiteral.hpp
 *
 *  Created on: Jan 2, 2014
 *      Author: Billy
 */

#ifndef FLOATLITERAL_HPP_
#define FLOATLITERAL_HPP_
#include "Literal.hpp"
#include "../VoidData.hpp"
class FloatLiteral:public Literal{
public:

	mutable mpfr_t value;
	FloatLiteral(const char* str, unsigned base):
		Literal(R_FLOAT),value(){
		mpfr_init_set_str(value,str,base,MPFR_RNDN);
	}
	FloatLiteral(mpfr_t m):
		Literal(R_FLOAT),value(){
		mpfr_init_set(value, m, MPFR_RNDN);
	}
	FloatLiteral(double d):
			Literal(R_FLOAT),value(){
			mpfr_init_set_d(value,d,MPFR_RNDN);
	}
	FloatLiteral(long double d):
			Literal(R_FLOAT),value(){
			mpfr_init_set_ld(value,d,MPFR_RNDN);
	}
	FloatLiteral(mpz_t const val):
		Literal(R_FLOAT),value(){
		mpfr_init(value);
		mpfr_set_z(value, val, MPFR_RNDN);
	}
	const AbstractClass* getReturnType() const override final;
	ConstantFP* getValue(RData& r, PositionID id) const override final;

	const Data* callFunction(RData& r, PositionID id, const std::vector<const Evaluatable*>& args) const override{
		id.error("Cannot use floating-point literal as function");
		return VOID_DATA;
	}
	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const override final{
		id.error("Floating-point literal cannot act as function");
		exit(1);
	}
	const AbstractClass* getMyClass(RData& r, PositionID id) const override final{
		id.error("Cannot use floating-point literal as class");
		exit(1);
	}
	bool hasCastValue(const AbstractClass* const a) const override final;

	int compareValue(const AbstractClass* const a, const AbstractClass* const b) const override final;
	const Data* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final;
	Constant* castToV(RData& r, const AbstractClass* const right, const PositionID id) const override final;
	virtual ~FloatLiteral(){
		mpfr_clear(value);
	}
	void toStream(ostream& s) const {
		if(mpfr_regular_p(value)){
			//hard part
			assert(0);

		    char *st = NULL;
		    std::string out;

			if(!(mpfr_asprintf(&st,"%.RNE",value) < 0))
			{
				s << st;
				mpfr_free_str(st);
			} else assert(0);
		}
		else if(mpfr_nan_p(value)){
			s << "nan";
		}
		else if(mpfr_inf_p(value)){
			auto sb=mpfr_signbit(value);
			if(sb) s << "-inf";
			else s << "inf";
		} else{
			auto sb=mpfr_signbit(value);
			if(sb) s << "-0.";
			else s << "0.";
		}
	}
};





#endif /* FLOATLITERAL_HPP_ */