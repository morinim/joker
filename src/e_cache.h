/*
 *  E_cache.h
 */

#if !defined(E_CACHE_H)
#define      E_CACHE_H

#include "configurazione.h"
#include "scacchiera.h"

namespace Joker
{

template<unsigned8 NBIT>
class e_cache
{
public:
  typedef unsigned64 tipo_chiave;

  struct info
  {
    bitboard       forti[2];
    bitboard     passati[2];
    valutazione   v_schermo;
    valutazione v_struttura;
  };

  e_cache();

  static tipo_chiave calcola_chiave(const scacchiera &);
  info *find(tipo_chiave, bool *);
  void update(tipo_chiave, bool *);

  void clear();

private:
  struct elemento
  {
    tipo_chiave chiave;
    info             i;
  };

  elemento *at(tipo_chiave);

  elemento base[1ul << NBIT];
  mutable elemento *attuale;
};

//-----------------------------------------------------------------------------
//  E_cache:
//-----------------------------------------------------------------------------
template<unsigned8 NBIT>
inline
e_cache<NBIT>::e_cache()
  : attuale(0)
{
  assert(8 <= NBIT && NBIT <= 32);
  clear();
}

//-----------------------------------------------------------------------------
//  At:
//-----------------------------------------------------------------------------
template<unsigned8 NBIT>
inline
e_cache<NBIT>::elemento *
e_cache<NBIT>::at(tipo_chiave k)
{
  return base + (k & ((1ul << NBIT)-1));
}

//-----------------------------------------------------------------------------
//  Calcola_chiave: calcola la chiave hash utilizzata per l'accesso alla
//                  tabella e_cache. La tecnica utilizzata e' una versione
//                  semplificata di quella proposta da Robert J. Jenkins Jr.
//                  con l'adozione della funzione mix64.
//-----------------------------------------------------------------------------
template<unsigned8 NBIT>
e_cache<NBIT>::tipo_chiave
e_cache<NBIT>::calcola_chiave(const scacchiera &s)
{
  tipo_chiave a(U64(0x9e3779b97f4a7c13));  // Golden ratio.
  tipo_chiave b(U64(0x9e3779b97f4a7c13));
  tipo_chiave c((s.p_re(nero) << 8) | s.p_re(bianco));
  
  a += s.mappa(  nero,pedone);
  b += s.mappa(bianco,pedone);
  
  a -= b;  a -= c;  a ^= (c >> 43);
  b -= c;  b -= a;  b ^= (a <<  9);
  c -= a;  c -= b;  c ^= (b >>  8);
  a -= b;  a -= c;  a ^= (c >> 38);
  b -= c;  b -= a;  b ^= (a << 23);
  // c -= a;  c -= b;  c ^= (b >>  5);
  a -= b;  a -= c;  a ^= (c >> 35);
  b -= c;  b -= a;  b ^= (a << 49);
  c -= a;  c -= b;  c ^= (b >> 11);
  // a -= b;  a -= c;  a ^= (c >> 12);
  // b -= c;  b -= a;  b ^= (a << 18);
  // c -= a;  c -= b;  c ^= (b >> 22);
  
  return c;
}

//-----------------------------------------------------------------------------
//  Clear:
//-----------------------------------------------------------------------------
template<unsigned8 NBIT>
void
e_cache<NBIT>::clear()
{
  const unsigned long nelem(1ul << NBIT);
 
  for (unsigned long i(0); i < nelem; i++)
  {
    base[i].chiave        = 0;
    base[i].i.v_schermo   = 0;
    base[i].i.v_struttura = 0;
  }

  attuale = 0;
}

//-----------------------------------------------------------------------------
//  Find:
//-----------------------------------------------------------------------------
template<unsigned8 NBIT>
inline
e_cache<NBIT>::info *
e_cache<NBIT>::find(tipo_chiave k, bool *trovato)
{
  attuale = at(k);
  
  *trovato = (k == attuale->chiave);

  return &attuale->i;
}

//-----------------------------------------------------------------------------
//  Update:
//-----------------------------------------------------------------------------
template<unsigned8 NBIT>
inline
void
e_cache<NBIT>::update(tipo_chiave k, bool *ok)
{
  assert(attuale && at(k) == attuale);

  attuale->chiave =    k;
  *ok             = true;
}

}  // Namespace Joker

#endif  // E_CACHE_H
