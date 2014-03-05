#ifndef E_PARENS_HPP_
#define E_PARENS_HPP_
#include "../language/statement/Statement.hpp"

class E_PARENS : public Statement{
public:
	Statement* inner;
	virtual ~E_PARENS(){};
	E_PARENS(Statement* t) : Statement(), inner(t) { };
	const Token getToken() const override{
		return T_PARENS;
	};
	void collectReturns(std::vector<const AbstractClass*>& vals, const AbstractClass* const toBe){
		inner->collectReturns(vals, toBe);
	}
	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const override final{
		return inner->getFunctionReturnType(id,args);
	}
	/*inline Resolvable getMetadata() override final{
		return inner->getMetadata();
	}*/
	void registerClasses() const override final{
		inner->registerClasses();
	}
	void registerFunctionPrototype(RData& r) const override final{
		inner->registerFunctionPrototype(r);
	}
	void buildFunction(RData& r) const override final{
		inner->buildFunction(r);
	}
	const Data* evaluate(RData& a) const override {
		return inner->evaluate(a);
	}
	const AbstractClass* getReturnType() const override final{
		return inner->getReturnType();
	}
	AbstractClass* getSelfClass(PositionID id) override final{ return inner->getSelfClass(id); }
};


#endif /* E_PARENS_HPP_ */
