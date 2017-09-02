/*
 *  Euristica_storica.h
 */

#if !defined(EURISTICA_STORICA_H)
#define      EURISTICA_STORICA_H

#include "configurazione.h"
#include "dettaglio.h"
#include "mossa.h"

namespace Joker
{

template<unsigned MP>
class euristica_storica
{
public:
  typedef unsigned merito;

  euristica_storica();

  merito operator[](const mossa &) const;
  template<bool L> mossa killer(unsigned) const;

  void incrementa(bool, unsigned, dettaglio, const mossa_e &);
  
  void clear();

  bool controlla() const;

private:
  merito statistica[64][64];

  mossa killers[MP+1][2];
};

//-----------------------------------------------------------------------------
//  Euristica_storica:
//-----------------------------------------------------------------------------
template<unsigned MP>
inline
euristica_storica<MP>::euristica_storica()
{
  clear(); 
}

//-----------------------------------------------------------------------------
//  Operator[]:
//-----------------------------------------------------------------------------
template<unsigned MP>
inline
euristica_storica<MP>::merito 
euristica_storica<MP>::operator[](const mossa &m) const
{
  assert( !(m.da().fuori() || m.a().fuori()) );

  return statistica[m.da()][m.a()];
}

//-----------------------------------------------------------------------------
//  Killer:
//-----------------------------------------------------------------------------
template<unsigned MP>
template<bool L>
inline
mossa
euristica_storica<MP>::killer(unsigned ply) const
{
  assert(ply <= MP);

  return killers[ply][L];
}

//-----------------------------------------------------------------------------
//  Incrementa:
//-----------------------------------------------------------------------------
template<unsigned MP>
inline
void
euristica_storica<MP>::incrementa(bool scacco, unsigned ply, dettaglio d,
                                  const mossa_e &m)
{
  assert(ply <= MP);

  // Dato che l'euristica storica viene utilizzata solo nell'aggiornamento di
  // mosse che non sono catture o promozioni, e' inutile considerare detti tipi
  // di mosse per l'aggiornamento.
  if ( m.tipo_mossa::e<(cattura|promozione)>() )
    return;

  // Altre tecniche di aggiornamento sono:
  // - somma di d   <-->    TSCP;
  // - somma di 2^d <--> Phoenix.
  // Questa variante e' usata in Crafty.
  statistica[m.da()][m.a()] += (d*d)/(unita*unita);

  // Aggiornamento mosse killer.
  if (killers[ply][0] != m)
  {
    killers[ply][1] = killers[ply][0];
    killers[ply][0] =               m;
  }
}

//-----------------------------------------------------------------------------
//  Clear:
//-----------------------------------------------------------------------------
template<unsigned MP>
inline
void
euristica_storica<MP>::clear()
{
  for (size_t i(0); i < 64; i++)
    for (size_t j(0); j < 64; j++)
      statistica[i][j] = 0;

  for (size_t i(0); i <= MP; i++)
    killers[i][0] = killers[i][1] = mossa::sentinella;
}

//-----------------------------------------------------------------------------
//  Controlla:
//-----------------------------------------------------------------------------
template<unsigned MP>
inline
bool
euristica_storica<MP>::controlla() const
{ 
  return true; 
}

}  // Namespace Joker.

#endif  // EURISTICA_STORICA_H
