/*
 * all.hpp
 *
 *  Created on: Sep 11, 2013
 *      Author: wmoses
 */

#ifndef ALL_HPP_
#define ALL_HPP_

#include "settings.hpp"
#include "ClassProto.hpp"
#include "FunctionProto.hpp"
#include "GenericClass.hpp"
#include "operations.hpp"
#include "RData.hpp"
#include "types.hpp"
#include "operators.hpp"
#include "basic_functions.hpp"

#ifndef REFERENCEELEM_C_
#define REFERENCEELEM_C_
class ReferenceElement;
#endif
class Resolvable{
	public:
	OModule* module;
	String name;
	Resolvable(OModule* m, String n):module(m),name(n){};
	virtual ~Resolvable(){};
	virtual ReferenceElement* resolve() = 0;
};

class funcMap{
	private:
		std::vector<std::pair<Value*,FunctionProto*>> data;
	public:
		unsigned int size() const{
			return data.size();
		}
		funcMap(std::pair<Value*,FunctionProto*> a):data(){
			data.push_back(a);
		}
		funcMap():data(){
		}
		void add(FunctionProto* f, Value* t, PositionID id,RData& r){
			if(f==NULL) todo("Error can't add NULL function proto",id);
			if(set(f,t,r)){
				todo("Error overwriting function "+f->toString(),id);
			}
		}
		bool set(FunctionProto* in, Value* t,RData& r){
			for(auto& a:data){
				if(a.second->equals(in,r)){
					a.first = t;
					return true;
				}
			}
			data.push_back(std::pair<Value*,FunctionProto*>(NULL,in));
			return false;
		}
		std::pair<Value*,FunctionProto*>  get(FunctionProto* func,PositionID id,RData& r){
			if(func==NULL) todo("NULL FunctionProto",id);
			std::vector<std::pair<std::pair<Value*,FunctionProto*>,std::pair<bool,std::pair<unsigned int, unsigned int> > > > possible;
			int bestind = -1;
			unsigned int count = UINT_MAX;
			unsigned int toCast = UINT_MAX;
			for(unsigned int i = 0; i<data.size(); ++i){
				auto temp = func->match(data[i].second,r);
				//cout << i << "*" << func->toString()+"|"+data[i].second->toString() << ":" << temp.first << "," << temp.second.first << "," << temp.second.second << endl << flush;
				if(temp.first){
					possible.push_back(std::pair<std::pair<Value*,FunctionProto*>,std::pair<bool,std::pair<unsigned int, unsigned int> > >
					(data[i],temp));
					if(temp.second.first < count){
						bestind = i;
						count = temp.second.first;
						toCast = temp.second.second;
					} else if(temp.second.first==count){
						if(temp.second.second<toCast){
							bestind = i;
							count = temp.second.first;
							toCast = temp.second.second;
						} else if(temp.second.second==toCast){
							bestind = -1;
						}
					}
				}

			}
			if(possible.size()==0){
					String t = "No matching function for "+func->toString()+" options are:\n";
					for(auto & a:data){
						t+=a.second->toString()+"\n";
					}
					todo(t,id);
			}
			if(bestind==-1){
				String t = "Ambiguous function for "+func->toString()+" options are:\n";
				for(unsigned int i=0; i<data.size(); i++){
					if(possible[i].second.second.first == count && possible[i].second.second.second == toCast)
					t+=data[i].second->toString()+"\n";
				}
				todo(t,id);
			}
			return data[bestind];
		}
};
#define REFERENCEELEM_C_
class ReferenceElement:public Resolvable{
	private:
		DATA llvmObject;
public:
	ClassProto* returnClass;
	funcMap funcs;
	ClassProto* selfClass;
	Value* llvmLocation;
	virtual ~ReferenceElement(){};
	ReferenceElement(String c, OModule* mod, String index, DATA value, ClassProto* cl, funcMap fun, ClassProto* selfCl,Value* al=NULL):
		Resolvable(mod,index), llvmObject(value), returnClass(cl), funcs(fun), selfClass(selfCl), llvmLocation(al)
	{
	}
	ReferenceElement(OModule* mod, String index, DATA value, ClassProto* cl, funcMap& fun, ClassProto* selfCl,Value* al=NULL):
		Resolvable(mod,index), llvmObject(value), returnClass(cl), funcs(fun), selfClass(selfCl), llvmLocation(al)
	{
	}
	ReferenceElement* resolve() override final;
	DATA getValue(RData& r,FunctionProto* func=NULL){
		if(llvmLocation==NULL) return llvmObject;
		else return r.builder.CreateLoad(llvmLocation);
	}
	//void ensureFunction(FunctionProto* func){

//	}
	DATA setValue(DATA d, RData& r,FunctionProto* func=NULL){
		if(llvmLocation==NULL) return llvmObject=d;
		else return r.builder.CreateStore(d,llvmLocation);
	}
};
#endif /* ALL_HPP_ */