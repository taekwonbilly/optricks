/*
 * IntLiteralClass.hpp
 *
 *  Created on: Feb 24, 2014
 *      Author: Billy
 */

#ifndef INTLITERALCLASS_HPP_
#define INTLITERALCLASS_HPP_
#include "../AbstractClass.hpp"


	struct mpzCompare
	{
	    bool operator() (const mpz_t& val1, const mpz_t& val2) const
	    {
	        return mpz_cmp(val1, val2) > 0;
	    }
	};
class IntLiteralClass: public AbstractClass{
public:
	static inline String str(const mpz_t& value){
		auto tmp =  mpz_get_str(nullptr, 10, value);
		void (*freefunc)(void*,size_t);
		String s(tmp);
		mp_get_memory_functions(nullptr, nullptr, &freefunc);
		freefunc(tmp, strlen(tmp)+1);
		return tmp;
	}
	const AbstractClass* inner;
	mutable mpz_t value;
protected:
	IntLiteralClass(const mpz_t& val):
		AbstractClass(nullptr,str(val),nullptr,LITERAL_LAYOUT,CLASS_INTLITERAL,true,nullptr),value(val){
		///register methods such as print / tostring / tofile / etc
		//check to ensure that you can pass mpz_t like that instead of using _init
	}
public:
	inline bool hasCast(const AbstractClass* const toCast) const{
		switch(toCast->classType){
		case CLASS_COMPLEX:{
			ComplexClass* ic = (ComplexClass*)toCast;
			return hasCast(ic->innerClass);
		}
		case CLASS_FLOAT:{
			return true;
		}
		case CLASS_INTLITERAL: return toCast==this;
		case CLASS_INT: {
			//if(!inner) return true;
			IntClass* ic = (IntClass*)toCast;
			return ic->hasFit(value);
		}
		case CLASS_RATIONAL:
		default:
			return false;
		}
	}

	AbstractClass* getLocalReturnClass(PositionID id, String s) const override{
		illegalLocal(id,s);
		exit(1);
	}

	const Data* getLocalData(RData& r, PositionID id, String s, const Data* instance) const override{
		illegalLocal(id,s);
		exit(1);
	}
	inline bool noopCast(const AbstractClass* const toCast) const override{

		//todo allow complex/floats as well
		switch(toCast->classType){
		case CLASS_INTLITERAL: return toCast==this;
		case CLASS_INT: {
			//if(!inner) return true;
			IntClass* ic = (IntClass*)toCast;
			return ic->hasFit(value);
		}
		default:
			return false;
		}
	}
	Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, Value* valueToCast) const{
		id.compilerError("This method should never be called.....ever....");
		exit(1);
	}

	int compare(const AbstractClass* const a, const AbstractClass* const b) const{
		//todo allow complex/floats as well
		assert(a->classType==CLASS_INT || a==this );
		assert(b->classType==CLASS_INT || b==this);
		if(a==this){
			if(b==this) return 0;
			else return -1;
		} else if(b==this) return 1;
		else return 0;
	}
	static IntLiteralClass* get(const mpz_t& l) {
		std::map<const mpz_t, IntLiteralClass*, mpzCompare> mmap;
		auto find = mmap.find(l);
		if(find==mmap.end()) return mmap[l] = new IntLiteralClass(l);
		else return find->second;
	}
	static inline IntLiteralClass* get(const char* str, unsigned base) {
		mpz_t value;
		mpz_init_set_str(value,str,base);
		auto tmp = get(value);
		mpz_clear(value);//should check
		return tmp;
	}
	static inline IntLiteralClass* get(signed long int val) {
		mpz_t value;
		mpz_init_set_si(value,val);
		auto tmp = get(value);
		mpz_clear(value);//should check
		return tmp;
	}
	virtual ~IntLiteralClass(){
		mpz_clear(value);
	}

};



#endif /* INTLITERALCLASS_HPP_ */
