/*
 *  T_ricerca.cc
 */

#include <cstdlib>
#include "inizializza.h"
#include "test.h"

int main(int argc, char *const argv[])
{
  Joker::inizializzatore::avvio();

  const unsigned prof(argc==2 ? atoi(argv[1]) : 8);

  Joker::test tst("../src/test/input/prove.epd",
                  "../src/test/output/t_ricerca.txt");

  if (tst)
  {
    const Joker::parametri_ricerca pr(false,false);

    while(tst.esegui(3600.0,prof,pr,false))
      ;
  
    tst.stampa_risultati();
  }

  return tst ? EXIT_SUCCESS : EXIT_FAILURE;
}
