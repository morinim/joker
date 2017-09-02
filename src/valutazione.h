/*
 *  Valutazione.h
 */

#if !defined(VALUTAZIONE_H)
#define      VALUTAZIONE_H

#include <cassert>
#include <iostream>
#include "configurazione.h"
#include "colore.h"

namespace Joker
{

// Infinito e matto.
const int inf(30000);
const int matto(inf-1000);
const int max_matto(matto-1000);

class valutazione
{
public:
  valutazione();
  valutazione(int);

  valutazione &operator+=(valutazione);
  valutazione &operator-=(valutazione);
  valutazione &operator*=(valutazione);
  valutazione operator-() const;

  operator int() const;
  valutazione relativa(colore) const;

  bool e_limitato() const;
  bool e_matto_subito() const;
  bool e_matto_inflitto() const;
  bool e_matto() const;
  unsigned matto_in() const;
  bool e_patta() const;

  bool controlla() const;

  friend std::ostream &operator<<(std::ostream &, valutazione);
  friend std::istream &operator>>(std::istream &, valutazione &);

private:
  int v;
};

std::ostream &operator<<(std::ostream &, valutazione);
std::istream &operator>>(std::istream &, valutazione &);



//-----------------------------------------------------------------------------
//  Valutazione:
//-----------------------------------------------------------------------------
inline
valutazione::valutazione()
{
}

inline
valutazione::valutazione(int v1) 
  : v(v1)
{ 
  assert(controlla()); 
}

//-----------------------------------------------------------------------------
//  Operator+=:
//-----------------------------------------------------------------------------
inline
valutazione &
valutazione::operator+=(valutazione v1)
{
  v += v1.v;
  return *this;
}

//-----------------------------------------------------------------------------
//  Operator-=:
//-----------------------------------------------------------------------------
inline
valutazione &
valutazione::operator-=(valutazione v1)
{
  v -= v1.v;
  return *this; 
}

//-----------------------------------------------------------------------------
//  Operator*=:
//-----------------------------------------------------------------------------
inline
valutazione &
valutazione::operator*=(valutazione v1)
{
  v *= v1.v;
  return *this; 
}

//-----------------------------------------------------------------------------
//  Operator-:
//-----------------------------------------------------------------------------
inline
valutazione
valutazione::operator-() const
{
  return valutazione(-v); 
}

//-----------------------------------------------------------------------------
//  Operator int:
//-----------------------------------------------------------------------------
inline
valutazione::operator int() const
{
  assert(controlla());
  return v; 
}

//-----------------------------------------------------------------------------
//  Relativa:
//-----------------------------------------------------------------------------
inline
valutazione
valutazione::relativa(colore c) const
{
  return c ? *this : -(*this);
}

//-----------------------------------------------------------------------------
//  E_limitato:
//-----------------------------------------------------------------------------
inline
bool
valutazione::e_limitato() const
{
  return abs(v) < max_matto;
}

//-----------------------------------------------------------------------------
//  E_matto_subito:
//-----------------------------------------------------------------------------
inline
bool
valutazione::e_matto_subito() const
{
  return v <= -max_matto; 
}

//-----------------------------------------------------------------------------
//  E_matto_inflitto:
//-----------------------------------------------------------------------------
inline
bool
valutazione::e_matto_inflitto() const
{
  return v >= max_matto; 
}

//-----------------------------------------------------------------------------
//  E_matto:
//-----------------------------------------------------------------------------
inline
bool
valutazione::e_matto() const
{
  return abs(v) >= max_matto;
}

//-----------------------------------------------------------------------------
//  Matto_in:
//-----------------------------------------------------------------------------
inline
unsigned
valutazione::matto_in() const
{ 
  assert(e_matto());

  return e_matto_subito() ? v+matto : matto-v;
}

//-----------------------------------------------------------------------------
//  E_patta:
//-----------------------------------------------------------------------------
inline
bool
valutazione::e_patta() const
{
  return v==0;
}

}  // Namespace Joker.

#endif  // VALUTAZIONE_H
