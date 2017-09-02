/*
 *  Posizione.cc
 */

#include "posizione.h"

namespace Joker
{

const unsigned posizione::mailbox[120] =
{
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
  64, a8, b8, c8, d8, e8, f8, g8, h8, 64,
  64, a7, b7, c7, d7, e7, f7, g7, h7, 64,
  64, a6, b6, c6, d6, e6, f6, g6, h6, 64,
  64, a5, b5, c5, d5, e5, f5, g5, h5, 64, 
  64, a4, b4, c4, d4, e4, f4, g4, h4, 64,
  64, a3, b3, c3, d3, e3, f3, g3, h3, 64,
  64, a2, b2, c2, d2, e2, f2, g2, h2, 64,
  64, a1, b1, c1, d1, e1, f1, g1, h1, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
};

const unsigned posizione::mailbox64[64] =
{
  91, 92, 93, 94, 95, 96, 97, 98,
  81, 82, 83, 84, 85, 86, 87, 88,
  71, 72, 73, 74, 75, 76, 77, 78,
  61, 62, 63, 64, 65, 66, 67, 68,
  51, 52, 53, 54, 55, 56, 57, 58,
  41, 42, 43, 44, 45, 46, 47, 48,
  31, 32, 33, 34, 35, 36, 37, 38,
  21, 22, 23, 24, 25, 26, 27, 28
};

//-----------------------------------------------------------------------------
//  Str:
//-----------------------------------------------------------------------------
std::string
posizione::str() const
{
  if (p != p_sent)
  {
    static const std::string nome_colonna[] ={"a","b","c","d","e","f","g","h"};
    static const std::string nome_traversa[]={"1","2","3","4","5","6","7","8"};

    return nome_colonna[colonna()] + nome_traversa[traversa()];
  }

  return "-";
}

//-----------------------------------------------------------------------------
//  Operator>>:
//-----------------------------------------------------------------------------
std::istream &
operator>>(std::istream &is, posizione &ps)
{
  int x;
  unsigned y;

  while ( isspace(x = is.get()) )
    ;

  if (x != '-')
  {
    is >> y;
    ps = posizione(            x-'a',y-1);
  }
  else
    ps = posizione(p_sent);

  return is;
}

} // Namespace Joker.
