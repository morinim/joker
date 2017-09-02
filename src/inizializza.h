/*
 *  Inizializza
 */

#if !defined(INIZIALIZZA_H)
#define      INIZIALIZZA_H

#include "chiave_hash.h"

namespace Joker
{

class inizializzatore : public bitboard, public chiave_hash
{
public:
  static void avvio();
};

}  // Namespace Joker.

#endif  // INIZIALIZZA_H
