/*
 *  Random.h
 */

#if !defined(RANDOM_H)
#define      RANDOM_H

#include "configurazione.h"

namespace Joker
{
  namespace random
  {
    unsigned32 random32();
    unsigned64 random64();
  }
}

#endif  // RANDOM_H
