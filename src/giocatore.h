/*
 *  Giocatore.h
 */

#if !defined(GIOCATORE_H)
#define      GIOCATORE_H

#include "configurazione.h"
#include "euristica_storica.h"
#include "libro_apertura.h"
#include "logger.h"
#include "statistico.h"
#include "supporto_ricerca.h"

namespace Joker
{

struct parametri_ricerca
{
  parametri_ricerca(bool = true,
                    bool = true, 
                    libro_apertura::modalita = libro_apertura::apprendimento,
                    valutazione = ASPIRATION_WINDOW);

  bool controlla() const;

  bool                    estendi_analisi;
  bool                       libro_attivo;
  libro_apertura::modalita libro_modalita;
  valutazione                    finestra;
};

//-----------------------------------------------------------------------------
//  Parametri_ricerca::parametri_ricerca:
//-----------------------------------------------------------------------------
inline parametri_ricerca::parametri_ricerca(bool e, bool a, 
                                            libro_apertura::modalita m,
                                            valutazione f)
  : estendi_analisi(e), libro_attivo(a), libro_modalita(m), finestra(f)
{
}

//-----------------------------------------------------------------------------
//  Parametri_ricerca::controlla:
//-----------------------------------------------------------------------------
inline bool parametri_ricerca::controlla() const
{ 
  return finestra.controlla() && finestra > 0;
}



class giocatore : public statistico
{
public:
  giocatore(situazione_valutata *, const parametri_ricerca &);
  valutazione analizza(secondi, unsigned, const lista_mosse * = 0);

  const lista_mosse &mosse_pv() const;

  bool controlla() const;

private:
  enum {max_stack_m = 4096, max_mosse_radice = 400};
  enum risultato_ricerca {valutazione_esatta, fail_low, fail_high, 
                          tempo_scaduto};

  void clear(secondi);

  bool muovi(const mossa_e *);
  void muovinull();
  void ripristina(const mossa_e *);
  void ripristinanull();

  bool stampa_info() const;
  void stampa_pv(valutazione v, char = 0) const;
  void recupera_variante(const mossa_e &, lista_mosse *);

  static valutazione valutazione_cattura(const mossa_e *);

  bool estrai_mossa(lista_mosse *);
  void sposta_in_testa(lista_mosse::iterator);

  dettaglio nuovo_d(dettaglio);
  valutazione ricerca(valutazione, valutazione, dettaglio, mossa_e *);
  risultato_ricerca ricerca_radice(valutazione, valutazione, bool *);
  valutazione ricerca_standard(valutazione, valutazione, dettaglio, mossa_e *);
  valutazione quiescenza(valutazione, valutazione, mossa_e *);

  static trasposizioni                                    tt;

  mossa_e stack_m[max_stack_m+1],           *const stack_top;

  supporto_ricerca                                        sr;

  euristica_storica<supporto_ricerca::massima_profondita> es;

  lista_mosse                                   mosse_radice;

  lista_mosse                                             pv;

  const parametri_ricerca                                 pr;

  valutazione                                      dm0, val0;
  
  situazione_valutata                                     *s;

  unsigned long                                        delta;
  unsigned long                                  fuori_libro;
};

//-----------------------------------------------------------------------------
//  Mosse_pv:
//-----------------------------------------------------------------------------
inline const lista_mosse &giocatore::mosse_pv() const
{
  return pv;
}

}  // Namespace Joker.

#endif  // GIOCATORE_H
