/*
 *  Main.cc
 */

#include <cstdlib>
#include "configurazione.h"
#include "egtb.h"

/*
 *  Function prototype.
 */
bool trova_pv(char []);



/*
 *  Trova_pv:
 */
bool trova_pv(char n[])
{
  Joker::situazione s;
  if (!s.carica(n))
    return false;

  Joker::egtb tb;

  Joker::valutazione v(0);
  if (tb.find(s,&v))
  {
    std::cout << v << std::endl;
  }
  else
    std::cout << "Posizione non trovata" << std::endl;

  return true;
}

int main(int argc, char *const argv[])
{
  if (argc < 2)
    return EXIT_FAILURE;

  Joker::Log::joker.stato(true);
 
  const bool ok(trova_pv(argv[1]));

  return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
