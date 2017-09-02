/*
 *  Supporto_ricerca.cc
 */

#include "supporto_ricerca.h"

namespace Joker
{

//-----------------------------------------------------------------------------
//  Clear:
//-----------------------------------------------------------------------------
void supporto_ricerca::clear(const situazione *const s)
{
  for (unsigned i(0); i <= massima_profondita; i++)
    ce[i] = ambiente_analisi();
  
  ce->attr = s->sotto_scacco() ? attributo::scacco : attributo::nessuno;
  ce->fase =  0;
  p        = ce;
}

//-----------------------------------------------------------------------------
//  Controlla:
//-----------------------------------------------------------------------------
bool supporto_ricerca::controlla() const
{
  return ce <= p && p <= ce+massima_profondita && 
         ply() <= massima_profondita && lrip.controlla();
}

}  // Namespace Joker.
