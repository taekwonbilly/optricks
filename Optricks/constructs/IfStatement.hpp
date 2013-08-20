/*
 * IfStatement.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef IFSTATEMENT_HPP_
#define IFSTATEMENT_HPP_

#include "./Expression.hpp"
#include "./Block.hpp"
#include "../containers/settings.hpp"

class IfStatement : public Statement{
	public:
		std::vector<std::pair<Expression*,Statement*>> condition;
		Statement* const finalElse;
		IfStatement(std::vector<std::pair<Expression*,Statement*>> & cond, Statement* const stat) : condition(cond), finalElse(stat){
			/*for(const auto& a:condition)
			if(a.first->returnType!=boolClass){
				cerr << "Cannot make non-bool type argument of conditional" << endl << flush;
				exit(0);
			}*/
			if(condition.size()<1){
				cerr << "Cannot make if statement with no conditions";
				exit(0);
			}
		}
		const Token getToken() const override {
			return T_IF;
		}
		void checkTypes(){
			for(auto& a:condition){
				a.first->checkTypes();
				if(a.first->returnType!=boolClass) todo("Cannot have non-bool as condition for if ",a.first->returnType->name);
				a.second->checkTypes();
			}
			finalElse->checkTypes();
		}
		Value* evaluate(RData& r) override{
			todo("If statement eval not implemented");
			/*
			for(auto &a: condition){
				if((bool) (a.first->evaluate())){
					a.second->evaluate(jump);
					return VOID;
				}
			}
			finalElse->evaluate(jump);
			return VOID;
			*/
		}
		Statement* simplify(Jump& jump) override{
			std::vector<std::pair<Expression*,Statement*>> stack;
			for(auto &a: condition){
				Expression* sim = a.first->simplify();
				if(sim->getToken()==T_OOBJECT){
					if((bool) ((obool*)sim) ){
						return a.second->simplify(jump);
					} else{

					}
				}
				else stack.push_back(std::pair<Expression*,Statement*>(sim,a.second->simplify(jump)));
			}
			if(stack.size()==0){
				return finalElse->simplify(jump);
			}
			else{
				return new IfStatement(stack,finalElse->simplify(jump));
			}
		}
		void write(ostream& a,String t) const override{
			a << "if " << condition[0].first << " ";
			condition[0].second->write(a,t);
			a << endl;
			for(unsigned int i = 1; i<condition.size();++i){
				a << t << "else if(" << condition[i].first << ")";
				condition[i].second->write(a,t);
				if(finalElse->getToken()==T_VOID || i<condition.size()-1)
				a << endl;
			}
			if(finalElse->getToken()!=T_VOID){
				a << t << "else " << finalElse;
			}
		}
};


#endif /* IFSTATEMENT_HPP_ */
