/*
 *  Buffer_libro.cc
 */

#include "buffer_libro.h"

namespace Joker
{

//-----------------------------------------------------------------------------
//  Buffer_libro:
//-----------------------------------------------------------------------------
buffer_libro::buffer_libro(const char nf[])
  throw(Joker::buffer_libro::eccezioni)
{
  std::ifstream in(nf,std::ios::in|std::ios::binary);
  
  clear();
  
  if (in.is_open() && !in.eof())
  {
    size_t dim;

    if (in >> dim)
      for (size_t n(0); n < dim; n++)
        try
          { insert(nodo_apertura(in)); }
        catch (Joker::nodo_apertura::eccezioni)
          { throw formato; }          
  }
}

//-----------------------------------------------------------------------------
//  Insert:
//-----------------------------------------------------------------------------
void
buffer_libro::insert(const nodo_apertura &na)
{
  const std::pair<chiave_hash,info> nuovo( na.chiave(),info(na.giocate(),
                                                            na.punteggio()) );
  std::pair<map_bl::iterator,bool> p(buffer.insert(nuovo));

  if (!p.second)  // L'elemento e' gia' presente.
  {
    const map_bl::iterator elem(p.first);

    elem->second.val += na.punteggio();

    ++elem->second.n;
  }
}

//-----------------------------------------------------------------------------
//  Controlla:
//-----------------------------------------------------------------------------
bool
buffer_libro::controlla() const
{
  for (map_bl::const_iterator p(buffer.begin()); p != buffer.end(); p++)
    if (!p->second.val.controlla())
      return false;

  return true;
}

//-----------------------------------------------------------------------------
//  Operator<<:
//-----------------------------------------------------------------------------
bool
buffer_libro::write(std::ostream &o) const
{
  o << std::setw(10) << buffer.size() << '\n';

  for (map_bl::const_iterator p(buffer.begin()), sent(buffer.end());
       p != sent; 
       p++)
  {
    valutazione val(p->second.val);

    if (val > 10)
      val = +10;
    else if (val < -10)
      val = -10;

    o << nodo_apertura(p->first,p->second.n,val);
  }

  return true;
}

}  // Namespace Joker.
