/*
 *  Trasposizioni.h
 */

#if !defined(TRASPOSIZIONI_H)
#define      TRASPOSIZIONI_H

#include "configurazione.h"
#include "chiave_hash.h"
#include "dettaglio.h"

namespace Joker
{

namespace attributo
{
  enum {nessuno = 0, forzata = 1, minaccia = 2, evita_null = 4, scacco = 8};
}

typedef unsigned attributi;


//
//  Organizzazione dei bit di informazione:
//
//   inizio  n bit   nome         informazione
//      0     16     dettaglio    profondita' della ricerca su cui si basano le
//                                informazioni memorizzate;
//     16      6     mossa_da     casella iniziale della miglior mossa nella
//                                posizione attuale;
//     22      6     mossa_a      casella finale della miglior mossa nella
//                                posizione attuale;
//     28      4                  non utilizzati;
//     32     20     valutazione  valutazione della posizione + inf (per avere
//                                un valore positivo);
//     52      7     attributi    caratteristiche della posizione (presenza di
//                                minacce, mosse forzate);
//     59      2     tipo         0->non utilizzato, 1->fail-low, 2->fail-high
//                                3->valore esatto;
//     61      3     eta          identificazione di elementi derivanti da
//                                vecchie ricerche.
//

class trasposizioni
{
public:
  enum limitazione {inutile = 0u, superiore = 1u, esatta = 2u, inferiore = 3u};

  explicit trasposizioni(unsigned8 = HASH_BIT);
  ~trasposizioni();

  limitazione find(const chiave_hash &, dettaglio, valutazione, valutazione *,
                   mossa *, attributi *) const;

  template<class T> T get(const chiave_hash &) const;
  
  void insert(const chiave_hash &, dettaglio, limitazione, mossa, valutazione,
              attributi);

  void clear();
  void inc_eta();

  bool controlla() const;

private:
  class elemento
  {
  public:
    chiave_hash chiave[2];

    elemento() { info[0] = info[1] = 0; };

    Joker::dettaglio dettaglio(bool l) const
      { return int(info[l] & 0xFFFF); };

    Joker::mossa mossa(bool l) const
      { return Joker::mossa((info[l] >> 16) & 63,(info[l] >> 22) & 63); };

    Joker::valutazione valutazione(bool l) const
      { return int((info[l] >> 32) & 0xFFFFF) - inf; };

    Joker::attributi attributi(bool l) const
      { return Joker::attributi(unsigned((info[l] >> 52) & 127)); };

    limitazione tipo(bool l) const
      { return limitazione((info[l] >> 59) & 3); };

    unsigned eta(bool l) const
      { return unsigned(info[l] >> 61); };

    template<bool L> void setta(chiave_hash k, unsigned64 i)
      { chiave[L] = k; info[L] = i; };

    void abbassa_livello_0() { chiave[1] = chiave[0]; info[1] = info[0]; };

  private:
    unsigned64 info[2];
  };

  elemento *at(const chiave_hash &) const;
  unsigned64 impacchetta(unsigned, limitazione, attributi, valutazione, mossa,
                         dettaglio) const;

  elemento *const         base;
  const unsigned long maschera;
  unsigned         eta_attuale;
};

//-----------------------------------------------------------------------------
//  Trasposizioni:
//-----------------------------------------------------------------------------
inline
trasposizioni::trasposizioni(unsigned8 n)
  : base(new elemento[1ul << n]), maschera((1ul << n)-1), eta_attuale(1)
{
  assert(8 <= n && n <= 32);
  clear();
}

//-----------------------------------------------------------------------------
//  ~Trasposizioni:
//-----------------------------------------------------------------------------
inline
trasposizioni::~trasposizioni()
{
  delete [] base;
}

//-----------------------------------------------------------------------------
//  Get:
//-----------------------------------------------------------------------------
template<>
inline
attributi
trasposizioni::get<attributi>(const chiave_hash &k) const
{
  const elemento *const elem = at(k);

  if (k == elem->chiave[0])
    return elem->attributi(0);
  if (k == elem->chiave[1])
    return elem->attributi(1);

  return attributo::nessuno;
}

template<>
inline
mossa
trasposizioni::get<mossa>(const chiave_hash &k) const
{
  const elemento *const elem = at(k);

  if (k == elem->chiave[0])
    return elem->mossa(0);
  if (k == elem->chiave[1])
    return elem->mossa(1);

  return mossa::sentinella;
}

//-----------------------------------------------------------------------------
//  Inc_eta:
//-----------------------------------------------------------------------------
inline
void
trasposizioni::inc_eta()
{
  eta_attuale = (eta_attuale+1) & 7;

  if (eta_attuale == 0)
    ++eta_attuale;
}

//-----------------------------------------------------------------------------
//  At:
//-----------------------------------------------------------------------------
inline
trasposizioni::elemento *
trasposizioni::at(const chiave_hash &k) const
{
  return base + unsigned32(k.maschera(maschera));
}

//-----------------------------------------------------------------------------
//  Impachetta:
//-----------------------------------------------------------------------------
inline
unsigned64
trasposizioni::impacchetta(unsigned eta, limitazione tipo, attributi attr,
                           valutazione val, mossa m, dettaglio d) const
{
  assert(eta < 8 && tipo < 4);

  unsigned64 info(eta);
  info <<=  2;  info |= unsigned64(   tipo);
  info <<=  7;  info |= unsigned64(   attr);
  info <<= 20;  info |= unsigned64(val+inf);
  info <<= 10;  info |= unsigned64(  m.a());
  info <<=  6;  info |= unsigned64( m.da()); 
  info <<= 16;  info |= unsigned64(      d);

  return info;
}

}  // Namespace Joker.

#endif  // TRASPOSIZIONI_H
