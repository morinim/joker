/*
 *  T_gm.cc
 */

#include <cstdlib>
#include <fstream>
#include "inizializza.h"
#include "lista_mosse.h"
#include "situazione_valutata.h"

using namespace Joker;

//-----------------------------------------------------------------------------
//  Controlla_relazioni_mosse: verifica che:
//                             - la funzione di calcolo delle mosse di cattura
//                               generi tutte e sole le mosse di cattura 
//                               semilegali (con l'esclusione delle mosse di
//                               cattura en passant e l'aggiunta delle
//                               promozioni);
//                             - la funzione di calcolo delle mosse non di
//                               cattura generi tutte e sole le mosse non di
//                               cattura semilegali (con l'aggiunta delle
//                               catture en passant e l'esclusione delle
//                               promozioni).
//                             - che tutte le mosse legali siano anche 
//                               semilegali.
//-----------------------------------------------------------------------------
bool controlla_relazioni_mosse(const lista_mosse &lmc,
                               const lista_mosse &lmn,
                               const lista_mosse &lms,
                               const lista_mosse &lml)
{
  size_t cont_c(0), cont_n(0);

  for (lista_mosse::const_iterator i(lms.begin()); i != lms.end(); i++)
    if (i->e<cattura_normale>() || i->e<promozione>())
    {
      if (lmc.find(*i))
        ++cont_c;
      else
        return false;
    }
    else
    {
      if (lmn.find(*i))
        ++cont_n;
      else
        return false;
    }      

  for (lista_mosse::const_iterator k(lml.begin()); k != lml.end(); k++)
    if (!lms.find(*k))
      return false;

  return cont_c == lmc.size() && cont_n == lmn.size();
}

//-----------------------------------------------------------------------------
//  Controlla_unicita: controlla che nella lista di mosse ogni mossa sia
//                     presente una sola volta.
//-----------------------------------------------------------------------------
bool controlla_unicita(const lista_mosse &lm)
{
  for (lista_mosse::const_iterator i(lm.begin()); i != lm.end(); i++)
    for (lista_mosse::const_iterator j(lm.begin()); j != lm.end(); j++)
      if (i != j && *i == *j)
        return false;

  return true;
}

//-----------------------------------------------------------------------------
//  Riordina: riordina le mosse secondo un criterio non noto all'utilizzatore.
//            Lo scopo di questa funzione e' di registrare le mosse secondo
//            un ordine indipendente da quello di generazione cosi' da
//            semplificare il processo di confronto fra liste di mosse.
//-----------------------------------------------------------------------------
void riordina(lista_mosse *lm)
{
  valutazione num[lista_mosse::max_varianti];

  for (lista_mosse::iterator i(lm->begin()); i != lm->end(); i++)
    num[i-lm->begin()] = (i->da() << 6) | i->a();

  lm->sort(num);
}

//-----------------------------------------------------------------------------
//  Registra_mosse:
//-----------------------------------------------------------------------------
bool registra_mosse(std::ofstream &of, situazione_valutata *s, 
                    lista_mosse *lmc, lista_mosse *lms, lista_mosse *lml)
{
  of << "\n---------------------------------------------------------\n"
     << *s
     << "\n---------------------------------------------------------\n";

  lista_mosse::iterator i(0);
  dati_r r;

  riordina(lmc);
  of << *lmc << '\n';
  for (i = lmc->begin(); i != lmc->end(); i++)
  {
    lista_mosse lmc1(lista_mosse::max_varianti);

    if (s->muovi(*i,r))
    {
      if (!s->controlla())
        return false;

      s->calcola_catture(&lmc1);
      riordina(&lmc1);
      of << lmc1 << '\n';
      s->ripristina(*i,r);
    }
  }

  riordina(lms);
  of << *lms << '\n';
  for (i = lms->begin(); i != lms->end(); i++)
  {
    lista_mosse lms1(lista_mosse::max_varianti);

    if (s->muovi(*i,r))
    {
      if (!s->controlla())
        return false;

      s->calcola(&lms1);
      riordina(&lms1);
      of << lms1 << '\n';
      s->ripristina(*i,r);
    }
  }

  riordina(lml);
  of << *lml << '\n';
  for (i = lml->begin(); i != lml->end(); i++)
  {
    lista_mosse lml1(lista_mosse::max_varianti);

    if (!s->muovi(*i,r) || !s->controlla())
      return false;

    s->calcola_legali(&lml1);
    riordina(&lml1);
    of << lml1 << '\n';
    s->ripristina(*i,r);
  }

  of << '\n';
  return true;
}

int main()
{
  inizializzatore::avvio();

  std::ifstream pf("../src/test/input/prove.epd");
  if (!pf)
  {
    std::cerr << "Impossibile aprire il file prove.epd\n";
    return EXIT_FAILURE;
  }

  std::ofstream of("../src/test/output/t_gm.txt");
  if (!of)
  {
    std::cerr << "Impossibile aprire il file t_generazione_mosse.txt\n";
    return EXIT_FAILURE;
  }

  situazione_valutata s, back;
  lista_mosse lms(lista_mosse::max_varianti), lmc(lista_mosse::max_varianti),
              lmn(lista_mosse::max_varianti), lml(lista_mosse::max_varianti);

  unsigned cont(1);

  while (s.carica(pf))
  {
    back = s;

    std::cout << s << "\n\n";

    // Controlla l'integrita' delle strutture dati prima...
    if (!s.controlla())
    {
      std::cerr << "Controllo iniziale della posizione " << cont 
                << " fallito.\n";
      return EXIT_FAILURE;
    }

    s.calcola_catture(&lmc);
    s.calcola_non_catture(&lmn);
    s.calcola(&lms);
    s.calcola_legali(&lml);

    std::cout << "Catture (" << lmc.size() << "): " << lmc << "\n\n";
    std::cout << "Non catture (" << lmn.size() << "): " << lmn << "\n\n";
    std::cout << "Mosse legali (" << lml.size() << "): " << lml << "\n\n\n\n";

    registra_mosse(of,&s,&lmc,&lms,&lml);

    // ... e dopo il calcolo delle mosse.
    if (!lmc.controlla() || !controlla_unicita(lmc))
    {
      std::cerr << "Controllo della lista catture della posizione " << cont 
                << " fallito.\n";
      return EXIT_FAILURE;
    }

    if (!lmn.controlla() || !controlla_unicita(lmn))
    {
      std::cerr << "Controllo della lista 'non catture' della posizione " 
                << cont << " fallito.\n";
      return EXIT_FAILURE;
    }

    if (!lms.controlla() || !controlla_unicita(lms))
    {
      std::cerr << "Controllo della lista mosse semilegali della posizione " 
                << cont << " fallito.\n";
      return EXIT_FAILURE;
    }

    if (!lml.controlla() || !controlla_unicita(lml))
    {
      std::cerr << "Controllo della lista mosse legali della posizione " 
                << cont << " fallito.\n";
      return EXIT_FAILURE;
    }

    if (!controlla_relazioni_mosse(lmc,lmn,lms,lml))
    {
      std::cerr << "Inconsistenza fra mosse generate nella posizione " 
                << cont << ".\n";
      return EXIT_FAILURE;
    }

    if (!s.controlla() || s != back)
    {
      std::cerr << "Controllo finale della posizione " << cont 
                << " fallito.\n";
      return EXIT_FAILURE;
    }

    lms.clear();
    lmc.clear();
    lmn.clear();
    lml.clear();

    ++cont;
  }

  return EXIT_SUCCESS;
}
