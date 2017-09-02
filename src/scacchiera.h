/*
 *  Scacchiera.h
 */

#if !defined(SCACCHIERA_H)
#define      SCACCHIERA_H

#include <clocale>
#include <fstream>
#include <string>
#include "configurazione.h"
#include "bitboard.h"
#include "mossa.h"

namespace Joker
{

class scacchiera
{
public:
  typedef bitboard             *iterator;
  typedef const bitboard *const_iterator;

  scacchiera();

  bool operator==(const scacchiera &) const;
  bool operator!=(const scacchiera &s1) const { return !(*this == s1); };

  colore c_pezzo(posizione) const;
  bool p_passato(colore, posizione) const;
  bool e_un(colore, posizione, pezzo) const;
  pezzo t_pezzo(colore, posizione) const;
  pezzo t_preda(colore, posizione) const;
  bitboard mappa() const;
  bitboard mappa(colore) const;
  bitboard mappa(colore, pezzo) const;
  posizione p_re(colore) const;

  unsigned n_pezzi(colore) const;
  unsigned n_pezzi(colore, pezzo) const;
  unsigned n_ufficiali(colore) const;
  
  void muovi(colore, const mossa_e &);
  void ripristina(colore, const mossa_e &);

  template<pezzo P> bool attacca(colore, posizione) const;
  bool attacca(colore, posizione) const;
  
  void clear();

  bool controllo_rapido_legalita(colore, mossa) const;
  bool controlla() const;

  friend std::istream &operator>>(std::istream &, scacchiera &);
  friend std::ostream &operator<<(std::ostream &, const scacchiera &);
  
private:
  void somma();

  bitboard pezzi[2][7];
};



//  Function prototypes per funzioni friend.
std::istream &operator>>(std::istream &, scacchiera &);
std::ostream &operator<<(std::ostream &, const scacchiera &);



//-----------------------------------------------------------------------------
//  C_Pezzo:
//-----------------------------------------------------------------------------
inline
colore
scacchiera::c_pezzo(posizione p) const
{ 
  assert( !p.fuori() && mappa()[p] );
  return pezzi[bianco][0][p]; 
}

//-----------------------------------------------------------------------------
//  P_passato:
//-----------------------------------------------------------------------------
inline
bool
scacchiera::p_passato(colore c, posizione pos) const
{
  return !( bitboard::m_passato[c][pos] & pezzi[!c][pedone] );
}

//-----------------------------------------------------------------------------
//  E_un:
//-----------------------------------------------------------------------------
inline
bool
scacchiera::e_un(colore c, posizione pos, pezzo pz) const
{
  return pezzi[c][pz][pos];
}

//-----------------------------------------------------------------------------
//  T_preda:
//-----------------------------------------------------------------------------
inline
pezzo
scacchiera::t_preda(colore c, posizione p) const
{
  assert(!p.fuori() && pezzi[c][0][p] && !pezzi[c][re][p]);

  if (pezzi[c][pedone][p])
    return pedone;
  if (pezzi[c][cavallo][p])
    return cavallo;
  if (pezzi[c][alfiere][p])
    return alfiere;
  return pezzi[c][torre][p] ? torre : donna;
}

//-----------------------------------------------------------------------------
//  Mappa:
//-----------------------------------------------------------------------------
inline
bitboard
scacchiera::mappa() const
{
  return pezzi[nero][0] | pezzi[bianco][0];
}

inline
bitboard
scacchiera::mappa(colore c) const
{
  return pezzi[c][0];
}

inline
bitboard
scacchiera::mappa(colore c, pezzo pz) const
{
  assert(pz != vuoto);
  return pezzi[c][pz];
}

//-----------------------------------------------------------------------------
//  P_re:
//-----------------------------------------------------------------------------
inline
posizione
scacchiera::p_re(colore c) const
{
  return pezzi[c][re].firstone();
}

//-----------------------------------------------------------------------------
//  N_pezzi:
//-----------------------------------------------------------------------------
inline
unsigned
scacchiera::n_pezzi(colore c) const
{
  return pezzi[c][0].count();
}

inline
unsigned
scacchiera::n_pezzi(colore c, pezzo pz) const
{
  assert(pz != vuoto);

  return pezzi[c][pz].count();
}

//-----------------------------------------------------------------------------
//  N_ufficiali:
//-----------------------------------------------------------------------------
inline
unsigned
scacchiera::n_ufficiali(colore c) const
{
  return bitboard(pezzi[c][0] ^ pezzi[c][pedone]).count();
  //return n_pezzi(c)-n_pezzi(c,pedone);
}

//-----------------------------------------------------------------------------
//  Attacca:
//-----------------------------------------------------------------------------
template<>
inline
bool
scacchiera::attacca<pedone>(colore c, posizione bersaglio) const
{
  return mappa(c,pedone) & mosse[!c][bersaglio];
}

template<>
inline
bool
scacchiera::attacca<cavallo>(colore c, posizione bersaglio) const
{
  return mappa(c,cavallo) & mosse[cavallo][bersaglio];
}

template<>
inline
bool
scacchiera::attacca<re>(colore c, posizione bersaglio) const
{
  return mappa(c,re) & mosse[re][bersaglio];
}

//-----------------------------------------------------------------------------
//  Controllo_rapido_legalita: esegue un approssimativo controllo della
//                             legalita' della mossa m nella situazione
//                             attuale. Piu' specificatamente viene
//                             verificato che nella casella di origine
//                             sia presente un pezzo del colore appropriato
//                             e che la casella di arrivo sia libera o
//                             contenente un pezzo avversario diverso dal
//                             re.
//-----------------------------------------------------------------------------
inline
bool
scacchiera::controllo_rapido_legalita(colore c, mossa m) const
{
  return pezzi[c][0][m.da()] && !pezzi[c][0][m.a()] && 
         !pezzi[!c][re][m.a()];
}

}  // Namespace Joker.

#endif  // SCACCHIERA_H
