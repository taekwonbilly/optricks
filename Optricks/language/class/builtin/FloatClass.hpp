/*
 * FloatClass.hpp
 *
 *  Created on: Dec 27, 2013
 *      Author: Billy
 */

#ifndef FLOATCLASS_HPP_
#define FLOATCLASS_HPP_
#include "RealClass.hpp"

#define FLOATCLASS_C_
class FloatClass: public RealClass{
public:
	enum FloatType{
		HalfTy=1, // 16-bit floating point type
		FloatTy=2, // 32-bit floating point type
		DoubleTy=3, // 64-bit floating point type
		X86_FP80Ty=4, // 80-bit floating point type (X87)
		FP128Ty=5,// 128-bit floating point type (112-bit mantissa)
		PPC_FP128Ty=6// 128-bit floating point type (two 64-bits, PowerPC)
	};
	const FloatType floatType;
	inline FloatClass(String nam, FloatType t):
		RealClass(nam, PRIMITIVE_LAYOUT,CLASS_FLOAT,
				(t==HalfTy)?Type::getHalfTy(getGlobalContext()):(
				(t==FloatTy)?Type::getFloatTy(getGlobalContext()):(
				(t==DoubleTy)?Type::getDoubleTy(getGlobalContext()):(
				(t==X86_FP80Ty)?Type::getX86_FP80Ty(getGlobalContext()):(
				(t==FP128Ty)?Type::getFP128Ty(getGlobalContext()):(
				/*t==PPC_FP128Ty*/Type::getPPC_FP128Ty(getGlobalContext())
				)))))
	),floatType(t){
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
	unsigned getWidth() const{
		return ((IntegerType*)type)->getBitWidth();
	}
	/*
	inline void checkFit(PositionID id, mpz_class const value) const{
		if(sgn(value)<0){
			if(!isSigned) id.error("Cannot cast negative integer literal to unsigned type");
			auto t_width=mpz_sizeinbase(value.get_mpz_t(),2)+1;
			auto r_width = getWidth();
			if(t_width > r_width) id.error("Cannot fit negative integer literal needing "+t_width+" bits in signed type of size "+r_width+" bits");
		} else {
			auto t_width = (isSigned)?(mpz_sizeinbase(value.get_mpz_t(),2)+1):(mpz_sizeinbase(value.get_mpz_t(),2));
			auto r_width = getWidth();
			if(t_width > r_width) id.error("Cannot fit positive integer literal needing "+t_width+" bits in integral type of size "+r_width+" bits");
			//TODO force APInt to be right width/sign for value
		}
	}*/
	inline const fltSemantics& getSemantics() const{
		switch(floatType){
		case HalfTy: return APFloat::IEEEhalf;
		case FloatTy: return APFloat::IEEEsingle;
		case DoubleTy: return APFloat::IEEEdouble;
		case X86_FP80Ty: return APFloat::x87DoubleExtended;
		case FP128Ty: return APFloat::IEEEquad;
		case PPC_FP128Ty: return APFloat::PPCDoubleDouble;
		default: return APFloat::Bogus;
		}
	}
	inline unsigned getWidth(){
		switch(floatType){
		case HalfTy: return 16;
		case FloatTy: return 32;
		case DoubleTy: return 64;
		case X86_FP80Ty: return 80;
		case FP128Ty: return 128;
		case PPC_FP128Ty: return 128;
		default: assert(0); return 0;
		}
	}
	inline ConstantFP* getLargest (bool Negative=false) const {
		return ConstantFP::get(getGlobalContext(),APFloat::getLargest(getSemantics(),Negative));
	}
	inline ConstantFP* getSmallest (bool Negative=false) const {
		return ConstantFP::get(getGlobalContext(),APFloat::getSmallest(getSemantics(),Negative));
	}
	inline ConstantFP* getSmallestNormalized (bool Negative=false) const {
		return ConstantFP::get(getGlobalContext(),APFloat::getSmallestNormalized(getSemantics(),Negative));
	}
	inline ConstantFP* getNaN() const{
		return ConstantFP::get(getGlobalContext(), APFloat::getNaN(getSemantics()));
	}
	inline ConstantFP* getZero(PositionID id, bool negative=false) const override final{
		return ConstantFP::get(getGlobalContext(), APFloat::getZero(getSemantics(),negative));
	}
	inline ConstantFP* getOne(PositionID id) const override final{
		return ConstantFP::get(getGlobalContext(), APFloat(getSemantics(),1));
	}
	inline ConstantFP* getEulerMasc(PositionID id) const{
		mpfr_t e;
		mpfr_init2(e, getWidth());
		mpfr_const_euler(e, MPFR_RNDN);
		auto tmp = getValue(id,e);
		mpfr_clear(e);
		return tmp;
	}
	inline ConstantFP* getPi(PositionID id) const{
		mpfr_t e;
		mpfr_init2(e, getWidth());
		mpfr_const_pi(e, MPFR_RNDN);
		auto tmp = getValue(id,e);
		mpfr_clear(e);
		return tmp;
	}
	inline ConstantFP* getE(PositionID id) const{
		mpfr_t e;
		mpfr_init2(e, getWidth());
		mpfr_t ze;
		mpfr_init2(ze,getWidth());
		mpfr_set_ui(ze,1,MPFR_RNDN);
		mpfr_exp(e,ze,MPFR_RNDN);
		mpfr_const_pi(e, MPFR_RNDN);
		auto tmp = getValue(id,e);
		mpfr_clear(e);
		return tmp;
	}
	inline ConstantFP* getLN2(PositionID id) const{
		mpfr_t e;
		mpfr_init2(e, getWidth());
		mpfr_const_log2(e, MPFR_RNDN);
		auto tmp = getValue(id,e);
		mpfr_clear(e);
		return tmp;
	}
	inline ConstantFP* getCatalan(PositionID id) const{
		mpfr_t e;
		mpfr_init2(e, getWidth());
		mpfr_const_catalan(e, MPFR_RNDN);
		auto tmp = getValue(id, e);
		mpfr_clear(e);
		return tmp;
	}
	inline ConstantFP* getInfinity(bool negative=false) const{
		return (ConstantFP*) ConstantFP::getInfinity(type,negative);
	}
	inline Constant* getValue(PositionID id, mpz_t const value) const override final{
		auto tmp =  mpz_get_str(nullptr, 10, value);
		void (*freefunc)(void*,size_t);
		Constant* ret = ConstantFP::get(type,tmp);
		mp_get_memory_functions(nullptr, nullptr, &freefunc);
		freefunc(tmp, strlen(tmp)+1);
		return ret;
	}
	inline ConstantFP* getValue(PositionID id, mpfr_t const value) const{
		if(mpfr_regular_p(value)){

		    char *s = NULL;
		    std::string out;

			if(!(mpfr_asprintf(&s,"%.RNE",value) < 0))
			{
				out = std::string(s);

				mpfr_free_str(s);
			} else id.compilerError("Error creating string for float to llvm conversion");

			return ConstantFP::get(getGlobalContext(),APFloat(getSemantics(),out));
		}
		else if(mpfr_nan_p(value)) return getNaN();
		else if(mpfr_inf_p(value)){
			return getInfinity(mpfr_signbit(value));
		} else{
			assert(mpfr_zero_p(value));
			return getZero(id,mpfr_signbit(value));
		}
	}
	bool noopCast(const AbstractClass* const toCast) const override{
		return toCast->classType==CLASS_FLOAT && type==toCast->type;
	}
	bool hasCast(const AbstractClass* const toCast) const override;
	int compare(const AbstractClass* const a, const AbstractClass* const b) const override final;
	/**
	 * Will error with id if this.hasCast(toCast)==false
	 */
	Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, Value* valueToCast) const override;
};

FloatClass* float16Class = new FloatClass("float16", FloatClass::HalfTy);
FloatClass* float32Class = new FloatClass("float32", FloatClass::FloatTy);
FloatClass* float64Class = new FloatClass("float64", FloatClass::DoubleTy);
FloatClass* float80Class = new FloatClass("float80", FloatClass::X86_FP80Ty);
FloatClass* float128Class = new FloatClass("float128", FloatClass::FP128Ty);


FloatClass* floatClass = new FloatClass("float", FloatClass::FloatTy);
FloatClass* doubleClass = new FloatClass("double", FloatClass::DoubleTy);
#endif /* FLOATCLASS_HPP_ */