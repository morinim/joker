/*
 *  Colore.h
 */

#if !defined(COLORE_H)
#define      COLORE_H

#include <cassert>
#include <ctype.h>
#include <iostream>
#include "configurazione.h"

namespace Joker
{

typedef bool colore;

const colore nero   = 0;
const colore bianco = 1;

//  Function prototypes per funzioni friend.
void inverti(colore *);



//-----------------------------------------------------------------------------
//  Inverti:
//-----------------------------------------------------------------------------
inline void inverti(colore *c)
{ 
  *c ^= 1;
}
 
}  // Namespace Joker.

#endif  // COLORE_H
