/*
 * DATA.hpp
 *
 *  Created on: Nov 21, 2013
 *      Author: wmoses
 */

#ifndef DATA_HPP_
#define DATA_HPP_
#include "../includes.hpp"
#include "../statement/Statement.hpp"
#include "../location/Location.hpp"

#define DATA_C_
class Data: public Statement{
public:
	const DataType type;
		Data(const DataType t):type(t){}
	public:
		virtual ~Data(){};
		inline const Data* evaluate(RData& r) const override final{
			return this;
		}
		virtual const Data* toValue(RData& r, PositionID id) const=0;
		virtual const Data* castTo(RData& r, const AbstractClass* const right, PositionID id) const=0;
		virtual const AbstractClass* getReturnType() const=0;
		virtual llvm::Value* getValue(RData& r, PositionID id) const=0;
		virtual llvm::Value* castToV(RData& r, const AbstractClass* const right, PositionID id) const=0;
		llvm::Value* evalV(RData& r,PositionID id) const override final{
			return getValue(r,id);
		}
		void reset() const override final{}
		virtual const Data* callFunction(RData& r, PositionID id, const std::vector<const Evaluatable*>& args, const Data* instance) const =0;
		/**
		 * Returns the class that this represents, if it represents a class
		 */
		void collectReturns(std::vector<const AbstractClass*>& vals, const AbstractClass* const toBe) override final{

		}
		void registerClasses() const override final{};
		void registerFunctionPrototype(RData& r) const override final{};
		void buildFunction(RData& r) const override final{};
		const Token getToken() const override{ return T_MAP; };
};

#endif /* DATA_HPP_ */
