/*
 * oarray.hpp
 *
 *  oobject*reated on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef OARRAY_HPP_
#define OARRAY_HPP_

#include "oobjectproto.hpp"
#include "oint.hpp"
#include "oslice.hpp"

class oarray : public oobject{
	public:
		std::vector<oobject*> data;
		oarray(): oobject(arrayClass), data(){}
		oarray(const std::vector<oobject*>& dat) : oobject(arrayClass),data(dat){}
		operator std::vector<oobject*>& (){
			return data;
		}
		void checkTypes() override{
			todo("Implement forced array typing");
		}
		Value* evaluate(RData& a) override final{
			//TODO
			todo("Array not implemented");
		}
		/*oobject* operator [] (oobject* a) override{
			if(a->returnType==intClass){
				oint* s = (oint*)a;
				return data[s->value];
			}
			else if(a->returnType==sliceClass){
				oslice* s = (oslice*)a;
				oarray* n = new oarray();
				int start = (s->start->returnType==intClass)?(((oint*)s->start)->value):0;
				int stop = (s->stop->returnType==intClass)?(((oint*)s->stop)->value):data.size();
				int step = (s->step->returnType==intClass)?(((oint*)s->step)->value):1;
				while(start<0) start+=data.size();
				while(stop<0) start+=data.size();
				for(int i = start; (step>0)?(i<stop):(i>stop); i+=step)
					n->data.push_back(data[i]);
				return n;
			}
			else{
				cerr << "Array index must be int or slice not " << a << " of class " << a->returnType;
				exit(0);
			}
		}*/
		operator String () const override{
			std::stringstream ss;
			ss << "[";
			if(data.size()>0) {
				for(unsigned int i = 0; i<data.size(); ++i){
					ss << data[i];
					if(i<data.size()-1) ss << ", ";
				}
			}
			ss << "]";
			return ss.str();
		}
};

class E_ARR : public Expression{
	public:
		std::vector<Expression*> values;
		E_ARR() : Expression(arrayClass),values() { };
		E_ARR(const std::vector<Expression*>& a) : Expression(arrayClass),values(a) { };
		const  Token getToken() const override{
			return T_ARR;
		};
		void write(ostream& f,String a="") const override{
			f << "[";
			for(unsigned int i = 0; i<values.size(); ++i){
				f << values[i];
				if(i<values.size()-1) f << ", ";
			}
			f << "]";
		}
		Value* evaluate(RData& a) override {
			//TODO
			todo("E_ARR not implemented");
			/*
			oarray* n = new oarray();
			for(Expression* a: values){
				n->data.push_back(a->evaluate(a));
			}
			return n;*/
		}

		Expression* simplify() override{

			std::vector<oobject*> vals;
			for(Expression* a: values){
				Expression* t = a->simplify();
				if(t->getToken()==T_OOBJECT)
					vals.push_back((oobject*)a);
				else{
					std::vector<Expression*> val2;
					for(Expression* b: values)
						val2.push_back(b->simplify());
					return new E_ARR(val2);
				}
			}
			return new oarray(vals);
		}

		void checkTypes() override{
			for(auto& a:values){
				a->checkTypes();
			}
			todo("Implement forced E_ARR typing");
		}
};

#endif /* OARRAY_HPP_ */
