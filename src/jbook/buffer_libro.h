/*
 *  Buffer_libro.h
 */

#if !defined(BUFFER_LIBRO_H)
#define      BUFFER_LIBRO_H

#include <map>
#include "configurazione.h"
#include "nodo_apertura.h"

namespace Joker
{

class buffer_libro
{
public:
  enum eccezioni {formato};

  buffer_libro();
  explicit buffer_libro(const char []) throw(eccezioni);

  void insert(chiave_hash, valutazione);
  void clear();

  bool write(std::ostream &) const;

  bool controlla() const;

private:
  void insert(const nodo_apertura &);

  struct info
  {
    info(size_t n1, valutazione val1) : n(n1), val(val1) {};

    size_t        n;
    valutazione val;
  };

  typedef std::map<chiave_hash,info> map_bl;

  map_bl buffer;
};

//  Function prototypes per funzioni friend.
std::ostream &operator<<(std::ostream &, const buffer_libro &);



//-----------------------------------------------------------------------------
//  Buffer_libro:
//-----------------------------------------------------------------------------
inline
buffer_libro::buffer_libro()
{
  clear();
}

//-----------------------------------------------------------------------------
//  Insert:
//-----------------------------------------------------------------------------
inline
void
buffer_libro::insert(chiave_hash k, valutazione v)
{
  insert(nodo_apertura(k,1,v));
}

//-----------------------------------------------------------------------------
//  Clear:
//-----------------------------------------------------------------------------
inline
void
buffer_libro::clear()
{
  buffer.clear();
}

}  // Namespace Joker.

#endif  // BUFFER_LIBRO_H
