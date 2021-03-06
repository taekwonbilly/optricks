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
	FloatLiteral(const mpfr_t& m):
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
	FloatLiteral(int a ,int b, int c):Literal(R_FLOAT),value(){
		mpfr_init(value);
	}
	FloatLiteral(const mpz_t& val):
		Literal(R_FLOAT),value(){
		mpfr_init(value);
		mpfr_set_z(value, val, MPFR_RNDN);
	}
	inline FloatLiteral(bool toAdd, String n) : Literal(R_FLOAT), value(){
		mpfr_init(value);
		if(n=="Nan" || n=="nan")
			mpfr_set_nan(value);
		else if(n=="inf" || n=="Inf"){
			mpfr_set_inf(value, 1);
		} else{
			assert(n=="-inf"||n=="-Inf");
			mpfr_set_inf(value, 1);
		}
		LANG_M.addVariable(PositionID(0,0,"#floatLiteral"),n,this);
	}
	const AbstractClass* getReturnType() const override final;
	llvm::ConstantFP* getValue(RData& r, PositionID id) const override final;

	bool hasCastValue(const AbstractClass* const a) const override final;

	int compareValue(const AbstractClass* const a, const AbstractClass* const b) const override final;
	const Data* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final;
	llvm::Constant* castToV(RData& r, const AbstractClass* const right, const PositionID id) const override final;
	virtual ~FloatLiteral(){
		mpfr_clear(value);
	}

	String toString() const {
		if(mpfr_regular_p(value)){
		    char *st = nullptr;
			if(!(mpfr_asprintf(&st,"%.RNE",value) < 0))
			{
				assert(st);
				String s(st);
				mpfr_free_str(st);
				return s;
			} else assert(0);
			return "";
		}
		else if(mpfr_nan_p(value)){
			return "nan";
		}
		else if(mpfr_inf_p(value)){
			auto sb=mpfr_signbit(value);
			if(sb) return "-inf";
			else return "inf";
		} else{
			auto sb=mpfr_signbit(value);
			if(sb) return "-0.";
			else return "0.";
		}
	}
	void toStream(ostream& s) const {
		if(mpfr_regular_p(value)){
		    char *st = nullptr;
			if(!(mpfr_asprintf(&st,"%.RNE",value) < 0))
			{
				assert(st);
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

const FloatLiteral MY_NAN  (true,"nan");
const FloatLiteral MY_P_INF(true,"inf");
const FloatLiteral ZEROF_LITERAL((double)0);
const FloatLiteral MY_N_INF(false,"-inf");


#endif /* FLOATLITERAL_HPP_ */
