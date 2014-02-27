/*
 * ComplexClassP.hpp
 *
 *  Created on: Jan 29, 2014
 *      Author: Billy
 */

#ifndef COMPLEXCLASSP_HPP_
#define COMPLEXCLASSP_HPP_
#include "./ComplexClass.hpp"
#include "../../data/literal/ComplexLiteral.hpp"
#include "../../data/ConstantData.hpp"
#include "../../data/LocationData.hpp"

const Data* ComplexClass::getLocalData(RData& r, PositionID id, String s, const Data* instance) const{
	if(s!="real" && s!="imag"){
		illegalLocal(id,s);
		exit(1);
	}
	assert(instance->type==R_COMPLEX || instance->type==R_LOC || instance->type==R_CONST);
	assert(instance->getReturnType()==this);
	if(instance->type==R_COMPLEX){
		ComplexLiteral* cl = (ComplexLiteral*)instance;
		auto a = (s=="real")?cl->real : cl->imag;
//if(cl->complexType)
			return a->castTo(r, this, id);
//	else return a;
	} else if(instance->type==R_CONST){
		Value* v = ((ConstantData*)instance)->value;
		return new ConstantData(r.builder.CreateExtractElement(v,(s=="real")?(0):(1)),this);

	} else {
		assert(instance->type==R_LOC);
		id.compilerError("Location framework not complete -- complex");
	}
	exit(1);
}


#endif /* COMPLEXCLASSP_HPP_ */
