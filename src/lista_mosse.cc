/*
 *  Lista_mosse.cc
 */

#include "lista_mosse.h"

namespace Joker
{

//-----------------------------------------------------------------------------
//  Operator=:
//-----------------------------------------------------------------------------
lista_mosse &
lista_mosse::operator=(const lista_mosse &lm)
{
  clear();

  for (const_iterator orig(lm.begin());
       orig != lm.end() && end() <= stop;
       orig++)
    push_back(*orig);

  return *this;
}

//-----------------------------------------------------------------------------
//  Equivalente:
//-----------------------------------------------------------------------------
bool
lista_mosse::equivalente(const lista_mosse &lm) const
{
  if (lm.size() == size())
  {
    const_iterator i(lm.begin());
    while (i != lm.end() && find(*i))  ++i;
    return i == lm.end();
  }

  return false;
}

//-----------------------------------------------------------------------------
//  Erase:
//-----------------------------------------------------------------------------
void
lista_mosse::erase(mossa m)
{
  iterator i(begin());

  while (i != end() && mossa(*i) != m)
    ++i;

  if (i != end())
    erase(i);
}

//-----------------------------------------------------------------------------
//  Find:
//-----------------------------------------------------------------------------
lista_mosse::const_iterator
lista_mosse::find(mossa m) const
{
  const_iterator i(begin());

  while (i != end() && mossa(*i) != m)
    ++i;

  return i != end() ? i : 0;
}

//-----------------------------------------------------------------------------
//  Controlla:
//-----------------------------------------------------------------------------
bool
lista_mosse::controlla() const
{
  for (const_iterator i(begin()); i != end(); i++)
    if (!i->controlla())
      return false;

  return base <= sent && base <= stop;
}

//-----------------------------------------------------------------------------
//  Operator<<:
//-----------------------------------------------------------------------------
std::ostream &
operator<<(std::ostream &o, const lista_mosse &l)
{
  for (lista_mosse::const_iterator i(l.begin()); i != l.end(); i++)
  {
    if (i != l.begin())
      o << ' ';
    o << *i;
  }

  return o;
}

}  // Namespace Joker.
