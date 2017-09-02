/*
 *  Pezzo.cc
 */

#include "pezzo.h"

namespace Joker
{

bitboard mosse[7][64];

namespace pezzi
{

//-----------------------------------------------------------------------------
//  Calcola_array:
//-----------------------------------------------------------------------------
void calcola_array()
{
  // Mosse di Pedone.
  colore c(false);
  do
    for (posizione pos(a1); !pos.fuori(); ++pos)
    {
      mosse[c][pos].clear();

      if (c==nero)
      {
        if (pos > h1)
        {
          if (pos.colonna() != colA)
            mosse[c][pos].aggiungi(pos+b_s);
          if (pos.colonna() != colH)
            mosse[c][pos].aggiungi(pos+b_d);
        }
      }
      else
        if (pos < a8)
        {
          if (pos.colonna() != colA)
            mosse[c][pos].aggiungi(pos+a_s);
          if (pos.colonna() != colH)
            mosse[c][pos].aggiungi(pos+a_d);
        }
    }
  while (c^=1);

  // Mosse dei pezzi.
  for (pezzo pz(cavallo); pz <= re; ++pz)
    for (posizione pos(a1); !pos.fuori(); ++pos)
    {
      mosse[pz][pos].clear();

      for (const direzione *d = dir[pz]; *d; d++)
        for (posizione arrivo(pos + *d); !arrivo.fuori(); arrivo += *d)
        {
          mosse[pz][pos].aggiungi(arrivo);

          if (pz == cavallo || pz == re)
            break;
        }
    }
}

}  // Namespace Pezzi.

}  // Namespace Joker.
