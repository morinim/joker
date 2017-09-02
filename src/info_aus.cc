/*
 *  Info_aus.cc
 */

#include "info_aus.h"

namespace Joker
{

//  Questo array viene utilizzato per determinare le possibilita' di arrocco
//  dopo una mossa. Per far questo si effettua un & fra il campo arrocco della
//  struttura info_aus e le celle di questo array corrispondenti alla posizione
//  iniziale e finale della mossa.
const unsigned info_aus::maschera_arrocco[64] =
{
  13, 15, 15, 15, 12, 15, 15, 14,
  15, 15, 15, 15, 15, 15, 15, 15,
  15, 15, 15, 15, 15, 15, 15, 15,
  15, 15, 15, 15, 15, 15, 15, 15,
  15, 15, 15, 15, 15, 15, 15, 15,
  15, 15, 15, 15, 15, 15, 15, 15,
  15, 15, 15, 15, 15, 15, 15, 15,
   7, 15, 15, 15,  3, 15, 15, 11
};

//-----------------------------------------------------------------------------
//  Operator==:
//-----------------------------------------------------------------------------
bool
info_aus::operator==(const info_aus &i) const
{
  return mossa_a() == i.mossa_a() && p_en_passant() == i.p_en_passant() &&
         arrocco() == i.arrocco() && cinquanta() == i.cinquanta();
}

//-----------------------------------------------------------------------------
//  Controlla:
//-----------------------------------------------------------------------------
bool
info_aus::controlla() const
{
  return p_en_passant().controlla();
}

//-----------------------------------------------------------------------------
//  Operator>>:
//-----------------------------------------------------------------------------
std::istream &
operator>>(std::istream &is, info_aus &ia)
{
  int c(is.get());

  switch (c)
  {
  case 'b': ia.attivo =   nero;  break;
  case 'w': ia.attivo = bianco;  break;
  default:  is.clear(std::ios::badbit|is.rdstate()); return is;
  }

  is.get();

  ia.ar = 0;
  while (!isspace(c = is.get()))
    switch (c)
    {
    case 'K': ia.ar |=   info_aus::O_Ob; break;
    case 'Q': ia.ar |= info_aus::O_O_Ob; break;
    case 'k': ia.ar |=   info_aus::O_On; break;
    case 'q': ia.ar |= info_aus::O_O_On; break;
    default: ;
    };

  is >> ia.ep;

  return is;
}

//-----------------------------------------------------------------------------
//  Operator<<:
//-----------------------------------------------------------------------------
std::ostream &
operator<<(std::ostream &os, const info_aus &ia)
{
  std::string ar("");

  os << (ia.mossa_a() == nero  ? 'b' : 'w');

  if (ia.arrocco() &   info_aus::O_Ob) ar += 'K';
  if (ia.arrocco() & info_aus::O_O_Ob) ar += 'Q';
  if (ia.arrocco() &   info_aus::O_On) ar += 'k';
  if (ia.arrocco() & info_aus::O_O_On) ar += 'q';
  if (ar == "") ar = "-";
  os << ' ' << ar;

  os << ' ' << ia.p_en_passant();

  return os;
}

}  // Namespace Joker.
