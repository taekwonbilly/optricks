/*
 * FunctionProto.hpp
 *
 *  Created on: Sep 11, 2013
 *      Author: wmoses
 */

#ifndef FUNCTIONPROTO_HPP_
#define FUNCTIONPROTO_HPP_

#include "settings.hpp"
#define FUNCTIONPROTO_C_
class FunctionProto{
		ClassProto* generatorType;
	public:
		String name;
		std::vector<Declaration*> declarations;
		ClassProto* returnType;
		FunctionProto(String n, std::vector<Declaration*>& a, ClassProto* r):generatorType(NULL),name(n),declarations(a), returnType(r){}
		FunctionProto(String n, ClassProto* r=NULL):generatorType(NULL),name(n),declarations(), returnType(r){}
		ClassProto* getGeneratorType(PositionID id);
		/*
		 * Checks if this can be casted to F, and how well
		 * In Declaration.hpp
		 */
		std::pair<bool,std::pair<unsigned int, unsigned int>> match(FunctionProto* func) const;
		bool equals(const FunctionProto* f, PositionID id) const;
		String toString() const;

};

class funcMap{
	private:
		std::vector<DATA> data;
	public:
		unsigned int size() const{ return data.size(); }
		funcMap(DATA a):data(){
			data.push_back(a);
		}
		funcMap():data(){
		}
		bool set(DATA t, PositionID id){
			FunctionProto* p = t.getFunctionType();
			for(unsigned int i = 0; i<data.size(); i++){
				if(data[i].getFunctionType()->equals(p, id)){
					data[i] = t;
					return true;
				}
			}
			data.push_back(t);
			return false;
		}
		void add(DATA t, PositionID id){
			if(set(t, id)){
				todo("Error overwriting function "+t.getFunctionType()->toString(),id);
			}
		}
		DATA get(FunctionProto* func,PositionID id) const{
			if(func==NULL) todo("NULL FunctionProto",id);
			std::vector<std::pair<DATA,std::pair<bool,std::pair<unsigned int, unsigned int> > > > possible;
			int bestind = -1;
			unsigned int count = UINT_MAX;
			unsigned int toCast = UINT_MAX;
			for(unsigned int i = 0; i<data.size(); ++i){
				FunctionProto* prot = data[i].getFunctionType();
				std::pair<bool,std::pair<unsigned int, unsigned int> > temp = func->match(prot);
				//cout << i << "*" << func->toString()+"|"+data[i].second->toString() << ":" << temp.first << "," << temp.second.first << "," << temp.second.second << endl << flush;
				if(temp.first){
					possible.push_back(std::pair<DATA,std::pair<bool,std::pair<unsigned int, unsigned int> > >
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
				for(const DATA & a:data){
					t+=a.getFunctionType()->toString()+"\n";
				}
				todo(t,id);
			}
			if(bestind==-1){
				String t = "Ambiguous function for "+func->toString()+" options are:\n";
				for(const std::pair<DATA, std::pair<bool,std::pair<unsigned int, unsigned int> > >& a:possible){
					if(a.second.second.first == count && a.second.second.second == toCast)
						t+=a.first.getFunctionType()->toString()+"\n";
				}
				todo(t,id);
			}
			return data[bestind];
		}
};


ClassProto* DATA::getMyClass() const{
	if(type!=R_CLASS && type!=R_GEN){
		cerr << "Cannot getClass of non-class " << type << endl << flush;
		exit(1);
	}
	if(data.pointer==NULL){
		cerr << "Cannot get NULL class" << endl << flush;
		exit(1);
	}
	if(type==R_GEN) return info.funcType->getGeneratorType(PositionID());
	return data.classP;
}

ClassProto* DATA::getReturnType() const{
	if(type==R_GEN){
		assert(info.funcType!=NULL);
		return info.funcType->getGeneratorType(PositionID());
	}
	assert((type==R_CONST || type==R_LOC || type==R_CLASS));
	assert(info.classType !=NULL);
	return info.classType;
}
#endif /* FUNCTIONPROTO_HPP_ */
