/*
 *  Dati_r.h
 */

#if !defined(DATI_R_H)
#define      DATI_R_H

#include "configurazione.h"
#include "chiave_hash.h"
#include "info_aus.h"

namespace Joker
{

class dati_r
{
public:
  dati_r();

  template<class T> const T& get() const;

private:
  friend class          situazione;
  friend class situazione_valutata;

  chiave_hash                chiave;
  info_aus                     info;
  valutazione valutazione_materiale;
};

//-----------------------------------------------------------------------------
//  Dati_r:
//-----------------------------------------------------------------------------
inline dati_r::dati_r()
{
}

//-----------------------------------------------------------------------------
//  Get:
//-----------------------------------------------------------------------------
template<>
inline const chiave_hash &dati_r::get<chiave_hash>() const
{
  assert(chiave.controlla());
  return chiave;
}

template<>
inline const info_aus &dati_r::get<info_aus>() const
{
  assert(info.controlla());
  return info;
}

template<>
inline const valutazione &dati_r::get<valutazione>() const
{
  assert(valutazione_materiale.controlla());
  return valutazione_materiale;
}

}  // Namespace Joker.

#endif  // DATI_R_H
