/*
 *  Direzione.cc
 */

#include "direzione.h"



namespace Joker
{

//-----------------------------------------------------------------------------
//  Controlla:
//-----------------------------------------------------------------------------
bool controlla(direzione d)
{
  return d == alto || d == alto+destra || d == destra ||
         d == basso+destra || d == basso || d == basso+sinistra ||
         d == sinistra || d == alto+sinistra || d == alto+destra+destra ||
         d == basso+destra+destra || d == basso+basso+destra ||
         d == basso+basso+sinistra || d == basso+sinistra+sinistra ||
         d == alto+sinistra+sinistra || d == alto+alto+sinistra ||
         d == alto+alto+destra || d == d*destra;
}

}  // Namespace Joker.
