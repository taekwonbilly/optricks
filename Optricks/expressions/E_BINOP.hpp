/*
 * E_INDEXER.hpp
 *
 *  Created on: Apr 12, 2013
 *      Author: wmoses
 */

#ifndef E_INDEXER_HPP_
#define E_INDEXER_HPP_
#include "../constructs/Statement.hpp"

byte precedence(String tmp){
	if (tmp == "." || tmp==":" || tmp=="::" || tmp == "->"){
		return 0;
	}
	else if(tmp=="++" || tmp == "--" || tmp=="%%"){
		return 1;
	}
	else if(tmp==".*" || tmp==":*" || tmp=="::*"||tmp=="->*" || tmp=="=>*"){
		return 2;
	}
	else if (tmp == "**" || tmp=="^"){
		return 3;
	}
	else if (tmp == "%" || tmp == "*" || tmp == "/" || tmp == "//"){
		return 4;
	}
	else if (tmp == "+" || tmp == "-"){
		return 5;
	}
	else if(tmp=="<<" || tmp == ">>"){
		return 6;
	}
	else if (tmp == "<=" || tmp == "<" || tmp == ">" || tmp == ">="){
		return 7;
	}
	else if (tmp=="==" || tmp=="===" || tmp == "!=" || tmp=="!=="){
		return 8;
	}
	else if (tmp == "&"){
		return 9;
	}
	else if (tmp == "^^"){
		return 10;
	}
	else if (tmp == "|"){
		return 11;
	}
	else if (tmp == "&&"){
		return 12;
	}
	else if (tmp == "||"){
		return 13;
	}

	else if(tmp == "\\."){
		return 14;
	}
	else if (tmp == "+=" || tmp == "%="
			|| tmp == "-=" || tmp == "*="
					|| tmp=="\\.=" || tmp == "\\=" || tmp=="/=" || tmp=="//="
							|| tmp == "**=" || tmp=="^=" || tmp=="|=" || tmp=="||="
									|| tmp=="&=" || tmp=="&&=" || tmp=="^^=" || tmp=="="
											|| tmp == ":=" || tmp == "::=" ||tmp == "<<=" || tmp==">>="
													|| tmp=="<<<=" || tmp==">>>="){
		return 15;
	}

	return 255;
}

class E_BINOP : public Statement{
	public:
		Statement* left;
		Statement* right;
		String operation;
		E_BINOP(PositionID a, Statement* t, Statement* ind,String op) :
			Statement(a),left(t), right(ind),operation(op){
		}
		const Token getToken() const override{
			return T_BINOP;
		}
		AllocaInst* getAlloc() override final{ return NULL; };
		Value* evaluate(RData& a) override final{
			//TODO allow short-circuit lookup of E_VAR
			return left->returnType->binops[operation][right->returnType]->apply(
					left->evaluate(a),
					right->evaluate(a),
					a);
		}
		ClassProto* checkTypes() override final{
			left->checkTypes();
			right->checkTypes();
			auto found = left->returnType->binops.find(operation);
			if(found==left->returnType->binops.end())
				error("Binary operator "+operation+" not implemented for class "+
						left->returnType->name+" [with right "+ right->returnType->name+"]");
			auto look = found->second;

			auto found2 = look.find(right->returnType);
			if(found2==look.end())
				error("Binary operator "+operation+" not implemented for class "+
						left->returnType->name+ " with right "+ right->returnType->name);
			return returnType = found2->second->returnType;
		}
		Statement* simplify() override{
			return new E_BINOP(filePos, left->simplify(), right->simplify(), operation);
		}
		void registerClasses(RData& r) override final{
			left->registerClasses(r);
			right->registerClasses(r);
		}
		void registerFunctionArgs(RData& r) override final{
			left->registerFunctionArgs(r);
			right->registerFunctionArgs(r);
		};
		void registerFunctionDefaultArgs() override final{
			left->registerFunctionDefaultArgs();
			right->registerFunctionDefaultArgs();
		};
		void resolvePointers() override final{
			left->resolvePointers();
			right->resolvePointers();
		};
		FunctionProto* getFunctionProto() override final{ return NULL; }
		void write(ostream& f,String s="") const override{
			left->write(f,s);
			if(operation=="[]"){
			f << "[";
			right->write(f,s);
			f << "]";
			}
			else{
			f << " " << operation << " ";
			right->write(f,s);
			}
		}

		//TODO CHECK IF WORKS
		Statement* fixOrderOfOperations(){
			Statement* tl = left;
			Statement* tr = right;
			E_BINOP* self = this;
			while(true){
				if(tl->getToken()==T_BINOP){
					E_BINOP* l = (E_BINOP*)(tl);
					if(operation!="[]" && precedence(l->operation) > precedence(self->operation)){
						self->left = l->right;
						tr = l->right = self;
						self = l;
					}
					else break;
				} else break;
			}

			while(true){
				if(tr->getToken()==T_BINOP){
					E_BINOP* r = (E_BINOP*)(tr);
					if(operation!="[]" && precedence(r->operation) > precedence(self->operation)){
						self->right = r->left;
						tl = r->left = self;
						self = r;
					}
					else break;
				} else break;
			}
			return self;
		}
};


#endif /* E_INDEXER_HPP_ */
