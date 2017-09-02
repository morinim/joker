/*
 *  Mossa.cc
 */

#include "mossa.h"

namespace Joker
{

const mossa mossa::sentinella(0,0);

//-----------------------------------------------------------------------------
//  Mossa::mossa: inizializza la classe a partire dalla rappresentazione
//                testuale di una mossa (es. e2e4, g2g1q). Questo e' l'unico
//                costruttore della classe che puo' sollevare eccezioni.
//-----------------------------------------------------------------------------
mossa::mossa(const char s[])
  : m(0)
{
  // Si controlla da prima l'esattezza degli argomenti...
  for (unsigned i(0); i <= 3; i++)
  {
    if (!s[i])
      return;

    if (i&1)
    {
      if (!isdigit(s[i]) || s[i] == '0')
        return;
    }
    else
    {
      if (s[i] != 'a' && s[i] != 'b' && s[i] != 'c' && s[i] != 'd' &&
          s[i] != 'e' && s[i] != 'f' && s[i] != 'g' && s[i] != 'h')
        return;
    }
  }

  const posizione da(s[0]-'a',s[1]-'1');
  const posizione  a(s[2]-'a',s[3]-'1');
  
  // ... e quindi esegue il costruttore vero e proprio.
  if (da != a)  
    m = (da << 6) | a;
}

//-----------------------------------------------------------------------------
//  Mossa::operator<<:
//-----------------------------------------------------------------------------
std::ostream &
operator<<(std::ostream &o, mossa m)
{
  return o << m.da().str() << m.a().str();
}

//-----------------------------------------------------------------------------
//  Mossa_e::str:
//-----------------------------------------------------------------------------
std::string mossa_e::str() const
{
  std::string ret(da().str() + a().str());

  return e<promozione>() ? ret+simbolo[promozione_a()] : ret;
}

//-----------------------------------------------------------------------------
//  Mossa_e::controlla:
//-----------------------------------------------------------------------------
bool
mossa_e::controlla() const
{
  if ( e<cattura_normale>() && (!preda() || preda()==re) )
    return false;
  if ( e<en_passant>() && preda() != pedone )
    return false;
  if ( (e<arrocco>() || e<doppio_p>()) && preda() )
    return false;
  if ( (e<di_pedone>() || e<promozione>()) && mosso() != pedone)
    return false;

  return mossa::controlla() && tipo_mossa::controlla();
}

}  // Namespace Joker.
