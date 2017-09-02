/*
 *  Secondi.cc
 */

#include "secondi.h"

namespace Joker
{

//-----------------------------------------------------------------------------
//  Secondi: questa funzione converte in secondi una stringa del tipo: min:sec.
//           Bisogna fare attenzione al fatto che per specificare, ad esempio,
//           un tempo di 3 secondi, bisogna scrivere "0:3" e non "3" (la
//           seconda stringa viene interpretata come 3 minuti).
//-----------------------------------------------------------------------------
secondi::secondi(const std::string &str) throw(Joker::secondi::eccezioni) 
  : sec(0.0)
{
  unsigned m(0), s(0), *ptr = &m;

  for (unsigned i(0); i < str.size(); i++)
    if (isdigit(str[i]))
      *ptr = 10*(*ptr) + static_cast<int>(str[i]-'0');
    else if (str[i] == ':')
      ptr = &s;
    else
      throw argomenti;

  sec = m*60+s;
  assert(controlla());
}

//-----------------------------------------------------------------------------
//  Operator<<:
//-----------------------------------------------------------------------------
std::ostream &operator<<(std::ostream &o, secondi s)
{
  const unsigned ore( static_cast<unsigned>(s/3600) );
  if (ore)
  {
    s -= ore*3600;
    o << ore << ':';
  }
  
  const unsigned minuti( static_cast<unsigned>(s/60) );
  if (minuti)
  {
    s -= minuti*60;
    o << minuti << ':';
  }

  const unsigned sec( static_cast<unsigned>(s) );
  s -= sec;
  o << sec;

  const unsigned csec(s.centisec());
  if (csec)
    o << '.' << csec;

  return o;
}

}  // Namespace Joker.
