/*
 *  T_vgm.cc
 */

#include <cstdlib>
#include <ctime>
#include <fstream>
#include "inizializza.h"
#include "situazione.h"

const unsigned ntest(200000);

//-----------------------------------------------------------------------------
//  Controllo_effettuazione_mosse:
//-----------------------------------------------------------------------------
bool
controllo_effettuazione_mosse(Joker::situazione &s, 
                              const Joker::lista_mosse &lm)
{
  Joker::dati_r r;

  for (Joker::lista_mosse::const_iterator i(lm.begin()); i != lm.end(); i++)
    if (s.muovi(*i,r))
    {
      if (!s.controlla())
        return false;
      s.ripristina(*i,r);
    }

  return true;
}

//-----------------------------------------------------------------------------
//  Controlla_legalita:
//-----------------------------------------------------------------------------
bool
controlla_legalita(const Joker::situazione &s, const Joker::lista_mosse &lm)
{
  using namespace Joker;

  mossa_e tmp;
  unsigned trovate(0);

  for (posizione da(a1); !da.fuori(); ++da)
    for (posizione a(a1); !a.fuori(); ++a)
      if (da != a && s.pseudo_legale(mossa(da,a),&tmp))
      {
        if (!lm.find(tmp))
          return false;
        ++trovate;
      }

  return lm.size() == trovate;
}

//-----------------------------------------------------------------------------
//  Controllo:
//-----------------------------------------------------------------------------
void
controllo(const Joker::situazione &back, Joker::situazione &s,
          const Joker::lista_mosse &lm, unsigned cont, bool legalita)
{
  if (!lm.controlla())
  {
    std::cerr << "Controllo della lista mosse della posizione " << cont 
              << " fallito.\n";
    exit(-1);
  }

  if (legalita && !controlla_legalita(s,lm))
  {
    std::cerr << "Controllo della legalita' delle mosse fallito.\n";
    exit(-1);
  }

  if (!controllo_effettuazione_mosse(s,lm))
  {
    std::cerr << "Controllo effettuazione mosse fallito.\n";
    exit(-1);
  }

  if (!s.controlla() || s != back)
  {
    std::cerr << "Controllo finale posizione " << cont << " fallito.\n";
    exit(-1);
  }
}

int
main()
{
  Joker::inizializzatore::avvio();

  std::ifstream pf("../src/test/input/prove.epd",std::ios::in);
  if (!pf)
  {
    std::cerr << "Impossibile aprire il file prove.epd\n";
    return EXIT_FAILURE;
  }

  std::ofstream of("../src/test/output/t_vgm.txt",std::ios::out);
  if (!of)
  {
    std::cerr << "Impossibile aprire il file t_vgm.txt\n";
    return EXIT_FAILURE;
  }

  Joker::situazione s, back;
  Joker::lista_mosse lm(Joker::lista_mosse::max_varianti);
  clock_t t1, t2;
  double tt1(0.0), tt2(0.0), mt1(0.0), mt2(0.0);

  unsigned cont(1);

  while (s.carica(pf))
  {
    back = s;

    if (!s.controlla())
    {
      std::cerr << "Controllo iniziale della posizione " << cont 
                << " fallito.\n";
      return EXIT_FAILURE;
    }

    // Velocita' di calcolo delle mosse normali.
    t1 = clock();
    for (unsigned i(0); i < ntest; i++)
    {
      lm.clear();
      s.calcola(&lm);
    }
    t2 = clock();

    const double nm1(lm.size()*ntest);
    const double dt1(t2>t1 ? (double(t2) - double(t1)) / double(CLOCKS_PER_SEC)
                           : 1/double(CLOCKS_PER_SEC));

    tt1 += dt1;
    mt1 += nm1;

    controllo(back,s,lm,cont,true);

    // Velocita' di calcolo delle mosse di cattura.
    t1 = clock();
    for (unsigned i(0); i < ntest; i++)
    {
      lm.clear();
      s.calcola_catture(&lm);
    }
    t2 = clock();

    const double nm2(lm.size()*ntest);
    const double dt2(t2>t1 ? (double(t2) - double(t1)) / double(CLOCKS_PER_SEC)
                           : 1/double(CLOCKS_PER_SEC));

    tt2 += dt2;
    mt2 += nm2;

    // Stampa informazioni riepilogative.
    std::cout << "Posizione " << std::setw(2) << cont << ": " 
              << std::setw(8) << static_cast<unsigned long>(nm1/dt1) 
              << " m/s, " 
              << std::setw(8) << static_cast<unsigned long>(nm2/dt2)
              << " c/s\n";
    of << "Posizione " << std::setw(2) << cont << ": " 
       << std::setw(8) << static_cast<unsigned long>(nm1/dt1) << " m/s, "
       << std::setw(8) << static_cast<unsigned long>(nm2/dt2) << " c/s\n";

    controllo(back,s,lm,cont,false);

    ++cont;
  }

  std::cout << "\n--------------------------------------------------\n";
  std::cout << "Velocita' media: " 
            << static_cast<unsigned long>(mt1/tt1) << " m/s, " 
            << static_cast<unsigned long>(mt2/tt2) << " c/s\n";
  std::cout << "Media mosse: " 
            << static_cast<unsigned>(mt1/(ntest*(cont-1))) << " m/pos, "
            << static_cast<unsigned>(mt2/(ntest*(cont-1))) << " c/pos\n";

  of << "\n--------------------------------------------------\n";
  of << "\nVelocita' media: " 
     << static_cast<unsigned long>(mt1/tt1) << " m/s, " 
     << static_cast<unsigned long>(mt2/tt2) << " c/s\n";
  of << "Media mosse: "
     << static_cast<unsigned>(mt1/(ntest*(cont-1))) << " m/pos, "
     << static_cast<unsigned>(mt2/(ntest*(cont-1))) << " c/pos\n";

  return EXIT_SUCCESS;
}
