/*
 *  Bitboard.cc
 */

#include "bitboard.h"

namespace Joker
{

const bitboard bitboard::caselle[2] =
{
  maschera[a1]|maschera[c1]|maschera[e1]|maschera[g1]|
  maschera[b2]|maschera[d2]|maschera[f2]|maschera[h2]|
  maschera[a3]|maschera[c3]|maschera[e3]|maschera[g3]|
  maschera[b4]|maschera[d4]|maschera[f4]|maschera[h4]|
  maschera[a5]|maschera[c5]|maschera[e5]|maschera[g5]|
  maschera[b6]|maschera[d6]|maschera[f6]|maschera[h6]|
  maschera[a7]|maschera[c7]|maschera[e7]|maschera[g7]|
  maschera[b8]|maschera[d8]|maschera[f8]|maschera[h8],

  maschera[b1]|maschera[d1]|maschera[f1]|maschera[h1]|
  maschera[a2]|maschera[c2]|maschera[e2]|maschera[g2]|
  maschera[b3]|maschera[d3]|maschera[f3]|maschera[h3]|
  maschera[a4]|maschera[c4]|maschera[e4]|maschera[g4]|
  maschera[b5]|maschera[d5]|maschera[f5]|maschera[h5]|
  maschera[a6]|maschera[c6]|maschera[e6]|maschera[g6]|
  maschera[b7]|maschera[d7]|maschera[f7]|maschera[h7]|
  maschera[a8]|maschera[c8]|maschera[e8]|maschera[g8]
};

const bitboard bitboard::colonnaA( (bitboard(1) <<  0u) |
                                   (bitboard(1) <<  8u) |
                                   (bitboard(1) << 16u) |
                                   (bitboard(1) << 24u) |
                                   (bitboard(1) << 32u) |
                                   (bitboard(1) << 40u) |
                                   (bitboard(1) << 48u) |
                                   (bitboard(1) << 56u) );

const bitboard bitboard::colonna[8] =
{
  colonnaA << 0u, colonnaA << 1u, colonnaA << 2u, colonnaA << 3u,
  colonnaA << 4u, colonnaA << 5u, colonnaA << 6u, colonnaA << 7u
};

const bitboard bitboard::traversa[8] =
{
  bitboard(255)<< 0u,bitboard(255)<< 8u,bitboard(255)<<16u,bitboard(255)<<24u,
  bitboard(255)<<32u,bitboard(255)<<40u,bitboard(255)<<48u,bitboard(255)<<56u
};

const bitboard bitboard::traversa7[2] =
{
  traversa[1], traversa[6]
};

// Notare che maschera[p_sent] == 0.
const bitboard bitboard::maschera[65] =
{
  bitboard(1) <<  0u,bitboard(1) <<  1u,bitboard(1) <<  2u,bitboard(1) <<  3u,
  bitboard(1) <<  4u,bitboard(1) <<  5u,bitboard(1) <<  6u,bitboard(1) <<  7u,
  bitboard(1) <<  8u,bitboard(1) <<  9u,bitboard(1) << 10u,bitboard(1) << 11u,
  bitboard(1) << 12u,bitboard(1) << 13u,bitboard(1) << 14u,bitboard(1) << 15u,
  bitboard(1) << 16u,bitboard(1) << 17u,bitboard(1) << 18u,bitboard(1) << 19u,
  bitboard(1) << 20u,bitboard(1) << 21u,bitboard(1) << 22u,bitboard(1) << 23u,
  bitboard(1) << 24u,bitboard(1) << 25u,bitboard(1) << 26u,bitboard(1) << 27u,
  bitboard(1) << 28u,bitboard(1) << 29u,bitboard(1) << 30u,bitboard(1) << 31u,
  bitboard(1) << 32u,bitboard(1) << 33u,bitboard(1) << 34u,bitboard(1) << 35u,
  bitboard(1) << 36u,bitboard(1) << 37u,bitboard(1) << 38u,bitboard(1) << 39u,
  bitboard(1) << 40u,bitboard(1) << 41u,bitboard(1) << 42u,bitboard(1) << 43u,
  bitboard(1) << 44u,bitboard(1) << 45u,bitboard(1) << 46u,bitboard(1) << 47u,
  bitboard(1) << 48u,bitboard(1) << 49u,bitboard(1) << 50u,bitboard(1) << 51u,
  bitboard(1) << 52u,bitboard(1) << 53u,bitboard(1) << 54u,bitboard(1) << 55u,
  bitboard(1) << 56u,bitboard(1) << 57u,bitboard(1) << 58u,bitboard(1) << 59u,
  bitboard(1) << 60u,bitboard(1) << 61u,bitboard(1) << 62u,bitboard(1) << 63u,
  bitboard(0)
};

const bitboard bitboard::grande_centro[2] =
{
  maschera[c3]|maschera[e3]|maschera[d4]|maschera[f4]|
  maschera[c5]|maschera[e5]|maschera[d6]|maschera[f6],
  maschera[d3]|maschera[f3]|maschera[c4]|maschera[e4]|
  maschera[d5]|maschera[f5]|maschera[c6]|maschera[e6]
};

const bitboard bitboard::traversa7abs[2] =
{
  traversa7[0] & ~(maschera[a2]|maschera[h2]),
  traversa7[1] & ~(maschera[a7]|maschera[h7])
};

const bitboard bitboard::m_isolato[8] =
{
                colonna[colB], colonna[colA]|colonna[colC], 
  colonna[colB]|colonna[colD], colonna[colC]|colonna[colE],
  colonna[colD]|colonna[colF], colonna[colE]|colonna[colG],
  colonna[colF]|colonna[colH],               colonna[colG]
};

const bitboard bitboard::fianchi[64] =
{
               maschera[b1], maschera[a1]|maschera[c1],
  maschera[b1]|maschera[d1], maschera[c1]|maschera[e1],
  maschera[d1]|maschera[f1], maschera[e1]|maschera[g1],
  maschera[f1]|maschera[h1],              maschera[g1],
               maschera[b2], maschera[a2]|maschera[c2],
  maschera[b2]|maschera[d2], maschera[c2]|maschera[e2],
  maschera[d2]|maschera[f2], maschera[e2]|maschera[g2],
  maschera[f2]|maschera[h2],              maschera[g2],
               maschera[b3], maschera[a3]|maschera[c3],
  maschera[b3]|maschera[d3], maschera[c3]|maschera[e3],
  maschera[d3]|maschera[f3], maschera[e3]|maschera[g3],
  maschera[f3]|maschera[h3],              maschera[g3],
               maschera[b4], maschera[a4]|maschera[c4],
  maschera[b4]|maschera[d4], maschera[c4]|maschera[e4],
  maschera[d4]|maschera[f4], maschera[e4]|maschera[g4],
  maschera[f4]|maschera[h4],              maschera[g4],
               maschera[b5], maschera[a5]|maschera[c5],
  maschera[b5]|maschera[d5], maschera[c5]|maschera[e5],
  maschera[d5]|maschera[f5], maschera[e5]|maschera[g5],
  maschera[f5]|maschera[h5],              maschera[g5],
               maschera[b6], maschera[a6]|maschera[c6],
  maschera[b6]|maschera[d6], maschera[c6]|maschera[e6],
  maschera[d6]|maschera[f6], maschera[e6]|maschera[g6],
  maschera[f6]|maschera[h6],              maschera[g6],
               maschera[b7], maschera[a7]|maschera[c7],
  maschera[b7]|maschera[d7], maschera[c7]|maschera[e7],
  maschera[d7]|maschera[f7], maschera[e7]|maschera[g7],
  maschera[f7]|maschera[h7],              maschera[g7],
               maschera[b8], maschera[a8]|maschera[c8],
  maschera[b8]|maschera[d8], maschera[c8]|maschera[e8],
  maschera[d8]|maschera[f8], maschera[e8]|maschera[g8],
  maschera[f8]|maschera[h8],              maschera[g8],
};

bitboard             bitboard::avanti[2][64];
bitboard          bitboard::m_passato[2][64];
bitboard           bitboard::m_scudo1[2][64];
bitboard           bitboard::m_scudo2[2][64];
bitboard            bitboard::raggio[64][64];
bitboard bitboard::torre_intrappolata[2][64];



//-----------------------------------------------------------------------------
//  Calcola_array:
//-----------------------------------------------------------------------------
void
bitboard::calcola_array()
{
  colore c(false);

  // Inizializzazione array m_passato.
  do
    for (posizione p(a1); !p.fuori(); ++p)
    {
      const unsigned nc(p.colonna());
      bitboard colonne(colonna[nc]);
      if (nc != colA) colonne |= colonna[nc-1];
      if (nc != colH) colonne |= colonna[nc+1];

      bitboard traverse(0);
      for (unsigned t(0); t < 8; t++)
        if ( (c==nero && t < p.traversa()) || (c==bianco && t > p.traversa()) )
          traverse |= traversa[t];
  
      m_passato[c][p] = colonne & traverse;
    }
  while (c^=1);

  // Inizializzazione array raggio.
  const direzione d[8] = { destra, a_d, alto, a_s, sinistra, b_s, basso, b_d };

  for (posizione da(a1); !da.fuori(); ++da)
    for (posizione a(a1); !a.fuori(); ++a)
      raggio[da][a].clear();

  for (posizione da(a1); !da.fuori(); ++da)
    for (unsigned i(0); i < 8; i++)
      for (posizione a(da+d[i]); !a.fuori(); a += d[i])
        if (a != da+d[i])
          raggio[da][a] = raggio[da][a+inversa(d[i])] | 
                          maschera[a+inversa(d[i])];

  // Inizializzazione array torre_intrappolata.
  do
  {
    for (posizione p(a1); !p.fuori(); ++p)
      torre_intrappolata[c][p].clear();

    torre_intrappolata[c][flip[c][f1]] = maschera[flip[c][g1]]|
                                         maschera[flip[c][h1]]|
                                         maschera[flip[c][h2]];
    torre_intrappolata[c][flip[c][g1]] = maschera[flip[c][h1]]|
                                         maschera[flip[c][h2]];
    torre_intrappolata[c][flip[c][h1]] = maschera[flip[c][h2]];
    torre_intrappolata[c][flip[c][c1]] = maschera[flip[c][a1]]|
                                         maschera[flip[c][b1]]|
                                         maschera[flip[c][a2]];
    torre_intrappolata[c][flip[c][b1]] = maschera[flip[c][a1]]|
                                         maschera[flip[c][a2]];
    torre_intrappolata[c][flip[c][a1]] = maschera[flip[c][a2]];
  } while (c^=1);

  // Inizializzazione array m_scudo1 e m_scudo2.
  do
    for (posizione p(a1); !p.fuori(); ++p)
    {
      m_scudo1[c][p].clear();
      m_scudo2[c][p].clear();
    }
  while (c^=1);

  const bitboard colonneABC(colonna[colA]|colonna[colB]|colonna[colC]);
  const bitboard colonneFGH(colonna[colF]|colonna[colG]|colonna[colH]);

  m_scudo1[nero][a8] = traversa[6] & colonneABC;  
  m_scudo1[nero][b8] = traversa[6] & colonneABC;  
  m_scudo1[nero][c8] = traversa[6] & colonneABC;
  m_scudo2[nero][a8] = traversa[5] & colonneABC;
  m_scudo2[nero][b8] = traversa[5] & colonneABC;
  m_scudo2[nero][c8] = traversa[5] & colonneABC;
  m_scudo1[nero][f8] = traversa[6] & colonneFGH;
  m_scudo1[nero][g8] = traversa[6] & colonneFGH;
  m_scudo1[nero][h8] = traversa[6] & colonneFGH;
  m_scudo2[nero][f8] = traversa[5] & colonneFGH;
  m_scudo2[nero][g8] = traversa[5] & colonneFGH;
  m_scudo2[nero][h8] = traversa[5] & colonneFGH;

  m_scudo1[bianco][a1] = traversa[1] & colonneABC;
  m_scudo1[bianco][b1] = traversa[1] & colonneABC;
  m_scudo1[bianco][c1] = traversa[1] & colonneABC;
  m_scudo2[bianco][a1] = traversa[2] & colonneABC;
  m_scudo2[bianco][b1] = traversa[2] & colonneABC;
  m_scudo2[bianco][c1] = traversa[2] & colonneABC;
  m_scudo1[bianco][f1] = traversa[1] & colonneFGH;
  m_scudo1[bianco][g1] = traversa[1] & colonneFGH;
  m_scudo1[bianco][h1] = traversa[1] & colonneFGH;
  m_scudo2[bianco][f1] = traversa[2] & colonneFGH;
  m_scudo2[bianco][g1] = traversa[2] & colonneFGH;
  m_scudo2[bianco][h1] = traversa[2] & colonneFGH;

  // Inizializzazione array avanti.
  do
    for (posizione p(a1); !p.fuori(); ++p)
    {
      const posizione arrivo(p.colonna(),c==nero ? 0 : 7);

      avanti[c][p] = raggio[p][arrivo] & ~maschera[p];
    }
  while (c^=1);
}

//-----------------------------------------------------------------------------
//  Controlla: verifica che le strutture precalcolate (non costanti per ragioni
//             di complessita' di dichiarazione) non vengano modificate durante
//             l'elaborazione del programma.
//-----------------------------------------------------------------------------
inline
bool
bitboard::controlla() const
{
  for (posizione pos(a1); !pos.fuori(); ++pos)
    if ( (avanti[nero][pos] | avanti[bianco][pos]) !=
         (colonna[pos.colonna()] & ~maschera[pos]) )
      return false;

  return true;
}

}  // Namespace Joker.


