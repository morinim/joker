/*
 *  Mossa.h
 */

#if !defined(MOSSA_H)
#define      MOSSA_H

#include <cassert>
#include <iostream>
#include "configurazione.h"
#include "tipo_mossa.h"

namespace Joker
{

class mossa
{
public:
  static const mossa sentinella;

  mossa();
  mossa(posizione, posizione);
  explicit mossa(const char []);

  bool operator!() const;
  bool operator==(mossa) const;
  bool operator!=(mossa) const;

  posizione  a() const;
  posizione da() const;
  // La funzione e' corretta solo con mosse di tipo en_passant.
  posizione preda_en_passant() const;

  bool controlla() const;

private:
  unsigned m;
};

//  Function prototypes per funzioni friend.
std::ostream &operator<<(std::ostream &, mossa);



//-----------------------------------------------------------------------------
//  Mossa:
//-----------------------------------------------------------------------------
inline
mossa::mossa()
{
}

inline
mossa::mossa(posizione da, posizione a) 
  : m((da << 6) | a)
{ 
  assert(controlla());
}

//-----------------------------------------------------------------------------
//  Operator!:
//-----------------------------------------------------------------------------
inline
bool
mossa::operator!() const
{ 
  return !m; 
}

//-----------------------------------------------------------------------------
//  Operator==:
//-----------------------------------------------------------------------------
inline
bool
mossa::operator==(mossa m1) const
{ 
  return m == m1.m; 
}

//-----------------------------------------------------------------------------
//  Operator!=:
//-----------------------------------------------------------------------------
inline
bool
mossa::operator!=(mossa m1) const
{ 
  return m != m1.m; 
}

//-----------------------------------------------------------------------------
//  A:
//-----------------------------------------------------------------------------
inline
posizione
mossa::a() const
{
  return posizione(m & 0x3Fu);
}
 
//-----------------------------------------------------------------------------
//  Da:
//-----------------------------------------------------------------------------
inline
posizione
mossa::da() const
{
  return posizione(m >> 6);
}

//-----------------------------------------------------------------------------
//  Preda_en_passant: LA FUNZIONE E' CORRETTA SOLO CON MOSSE DI TIPO EN 
//                    PASSANT.
//-----------------------------------------------------------------------------
inline
posizione
mossa::preda_en_passant() const
{
  return a()<da() ? a()+alto : a()+basso;
}

//-----------------------------------------------------------------------------
//  Controlla:
//-----------------------------------------------------------------------------
inline
bool
mossa::controlla() const
{ 
  return !m || (!da().fuori() && !a().fuori() && da() != a());
}


                                       
class mossa_e : public mossa, public tipo_mossa
{
public:
  mossa_e() {};
  mossa_e(posizione, posizione, pezzo, pezzo, tipo_mossa);
  mossa_e(mossa, pezzo, pezzo, tipo_mossa);

  bool operator==(const mossa_e &) const;
  bool operator!=(const mossa_e &m1) const { return !(*this == m1); };

  pezzo mosso() const;
  pezzo preda() const;

  std::string str() const;

  bool controlla() const;

private:
  pezzo pm;
  pezzo pp;
};



//-----------------------------------------------------------------------------
//  Mossa_e:
//-----------------------------------------------------------------------------
inline
mossa_e::mossa_e(posizione da, posizione a, pezzo pm1, pezzo pp1,
                 tipo_mossa tm) 
  : mossa(da,a), tipo_mossa(tm), pm(pm1), pp(pp1)
{
  assert(controlla());
}

inline
mossa_e::mossa_e(mossa m, pezzo pm1, pezzo pp1, tipo_mossa tm)
  : mossa(m), tipo_mossa(tm), pm(pm1), pp(pp1)
{
  assert(controlla());
}

//-----------------------------------------------------------------------------
//  Operator==:
//-----------------------------------------------------------------------------
inline
bool
mossa_e::operator==(const mossa_e &m1) const
{ 
  return tipo_mossa::operator==(m1) && mossa::operator==(m1) && 
         mosso() == m1.mosso() && preda() == m1.preda();
}

//-----------------------------------------------------------------------------
//  Mosso
//-----------------------------------------------------------------------------
inline
pezzo
mossa_e::mosso() const
{
  return pm;
}

//-----------------------------------------------------------------------------
//  Preda:
//-----------------------------------------------------------------------------
inline
pezzo
mossa_e::preda() const
{
  return pp;
}

}  // Namespace Joker.

#endif  // MOSSA_H
