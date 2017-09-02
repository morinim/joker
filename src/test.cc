/*
 *  Test.cc
 */

#include "test.h"

namespace Joker
{

std::string test::token(std::string *s, tipo_token t)
{
  if (s->empty())
    return std::string("");

  char delimitatore(t==argomento ? ';' : ' ');

  // Salta eventuali delimitatori iniziali.
  unsigned inizio(0);
  while ( inizio < s->size() && (s->at(inizio)==' ' || s->at(inizio)==';') )
    ++inizio;

  // Identifica la fine del token.
  unsigned fine(inizio);
  while (fine < s->size() && s->at(fine) != delimitatore)
    ++fine;

  // Preleva il token e modifica la stringa argomento.
  std::string ret(s->substr(inizio,fine-inizio));
  s->erase(0,fine);
  return ret;
}

//-----------------------------------------------------------------------------
//  Analizza_codici_epd:
//-----------------------------------------------------------------------------
void test::analizza_codici_epd(situazione &s, std::string cmd, std::string *id,
                               lista_mosse *lm)
{
  std::string op(token(&cmd,tag));

  lm->clear();

  while (!op.empty())
  {
    if (op == "bm")              // **** BM ****
    {
      // Legge la sequenza di mosse migliori...
      std::string mosse(token(&cmd,argomento)), mossa_attuale("");
      for (size_t i(0); i < mosse.size(); i++)
      {
        // ... e la scompone nelle singole mosse.
        if (mosse[i] != ' ')
          mossa_attuale += mosse[i];

        if (mosse[i] == ' ' || i+1 == mosse.size())
        {
          mossa_e m;
          san::da(s,mossa_attuale,&m);
          lm->push_back(m);

          mossa_attuale = "";
        }
      }
    }
    else if (op[0] == 'c' && isdigit(op[1]))  // **** C1-C9 ****
      token(&cmd,argomento);
    else if (op == "id")                      // **** ID ****
      *id = token(&cmd,argomento);

    op = token(&cmd,tag);
  }
}

//-----------------------------------------------------------------------------
//  Esegui:
//-----------------------------------------------------------------------------
bool test::esegui(secondi tempo, unsigned profondita, 
                  const parametri_ricerca &pr, bool veloce)
{
  // Se non e' disponibile una suite di test e' inutile cominciare.
  if (!ift)
    return false;

  // Carica la posizione successiva, ne controlla la correttezza e ne crea una
  // copia di backup.
  std::string cmd;
  situazione_valutata s;
  if (!s.carica(ift,&cmd))
    return false;
  if (!s.controlla())
  {
    std::cerr << "Broken test position.\n";
    return false;
  }
  const situazione_valutata back(s);

  // Definizione delle variabili in cui si registrano le informazione
  // riguardanti il test attuale.
  lista_mosse migliori(lista_mosse::max_varianti);
  std::string id;
  analizza_codici_epd(s,cmd,&id,&migliori);

  std::cout << id << '\n';

  // Esecuzione del test vero e proprio.
  giocatore g(&s,pr);
  
  const valutazione v(g.analizza(tempo,profondita,veloce ? &migliori : 0));

  const mossa ms( g.mosse_pv().empty() ? mossa::sentinella
                                       : *g.mosse_pv().begin() );

  ris.push_back( risultato(id,migliori,ms,
                           g.get<statistico::nodi_ricerca>(),
                           g.get<statistico::nodi_quiescenza>(),
                           g.get<statistico::nps>(),
                           g.get<statistico::hash_hit>(),
                           g.get<statistico::estensioni>(),
                           g.get<statistico::anomalie>(),
                           g.get<statistico::iterazione>(),
                           g.tempo<statistico::trascorso>()) );

  // Visualizza alcune informazioni sulla ricerca corrente.
  std::cout << "Search: " << g.get<statistico::nps>() << " Nps, "
            << g.get<statistico::nodi_ricerca>() << " SNodes, " 
            << g.get<statistico::nodi_quiescenza>() << " QNodes, "
            << g.get<statistico::hash_hit>() << " HHits.\n";

  if (ms != mossa::sentinella)
    std::cout << "Hint move (" << v << "): " << ms 
              << (mossa_corretta(migliori,ms) ? '!' : '?') << '\n';
  std::cout << '\n';

  // Controlla la consistenza della situazione dopo l'effettuazione del test.
  if (!s.controlla() || s != back)
  {
    std::cerr << "Internal error: broken position after search.\n";
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------
//  Controlla:
//-----------------------------------------------------------------------------
bool test::controlla() const
{
  for (lista_risultati::const_iterator i(ris.begin()); i != ris.end(); i++)
    if (!i->controlla())
      return false;

  return interfaccia::instance().controlla();
}

//-----------------------------------------------------------------------------
//  Operator<<:
//-----------------------------------------------------------------------------
std::ostream &operator<<(std::ostream &o, const risultato &r)
{
  o.setf(std::ios::left);
  o << std::setw(10) << r.id.c_str();
  o.setf(std::ios::right);
  o << std::setw( 7) <<        r.nps;
  o << std::setw(10) <<      r.nodir;
  o << std::setw(10) <<      r.nodiq;
  o << std::setw(10) <<       r.hits;
  o << std::setw(10) << r.estensioni;
  o << std::setw( 5) <<   r.anomalie;

  o << "  ";
	
  if (r.suggerita != mossa::sentinella)
    o << r.suggerita;
  
  if (!r.corretta)
    o << '?';
  
  return o;
}

//-----------------------------------------------------------------------------
//  Stampa_risultati:
//-----------------------------------------------------------------------------
void test::stampa_risultati()
{
  unsigned corretti(0), prof(0), significativi(0), test_num(0);
  unsigned long anomalie(0), estensioni(0), hits(0), nr(0), nq(0), nps(0);
  secondi tempo(0.0);

  oft << "Joker " << VERSIONE << "\n\n";
  oft <<"Test          Nps  S. Nodes  Q. Nodes Hash hits      Exts S.A.  Hint\n";
  for (lista_risultati::const_iterator i(ris.begin()); i != ris.end(); i++)
  {
    ++test_num;

    corretti   +=   i->corretta;
    nr         +=      i->nodir;
    nq         +=      i->nodiq;
    hits       +=       i->hits;
    estensioni += i->estensioni;
    anomalie   +=   i->anomalie;
    prof       +=       i->prof;
    tempo      +=      i->tempo;

    // Tutte le posizioni in cui si sono analizzati pochi nodi non sono
    // significative.
    if (i->nodir > 5000)
    {
      ++significativi;
      nps +=   i->nps;
    }
    
    oft << *i << '\n';
  }

  const unsigned long nps_avg(significativi ? nps/significativi : 0);

  oft <<"-----------------------------------------------------------------------\n";
  oft << "       " << std::setw(10) << nps_avg << std::setw(10) << nr 
      << std::setw(10) << nq << std::setw(10) << hits << std::setw(10) 
      << estensioni << std::setw(5) << anomalie << "  " << corretti << '/' 
      << test_num << '\n';
  oft << "                   ------------------\n";
  oft << "                 " << std::setw(20) << (nr+nq) << "\n\n";

  oft << "Average depth:            " << std::setprecision(4)
      << static_cast<float>(prof)/test_num << '\n';
  oft << "Average time to solution: " << secondi(tempo/test_num) 
      << " (total time " << tempo << ")\n";

  oft << "Search anomalies:         " << anomalie << '\n';
}

}  // Namespace Joker.
