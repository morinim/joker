/*
 *  Pezzo.h:
 */

#if !defined(PEZZO_H)
#define      PEZZO_H

#include <iostream>
#include "configurazione.h"
#include "bitboard.h"
#include "posizione.h"
#include "valutazione.h"

namespace Joker
{

typedef unsigned int pezzo;

const pezzo   vuoto(0);
const pezzo  pedone(1);
const pezzo cavallo(2);
const pezzo alfiere(3);
const pezzo   torre(4);
const pezzo   donna(5);
const pezzo      re(6);

// Scivola[p]: vero per Alfiere, Torre e Donna.
const bool scivola[7] = { false, false, false, true, true, true, false };

// Direzione[p][i]: i-esima direzione in cui puo' spostarsi il pezzo p. Notare
// che ogni riga dell'array termina col valore nullo (sentinella).
const direzione dir[7][9] = 
{
  {   b_d,    b_s,      0,        0,     0,     0,        0,     0, 0 },
  {   a_d,    a_s,      0,        0,     0,     0,        0,     0, 0 },
  { a_a_d,  a_d_d,  b_d_d,    b_b_d, b_b_s, b_s_s,    a_s_s, a_a_s, 0 },
  {   a_d,    b_d,    b_s,      a_s,     0,     0,        0,     0, 0 },
  {  alto, destra,  basso, sinistra,     0,     0,        0,     0, 0 },
  {  alto,    a_d, destra,      b_d, basso,   b_s, sinistra,   a_s, 0 },
  {  alto,    a_d, destra,      b_d, basso,   b_s, sinistra,   a_s, 0 }
};

// Simbolo[p]: simbolo del pezzo p.
const char simbolo[7] = { ' ', 'P', 'N', 'B', 'R', 'Q', 'K' };

// Valore[p]: valore statico del pezzo p.
const valutazione valore[7] = { 0, 100, 310, 310, 500, 1000, matto };

extern bitboard mosse[7][64];

namespace pezzi
{
  void calcola_array();
}

template<colore C> bitboard attacchi_dx(bitboard);
template<colore C> bitboard attacchi_sx(bitboard);
template<colore C> bitboard attacchi(bitboard);
template<colore C> bitboard avanza(bitboard);
template<colore C> bitboard avanza(bitboard, unsigned);

//-----------------------------------------------------------------------------
//  Attacchi:
//-----------------------------------------------------------------------------
template<>
inline
bitboard
attacchi_dx<nero>(bitboard bb)
{
  return (bb & ~bitboard::colonna[colH]) >> 7u;
}

template<>
inline
bitboard
attacchi_sx<nero>(bitboard bb)
{
  return (bb & ~bitboard::colonna[colA]) >> 9u;
}

template<>
inline
bitboard
attacchi_dx<bianco>(bitboard bb) 
{
  return (bb & ~bitboard::colonna[colH]) << 9u;
}

template<>
inline
bitboard
attacchi_sx<bianco>(bitboard bb)
{
  return (bb & ~bitboard::colonna[colA]) << 7u;
}

template<colore C>
inline
bitboard
attacchi(bitboard bb)
{
  return attacchi_dx<C>(bb) | attacchi_sx<C>(bb);
}

//-----------------------------------------------------------------------------
//  Avanza:
//-----------------------------------------------------------------------------
template<>
inline
bitboard
avanza<nero>(bitboard bb)
{
  return bb >> 8u;
}

template<>
inline
bitboard
avanza<bianco>(bitboard bb)
{
  return bb << 8u;
}

template<>
inline
bitboard
avanza<nero>(bitboard bb, unsigned passi)
{
  return bb >> (passi << 3u);
}

template<>
inline
bitboard
avanza<bianco>(bitboard bb, unsigned passi)
{
  return bb << (passi << 3u);
}

}  // Namespace Joker.

#endif  // PEZZO_H
