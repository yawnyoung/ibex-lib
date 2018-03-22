//============================================================================
//                                  I B E X                                   
// File        : ibex_CtcIdentity.cpp
// Author      : Gilles Chabert
// Copyright   : IMT Atlantique (France)
// License     : See the LICENSE file
// Created     : Feb 18, 2013
// Last Update : Jul 18, 2013
//============================================================================

#include "ibex_CtcIdentity.h"

namespace ibex {

CtcIdentity::CtcIdentity(int n) : Ctc(n) { }

void CtcIdentity::contract(IntervalVector& box) {
	set_flag(FIXPOINT);
	set_flag(INACTIVE);
}

} // end namespace ibex
