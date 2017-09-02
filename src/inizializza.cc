/*
 *  Inizializza.cc
 */

#include "inizializza.h"

//-----------------------------------------------------------------------------
//  Avvio:
//-----------------------------------------------------------------------------
void Joker::inizializzatore::avvio()
{
  static bool prima_chiamata(true);

  assert(prima_chiamata);

  if (prima_chiamata)
  {
    bitboard::calcola_array();
    chiave_hash::calcola_array();
    pezzi::calcola_array();

    prima_chiamata = false;
  }
}
