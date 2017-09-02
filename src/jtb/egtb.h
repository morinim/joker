/*
 *  Egtb.h
 */

#if !defined(EGTB_H)
#define      EGTB_H

#include "configurazione.h"
#include "log_file.h"
#include "situazione.h"

#include "egtb_def.h"

namespace Joker
{

class egtb
{
public:
  egtb(char [] = "./TB");
  ~egtb();

  bool find(const situazione &, valutazione *) const;

  bool controlla() const;

private:
  void compila(const situazione &, int *const [2], square *const [2]) const;

  char        *cache;
  const unsigned num;
};

/*
 *  Controlla:
 */
inline bool egtb::controlla() const
{
  return true;
}

}  // Namespace Joker.

#endif  // EGTB_H
