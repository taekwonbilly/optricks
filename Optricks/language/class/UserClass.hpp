/*
 * UserClass.hpp
 *
 *  Created on: Jan 19, 2014
 *      Author: Billy
 */

#ifndef USERCLASS_HPP_
#define USERCLASS_HPP_
#include "./AbstractClass.hpp"
#include "../function/UnaryFunction.hpp"
//TODO note if class has 1 arg, does not make struct
//TODO note if class has 0 arg, does not make anything
class UserClass: public AbstractClass{
private:
	std::map<String, OverloadedFunction*> localFunctions;
	std::vector<const AbstractClass*> localVars;
	std::map<String,unsigned int> localMap;
	std::map<String,UnaryFunction> preop;
	std::map<String,UnaryFunction> postop;
	unsigned int start;
	bool final;
public:
	UserClass(const Scopable* sc, String nam, const AbstractClass* const supa, LayoutType t, bool fina,bool isObject=false);
	inline OverloadedFunction* addLocalFunction(const String s, void* generic=nullptr){
		auto find = localFunctions.find(s);
		if(find==localFunctions.end()){
			return localFunctions.insert(
					std::pair<String,OverloadedFunction*>(s,new OverloadedFunction(s, generic))).first->second;
		}
		return find->second;
	}
	inline const UnaryFunction& getPostop(PositionID id, String s) const{
		auto tmp = this;
		do{
			auto find = tmp->postop.find(s);
			if(find!=tmp->postop.end()) return find->second;
			tmp = (const UserClass*)superClass;
		}while(tmp);
		id.error("Could not find unary post-operator '"+s+"' in class '"+getName()+"'");
		exit(1);
	}
	inline const UnaryFunction& getPreop(PositionID id, String s) const{
		auto tmp = this;
		do{
			auto find = tmp->preop.find(s);
			if(find!=tmp->preop.end()) return find->second;
			tmp = (const UserClass*)superClass;
		}while(tmp);
		id.error("Could not find unary pre-operator '"+s+"' in class '"+getName()+"'");
		exit(1);
	}
	inline SingleFunction* getLocalFunction(PositionID id, String s, const std::vector<const AbstractClass*>& v) const{
		auto tmp = this;
		do{
			auto find = tmp->localFunctions.find(s);
			if(find!=tmp->localFunctions.end()) return find->second->getBestFit(id,v);
			tmp = (const UserClass*)superClass;
		}while(tmp);
		id.error("Could not find local method '"+s+"' in class '"+getName()+"'");
		exit(1);
	}

	inline Value* generateData(RData& r, PositionID id) const;
	inline SingleFunction* getLocalFunction(PositionID id, String s, const std::vector<const Evaluatable*>& v) const{
		auto tmp = this;
		do{
			auto find = tmp->localFunctions.find(s);
			if(find!=tmp->localFunctions.end()) return find->second->getBestFit(id,v);
			tmp = (const UserClass*)superClass;
		}while(tmp);
		id.error("Could not find local method '"+s+"' in class '"+getName()+"'");
		exit(1);
	}
	void finalize(PositionID id){
		assert(!final);
		final = true;
		if(superClass){
			UserClass* uc = (UserClass*)superClass;
			if(!uc->final) id.compilerError("Superclass is not finalized");
			start = uc->start+uc->localVars.size();
		}
		StructType* structType = (layout==POINTER_LAYOUT)?(
			(StructType*)(((PointerType*)type)->getArrayElementType())):
		((StructType*)type);
		std::vector<Type*> types;
		UserClass* tmp = this;
		do{
			const auto at=tmp->localVars.size();
			if(at >0){
			for(unsigned int i=at-1; ; i--){
				types.push_back(tmp->localVars[i]->type);
				if(i==0) break;
			}
			}
			tmp = (UserClass*) tmp->superClass;
		}while(tmp);
		reverse(types.begin(), types.end());
		structType->setBody(ArrayRef<Type*>(types),false);
	}
	void addLocalVariable(PositionID id, String s, const AbstractClass* const ac){
		assert(ac);
		if(ac->layout==LITERAL_LAYOUT) id.error("Cannot create local variable inside of class literal");
		auto tmp=this;
		do{
			if(tmp->localMap.find(s)!=tmp->localMap.end()){
				id.error("Cannot create local variable '"+s+"' in class '"+getName()+"', it is already defined as a variable in '"+tmp->getName()+"'");
				return;
			}
			if(tmp->localFunctions.find(s)!=tmp->localFunctions.end()){
				id.error("Cannot create local variable '"+s+"' in class '"+getName()+"', it is already defined as a function in '"+tmp->getName()+"'");
				return;
			}
			tmp = (UserClass*)(tmp->superClass);
		}while(tmp);
		//if(staticVariables.exists(s)){}
		localMap[s]=localVars.size();
		localVars.push_back(ac);
	}
	bool hasLocalData(String s) const override final{
		if(!final) PositionID(0,0,"#user").compilerError("Cannot hasLocalData() on unfinalized type");
		auto tmp=this;
		do{
			auto fd = tmp->localMap.find(s);
			if(fd!=tmp->localMap.end()){
				return true;
			}
			tmp = (UserClass*)(tmp->superClass);
		}while(tmp);
		return false;
	}
	const AbstractClass* getLocalReturnClass(PositionID id, String s) const override final{
		if(!final) id.compilerError("Cannot getLocalReturnClass() on unfinalized type");
		auto tmp=this;
			do{
				auto fd = tmp->localMap.find(s);
				if(fd!=tmp->localMap.end()){
					return tmp->localVars[fd->second];
				}
				tmp = (UserClass*)(tmp->superClass);
			}while(tmp);
		illegalLocal(id,s);
		exit(1);
	}

