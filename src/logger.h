/*
 *  Logger.h
 */

#if !defined(LOGGER_H)
#define      LOGGER_H

#include <fstream>
#include <string>
#include "configurazione.h"
#include "componenti_valutazione.h"

namespace Joker
{

class logger
{
public:
  enum eccezioni {impossibile_aprire_file};

  template<class T> logger &operator<<(const T &);

  void stato_finale(double, unsigned long);
  void stato_iniziale(double, const componenti_valutazione &);

  void stato(bool) throw(eccezioni);

  bool controlla() const;

  static logger &instance();

private:
  explicit logger(const char []=STANDARD_LOG, bool=false) throw(eccezioni);

  std::ofstream out;
  std::string  nome;
  bool       attivo;

  static logger _instance;
};

//-----------------------------------------------------------------------------
//  Operator<<:
//-----------------------------------------------------------------------------
template<class T>
inline
logger &
logger::operator<<(const T &s)
{
  if (attivo)
    out << s;
  return *this;
}

//-----------------------------------------------------------------------------
//  Operator<<:
//-----------------------------------------------------------------------------
inline
logger &
logger::instance()
{
  return _instance;
}

}  // Namespace Joker.

#endif  // LOGGER_H
