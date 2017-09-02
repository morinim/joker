/*
 *  Partita.cc
 */

#include "partita.h"

namespace Joker
{

//-----------------------------------------------------------------------------
//  Partita:
//-----------------------------------------------------------------------------
partita::partita(const parametri_ricerca &pr1) 
  : pr(pr1), mosse(sup_mosse), gt(0), g(0)
{
  new_game();
}

//-----------------------------------------------------------------------------
//  Gioca:
//-----------------------------------------------------------------------------
void
partita::gioca()
{
  bool fine(false);
  std::string comando(""), args[] = {"", "", "", "", "", ""};

  do
    if (interfaccia::instance().leggi_input(comando,args))
    {
      if (comando == "analyze")
        analyze();
      else if (comando == "computer")
        computer();
      else if (comando == "exit" || comando == "force")
        force();
      else if (comando == "go")
        go();
      else if (comando == "level")
        level(args);
      else if (comando == "name")
        name(args[0]);
      else if (comando == "new")
        new_game();
      else if (comando == "playother")
        playother();
      else if (comando == "quit")
        fine = true;
      else if (comando == "remove")
        remove();
      else if (comando == "result")
        result(args[0]);
      else if (comando == "setboard")
      {
        std::istringstream ist(args[0]+" "+args[1]+" "+args[2]+" "+args[3]);
        situazione s;
        
        ist >> s;
        
        new_game(s);
      }
      else if (comando == "undo")
        undo();
      else if (comando == "usermove")
        usermove(args[0]);
    }
  while (!fine);
}

//-----------------------------------------------------------------------------
//  Fine_partita:
//-----------------------------------------------------------------------------
bool
partita::fine_partita(valutazione v) const
{
  const colore c(s.mossa_a());

  esito e(sconosciuto);

  if (v.e_limitato() && v <= -valore[torre] && c==joker)
    e = (c==nero) ? resa_nero : resa_bianco;
  else if (v.e_matto() && v.matto_in() == 0)
  {        
    if ( (c ==   nero && v.e_matto_inflitto()) || 
         (c == bianco &&   v.e_matto_subito()) )
      e =   vince_nero;
    else
      e = vince_bianco;
  }
  else if (v.e_patta() && g->mosse_pv().empty())
    e = patta;

  if (e != sconosciuto)
    interfaccia::instance().stampa_esito(e);
  
  return e != sconosciuto;
}

//-----------------------------------------------------------------------------
//  Calcola:
//-----------------------------------------------------------------------------
void
partita::calcola()
{
  assert( (stato == attivo && joker == s.mossa_a()) || stato == analizza );

  if (stato == attivo)
  {
    const secondi riflessione(gt->tempo_calcolo());
    assert(riflessione >= gestore_tempo::tempo_minimo);
    
    const valutazione v(g->analizza(riflessione,10000));
    assert(v.e_matto() || v.e_patta() || g->mosse_pv().size());

    if (!fine_partita(v))
    {
      const mossa_e suggerita(*g->mosse_pv().begin());

      interfaccia::instance().stampa_mossa(suggerita.str());
      muovi(suggerita);
    }

    gt->aggiorna(g->tempo<statistico::trascorso>());
  }
  else  // stato == analizza.
    g->analizza(10000.0,10000);
}

//-----------------------------------------------------------------------------
//  Muovi:
//-----------------------------------------------------------------------------
void
partita::muovi(const mossa_e &m)
{
  assert(s.legale(m));

  mosse.push_back(m);
  s.muovi(m,rimuovi[mosse.size()]);
}

//-----------------------------------------------------------------------------
//  Analyze:
//-----------------------------------------------------------------------------
void
partita::analyze()
{
  stato = analizza;
  calcola();
}

//-----------------------------------------------------------------------------
//  Computer:
//-----------------------------------------------------------------------------
void
partita::computer()
{
  computer_avversario = true;
}

//-----------------------------------------------------------------------------
//  Force:
//-----------------------------------------------------------------------------
void
partita::force()
{
  stato = inattivo;
}

//-----------------------------------------------------------------------------
//  Go:
//-----------------------------------------------------------------------------
void
partita::go()
{
  stato =      attivo;
  joker = s.mossa_a();

  calcola();
}

//-----------------------------------------------------------------------------
//  Level:
//-----------------------------------------------------------------------------
void
partita::level(const std::string arg[])
{
  delete gt;
  gt = new gestore_tempo(arg[0],arg[1],arg[2]);
}

//-----------------------------------------------------------------------------
//  Name:
//-----------------------------------------------------------------------------
void
partita::name(const std::string &name)
{
  nome_avversario = name;
}

//-----------------------------------------------------------------------------
//  New_game:
//-----------------------------------------------------------------------------
void
partita::new_game(const situazione_valutata &s1)
{
  delete gt;
  delete  g;

  mosse.clear();

  nome_avversario     =                   "";
  elo_avversario      =                    0;
  computer_avversario =                false;
  stato               =               attivo;
  joker               =                 nero;
  s                   =                   s1;
  gt                  =  new gestore_tempo();
  g                   = new giocatore(&s,pr);
}

//-----------------------------------------------------------------------------
//  Playother:
//-----------------------------------------------------------------------------
void
partita::playother()
{
  stato =       attivo;
  joker = !s.mossa_a();
}

//-----------------------------------------------------------------------------
//  Remove:
//-----------------------------------------------------------------------------
void
partita::remove()
{
  undo();
  undo();
}

//-----------------------------------------------------------------------------
//  Result:
//-----------------------------------------------------------------------------
void
partita::result(const std::string &r)
{
  esito e(sconosciuto);

  if (r == "1-0")
    e = vince_bianco;
  else if (r == "0-1")
    e = vince_nero;
  else
    e = patta;

  if (e != sconosciuto)
    libro_apertura::instance().aggiorna_variante(mosse,joker,elo_avversario,
                                                 computer_avversario,e);
}

//-----------------------------------------------------------------------------
//  Undo:
//-----------------------------------------------------------------------------
void
partita::undo()
{
  s.ripristina(*(mosse.end()-1),rimuovi[mosse.size()]); 
  mosse.erase(mosse.end()-1);
}

//-----------------------------------------------------------------------------
//  Usermove:
//-----------------------------------------------------------------------------
void
partita::usermove(const std::string &mstr)
{
  mossa_e tmp;

  if (san::da(s,mstr,&tmp))
  {
    muovi(tmp);
    calcola();
  }
  else
    interfaccia::instance().stampa_errore(interfaccia::mossa_illegale,mstr);
}

//-----------------------------------------------------------------------------
//  Controlla:
//-----------------------------------------------------------------------------
bool
partita::controlla() const
{
  return mosse.controlla() && s.controlla() && gt && gt->controlla() && 
         g && g->controlla() && interfaccia::instance().controlla();
}

}  // Namespace Joker.
