// Copyright (c) 2014 Denis Maua
// All Rights Reserved.
//
// This file is part of MSP library
//
// MSP is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MSP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with MSP.  If not, see <http://www.gnu.org/licenses/>.

// This file implements the Factor class

#include "Factor.h"
#include <vector>


namespace msp {


  unsigned Factor::_num_instances = 0;

  /** Constructor from two factors */
  Factor::Factor(const Factor& f1, const Factor& f2)
  {
    _id = _num_instances++;
    _domain=union_of(f1._domain, f2._domain);  // domain is now union of domains of f1 and f2
    _values.resize(_domain->size(),MSP_DEFAULT_VALUE); // resize vector of values to match new domain's size
  }

  /** Constructor from two factors and a variables*/
  Factor::Factor(const Factor& f1, const Factor& f2, const Variable& var)
  {
    _id = _num_instances++;
    _domain=union_of(f1._domain,f2._domain,&var);
    _values.resize(_domain->size(),MSP_DEFAULT_VALUE);
  }

  /** Default printing. */
  std::ostream& operator<<(std::ostream &o, const Factor &f) {
    o << "Factor(" << *f._domain << ", values:[";
    if (f.size() <= MSP_DISP_MAX_VALS)
      for (unsigned int i=0; i < f.size(); ++i) { o << f[i]; if (i < f.size()-1) o << ", "; }
    else 
      {
      for (unsigned int i=0; i < MSP_DISP_MAX_VALS-1; ++i) o << f[i] << ", "; 
      o << "..., " << f[f.size()-1];
      }
    o << "])";
    return o;
  }

  /** Default printing. */
  std::ostream& operator<<(std::ostream &o, Factor &f) {
    o << "Factor(" << *f._domain << ", values:[";
    if (f.size() <= MSP_DISP_MAX_VALS)
      for (unsigned int i=0; i < f.size(); ++i) { o << f[i]; if (i < f.size()-1) o << ", "; }
    else 
      {
      for (unsigned int i=0; i < MSP_DISP_MAX_VALS-1; ++i) o << f[i] << ", "; 
      o << "..., " << f[f.size()-1];
      }
    o << "])";
    return o;
  }

}
