/*
 *  Statistico.h
 */

#if !defined(STATISTICO_H)
#define      STATISTICO_H

#include "configurazione.h"
#include "interfaccia.h"
#include "secondi.h"

namespace Joker
{

class statistico
{
public:
  // Notare che a costanti negative corrispondono entita' da calcolare, a
  // costanti positive entita' memorizzate. Si potrebbero utilizzare solo
  // costanti positive ma si sprecherebbe spazio nell'array di memorizzazione.
  enum info {nps=-2, nodi=-1, 
             iterazione,
             anomalie, estensioni, nodi_quiescenza, nodi_ricerca, hash_hit,
             sup_info};

  enum {percentuale=-2, trascorso=-1,
        inizio, fine, preventivato, 
        sup_tempo};
  
  statistico(secondi = 1.0);

  template<int I> secondi tempo() const;
  template<info I> unsigned long get() const;

protected:
  void aumenta_tempo(double);
  template<info I> void inc();
  
  void interrompi(bool);
  bool interrompi() const;
  void controllo_interruzione();
  
  void clear(secondi);

  bool controlla() const;

private:
  secondi a_tempo[sup_tempo];
 
  unsigned long a_info[sup_info];
  
  bool check;
  bool  stop;
};

//-----------------------------------------------------------------------------
//  Statistico: 
//-----------------------------------------------------------------------------
inline
statistico::statistico(secondi s)
{
  clear(s);
}

//-----------------------------------------------------------------------------
//  Clear: reinizializza i campi della classe statistico. Notare il numero 
//         di nodi fra controlli inizialmente triplo rispetto al normale.
//-----------------------------------------------------------------------------
inline
void
statistico::clear(secondi s)
{
  for (unsigned i(0); i < sup_info; i++)
    a_info[i] = 0;

  a_tempo[      inizio] =        read_clock();
  a_tempo[        fine] = a_tempo[inizio] + s;  
  a_tempo[preventivato] =                   s;

  check =  true;
  stop  = false;
  
  assert(controlla());
}

//-----------------------------------------------------------------------------
//  Tempo:
//-----------------------------------------------------------------------------
template<int I>
inline
secondi
statistico::tempo() const
{
  assert(0 <= I && I < sup_tempo);
  return a_tempo[I];
}
template<>
inline
secondi
statistico::tempo<statistico::trascorso>() const
{
  return read_clock()-tempo<inizio>();
}
template<>
inline
secondi
statistico::tempo<statistico::percentuale>() const
{
  return tempo<trascorso>()/tempo<preventivato>();
}

//-----------------------------------------------------------------------------
//  Aumenta_tempo: incrementa il tempo di riflessione (impostato mediante
//                 costruttore o funzione clear) in base al fattore
//                 moltiplicativo mul.
//-----------------------------------------------------------------------------
inline
void
statistico::aumenta_tempo(double mul)
{
  const secondi assegnato(tempo<preventivato>()*mul);
  a_tempo[fine] = tempo<inizio>() + assegnato;
}

//-----------------------------------------------------------------------------
//  Get:
//-----------------------------------------------------------------------------
template<statistico::info I>
inline
unsigned long
statistico::get() const
{
  return a_info[I];
}
template<>
inline
unsigned long
statistico::get<statistico::nodi>() const
{
  return get<nodi_quiescenza>()+get<nodi_ricerca>();
}
template<>
inline
unsigned long
statistico::get<statistico::nps>() const
{
  secondi s( read_clock() < tempo<fine>() ? tempo<trascorso>() 
                                          : tempo<fine>()-tempo<inizio>() );

  if (s < read_clock_resolution)
    s = read_clock_resolution;

  return static_cast<unsigned long>(get<nodi>()/s);
}

//-----------------------------------------------------------------------------
//  Inc:
//-----------------------------------------------------------------------------
template<statistico::info I>
inline
void
statistico::inc()
{
  ++a_info[I];
}

//-----------------------------------------------------------------------------
//  Interrompi:
//-----------------------------------------------------------------------------
inline
bool
statistico::interrompi() const
{
  return stop;
}

inline
void
statistico::interrompi(bool v)
{
  check = !v;
}

//-----------------------------------------------------------------------------
//  Controllo_interruzione:
//-----------------------------------------------------------------------------
inline
void
statistico::controllo_interruzione()
{
  if (!stop && check)
    stop = read_clock() >= tempo<fine>() || 
           interfaccia::instance().input_disponibile();
}

//-----------------------------------------------------------------------------
//  Controlla:
//-----------------------------------------------------------------------------
inline
bool
statistico::controlla() const
{
  return tempo<inizio>() < tempo<fine>();
}

}  // Namespace Joker.

#endif  // STATISTICO_H
