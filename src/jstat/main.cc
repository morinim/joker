/*
 *  Main.cc
 */

#include <climits>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include "configurazione.h"
#include "get_opt.h"
#include "inizializza.h"
#include "interfaccia.h"
#include "situazione_valutata.h"



enum operazione {aiuto, esamina, errore};



//-----------------------------------------------------------------------------
// Variabili di stato.
//-----------------------------------------------------------------------------
bool quiet        = false;
const char *suite =     0;
 


//-----------------------------------------------------------------------------
// Funzioni.
//-----------------------------------------------------------------------------
bool esamina_suite();
void help_linea_comando();
operazione setta_opzioni(int, char *const []);
void stampa_intestazione();



//-----------------------------------------------------------------------------
//  Esamina_suite:
//-----------------------------------------------------------------------------
bool esamina_suite()
{
  std::ifstream ifs(suite);
  if (!ifs)
    return false;
  
  // Definizione ed inizializzazione delle variabili in cui si registrano le 
  // informazione riguardanti l'analisi.
  const unsigned vdim(4);
    
  // Valori minimi e massimi della somma delle componenti di valutazione
  // posizionale suddivisi per fase di gioco.
  Joker::valutazione v_sum_min[3][vdim], v_sum_max[3][vdim];
  // Valori minimi e massimi delle singole componenti di valutazione suddivisi
  // per fase di gioco.
  Joker::valutazione v_min[3][vdim], v_max[3][vdim];
  for (unsigned j1(0); j1 < 3; j1++)
    for (unsigned j2(0); j2 < vdim; j2++)
    {
      v_sum_min[j1][j2] = +Joker::inf;
          v_min[j1][j2] = +Joker::inf;
      v_sum_max[j1][j2] = -Joker::inf;
          v_max[j1][j2] = -Joker::inf;
    }

  Joker::situazione_valutata s;
  for (unsigned n(0); s.carica(ifs); n++)
  {
    if (!(quiet || n%10))
      std::cout << n << '\r' << std::flush;

    // Controllo iniziale.
    if (!s.controlla())
    {
      std::cerr << "Broken position (" << n << ").\n";
      return false;
    }
    const Joker::situazione_valutata back(s);

    // Esecuzione dell'analisi vera e propria.
    Joker::valutazione v[vdim];
    size_t ind(0);

    Joker::componenti_valutazione componenti;  
    s.elementi_di_valutazione(componenti);
    Joker::componenti_valutazione::const_iterator i1(componenti.begin());

    // Notare che si evita la prima componente (nome della fase di gioco).
    while (++i1 != componenti.end() && ind < vdim)
    {
      v[ind] = std::atoi(i1->second.c_str());

      if (v[ind] < v_min[s.fase_gioco()][ind])
        v_min[s.fase_gioco()][ind] = v[ind];
      if (v[ind] > v_max[s.fase_gioco()][ind])
        v_max[s.fase_gioco()][ind] = v[ind];

      ++ind;
    }

    // Si calcola, da prima, il vettore delle somme parziali relativo alla
    // posizione attuale...
    Joker::valutazione v_sum[vdim+1];  v_sum[vdim] = 0;
    for (unsigned t(1); t <= vdim; t++)
      v_sum[vdim-t] = v[vdim-t] + v_sum[vdim-t+1];
    
    // ... e, quindi, lo si confronta con gli analoghi vettori delle somme
    // parziali minime e massime.
    for (unsigned i2(0); i2 < vdim; i2++)
    {
      if (v_sum[i2] < v_sum_min[s.fase_gioco()][i2])
        v_sum_min[s.fase_gioco()][i2] = v_sum[i2];
      if (v_sum[i2] > v_sum_max[s.fase_gioco()][i2])
        v_sum_max[s.fase_gioco()][i2] = v_sum[i2];
    }

    // Controlla finale.
    if (!s.controlla() || s != back)
    {
      std::cerr << "Internal error: broken position after search (" << n 
                << ").\n";
      return false;
    }
  }

  std::cout << "***************************\n";
  std::cout << "* Static analysis results *\n";
  std::cout << "***************************\n";
    
  for (unsigned i1(0); i1 < 3; i1++)
  {
    assert(v_sum_min[i1] <= v_sum_max[i1]);

    std::cout << "\nPHASE: " 
              << (i1==0 ? "OPENING" : (i1==1 ? "MIDDLEGAME" : "ENDING"))
              << ".\n";
    std::cout << "Min overall positional valutation: " << std::setw(5) 
              << v_sum_min[i1][1] << ".\n";
    std::cout << "Max overall positional valutation: " << std::setw(5)
              << v_sum_max[i1][1] << ".\n";


    std::cout << "  Components:\n";
    for (unsigned i2(0); i2 < vdim; i2++)
    {
      assert(v_min[i1][i2] <= v_max[i1][i2]);

      std::cout << "    valutation[" << i2 << "] min: " << std::setw(5) 
                << v_min[i1][i2] << ", max: " << std::setw(5) << v_max[i1][i2]
                << ".\n";
    }

    std::cout << "  Partial sums\n";
    for (unsigned i3(1); i3 < vdim; i3++)
      std::cout << "    " << vdim-i3 << " components: min=" << std::setw(5) 
                << v_sum_min[i1][i3] << ", max=" << std::setw(5) 
                << v_sum_max[i1][i3] << ".\n";
  }
  
  return true;
}

//-----------------------------------------------------------------------------
//  Help_linea_comando:
//-----------------------------------------------------------------------------
void help_linea_comando()
{
  stampa_intestazione();

  std::cout << "Available command line options (read jstat.doc for a detailed description):\n\n";

  std::cout << "-help            write up this screen;\n";
  std::cout << "-quiet               no info messages;\n";


  std::cout << "\njstat [options] epdsuite\n\n";
}

//-----------------------------------------------------------------------------
//  Setta_opzioni:
//-----------------------------------------------------------------------------
operazione setta_opzioni(int argc, char *const argv[])
{
  static const opzione opz[] =
  {
    opzione("help",false,0),
    opzione("quiet",false,1),
    opzione("",false,0)
  };

  int c;
  unsigned ind;

  while ((c = get_opt(argc,argv,opz,ind)) != -1)
    switch (c)
    {
    case 0:  // Help.
      return aiuto;

    case 1:  // Quiet.
      quiet = true;
      break;

    default:  // Errore.
      return errore;
    }

  // Controlla se manca l'argomento obbligatorio...
  if (!(suite = argv[argc-1]))
    return errore;
      
  // ... e se e' corretto.
  if (std::string(suite).find(".epd") == std::string::npos)
     return errore;

  return esamina;
}

//-----------------------------------------------------------------------------
//  Stampa_intestazione:
//-----------------------------------------------------------------------------
void stampa_intestazione()
{
  std::cout << "JStat version " << VERSIONE << " (" << __DATE__ << ")\n";
  std::cout << "by Manlio Morini\n\n";
}



int main(int argc, char *const argv[])
{
  Joker::inizializzatore::avvio();

  switch (setta_opzioni(argc,argv))
  {
  case aiuto:
    help_linea_comando();
    return EXIT_SUCCESS;

  case esamina:
    if (!quiet)
      stampa_intestazione();
    
    return esamina_suite() ? EXIT_SUCCESS : EXIT_FAILURE;

  default:
    help_linea_comando();
    return EXIT_FAILURE;
  }
}
