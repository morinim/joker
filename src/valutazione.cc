/*
 *  Valutazione.cc
 */

#include "valutazione.h"

namespace Joker
{

//-----------------------------------------------------------------------------
//  Operator<<:
//-----------------------------------------------------------------------------
std::ostream &
operator<<(std::ostream &o, valutazione v)
{ 
  if (v.e_matto_subito()) 
    return o << "-mat" << v.matto_in(); 
  else if (v.e_matto_inflitto()) 
    return o << "mat" << v.matto_in();
  else return o << v.v;
}

//-----------------------------------------------------------------------------
//  Operator>>:
//-----------------------------------------------------------------------------
std::istream &
operator>>(std::istream &i, valutazione &v)
{
  return i >> v.v;
}

//-----------------------------------------------------------------------------
//  Controlla:
//-----------------------------------------------------------------------------
bool
valutazione::controlla() const
{
  return abs(v) <= inf;
}

}  // Namespace Joker.
