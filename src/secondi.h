/*
 *  Secondi.h
 */

#if !defined(SECONDI_H)
#define      SECONDI_H

#include <cassert>
#include <iostream>
#include <string>
#include "configurazione.h"
#include "nonstd.h"

namespace Joker
{

//  Function prototypes per funzioni friend.
class secondi;
std::ostream &operator<<(std::ostream &, secondi);



class secondi
{
public:
  enum eccezioni {argomenti};

  secondi(double s = 0.0) : sec(s) { assert(controlla()); };
  secondi(const std::string &) throw(eccezioni);

  secondi &operator=(double);
  operator double() const;
  unsigned centisec() const;

  secondi operator+(secondi) const;
  secondi operator-(secondi) const;

  void operator+=(secondi);
  void operator-=(secondi);

  bool controlla() const;

private:
  double sec;
};

//-----------------------------------------------------------------------------
//  Operator=:
//-----------------------------------------------------------------------------
inline secondi &secondi::operator=(double s)
{
  sec = s;
  return *this;
}

//-----------------------------------------------------------------------------
//  Operator double:
//-----------------------------------------------------------------------------
inline secondi::operator double() const
{
  return sec;
}

//-----------------------------------------------------------------------------
//  Centisec:
//-----------------------------------------------------------------------------
inline unsigned secondi::centisec() const
{
  return static_cast<unsigned>(100.0*sec);
}

//-----------------------------------------------------------------------------
//  Operator+:
//-----------------------------------------------------------------------------
inline secondi secondi::operator+(secondi s1) const
{
  return secondi(sec+s1.sec);
}

//-----------------------------------------------------------------------------
//  Operator -:
//-----------------------------------------------------------------------------
inline secondi secondi::operator-(secondi s1) const
{
  return secondi(sec-s1.sec);
}

//-----------------------------------------------------------------------------
//  Operator+=:
//-----------------------------------------------------------------------------
inline void secondi::operator+=(secondi s1)
{
  sec += s1.sec;
}

//-----------------------------------------------------------------------------
//  Operator-=:
//-----------------------------------------------------------------------------
inline void secondi::operator-=(secondi s1)
{
  sec -= s1.sec;
}

//-----------------------------------------------------------------------------
//  Controlla:
//-----------------------------------------------------------------------------
inline bool secondi::controlla() const
{
  return true;
} 

}  // Namespace Joker.

#endif  // SECONDI_H
