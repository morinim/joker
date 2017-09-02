/*
 *  Situazione_valutata.h
 */

#if !defined(SITUAZIONE_VALUTATA_H)
#define      SITUAZIONE_VALUTATA_H

#include "configurazione.h"
#include "componenti_valutazione.h"
#include "e_cache.h"
#include "fase.h"
#include "itos.h"
#include "pesi.h"
#include "situazione.h"

namespace Joker
{

class situazione_valutata : public situazione
{
public:
  situazione_valutata(const situazione & = situazione());  
  
  bool muovi(const mossa_e &, dati_r &);
  void ripristina(const mossa_e &, const dati_r &);

  bool carica(std::istream &, std::string * = 0);

  bool aggiorna_fase_gioco();
  fase fase_gioco() const;

  valutazione valutazione_posizione(valutazione=-inf, valutazione=+inf) const;
  valutazione delta_materiale() const;
  void elementi_di_valutazione(componenti_valutazione &) const;
    
  bool operator==(const situazione_valutata &) const;
  bool operator!=(const situazione_valutata &s1) const {return !(*this==s1);};

  bool controlla() const;

private:
  typedef valutazione (situazione_valutata::*PFV)() const;
  typedef e_cache<E_CACHE_BIT>                     ecache;

  void reset_array();
  template<fase::fs_val> void calcola_array();
  void setta_funzioni_valutazione();

  template<colore C> bitboard case_attaccabili() const;
  template<colore C> bitboard case_forti() const;
  template<colore C> bitboard pedoni_deboli() const;

  bool colonna_aperta(unsigned) const;
  bool fianchi_bloccati(posizione) const;
  bool torre_intrappolata(colore, posizione) const;

  std::string decodifica(const bitboard [2]) const;

  valutazione v_differenza_materiale() const;
  valutazione v_disposizione_pezzi() const;
  valutazione v_sicurezza_re() const;
  valutazione v_struttura_pedonale() const;
  valutazione v_sviluppo() const;
  valutazione valutazione_apertura(valutazione,valutazione) const;
  valutazione valutazione_mediogioco(valutazione,valutazione) const;
  valutazione valutazione_finale(valutazione,valutazione) const;

  static const pesi                    w;
  
  static  ecache                   cache;
  mutable ecache::info             *info;
  mutable bool                   info_ok;

  PFV                            fval[4];
  valutazione                 residuo[4];

  valutazione        bonus_pedone[2][64];
  valutazione       bonus_cavallo[2][64];
  valutazione            bonus_re[2][64];
  valutazione bonus_cavallo_forte[2][64];

  valutazione         alfiere_cattivo[9];
  valutazione    bonus_coppia_passata[8];
  valutazione      cavallo_bloccatore[8];

  valutazione           tropism_donna[8];
  valutazione           tropism_torre[8];
  valutazione         tropism_alfiere[8];
  valutazione         tropism_cavallo[8];

  fase                                fs;
  valutazione                         dm;
};

//-----------------------------------------------------------------------------
//  Muovi:
//-----------------------------------------------------------------------------
inline
bool
situazione_valutata::muovi(const mossa_e &m, dati_r &r)
{
  if (situazione::muovi(m,r))
  {
    const colore c(!mossa_a());

    r.valutazione_materiale = dm;

    if (m.e<cattura>())
      dm += valore[m.preda()].relativa(c);

    if (m.e<promozione>())
      dm += valutazione(valore[m.promozione_a()] - valore[pedone]).relativa(c);

    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
//  Ripristina:
//-----------------------------------------------------------------------------
inline
void
situazione_valutata::ripristina(const mossa_e &m, const dati_r &r)
{
  situazione::ripristina(m,r);

  dm = r.get<valutazione>();
}

//-----------------------------------------------------------------------------
//  Fase_gioco:
//-----------------------------------------------------------------------------
inline
fase
situazione_valutata::fase_gioco() const
{
  return fs; 
}

//-----------------------------------------------------------------------------
//  Delta_materiale:
//-----------------------------------------------------------------------------
inline
valutazione
situazione_valutata::delta_materiale() const
{
  return dm; 
}

//-----------------------------------------------------------------------------
//  Operator==:
//-----------------------------------------------------------------------------
inline
bool
situazione_valutata::operator==(const situazione_valutata &s1) const
{
  return situazione::operator==(s1) && dm == s1.dm; 
}

//-----------------------------------------------------------------------------
//  Operator>>:
//-----------------------------------------------------------------------------
inline
std::istream &
operator>>(std::istream &i, situazione_valutata &s)
{ 
  s.carica(i); 
  return i; 
}

}  // Namespace Joker.

#endif  // SITUAZIONE_VALUTATA_H
