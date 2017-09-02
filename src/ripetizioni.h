/*
 *  Ripetizioni.h
 */

#if !defined(RIPETIZIONI_H)
#define      RIPETIZIONI_H

#include "configurazione.h"
#include "situazione.h"

namespace Joker
{

class ripetizioni
{
public:
  bool patta(const situazione *const , unsigned);
  
  bool controlla() const;

private:
  enum {sup = 256};

  static bool materiale_insufficiente(const situazione *const);
  static bool patta_50_mosse(const situazione *const);

  chiave_hash seq[sup];
};

//-----------------------------------------------------------------------------
//  Materiale_insufficiente:
//-----------------------------------------------------------------------------
inline
bool
ripetizioni::materiale_insufficiente(const situazione *const s)
{
  if (s->mappa(nero,pedone)|s->mappa(bianco,pedone))
    return false;

  if (s->mappa(  nero) == s->mappa(  nero,re))
    return !(s->mappa(bianco,donna)|s->mappa(bianco,torre)) &&
           s->mappa(bianco).count() <= 2;

  if (s->mappa(bianco) == s->mappa(bianco,re))
    return !(s->mappa(  nero,donna)|s->mappa(  nero,torre)) &&
           s->mappa(  nero).count() <= 2;

  return false;
}

//-----------------------------------------------------------------------------
//  Patta_50_mosse:
//-----------------------------------------------------------------------------
inline
bool
ripetizioni::patta_50_mosse(const situazione *const s)
{
  return s->cinquanta() > 99;
}

//-----------------------------------------------------------------------------
//  Patta:
//-----------------------------------------------------------------------------
inline
bool
ripetizioni::patta(const situazione *const s, unsigned ply)
{
  assert(ply < sup);

  const chiave_hash k(s->chiave());

  if (ply > 4)
  {
    for (unsigned i(ply); i; i--)
      if (seq[i-1] == k)
        return true;
  }
  else
  {
    unsigned cont(0);

    for (unsigned i(ply); i; i--)
      if (seq[i-1] == k)
        ++cont;

    if (cont > 1)
      return true;
  }

  if (patta_50_mosse(s))
    return true;

#if defined(MATERIAL_DRAW)
  if (materiale_insufficiente(s))
    return true;
#endif


  seq[ply] = k;
  return false;
}

//-----------------------------------------------------------------------------
//  Controlla:
//-----------------------------------------------------------------------------
inline
bool
ripetizioni::controlla() const
{
  return true;
}

}  // Namespace Joker.

#endif  // RIPETIZIONI_H
