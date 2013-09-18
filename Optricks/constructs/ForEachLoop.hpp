/*
 * ForEachLoop.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef FOREACHLOOP_HPPO_
#define FOREACHLOOP_HPPO_

#include "./Statement.hpp"
//TODO implement iterator
class ForEachLoop : public Statement{
	public:
		E_VAR* localVariable;
		Statement* iterable;
		Statement* toLoop;
		String name;
		ForEachLoop(PositionID id, E_VAR* var, Statement* it,Statement* tL, String n="") :
			Statement(id, voidClass), localVariable(var), iterable(it),toLoop(tL){
			/*if(condition->returnType!=boolClass){
				cerr << "Cannot make non-bool type argument of conditional" << endl << flush;
				exit(0);
			}*/
			name = n;
		}

		ClassProto* checkTypes(){
			iterable->checkTypes();
			toLoop->checkTypes();
			error("Type checking for foreach incomplete due to lack of iterator");
			return returnType;
		}
		const Token getToken() const override {
			return T_FOREACH;
		}
		DATA evaluate(RData& a) override{
			//TODO iterators not implemented yet
			error("For-each loop eval not implented");
/*			while((bool)condition->evaluate()){
				statement->evaluate(jump);
				switch(jump.type){
					case NJUMP:
						break;
					case CONTINUE:
						break;
					case BREAK:
						if(name==jump.label || jump.label=="") jump = NOJUMP;
						return VOID;
					case RETURN:
						return VOID;
				}
			}*/
			return NULL;
		}
		void write(ostream& a, String b="") const override{
			a << "for " << localVariable << " in "<< iterable << ":";
			toLoop->write(a,b+"  ");
		}
		Statement* simplify() override{
			Statement* in = toLoop->simplify();
			//if(jump.type==BREAK && (jump.label=="" || jump.label==name))
			error("Cannot simplify foreach loop");
			//return new ForEachLoop(filePos, localVariable, iterable->simplify(),in,name);
			//TODO [loop unrolloing]
			return NULL;
		}
		FunctionProto* getFunctionProto() override final{ return NULL; }
		void setFunctionProto(FunctionProto* f) override final { error("Cannot set function prototype"); }
		ClassProto* getClassProto() override final{ return NULL; }
		void setClassProto(ClassProto* f) override final { error("Cannot set class prototype"); }
		AllocaInst* getAlloc() override final{ return NULL; };
		void setAlloc(AllocaInst* f) override final { error("Cannot set allocated instance"); }
		String getObjName() override final { error("Cannot get name"); return ""; }
		void setResolve(DATA v) override final { error("Cannot set resolve"); }
		DATA getResolve() override final { error("Cannot get resolve"); return NULL;}
};


#endif /* FOREACHLOOP_HPP_ */
