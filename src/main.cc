/*
 *  Main.cc
 */

#include <sstream>
#include "configurazione.h"
#include "get_opt.h"
#include "inizializza.h"
#include "partita.h"
#include "test.h"

using namespace Joker;

enum risultato_scansione {errore_scansione, aiuto_linea_comando, 
                          modalita_interattiva, esecuzione_test,
                          esecuzione_test_completo, effettuazione_controllo};

//-----------------------------------------------------------------------------
//  Variabili di stato.
//-----------------------------------------------------------------------------
const char *test_file                    =                                   0;
secondi tempo_per_test                   =                             10000.0;
unsigned profondita_per_test             =                               10000;
parametri_ricerca p_ricerca;



//-----------------------------------------------------------------------------
// Funzioni.
//-----------------------------------------------------------------------------
void esegui_test(bool);
bool file_esistente(const char []);
void gioco_interattivo();
void help_linea_comando(std::ostream &);
risultato_scansione setta_opzioni(int, char *const []);
void stampa_intestazione(std::ostream &);



//-----------------------------------------------------------------------------
//  File_esistente:
//-----------------------------------------------------------------------------
bool file_esistente(const char nome[])
{
  std::ifstream test(nome);

  return !(!test);
}

//-----------------------------------------------------------------------------
//  Help_linea_comando:
//-----------------------------------------------------------------------------
void help_linea_comando(std::ostream &o)
{
  stampa_intestazione(o);

  o << "\nAvailable command line options:\n\n";

  o << "-aspiration 1..inf                    set aspiration search window;\n";
  o << "-book on|off                             set opening book use mode;\n";
  o << "-book_mode learn|random|tournament              opening book usage;\n";
  o << "-full_test filename.epd       run a suite of positions (slow mode);\n";
  o << "-help                                         write up this screen;\n";
  o << "-log on|off                               enable/disable log files;\n";
  o << "-sd 1..inf                                        set search depth;\n";
  o << "-self_check                            perform an engine self test;\n";
  o << "-st 1..inf                                         set search time;\n";
  o << "-test filename.epd            run a suite of positions (fast mode);\n";

  o << "\njoker [options]\n\n";
}

//-----------------------------------------------------------------------------
//  Setta_opzioni:
//-----------------------------------------------------------------------------
risultato_scansione setta_opzioni(int argc, char *const argv[])
{
  static const opzione opz[] =
  {
    opzione("help",false,0),
    opzione("sd",true,1),
    opzione("st",true,2),
    opzione("test",true,3),
    opzione("log",true,4),
    opzione("book",true,5),
    opzione("book_mode",true,6),
    opzione("full_test",true,7),
    opzione("self_check",false,8),
    opzione("aspiration",true,9),
    opzione("",false,0)
  };

  int c;
  risultato_scansione r(modalita_interattiva);
  unsigned ind;

  while ((c = get_opt(argc,argv,opz,ind)) != -1)
    switch (c)
    {
    case 0:  // Help.
      r = aiuto_linea_comando;
      break;

    case 1:  // Sd.
      profondita_per_test = atoi(arg_corrente);
      break;

    case 2:  // St.
      tempo_per_test = atoi(arg_corrente);
      break;

    case 3:  // Test.
      r         = esecuzione_test;
      test_file =    arg_corrente;
      break;

    case 4:  // Log.
      logger::instance().stato(std::string(arg_corrente) == "on");
      break;

    case 5:  // Book.
      p_ricerca.libro_attivo = (std::string(arg_corrente) == "on");
      break;

    case 6:  // Book_mode.
      if (std::string(arg_corrente) == "random")
        p_ricerca.libro_modalita =       libro_apertura::casuale;
      else if (std::string(arg_corrente) == "tournament")
        p_ricerca.libro_modalita =        libro_apertura::torneo;
      else
        p_ricerca.libro_modalita = libro_apertura::apprendimento;

      break;
      
    case 7:  // Full_test.
      r         = esecuzione_test_completo;
      test_file =             arg_corrente;
      break;

    case 8:  // Self_check.
      r = effettuazione_controllo;
      break;

    case 9: // Aspiration.
      p_ricerca.finestra = atoi(arg_corrente);
      break;

    default: // Help od opzione non riconosciuta.
      return errore_scansione;
    };

  return r;
}

//-----------------------------------------------------------------------------
//  Stampa_intestazione:
//-----------------------------------------------------------------------------
void stampa_intestazione(std::ostream &o)
{
  const std::string obk(p_ricerca.libro_attivo ? "enabled" : "disabled");

#if defined(ASSEMBLER)
  std::string blt("assembler");
#else
  std::string blt("C++ only");
#endif

  std::string srh("");
#if defined(ASPIRATION_SEARCH)
  srh += "aspirationSearch ";
#endif
#if defined(LAZY_EVAL)
  srh += "lazyEval ";
#endif
#if defined(NULL_MOVE)
  srh += "nullMove ";
#endif
#if defined(VALIDATION_SEARCH)
  srh += "validationSearch";
#endif

  o << "Joker " << VERSIONE << " (" << __DATE__ << ") by Manlio Morini\n";
  o << "Built: " << blt << ", cpu(" << MICROPROCESSORE << "), hash("
    << HASH_BIT << "), e_cache(" << E_CACHE_BIT << ")\n";
  o << "Opening book: " << obk << " (mode " << p_ricerca.libro_modalita 
    << ")\n";
  o << "Search: " << srh << "\n\n";
  o.flush();
}

//-----------------------------------------------------------------------------
//  Esecuzione_test:
//-----------------------------------------------------------------------------
void esegui_test(bool veloce)
{
  const std::string ris(std::string(test_file)+".ris");
  test tst(test_file,ris.c_str());

  if (tst)
  {
    const parametri_ricerca backup(p_ricerca);
    p_ricerca.estendi_analisi = false;
    p_ricerca.libro_attivo    = false;
    
    while( tst.esegui(tempo_per_test,profondita_per_test,p_ricerca,veloce) )
      ;

    p_ricerca = backup;
  
    tst.stampa_risultati();
  }
}

//-----------------------------------------------------------------------------
//  Gioco_interattivo:
//-----------------------------------------------------------------------------
void gioco_interattivo()
{
  partita pt(p_ricerca);

  pt.gioca();
}

int main(int argc, char *const argv[])
{
  inizializzatore::avvio();

  int ret(0);

  switch (setta_opzioni(argc,argv))
  {
  case aiuto_linea_comando:
    help_linea_comando(std::cout);
    break;

  case esecuzione_test:
    esegui_test(true);
    break;

  case esecuzione_test_completo:
    esegui_test(false);
    break;

  case modalita_interattiva:
    gioco_interattivo();
    break;

  default:
    help_linea_comando(std::cerr);
    ret = -1;
  }
 
  return ret;
}