	const Data* getLocalData(RData& r, PositionID id, String s, const Data* instance) const override final{
		if(!final) id.compilerError("Cannot getLocalData() on unfinalized type");

		auto tmp=this;
				do{
					auto fd = tmp->localMap.find(s);
					if(fd!=tmp->localMap.end()){
						unsigned start = tmp->start+fd->second;
						assert(instance->type==R_LOC || instance->type==R_CONST);
						assert(instance->getReturnType()==this);
						if(instance->type==R_LOC){
							//TODO location
							id.compilerError("TODO:// allow getting class data from location");
						} else{
							assert(instance->type==R_CONST);
							Value* v = ((ConstantData*)instance)->value;
							return new ConstantData(r.builder.CreateExtractValue(v,start),tmp->localVars[fd->second]);
						}
					}
					tmp = (UserClass*)(tmp->superClass);
				}while(tmp);
			illegalLocal(id,s);
			exit(1);
	}
	/*std::pair<AbstractClass*,unsigned int> getLocalVariable(PositionID id, String s) override final{
		if(!final) id.compilerError("Cannot getLocalVariable() on unfinalized type");
		auto tmp=this;
		do{
			auto fd = tmp->localMap.find(s);
			if(fd!=tmp->localMap.end()){
				return std::pair<AbstractClass*,unsigned int>(tmp->localVars[fd->second], tmp->start+fd->second);
			}
			tmp = (UserClass*)(tmp->superClass);
		}while(tmp);
		id.error("Cannot find local variable '"+s+"' in class '"+getName()+"'");
		return std::pair<AbstractClass*,unsigned int>(this,0);
	}*/
	bool noopCast(const AbstractClass* const toCast) const override{
		return this==toCast;
		//todo decide if it is no-op class to
//		return toCast->classType==CLASS_BOOL;
	}
	bool hasCast(const AbstractClass* const toCast) const override{
		if(toCast->classType!=CLASS_USER) return false;
		if(this==toCast) return true;
		if(layout==PRIMITIVE_LAYOUT || toCast->layout==PRIMITIVE_LAYOUT) return false;
		UserClass* tmp = (UserClass*)superClass;
		while(tmp!=nullptr){
			if(tmp==toCast) return true;
			else tmp=(UserClass*)tmp->superClass;
		}
		return false;
	}
	/**
	 * Will error with id if this.hasCast(toCast)==false
	 */
	Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, Value* valueToCast) const override{
		if(toCast->classType!=CLASS_USER){
			id.error("Cannot promote class '"+getName()+"' to "+toCast->getName());
			exit(1);
		}
		if(this==toCast) return valueToCast;
		if(layout==PRIMITIVE_LAYOUT || toCast->layout==PRIMITIVE_LAYOUT){
			id.error("Cannot promote user-defined primitive types");
			exit(1);
		}
		UserClass* tmp = (UserClass*)superClass;
		while(tmp!=nullptr){
			if(tmp==toCast) return r.builder.CreatePointerCast(valueToCast, toCast->type);
			else tmp=(UserClass*)tmp->superClass;
		}
		id.error("Cannot promote class '"+getName()+"' to "+toCast->getName());
		exit(1);
	}
	int compare(const AbstractClass* const a, const AbstractClass* const b) const{
		assert(a->classType==CLASS_USER);
		assert(b->classType==CLASS_USER);

		const UserClass* tmp = this;
		do{
			if(tmp==a){
				return (tmp==b)?(0):(-1);
			} else if(tmp==b) return 1;
			tmp = (UserClass*) ( tmp->superClass);
		}while(tmp!=nullptr);
		return 0;
	}
};

UserClass* objectClass = new UserClass(LANG_M,"object",nullptr,POINTER_LAYOUT,false,true);

#endif /* USERCLASS_HPP_ */