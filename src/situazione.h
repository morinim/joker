/*
 *  Situazione.h
 */

#if !defined(SITUAZIONE_H)
#define      SITUAZIONE_H

#include <fstream>
#include <string>
#include "configurazione.h"
#include "dati_r.h"
#include "lista_mosse.h"

namespace Joker
{

//  Function prototypes per funzioni friend.
class situazione;
std::istream &operator>>(std::istream &, situazione &);
std::ostream &operator<<(std::ostream &, const situazione &);


  
class situazione : public scacchiera, public info_aus
{
public:
  situazione() : hk(scacchiera(),bianco) {};

  bool sotto_scacco() const { return attacca(!mossa_a(),p_re(mossa_a())); };
  bool sotto_scacco(colore c) const { return attacca(!c,p_re(c)); };
  bool da_scacco(const mossa_e &);

  void calcola(lista_mosse *) const;
  void calcola_catture(lista_mosse *) const;
  void calcola_non_catture(lista_mosse *) const;
  void calcola_legali(lista_mosse *);
  void calcola_tutte(lista_mosse *);
  
  bool pseudo_legale(mossa, mossa_e * = 0) const;
  bool legale(mossa, mossa_e * = 0);

  bool muovi(const mossa_e &, dati_r &);
  void muovinull(dati_r &);
  void ripristina(const mossa_e &, const dati_r &);
  void ripristinanull(const dati_r &);

  const chiave_hash &chiave() const { return hk; };

  bool carica(std::istream &, std::string * = 0);

  friend std::ostream &operator<<(std::ostream &, const situazione &);

  bool operator==(const situazione &) const;
  bool operator!=(const situazione &s1) const { return !(*this == s1); };
  bool controlla() const;

private:
  situazione(const scacchiera &, const info_aus &);

  chiave_hash hk;
};



//-----------------------------------------------------------------------------
//  Situazione:
//-----------------------------------------------------------------------------
inline situazione::situazione(const scacchiera &s1, const info_aus &ia1)
  : scacchiera(s1), info_aus(ia1)
{
  // Notare che la classe hk va inizializzata dopo info_aus in modo che il 
  // giocatore attivo sia quello corretto.
  hk = chiave_hash(s1,mossa_a());

  assert(controlla());
}

//-----------------------------------------------------------------------------
//  Muovi:
//-----------------------------------------------------------------------------
inline bool situazione::muovi(const mossa_e &m, dati_r &r)
{
  const colore c(mossa_a());
  assert(c == c_pezzo(m.da()));

  assert(controllo_rapido_legalita(c,m));

  scacchiera::muovi(c,m);

  r.info = *this;
  info_aus::muovi(m);

  if (!sotto_scacco(c))
  {
    r.chiave = hk;
    hk.muovi(c,m);

#if defined(_DEBUG)
    assert(hk.verifica(*this,!c));
#endif

    return true;
  }

  scacchiera::ripristina(c,m);
  info_aus::ripristina(r.info);
  return false;
}

//-----------------------------------------------------------------------------
//  Muovinull:
//-----------------------------------------------------------------------------
inline void situazione::muovinull(dati_r &r)
{ 
  info_aus::muovinull(&r.info);
  hk.muovinull(); 
}

//-----------------------------------------------------------------------------
//  Ripristina:
//-----------------------------------------------------------------------------
inline void situazione::ripristina(const mossa_e &m, const dati_r &r)
{
  const colore c( !mossa_a() );
  assert(c == c_pezzo(m.a()));

  scacchiera::ripristina(c,m);
  info_aus::ripristina(r.get<info_aus>());
  hk.ripristina(r.get<chiave_hash>());
}

//-----------------------------------------------------------------------------
//  Ripristinanull:
//-----------------------------------------------------------------------------
inline void situazione::ripristinanull(const dati_r &r)
{
  info_aus::ripristinanull(r.get<info_aus>());
  hk.ripristinanull(); 
}

//-----------------------------------------------------------------------------
//  Calcola:
//-----------------------------------------------------------------------------
inline void situazione::calcola(lista_mosse *lm) const
{
  calcola_catture(lm);
  calcola_non_catture(lm);
}

}  // Namespace Joker.

#endif  // SITUAZIONE_H
