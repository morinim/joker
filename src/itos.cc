/*
 *  Itos.cc
 */

#include "itos.h"

//-----------------------------------------------------------------------------
//  Itos: converte l'intero num in una stringa che restituisce come valore 
//        della funzione.
//-----------------------------------------------------------------------------
std::string itos(long num)
{
  std::string ret("");

  // Gestione numeri negativi.
  if (num < 0)
  {
    ret = "-";
    num = -num;
  }

  // Individua la massima potenza di 10 minore od uguale a num.
  unsigned long potenza(1);
  while (num/(potenza*10))
    potenza *= 10;

  // Inserimento delle cifre nella stringa.
  while (potenza)
  {
    ret     += static_cast<char>('0' + num/potenza);
    num     %=                              potenza;
    potenza /=                                   10;
  }

  return ret;
}
