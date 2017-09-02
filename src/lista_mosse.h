/*
 *  Lista_mosse.h
 */

#if !defined(LISTA_MOSSE_H)
#define      LISTA_MOSSE_H

#include <iostream>
#include "configurazione.h"
#include "mossa.h"

namespace Joker
{

class lista_mosse
{
public:
  enum {max_varianti = 200};
  
  typedef mossa_e             *iterator;
  typedef const mossa_e *const_iterator;

  explicit lista_mosse(size_t = max_varianti+1);
  lista_mosse(iterator, iterator);
  ~lista_mosse();

  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;

  lista_mosse &operator=(const lista_mosse &);

  const mossa_e &operator[](unsigned) const;
  
  void push_back(posizione, posizione, pezzo, pezzo=vuoto, tipo_mossa=normale);
  void push_back(const mossa_e &);

  void erase(iterator);
  void erase(mossa);
  void clear();

  template<class T> void sort(T *);
  void swap(iterator, iterator);
  
  const_iterator find(mossa) const;
  bool equivalente(const lista_mosse &) const;

  bool empty() const;
  unsigned size() const;
  
  bool controlla() const;

private:
  mossa_e *const base;
  mossa_e *const stop;
  mossa_e       *sent;

  const bool cancella;
};

//  Function prototypes.
std::ostream &operator<<(std::ostream &, const lista_mosse &);

//-----------------------------------------------------------------------------
//  Lista_mosse:
//-----------------------------------------------------------------------------
inline lista_mosse::lista_mosse(size_t n) 
  : base(new mossa_e[n+1]), stop(base+n-1), sent(base), cancella(true)
{
  assert(n);
}

inline lista_mosse::lista_mosse(iterator b, iterator f) 
  : base(b), stop(f), sent(base), cancella(false)
{
  assert(b < f);
}

//-----------------------------------------------------------------------------
//  ~Lista_mosse:
//-----------------------------------------------------------------------------
inline lista_mosse::~lista_mosse()
{
  if (cancella) 
    delete [] base;
}

//-----------------------------------------------------------------------------
//  Begin:
//-----------------------------------------------------------------------------
inline lista_mosse::const_iterator lista_mosse::begin() const
{
  return base;
}

inline lista_mosse::iterator lista_mosse::begin()
{
  return base; 
}

//-----------------------------------------------------------------------------
//  End:
//-----------------------------------------------------------------------------
inline lista_mosse::const_iterator lista_mosse::end() const
{
  return sent;
}
inline lista_mosse::iterator lista_mosse::end()
{ 
  return sent; 
}

//-----------------------------------------------------------------------------
//  Operator[]:
//-----------------------------------------------------------------------------
inline const mossa_e &lista_mosse::operator[](unsigned n) const
{
  assert(n < size());
  return base[n]; 
}

//-----------------------------------------------------------------------------
//  Push_back:
//-----------------------------------------------------------------------------
inline void lista_mosse::push_back(posizione da, posizione a, pezzo mosso,
                                   pezzo preda, tipo_mossa t)
{ 
  assert(end() <= stop);
  *sent++ = mossa_e(da,a,mosso,preda,t);
}

inline void lista_mosse::push_back(const mossa_e &m)
{
  assert(end() <= stop);
  *sent++ = m; 
}

//-----------------------------------------------------------------------------
//  Erase:
//-----------------------------------------------------------------------------
inline void lista_mosse::erase(iterator i)
{
  assert(begin() <= i && i < end());
  *i = *(--sent);
}

//-----------------------------------------------------------------------------
//  Swap:
//-----------------------------------------------------------------------------
inline void lista_mosse::swap(iterator a, iterator b)
{
  assert(begin() <= a && a < end() && begin() <= b && b < end());
  
  const mossa_e t(*a);
  *a = *b;
  *b = t;
}

//-----------------------------------------------------------------------------
//  Clear:
//-----------------------------------------------------------------------------
inline void lista_mosse::clear()
{
  sent = base;
}

//-----------------------------------------------------------------------------
//  Empty:
//-----------------------------------------------------------------------------
inline bool lista_mosse::empty() const
{
  return end() == begin();
}

//-----------------------------------------------------------------------------
//  Size:
//-----------------------------------------------------------------------------
inline unsigned lista_mosse::size() const
{
  return end()-begin();
}

//-----------------------------------------------------------------------------
//  Sort: riordina le mosse della lista attuale in base ai valori contenuti
//        nell'array score (anche score viene riordinato); l'ordinamento e'
//        decrescente e l'algoritmo di ordinamento stabile.
//-----------------------------------------------------------------------------
template<class T>
void
lista_mosse::sort(T score[])
{
  // L'algoritmo di riordino utilizzato e' il selection sort.
  for (iterator i(begin()); i != end(); i++)
  {
    iterator mx(i);

    // Individua la mossa di valore massimo.
    for (iterator j(i+1); j != end(); j++)
      if ( score[j-begin()] > score[mx-begin()] )
        mx = j;

    // Scambia i valori delle mosse...
    const T tmp(score[i-begin()]);
    score[i-begin()]  = score[mx-begin()];
    score[mx-begin()] =               tmp;

    // ... e le mosse.
    swap(i,mx);
  }
}

}  // Namespace Joker.

#endif  // LISTA_MOSSE_H
