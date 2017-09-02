/*
 *  Chiave_hash.cc
 */

#include "chiave_hash.h"

namespace Joker
{

unsigned64 chiave_hash::codice_posizione[2][7][64];



//-----------------------------------------------------------------------------
//  Calcola_array:
//-----------------------------------------------------------------------------
void chiave_hash::calcola_array()
{
  colore c(false);

  do
  {
    for (unsigned pz(vuoto); pz <= re; pz++)
      for (posizione p(a1); !p.fuori(); ++p)
        codice_posizione[c][pz][p] = random::random64();
  } while (c^=1);
}

//-----------------------------------------------------------------------------
//  Chiave_hash:
//-----------------------------------------------------------------------------
chiave_hash::chiave_hash(const scacchiera &s, colore c_mossa)
{
  chiave = 0;
  
  colore c(false);
  do
    for (pezzo pz(pedone); pz <= re; ++pz)
    {
      bitboard mp(s.mappa(c,pz));
      
      while (mp)
      {
        const posizione pos(mp.firstone());
        chiave ^= codice_posizione[c][pz][pos];
        mp.elimina(pos);
      }
    }
  while (c^=1);

  if (c_mossa == nero)
    chiave = ~chiave;
}

//-----------------------------------------------------------------------------
//  Operator>>:
//-----------------------------------------------------------------------------
std::istream &operator>>(std::istream &i, chiave_hash &c)
{
  unsigned64 tmp1(0), tmp2(0);

  i >> tmp1 >> tmp2;

  c.chiave = (tmp1 << 32) | tmp2;
  return i;
}

//-----------------------------------------------------------------------------
//  Operator<<:
//-----------------------------------------------------------------------------
std::ostream &operator<<(std::ostream &o, const chiave_hash &c)
{
  const unsigned32 n1(unsigned32(c.chiave >>      32));
  const unsigned32 n2(unsigned32(c.chiave &  u32_max));

  o << std::setw(10) << n1 << ' ' << std::setw(10) << n2;
  return o;
}

#if defined(_DEBUG)
bool chiave_hash::verifica(const scacchiera &s, colore c) const
{
  chiave_hash tmp(s,c);

  return chiave == tmp.chiave;
}
#endif

}  // Namespace Joker.
