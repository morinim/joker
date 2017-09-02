/*
 *  Main.cc
 */

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include "configurazione.h"
#include "get_opt.h"
#include "inizializza.h"
#include "libro_apertura.h"
#include "pgn_stream.h"



enum operazione {aiuto, aggiungi, albero, fondi, errore};



//-----------------------------------------------------------------------------
// Variabili di stato.
//-----------------------------------------------------------------------------
bool quiet           = false;
bool stat            =  true;
const char *bookfile =     0;
const char  *newfile =     0;
unsigned cut         =     0;



//-----------------------------------------------------------------------------
// Funzioni.
//-----------------------------------------------------------------------------
bool aggiungi_partite();
bool file_esistente(const char []);
void help_linea_comando();
operazione setta_opzioni(int, char *const []);
void stampa_albero();
void stampa_intestazione();
void stampa_livello(Joker::situazione &, const std::string &);



//-----------------------------------------------------------------------------
//  File_esistente:
//-----------------------------------------------------------------------------
bool
file_esistente(const char nome[])
{
  std::ifstream test(nome);

  return !test ? false : true;
}

//-----------------------------------------------------------------------------
//  Help_linea_comando:
//-----------------------------------------------------------------------------
void
help_linea_comando()
{
  stampa_intestazione();

  std::cout << "Available command line options (read jbook.doc for a detailed description):\n\n";

  std::cout << "-help                             write up this screen;\n";
  std::cout << "-add pgnfile   update the book on the basis of pgnfile;\n";
  std::cout << "-cut n                              cut games at ply n;\n"; 
  std::cout << "-nostat                  don't save opening statistics;\n";
  std::cout << "-quiet                                no info messages;\n";
  std::cout << "-tree  print the tree associated with the opening book.\n";

  std::cout << "\njbook [options] bookfile\n\n";
}

//-----------------------------------------------------------------------------
//  Setta_opzioni:
//-----------------------------------------------------------------------------
operazione
setta_opzioni(int argc, char *const argv[])
{
  static const opzione opz[] =
  {
    opzione("help",false,0),
    opzione("add",true,1),
    opzione("quiet",false,2),
    opzione("cut",true,3),
    opzione("tree",true,4),
    opzione("nostat",false,5),
    opzione("",false,0)
  };

  int c;
  operazione op(errore);
  unsigned ind;
  
  while ((c = get_opt(argc,argv,opz,ind)) != -1)
    switch (c)
    {
    case 0:  // Help.
      return aiuto;

    case 1:  // Add.
      newfile = arg_corrente;
      if (std::string(newfile).find(".pgn") == std::string::npos)
        return errore;

      // Controlla se manca l'argomento obbligatorio...
      if (!(bookfile = argv[argc-1]))
        return errore;
      
      // ... e se e' corretto.
      if (std::string(bookfile).find(".dat") == std::string::npos)
        return errore;

      op = aggiungi;
      break;

    case 2:  // Quiet.
      quiet = true;
      break;
      
    case 3:  // Cut.
      cut = atoi(arg_corrente);
      break;

    case 4:  // Tree.
      op = albero;
      break;

    case 5:  // Nostat.
      stat = false;
      break;

    default: // Help od opzione non riconosciuta.
      ;
    }

  return op;
}

//-----------------------------------------------------------------------------
//  Stampa_intestazione:
//-----------------------------------------------------------------------------
void
stampa_intestazione()
{
  std::cout << "JBook version " << VERSIONE << " (" << __DATE__ << ")\n";
  std::cout << "by Manlio Morini\n\n";
}

//-----------------------------------------------------------------------------
//  Stampa_albero:
//-----------------------------------------------------------------------------
void
stampa_albero()
{
  Joker::situazione s;
  stampa_livello(s,"");
}

//-----------------------------------------------------------------------------
//  Stampa_livello:
//-----------------------------------------------------------------------------
void
stampa_livello(Joker::situazione &s, const std::string &p)
{
  Joker::lista_mosse lm(Joker::lista_mosse::max_varianti);

  Joker::libro_apertura::instance().mosse_previste(s,&lm);

  for (Joker::lista_mosse::iterator i(lm.begin()); i != lm.end(); i++)
  {
    const std::string m(Joker::san::a(s,*i));
    const std::string comp(6-m.size(),' ');

    if (i == lm.begin())
      if (i+1 == lm.end())
        std::cout << "---";
      else
        std::cout << "-+-";
    else
      std::cout << " +-";
    
    std::cout << '(' << comp+m << ')';
          
    Joker::dati_r r;
    s.muovi(*i,r);
    if (i+1 != lm.end())
      stampa_livello(s,p+" |              ");
    else
      stampa_livello(s,p+"                ");
    s.ripristina(*i,r);

    std::cout << '\n' << p;
  }
}

//-----------------------------------------------------------------------------
//  Aggiungi_partite:
//-----------------------------------------------------------------------------
bool
aggiungi_partite()
{
  Joker::pgn_stream infile(newfile);
  if (!infile)
    return false;

  Joker::buffer_libro db(bookfile);
  for (unsigned long n(0); infile.read(&db,cut,stat); n++)
  {
    // Mostra, se permesso, alcune informazioni sull'operazione in corso.
    if (!quiet)
      std::cout << n << " games analyzed." << '\r' << std::flush;
  }
  std::cout << std::endl;

#if defined(__GNUC__)
  const std::ios::openmode om(std::ios::out);
#else
  const std::ios::openmode om(std::ios::out|std::ios::binary);
#endif

  std::ofstream out(bookfile,om);
  db.write(out);
    
  return infile.eof();
}



int
main(int argc, char *const argv[])
{
  Joker::inizializzatore::avvio();

  switch (setta_opzioni(argc,argv))
  {
  case aiuto:
    help_linea_comando();
    return EXIT_SUCCESS;

  case aggiungi:
    if (!quiet)
      stampa_intestazione();
      
    aggiungi_partite();
    return EXIT_SUCCESS;

  case albero:
    if (!quiet)
      stampa_intestazione();

    stampa_albero();
    return EXIT_SUCCESS;

  default:
    help_linea_comando();
    return EXIT_FAILURE;
  }
}
