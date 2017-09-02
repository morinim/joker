/*
 *  Nodo_apertura.h
 */

#if !defined(NODO_APERTURA_H)
#define      NODO_APERTURA_H

#include <cassert>
#include <iomanip>
#include "configurazione.h"
#include "chiave_hash.h"

namespace Joker
{

class nodo_apertura
{
public:
  enum eccezioni {formato};
  enum {dimensione = 38};

  explicit nodo_apertura(const chiave_hash & = chiave_hash(0), 
                         unsigned long = 0, valutazione = 0);
  explicit nodo_apertura(std::istream &) throw (eccezioni);

  bool operator==(const nodo_apertura &) const;
  bool operator!=(const nodo_apertura &) const;
  bool operator<(const nodo_apertura &) const;
  bool operator<=(const nodo_apertura &) const;
  bool operator>(const nodo_apertura &) const;

  chiave_hash chiave() const;
  unsigned long giocate() const;
  valutazione punteggio() const;

  bool controlla() const;
  
  friend std::istream &operator>>(std::istream &, nodo_apertura &);

private:
  chiave_hash    k;
  unsigned long ng;
  valutazione  val;
};

// Function prototypes per funzioni friend.
std::istream &operator>>(std::istream &, nodo_apertura &);
std::ostream &operator<<(std::ostream &, const nodo_apertura &);
unsigned64 distanza(const nodo_apertura &, const nodo_apertura &);



//-----------------------------------------------------------------------------
//  Nodo_apertura: nodo di apertura relativo ad una posizione con chiave k1,
//                 giocata ng1 volte e valore stimato v.
//-----------------------------------------------------------------------------
inline
nodo_apertura::nodo_apertura(const chiave_hash &k1, unsigned long ng1, 
                             valutazione v) 
  : k(k1), ng(ng1), val(v)
{
  assert(controlla());
}

inline
nodo_apertura::nodo_apertura(std::istream &i)
throw(Joker::nodo_apertura::eccezioni)
{
  if (!(i >> *this))
    throw formato;
}

//-----------------------------------------------------------------------------
//  Operator>>:
//-----------------------------------------------------------------------------
inline
std::istream &
operator>>(std::istream &i, nodo_apertura &n)
{
  if (!i.eof())
    i >> n.k >> n.ng >> n.val;
  else
    i.clear(std::ios::badbit|i.rdstate());

  return i;
}

//-----------------------------------------------------------------------------
//  Operator<<:
//-----------------------------------------------------------------------------
inline
std::ostream &
operator<<(std::ostream &o, const nodo_apertura &n)
{
  o << n.chiave()
    << std::setw(9) << n.giocate()
    << std::setw(7) << n.punteggio() << '\n';
  
  return o;
}

//-----------------------------------------------------------------------------
//  Operator==:
//-----------------------------------------------------------------------------
inline
bool
nodo_apertura::operator==(const nodo_apertura &n) const
{
  return k == n.chiave();
}

//-----------------------------------------------------------------------------
//  Operator!=:
//-----------------------------------------------------------------------------
inline
bool
nodo_apertura::operator!=(const nodo_apertura &n) const
{
  return !(*this == n);
}

//-----------------------------------------------------------------------------
//  Operator<:
//-----------------------------------------------------------------------------
inline
bool
nodo_apertura::operator<(const nodo_apertura &n) const
{
  return k < n.chiave();
}

//-----------------------------------------------------------------------------
//  Operator<=:
//-----------------------------------------------------------------------------
inline
bool
nodo_apertura::operator<=(const nodo_apertura &n) const
{
  return *this < n || *this == n;
}

//-----------------------------------------------------------------------------
//  Operator>:
//-----------------------------------------------------------------------------
inline
bool
nodo_apertura::operator>(const nodo_apertura &n) const
{
  return k > n.chiave();
}

//-----------------------------------------------------------------------------
//  Distanza:
//-----------------------------------------------------------------------------
inline
unsigned64
distanza(const nodo_apertura &n1, const nodo_apertura &n2)
{
  return distanza(n1.chiave(),n2.chiave());
}

//-----------------------------------------------------------------------------
//  Chiave:
//-----------------------------------------------------------------------------
inline
chiave_hash
nodo_apertura::chiave() const
{
  return k;
}

//-----------------------------------------------------------------------------
//  Giocate:
//-----------------------------------------------------------------------------
inline
unsigned long
nodo_apertura::giocate() const
{
  return ng;
}

//-----------------------------------------------------------------------------
//  Punteggio:
//-----------------------------------------------------------------------------
inline
valutazione
nodo_apertura::punteggio() const
{
  return val;
}

//-----------------------------------------------------------------------------
//  Controlla:
//-----------------------------------------------------------------------------
inline
bool
nodo_apertura::controlla() const
{
  return chiave().controlla() && punteggio().controlla();
}

}  // Namespace Joker.

#endif  // NODO_APERTURA_H
