/*
 *  Fase.h
 */

#if !defined(FASE_H)
#define      FASE_H

#include "configurazione.h"
#include "situazione.h"

namespace Joker
{
 
class fase
{
public:
  enum fs_val {apertura, mediogioco, finale};

  explicit fase(fs_val);
  explicit fase(const situazione &);

  operator unsigned() const;
  std::string str() const;

  bool controlla() const;

private:
  fs_val f;
};

//-----------------------------------------------------------------------------
//  Fase:
//-----------------------------------------------------------------------------
inline
fase::fase(fs_val f1) 
  : f(f1)
{
  assert(controlla());
}

//-----------------------------------------------------------------------------
//  Operator unsigned:
//-----------------------------------------------------------------------------
inline
fase::operator unsigned() const
{
  return f;
}

//-----------------------------------------------------------------------------
//  Str:
//-----------------------------------------------------------------------------
inline
std::string
fase::str() const
{
  switch (f)
  {
  case fase::apertura:    return    "opening";
  case fase::mediogioco:  return "middlegame";
  default:                return    "endgame";
  }
}

//-----------------------------------------------------------------------------
//  Controlla:
//-----------------------------------------------------------------------------
inline
bool
fase::controlla() const
{
  return true;
}

}  // Namespace Joker.

#endif  // FASE_H
