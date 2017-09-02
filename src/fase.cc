/*
 *  Fase.cc
 */

#include "fase.h"

namespace Joker
{

//-----------------------------------------------------------------------------
//  Fase:
//-----------------------------------------------------------------------------
Joker::fase::fase(const situazione &s)
{
  const colore c(s.mossa_a()), xc(!c);
  
  // Si e' in fase di apertura se al giocatore che deve muovere non sono stati
  // catturati piu' di 4 pezzi e almeno un pezzo minore e' ancora nella casa di
  // origine.
  if (s.n_pezzi(c) >= 12)
  {
    if ( c==nero && (s.e_un(nero,b8,cavallo) || 
                     s.e_un(nero,c8,alfiere) ||
                     s.e_un(nero,g8,cavallo) || 
                     s.e_un(nero,f8,alfiere)) )
    {
      f = apertura;
      return;
    }
    if ( c==bianco && (s.e_un(bianco,b1,cavallo) ||
                       s.e_un(bianco,c1,alfiere) ||
                       s.e_un(bianco,g1,cavallo) || 
                       s.e_un(bianco,f1,alfiere)) )
    {
      f = apertura;
      return;
    }
  }

  // Si e' nel mediogioco se all'avversario del giocatore che deve muovere
  // rimane, pedoni esclusi, materiale per un valore superiore ad una donna ed
  // un pezzo minore.
  valutazione v(0);
  for (pezzo pz(donna); pz>pedone && v<=valore[donna]+valore[alfiere]; --pz)
    v += s.mappa(xc,pz).count() * valore[pz];

  f = (v > valore[donna]+valore[alfiere]) ? mediogioco : finale;
}

}  // Namespace Joker.
