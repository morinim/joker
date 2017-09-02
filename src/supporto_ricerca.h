/*
 *  Supporto_ricerca.h
 */

#if !defined(SUPPORTO_RICERCA_H)
#define      SUPPORTO_RICERCA_H

#include "configurazione.h"
#include "fase_estrazione.h"
#include "lista_mosse.h"
#include "ripetizioni.h"
#include "situazione_valutata.h"
#include "trasposizioni.h"

namespace Joker
{

struct ambiente_analisi
{
  ambiente_analisi(lista_mosse::iterator ma = 0, bool ss = false, 
                   mossa mm = mossa::sentinella)
    : m_attuale(ma), attr(attributo::nessuno), m_migliore(mm),
      fase(f_estrazione(ss))
  { assert(controlla()); };

  bool controlla() const;

  dati_r                        r;
  lista_mosse::iterator m_attuale;
  attributi                  attr;
  mossa                m_migliore;
  unsigned                   fase;
};

//-----------------------------------------------------------------------------
//  Controlla:
//-----------------------------------------------------------------------------
inline bool ambiente_analisi::controlla() const
{
  return m_migliore.controlla();
}



class supporto_ricerca
{
public:
  enum {massima_profondita = 63};

  explicit supporto_ricerca(const situazione *const);

  void muovi(const situazione *const);
  void muovinull();
  void ripristina();
  void ripristinanull();

  ambiente_analisi *cep();
  
  unsigned ply() const;
  bool profondita_massima() const;

  bool patta(const situazione *const);

  void clear(const situazione *const);
  bool controlla() const;
  
private:
  ambiente_analisi ce[massima_profondita+1];
  ambiente_analisi                       *p;

  ripetizioni                          lrip;
};

//-----------------------------------------------------------------------------
//  Supporto_ricerca:
//-----------------------------------------------------------------------------
inline
supporto_ricerca::supporto_ricerca(const situazione *const s)
{
  clear(s);
}

//-----------------------------------------------------------------------------
//  Muovi:
//-----------------------------------------------------------------------------
inline
void
supporto_ricerca::muovi(const situazione *const s)
{
  ++p;
  assert(p <= ce+massima_profondita);

  p->attr = s->sotto_scacco() ? attributo::scacco : attributo::nessuno;
}

//-----------------------------------------------------------------------------
//  Muovinull:
//-----------------------------------------------------------------------------
inline
void
supporto_ricerca::muovinull()
{
  p->m_attuale =     0;

  ++p;
  assert(p <= ce+massima_profondita);

  p->attr = attributo::nessuno;
  // Notare che il giocatore cui si e' ceduta la mossa non puo' essere sotto
  // scacco perche', se cosi' fosse, l'ultima mossa da lui giocata sarebbe
  // stata illegale.
}

//-----------------------------------------------------------------------------
//  Ripristina:
//-----------------------------------------------------------------------------
inline
void
supporto_ricerca::ripristina()
{
  assert(p > ce);
  --p;  
}

//-----------------------------------------------------------------------------
//  Ripristinanull:
//-----------------------------------------------------------------------------
inline
void
supporto_ricerca::ripristinanull()
{
  assert(p > ce);
  --p;
}

//-----------------------------------------------------------------------------
//  Cep:
//-----------------------------------------------------------------------------
inline
ambiente_analisi *
supporto_ricerca::cep()
{
  return p;
}

//-----------------------------------------------------------------------------
//  Ply:
//-----------------------------------------------------------------------------
inline
unsigned
supporto_ricerca::ply() const
{
  return p-ce;
}

//-----------------------------------------------------------------------------
//  Profondita_massima:
//-----------------------------------------------------------------------------
inline
bool
supporto_ricerca::profondita_massima() const
{
  return ply() == massima_profondita;
}

//-----------------------------------------------------------------------------
//  Patta:
//-----------------------------------------------------------------------------
inline
bool
supporto_ricerca::patta(const situazione *const s)
{
  return lrip.patta(s,ply());
}

}  // Namespace Joker.

#endif  // SUPPORTO_RICERCA
