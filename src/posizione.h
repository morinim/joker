/*
 *  Posizione.h
 */

#if !defined(POSIZIONE_H)
#define      POSIZIONE_H

#include <cctype>
#include <string>
#include "configurazione.h"
#include "colore.h"
#include "direzione.h"
#include "nonstd.h"

namespace Joker
{

enum id_colonna {colA=0, colB, colC, colD, colE, colF, colG, colH};

 enum id_posizione {p_sent = 64,
                    a8 = 56, b8, c8, d8, e8, f8, g8, h8,
                    a7 = 48, b7, c7, d7, e7, f7, g7, h7,
                    a6 = 40, b6, c6, d6, e6, f6, g6, h6,
                    a5 = 32, b5, c5, d5, e5, f5, g5, h5,
                    a4 = 24, b4, c4, d4, e4, f4, g4, h4,
                    a3 = 16, b3, c3, d3, e3, f3, g3, h3,
                    a2 =  8, b2, c2, d2, e2, f2, g2, h2,
                    a1 =  0, b1, c1, d1, e1, f1, g1, h1};

const id_posizione flip[2][64] =
{
  {
    a8, b8, c8, d8, e8, f8, g8, h8,   
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1
  },
  {
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8
  }
};

const id_posizione avanti1[2][64] =
{
  {
    p_sent, p_sent, p_sent, p_sent, p_sent, p_sent, p_sent, p_sent,
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7
  },
  {
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8,
    p_sent, p_sent, p_sent, p_sent, p_sent, p_sent, p_sent, p_sent
  }
};



class posizione
{
public:
  posizione();
  posizione(unsigned);
  posizione(unsigned, unsigned);
  
  unsigned  colonna() const;
  unsigned traversa() const;

  posizione operator+(direzione) const;
  posizione &operator+=(direzione);
  posizione &operator=(unsigned);

  posizione &operator++();
  posizione &operator--();

  operator unsigned() const;

  Joker::colore campo() const;
  Joker::colore colore() const;
  unsigned distanza_da_base(Joker::colore) const;
  unsigned distanza_da_centro() const;
  bool fuori() const;

  bool controlla() const;

  std::string str() const;
  friend std::istream &operator>>(std::istream &, posizione &);

private:
  static const unsigned mailbox[120], mailbox64[64];
  
  unsigned p;
};

// Function prototypes per funzioni friend.
std::istream &operator>>(std::istream &, posizione &);
unsigned distanza_x(posizione, posizione);
unsigned distanza_y(posizione, posizione);
unsigned distanza(posizione, posizione);



//-----------------------------------------------------------------------------
//  Posizione:
//-----------------------------------------------------------------------------
inline
posizione::posizione()
{
}

inline
posizione::posizione(unsigned ps) 
  : p(ps)
{
  assert(controlla());
}

inline
posizione::posizione(unsigned x, unsigned y) 
  : p(y*8+x)
{
  assert(y < 8 && x <= colH && controlla());
}

//-----------------------------------------------------------------------------
//  Colonna:
//-----------------------------------------------------------------------------
inline
unsigned
posizione::colonna() const
{
  assert(!fuori());
  return p & 7;
}

//-----------------------------------------------------------------------------
//  Traversa:
//-----------------------------------------------------------------------------
inline
unsigned
posizione::traversa() const
{
  assert(!fuori());
  return p >> 3;
}

//-----------------------------------------------------------------------------
//  Operator+:
//-----------------------------------------------------------------------------
inline
posizione
posizione::operator+(direzione d) const
{
  assert(Joker::controlla(d));
  return posizione(mailbox[mailbox64[p]+d]);
}

//-----------------------------------------------------------------------------
//  Operator+=:
//-----------------------------------------------------------------------------
inline
posizione &
posizione::operator+=(direzione d)
{
  assert(Joker::controlla(d));
  p = mailbox[mailbox64[p]+d];
  return *this;
}

//-----------------------------------------------------------------------------
//  Operator=:
//-----------------------------------------------------------------------------
inline
posizione &
posizione::operator=(unsigned ps)
{
  p = ps;
  return *this;
}

//-----------------------------------------------------------------------------
//  Fuori:
//-----------------------------------------------------------------------------
inline
bool
posizione::fuori() const
{
  return p == p_sent;
}

//-----------------------------------------------------------------------------
//  Operator++:
//-----------------------------------------------------------------------------
inline
posizione &
posizione::operator++()
{
  if (p != p_sent)
    ++p;

  return *this; 
}

//-----------------------------------------------------------------------------
//  Operator--:
//-----------------------------------------------------------------------------
inline
posizione &
posizione::operator--()
{
  if (p)
    --p;
  else
    p = p_sent;

  return *this;
}

//-----------------------------------------------------------------------------
//  Operator unsigned:
//-----------------------------------------------------------------------------
inline
posizione::operator unsigned() const
{
  return p;
}

//-----------------------------------------------------------------------------
//  Campo:
//-----------------------------------------------------------------------------
inline
Joker::colore
posizione::campo() const
{
  return p <= h4;
}

//-----------------------------------------------------------------------------
//  Colore:
//-----------------------------------------------------------------------------
inline
Joker::colore
posizione::colore() const
{
  return (p + traversa()) & 1;
}

//-----------------------------------------------------------------------------
//  Distanza_da_base:
//-----------------------------------------------------------------------------
inline
unsigned
posizione::distanza_da_base(Joker::colore c) const
{
  return c ? traversa() : 7-traversa();
}

//-----------------------------------------------------------------------------
//  Distanza_da_centro:
//-----------------------------------------------------------------------------
inline
unsigned
posizione::distanza_da_centro() const
{
  const unsigned x(colonna()  <= colD ? colD : colE);
  const unsigned y(traversa() <=    3 ?    3 :    4);

  return distanza(posizione(x,y),*this);
}

//-----------------------------------------------------------------------------
//  Controlla:
//-----------------------------------------------------------------------------
inline
bool
posizione::controlla() const
{
  return p <= p_sent;
}

//-----------------------------------------------------------------------------
//  Distanza_x:
//-----------------------------------------------------------------------------
inline
unsigned
distanza_x(posizione p1, posizione p2)
{
  return abs( int(p1.colonna()) - int(p2.colonna()) );
}

//-----------------------------------------------------------------------------
//  Distanza_y:
//-----------------------------------------------------------------------------
inline
unsigned
distanza_y(posizione p1, posizione p2)
{
  return abs( int(p1.traversa()) - int(p2.traversa()) );
}

//-----------------------------------------------------------------------------
//  Distanza:
//-----------------------------------------------------------------------------
inline
unsigned
distanza(posizione p1, posizione p2)
{
  return MAX(distanza_x(p1,p2),distanza_y(p1,p2));
}

}  // Namespace Joker.

#endif  // POSIZIONE_H
