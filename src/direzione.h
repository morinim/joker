/*
 *  Direzione.h
 */

#if !defined(DIREZIONE_H)
#define      DIREZIONE_H

#include <cassert>
#include "configurazione.h"

namespace Joker
{

typedef int direzione;

const direzione alto     =                     -10;
const direzione destra   =                       1;
const direzione sinistra =                      -1;
const direzione basso    =                      10;
const direzione a_a_d    =        alto+alto+destra;
const direzione a_d      =             alto+destra;
const direzione a_d_d    =      alto+destra+destra;
const direzione b_d_d    =     basso+destra+destra;
const direzione b_d      =            basso+destra;
const direzione b_b_d    =      basso+basso+destra;
const direzione b_b_s    =    basso+basso+sinistra;
const direzione b_s      =          basso+sinistra;
const direzione b_s_s    = basso+sinistra+sinistra;
const direzione a_s_s    =  alto+sinistra+sinistra;
const direzione a_s      =           alto+sinistra;
const direzione a_a_s    =      alto+alto+sinistra;

//-----------------------------------------------------------------------------
//  Diagonale:
//-----------------------------------------------------------------------------
inline bool diagonale(direzione d)
{ 
  return d ==    alto+destra || d ==  basso+destra || 
         d == basso+sinistra || d == alto+sinistra; 
}

//-----------------------------------------------------------------------------
//  Opposte:
//-----------------------------------------------------------------------------
inline bool opposte(direzione d1, direzione d2)
{ 
  return !(d1+d2);
}

//-----------------------------------------------------------------------------
//  Inversa:
//-----------------------------------------------------------------------------
inline direzione inversa(direzione d)
{ 
  return -d;
}

bool controlla(direzione);

}  // Namespace Joker.

#endif  // DIREZIONE_H
