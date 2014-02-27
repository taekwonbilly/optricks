/*
 * ArrayClass.hpp
 *
 *  Created on: Feb 7, 2014
 *      Author: Billy
 */

#ifndef ARRAYCLASS_HPP_
#define ARRAYCLASS_HPP_
#include "../AbstractClass.hpp"

class ArrayClass: public AbstractClass{
public:
	static inline String str(const AbstractClass* const d, uint64_t l){
		assert(d);
		//if(d==nullptr) return "array";
		/** length 0 is arbitrary-length **/
		if(l==0) return d->getName()+"[]";
		else{
			std::stringstream ss;
			ss << d->getName() << '[' << l << ']';
			return ss.str();
		}
	}
	static inline Type* getArrayType(const AbstractClass* const d, uint64_t l){
		if(l>0){
			ArrayType* at = ArrayType::get(d->type,l);
			return at;
		} else {
			Type* ar[4] = {
					/* Counts (for garbage collection) */ IntegerType::get(getGlobalContext(),32),
					/* Length of array */ IntegerType::get(getGlobalContext(),64),
					/* Amount of memory allocated */ IntegerType::get(getGlobalContext(),64),
					/* Actual data */ PointerType::getUnqual(d->type)
				};
		return PointerType::getUnqual(StructType::create(ArrayRef<Type*>(ar, l),str(d,l),false));
		}
	}
	const AbstractClass* inner;
	const uint64_t len;
protected:
	ArrayClass(const AbstractClass* a,uint64_t le):
		AbstractClass(nullptr,str(a,le),nullptr,PRIMITIVE_LAYOUT,CLASS_ARRAY,true,getArrayType(a,le)),inner(a),len(le){
		///register methods such as print / tostring / tofile / etc
	}
public:
	inline bool hasCast(const AbstractClass* const toCast) const{
		switch(toCast->classType){
		case CLASS_ARRAY: {
			//if(!inner) return true;
			ArrayClass* tc = (ArrayClass*)toCast;
			if(!inner->hasCast(tc->inner)) return false;
			if(tc->len==0) return true;
			return tc->len==len;
		}
		default:
			return false;
		}
	}

	AbstractClass* getLocalReturnClass(PositionID id, String s) const override{
		if(s!="length"){
			illegalLocal(id,s);
			exit(1);
		}
		else return int32Class;
	}

	const Data* getLocalData(RData& r, PositionID id, String s, const Data* instance) const override{
		if(s!="length"){
			illegalLocal(id,s);
			exit(1);
		}
		if(len>0)
			return new IntLiteral(len, int32Class);
		else{
			assert(instance->type==R_LOC);
			auto f = ((LocationData*)instance)->getMyLocation()->getPointer(r,id);
			return new ConstantData(r.builder.CreateConstGEP2_32(f, 0, 1), int32Class);
		}
	}
	inline bool noopCast(const AbstractClass* const toCast) const override{
		switch(toCast->classType){
		case CLASS_ARRAY: {
			//if(!inner) return true;
			ArrayClass* tc = (ArrayClass*)toCast;
			if(!inner->noopCast(tc->inner)) return false;
			if(tc->len==0) return true;
			return tc->len==len;
		}
		default:
			return false;
		}
	}
	Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, Value* valueToCast) const;

	int compare(const AbstractClass* const a, const AbstractClass* const b) const{
		assert(a->classType==CLASS_ARRAY);
		assert(b->classType==CLASS_ARRAY);
		assert(hasCast(a));
		assert(hasCast(b));
		ArrayClass* fa = (ArrayClass*)a;
		ArrayClass* fb = (ArrayClass*)b;
		auto NEX = inner->compare(fa->inner, fb->inner);
		if(NEX==0){
			if(fa->len==len){
				if(fb->len==len) return 0;
				else return -1;
			} else if(fb->len==len){
				return 1;
			} else return 0;
		} else return NEX;
	}
	static ArrayClass* get(const AbstractClass* args, uint64_t l) {
		if(args==nullptr){
			static ArrayClass* ac = new ArrayClass(nullptr,0);
			return ac;
		} else {
			static std::map<const AbstractClass*,std::map<uint64_t, ArrayClass*>> mp;
			auto tmp = mp.find(args);
			if(tmp==mp.end()){
				return mp[args][l] = new ArrayClass(args,l);
			}
			auto tmp2 = tmp->second;
			auto fd = tmp2.find(l);
			if(fd==tmp2.end()){
				return tmp2[l] = new ArrayClass(args,l);
			} else return fd->second;
		}
	}
};




#endif /* ARRAYCLASS_HPP_ */
