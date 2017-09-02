/*
 *  San.h
 */

#if !defined(SAN_H)
#define      SAN_H

#include "configurazione.h"
#include "situazione.h"

namespace Joker
{

class san
{
public:
  static std::string a(situazione &, mossa);
  static bool da(situazione &, const std::string &, mossa_e *);
  static std::string lista(const situazione &, const lista_mosse &, char);

private:
  static pezzo id(char);
};

}  // Namespace Joker.

#endif  // SAN_H
