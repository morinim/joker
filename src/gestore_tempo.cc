/*
 *  Gestore_tempo.cc
 */

#include "gestore_tempo.h"

namespace Joker
{

const double gestore_tempo::margine_sicurezza_percentuale = 0.05;
const secondi Joker::gestore_tempo::tempo_minimo          = 0.20;



//-----------------------------------------------------------------------------
//  Gestore_tempo:
//-----------------------------------------------------------------------------
gestore_tempo::gestore_tempo(const std::string &m, const std::string &t, 
                             const std::string &i) throw(Joker::gestore_tempo::eccezioni)
  : primo(atoi(m.c_str()),secondi(t)*(1.0-margine_sicurezza_percentuale)),
    secondo(-1,-1.0), 
    incremento(secondi(i)),
    residuo(primo)
{ 
  assert(controlla());
}

gestore_tempo::gestore_tempo(secondi s)
  : primo(1,s*(1.0-margine_sicurezza_percentuale)),
    secondo(1,s*(1.0-margine_sicurezza_percentuale)),
    incremento(0),
    residuo(primo)
{ 
  assert(controlla());
}

//-----------------------------------------------------------------------------
//  Aggiorna:
//-----------------------------------------------------------------------------
void
gestore_tempo::aggiorna(secondi usato)
{
  residuo.aggiorna(usato-incremento);

  if (!residuo.mosse())
    residuo += (secondo.tempo() > 0.0) ? secondo : primo;
}

//-----------------------------------------------------------------------------
//  Tempo_calcolo:
//-----------------------------------------------------------------------------
secondi
gestore_tempo::tempo_calcolo() const
{
  const double overhead(.10);
  const secondi riflessione((1.0-overhead) * residuo.ritmo());

  return riflessione > 5*tempo_minimo ? riflessione : tempo_minimo;
}

//-----------------------------------------------------------------------------
//  Controlla:
//-----------------------------------------------------------------------------
bool
gestore_tempo::controlla() const
{
  return primo.controlla() && secondo.controlla() && incremento.controlla() &&
         residuo.controlla();
}

}  // Namespace Joker.
