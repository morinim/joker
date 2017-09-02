/*
 *  Get_opt.cc
 */

#include "get_opt.h"

char *arg_corrente = 0;
unsigned opt_ind   = 1;

//-----------------------------------------------------------------------------
//  Get_opt: esamina la linea di comando come sotto Unix. Ritorna:
//           - il numero dell'opzione scelta, purche' non si riscontrino 
//             problemi;
//           -  -1 se sono terminati gli switch;
//           - ':' se viene individuato uno switch in cui manca un parametro;
//           - '?' nel caso di problemi diversi da quelli sopra descritti 
//             riscontrati durante la scansione della linea comando.
//-----------------------------------------------------------------------------
int
get_opt(int argc, char *const argv[], const opzione opz[], unsigned &indice)
{
  static unsigned pos(1);
  
  // Controlla se sono finite le opzioni.
  if (argc == 1 || !argv[pos] || argv[pos][0] != SW )
    return -1;

  // Controlla se il nome dell'opzione e' preceduto da uno o due SW e setta
  // opportunamente la variabile num_sw.
  const unsigned num_sw(argv[pos][1] == SW ? 2 : 1);

  // Cerca l'opzione attuale fra quelle specificate dall'utente.
  for (unsigned i(0); opz[i].nome != ""; i++)
    if (opz[i].nome == argv[pos]+num_sw)
    {
      ++pos;

      // Se l'opzione prevede un parametro ed il parametro e' presente sulla
      // riga comando, lo copia in arg_corrente; se e' assente ritorna
      // l'opportuno codice di errore.
      if (opz[i].argomento)
        if (argv[pos][0] == SW)
          return ':';
        else
          arg_corrente = argv[pos++];

      opt_ind = pos;
      indice  =   i;

      return opz[i].val;
    }

  return '?';
}
