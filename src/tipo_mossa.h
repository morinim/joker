/*
 *  Tipo_mossa.h
 */

#if !defined(TIPO_MOSSA_H)
#define      TIPO_MOSSA_H

#include <cassert>
#include "configurazione.h"
#include "pezzo.h"

namespace Joker
{

const unsigned normale    =   0;
const unsigned cattura    =   1;
const unsigned arrocco    =   2;
const unsigned en_passant =   4;
const unsigned doppio_p   =   8;
const unsigned di_pedone  =  16;
const unsigned p_donna    =  32;
const unsigned p_torre    =  64;
const unsigned p_alfiere  = 128;
const unsigned p_cavallo  = 256;

const unsigned cattura_normale =                  cattura|en_passant;
const unsigned irreversibile   =                   cattura|di_pedone;
const unsigned promozione      = p_donna|p_torre|p_alfiere|p_cavallo;

class tipo_mossa
{
public:
  tipo_mossa() {};
  tipo_mossa(unsigned);
  
  bool operator==(const tipo_mossa &) const;
  
  template<unsigned T> unsigned e() const;

  pezzo promozione_a() const;

  operator unsigned() const { return t; };

  bool controlla() const;

private:
  unsigned t;
};

//-----------------------------------------------------------------------------
//  Tipo_mossa:
//-----------------------------------------------------------------------------
inline tipo_mossa::tipo_mossa(unsigned t1) : t(t1)
{
  assert(t1 <= (normale|cattura|arrocco|en_passant|doppio_p|
                di_pedone|p_donna|p_torre|p_alfiere|p_cavallo));
}

//-----------------------------------------------------------------------------
//  E:
//-----------------------------------------------------------------------------
template<unsigned T>
inline unsigned tipo_mossa::e() const
{
  return t & T;
}
template<>
inline unsigned tipo_mossa::e<normale>() const
{
  return !t;
}
template<>
inline unsigned tipo_mossa::e<cattura_normale>() const
{
  return (t & cattura_normale) == cattura;
}

//-----------------------------------------------------------------------------
//  Operator==:
//-----------------------------------------------------------------------------
inline bool tipo_mossa::operator==(const tipo_mossa &t1) const
{
  return t == t1.t;
}

//-----------------------------------------------------------------------------
//  Promozione_a:
//-----------------------------------------------------------------------------
inline pezzo tipo_mossa::promozione_a() const
{
  assert(e<promozione>());

  if (e<p_donna>())
    return donna;
  if (e<p_torre>())
    return torre;
  return e<p_alfiere>() ? alfiere : cavallo;
}

//-----------------------------------------------------------------------------
//  Controlla:
//-----------------------------------------------------------------------------
inline bool tipo_mossa::controlla() const
{
  if ( (t&arrocco) && (t&(cattura|doppio_p|en_passant|di_pedone|promozione)) )
    return false;

  if ( (t&cattura) && (t&doppio_p) )
    return false;

  if ( (t&doppio_p) && (t&(en_passant|promozione)) )
    return false;

  if ( (t&en_passant) && (t&(promozione)) )
    return false;

  return true;
}

}  // Namespace Joker.

#endif  // TIPO_MOSSA_H
