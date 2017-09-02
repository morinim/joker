/*
 *  Giocatore.cc
 */

#include "giocatore.h"

namespace Joker
{

trasposizioni giocatore::tt;

//-----------------------------------------------------------------------------
//  Giocatore:
//-----------------------------------------------------------------------------
giocatore::giocatore(situazione_valutata *s1, const parametri_ricerca &pr1)
  : stack_top(stack_m+max_stack_m), sr(s1), 
    mosse_radice(max_mosse_radice), pv(supporto_ricerca::massima_profondita),
    pr(pr1), s(s1), delta(NODI_FRA_CONTROLLI), fuori_libro(0)
{
  assert(s1);
  
  clear(10);

  // La tabella hash, essendo condivisa fra le istanze della classe giocatore,
  // va ripulita ogni volta che si inizializza un nuovo giocatore.
  tt.clear();
}

//-----------------------------------------------------------------------------
//  Stampa_info:
//-----------------------------------------------------------------------------
inline
bool
giocatore::stampa_info() const
{
  return tempo<trascorso>() >= 1.0 || tempo<percentuale>() >= 0.1;
}

//-----------------------------------------------------------------------------
//  Stampa_pv:
//-----------------------------------------------------------------------------
void
giocatore::stampa_pv(valutazione v, char commento) const
{
  if (!pv.empty())
    interfaccia::instance().stampa_pv(get<iterazione>(),v,tempo<trascorso>(),
                                      get<nodi>(),san::lista(*s,pv,commento));
}

//-----------------------------------------------------------------------------
//  Clear:
//-----------------------------------------------------------------------------
void
giocatore::clear(secondi sec)
{
  logger::instance() << "ENGINE> clear\n";

  // Le informazioni statistiche vengono reinizializzate per prime perche' 
  // bisogna tener conto del tempo complessivo di ricerca.
  statistico::clear(sec);
  es.clear();
  mosse_radice.clear();
  pv.clear();
  sr.clear(s);

  delta = NODI_FRA_CONTROLLI;
  
  libro_apertura::instance().clear(pr.libro_attivo,pr.libro_modalita);
  
  // Se si cambia la fase di gioco si rende necessario azzerare i dati delle
  // tabelle di trasposizione (tutte le valutazioni non sono piu' corrette 
  // essendo cambiata la funzione di valutazione).
  if (s->aggiorna_fase_gioco())
  {
    logger::instance() << "ENGINE> clearing hash table\n";
    tt.clear();
  }
  else tt.inc_eta();

  dm0  = s->delta_materiale();
  val0 =                 +inf;

  logger::instance() << '\n' << scacchiera(*s);
  componenti_valutazione componenti;
  s->elementi_di_valutazione(componenti);
  logger::instance().stato_iniziale(sec,componenti);
}

//-----------------------------------------------------------------------------
//  Valutazione_cattura:
//-----------------------------------------------------------------------------
inline
valutazione
giocatore::valutazione_cattura(const mossa_e *m)
{
  assert(m->e<cattura>() || m->e<promozione>());

  valutazione v( m->e<promozione>() ? int(valore[m->promozione_a()]) : 
                                      int(              -m->mosso()) );

  return v + valore[m->preda()];
}

//-----------------------------------------------------------------------------
//  Muovi:
//-----------------------------------------------------------------------------
inline
bool
giocatore::muovi(const mossa_e *m)
{
  if (s->muovi(*m,sr.cep()->r))
  {
    sr.muovi(s);
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
//  Muovinull:
//-----------------------------------------------------------------------------
inline
void
giocatore::muovinull()
{ 
  s->muovinull(sr.cep()->r);
  sr.muovinull();
}

//-----------------------------------------------------------------------------
//  Ripristina:
//-----------------------------------------------------------------------------
inline
void
giocatore::ripristina(const mossa_e *m)
{
  sr.ripristina();
  s->ripristina(*m,sr.cep()->r);
}

//-----------------------------------------------------------------------------
//  Ripristinanull:
//-----------------------------------------------------------------------------
inline
void
giocatore::ripristinanull()
{ 
  sr.ripristinanull();
  s->ripristinanull(sr.cep()->r);
}

//-----------------------------------------------------------------------------
//  Nuovo_d:
//-----------------------------------------------------------------------------
inline
dettaglio
giocatore::nuovo_d(dettaglio d)
{
  enum {ext_avanzata =     unita,  ext_arrocco =   unita/2,
        ext_cattura  = 3*unita/4,  ext_forzata = 3*unita/4,
        ext_minaccia =     unita,  ext_scacco  =     unita};

  assert(sr.ply() >= 1);
  dettaglio delta(-unita);

  // La profondita' di ricerca effettiva non deve superare il doppio di quella
  // di base.
  if (sr.ply()*unita + d <= 2*unita*get<iterazione>())
  {
    ambiente_analisi *const cep = sr.cep();

    cep->attr |= tt.get<attributi>(s->chiave());
    if (cep->attr)
    {
      if (cep->attr & attributo::scacco)
        delta += ext_scacco;
      if (cep->attr & attributo::minaccia)
        delta += ext_minaccia;
      if (cep->attr & attributo::forzata)
        delta += ext_forzata;
    }

    lista_mosse::const_iterator ma = (cep-1)->m_attuale;
    if (ma->tipo_mossa::e<(cattura|promozione)>() && 
        dm0 == s->delta_materiale())
      delta += ext_cattura;

    /*lista_mosse::const_iterator mp = (cep-2)->m_attuale;
      if ( ma->tipo_mossa::e<(cattura)>() && 
         mp && mp->tipo_mossa::e<(cattura)>() &&
         ma->a() == mp->a() &&
         (valore[ma->preda()] == valore[mp->preda()] || 
          mp->tipo_mossa::e<promozione>()) )
         delta += ext_cattura;*/
            
    if ( ma->tipo_mossa::e<di_pedone>() && d <= unita &&
         (ma->a().traversa() <= 1 || ma->a().traversa() >= 6) )
      delta += ext_avanzata;

    if (ma->tipo_mossa::e<arrocco>())
      delta += ext_arrocco;

    // Le estensioni devono entrare in gioco solo quando si ha una sequenza di
    // posizioni interessanti.
    if (delta >= 0)
    {
      inc<estensioni>();
      delta = 0;
    }
  }

  return d+delta;
}

//-----------------------------------------------------------------------------
//  Ricerca:
//-----------------------------------------------------------------------------
inline
valutazione
giocatore::ricerca(valutazione alpha, valutazione beta, dettaglio d,
                   mossa_e *prima)
{
  return d < unita ? quiescenza(alpha,beta,prima)
                   : ricerca_standard(alpha,beta,d,prima);
}

//-----------------------------------------------------------------------------
//  Analizza: analizza la posizione corrente tenendo conto dei vincoli imposti
//            dal tempo massimo di 'sec' secondi e di 'profondita' massima di
//            analisi. Se l'argomento 'lm' e' un puntatore non nullo, 
//            interrompe l'analisi quando il motore suggerisce per due volte, 
//            consecutivamente, una mossa indicata in lm.
//-----------------------------------------------------------------------------
valutazione
giocatore::analizza(secondi sec, unsigned profondita, const lista_mosse *lm)
{
  assert(sec.controlla() && pr.controlla() && (!lm || lm->controlla()));

  clear(sec);

  // Si controlla se la posizione attuale e' memorizzata nel libro di apertura.
  if (++fuori_libro < 4)
  {
    mossa_e   tmp;
    valutazione v;

    if (libro_apertura::instance().mosse_previste(*s,&mosse_radice,&tmp,&v) &&
        v > -valore[pedone]/2)
    {
      fuori_libro = 0;
      pv.push_back(tmp);
      return v;
    }
  }

  mosse_radice.clear();
  s->calcola_tutte(&mosse_radice);

  // La posizione non e' nel libro di apertura, si avvia l'analisi.
  if (mosse_radice.size() > 1)
    profondita = MIN(profondita,supporto_ricerca::massima_profondita);
  else
    profondita = 5;
  
  bool pv_stampata(false);
  risultato_ricerca precedente(valutazione_esatta);
  unsigned conferme(0);
  valutazione stima_iniziale(-inf);
  valutazione alpha0(-inf), beta0(+inf);

  do
  {
    inc<iterazione>();

    // Bisogna ricordare che la funzione ricerca_radice modifica pv e val0
    // (rispettivamente variante principale e valutazione della posizione) solo
    // quando lo puo' fare "in sicurezza": il tempo non e' scaduto e il
    // risultato della ricerca e' una valutazione esatta. In questo modo 
    // sequenze di fail high e/o fail low, concluse dall'esaurimento del tempo
    // di ricerca o da inconsistenze non provocano la memorizzazione di valori
    // incoerenti.
    // Si osserva anche che si possono verificare quattro situazioni:
    // 1) scade il tempo, la funzione ricerca_radice ritorna tempo_scaduto;
    // 2) si verifica un fail high:
    //    2a) l'ultima ricerca aveva prodotto un fail low: inconsistenza, si
    //        ignora il fail high ed il successivo fail low;
    //    2b) l'ultima ricerca aveva prodotto un fail high: la posizione e'
    //        ancora migliore di quanto ci si aspettasse, si incrementa
    //        ulteriormente beta0.
    // 3) si verifica un fail low:
    //    3a) l'ultima ricerca aveva prodotto un fail low: la posizione e'
    //        peggiore di quanto si pensasse, si decrementa alpha0;
    //    3b) l'ultima ricerca aveva prodotto un fail high: inconsistenza, si
    //        ignora il fail low ed il successivo fail high.
    // 4) si ottiene una valutazione esatta con modifica di pv e val0.
    do
    {
      const risultato_ricerca attuale(ricerca_radice(alpha0,beta0,
                                                     &pv_stampata));
      switch (attuale)
      {
      case tempo_scaduto:
        assert(interrompi());
        break;

      case fail_high:
        if (stampa_info() && pv_stampata)
          interfaccia::instance().stampa_fail_high();

        if (precedente == fail_low)  // Anomalia di ricerca.
        {
          inc<anomalie>();

          alpha0 = -inf;
          beta0  = +inf;
        }
        else
        {
          alpha0 = beta0-1;
          if (precedente==valutazione_esatta)
            beta0 += valore[alfiere];
          else
            beta0 =             +inf;
        }
      
        break;
     
      case fail_low:
        if (stampa_info() && pv_stampata)
          interfaccia::instance().stampa_fail_low();

        if (precedente == fail_high)  // Anomalia di ricerca.
        {
          inc<anomalie>();
          alpha0 = -inf;
          beta0  = +inf;
        }
        else
        {
          beta0 = alpha0+1;

          if (precedente==valutazione_esatta)
            alpha0 -= valore[alfiere];
          else
            alpha0 =             -inf;
        }
        
        break;
        
      case valutazione_esatta:
        ;
      }
      precedente = attuale;

      // Gestione dell'estensione del tempo di ricerca.
      if (pr.estendi_analisi && stima_iniziale.e_limitato() && !interrompi())
      {
        const double sup_mul(6.0);
        double mul(1.0);

        if (attuale == valutazione_esatta)
        {
          valutazione perdita(stima_iniziale-val0);

          if (perdita > 0)
          {
            mul = 1.0 + 2.0*static_cast<double>(perdita)/valore[pedone];
            if (mul > sup_mul) mul = sup_mul;
          }
        }
        else if (attuale == fail_low)
          mul = sup_mul;

        aumenta_tempo(mul);

        if (mul > 1.0)
          logger::instance() << "ENGINE> extending search time (x" << mul 
                             << ")\n";
      }  
    } while (precedente != valutazione_esatta && !interrompi());

    alpha0 = val0 - pr.finestra;
    beta0  = val0 + pr.finestra;

    assert(s->controlla() && val0.controlla() && pv.controlla());

    if (!interrompi())
    {
      // In 'stima_iniziale' si memorizza il valore 'ovvio' della posizione. 
      // Questa valutazione viene confrontata con quelle restituite al 
      // proseguire dell'analisi: se ci si imbatte in valutazioni piu' basse
      // si sono presentati problemi imprevisti.
      if ( stima_iniziale == -inf && 
           (tempo<percentuale>() >= 0.1 || get<iterazione>() >= 5) )
      {
        stima_iniziale = val0;
        logger::instance() << "ENGINE> estimated value " << stima_iniziale
                           << '\n';
      }

      // Codice relativo alla gestione dei test.
      if (lm)
        if ( pv.empty() || !lm->find(*pv.begin()) )
          conferme = 0;
        else
          ++conferme;
    }
  } while (!interrompi() && !val0.e_matto() &&
           get<iterazione>() < profondita && conferme < 2);

  if (!pv_stampata)
    stampa_pv(val0);

  logger::instance().stato_finale(tempo<trascorso>(),get<hash_hit>());
  return val0;
}

//-----------------------------------------------------------------------------
//  Recupera_variante: in base alle informazioni memorizzate nella tabella
//                     delle trasposizioni, ricostruisce la miglior variante 
//                     che inizia con la mossa 'prima'. Notare che l'uso del
//                     parametro 'prima' permette di considerare anche mosse
//                     di promozione a pezzi minori.
//-----------------------------------------------------------------------------
void
giocatore::recupera_variante(const mossa_e &prima, lista_mosse *pv)
{
  pv->clear();
  
  dati_r r;
  situazione s1(*s);
 
  pv->push_back(prima);
  s1.muovi(prima,r);

  const unsigned sup( MIN(2*get<iterazione>(),
                          supporto_ricerca::massima_profondita) );
  for (unsigned ply(0); ply < sup; ply++)
  {
    const mossa m(tt.get<mossa>(s1.chiave()));
    mossa_e me;

    if (!m || !s1.legale(m,&me))
      break;
           
    pv->push_back(me);
    s1.muovi(me,r);
  }
}

//-----------------------------------------------------------------------------
//  Estrai_mossa:
//-----------------------------------------------------------------------------
bool
giocatore::estrai_mossa(lista_mosse *lm)
{
  ambiente_analisi *const cep = sr.cep();
  mossa_e me;

  ++cep->m_attuale;
  
  switch (cep->fase)
  {
  case mossa_hash:
    assert(!(cep->attr & attributo::scacco));
    cep->fase = generazione_catture;

    // Se abbiamo una mossa valida suggerita dalla hash table si tenta di
    // evitare la generazione della lista completa di mosse.
    if (cep->m_migliore != mossa::sentinella && 
        s->pseudo_legale(cep->m_migliore,&me))
    {
      lm->push_back(me);

      cep->m_attuale = lm->begin();
      return true;
    }

    // Se la mossa hash non e' valida si passa alla generazione catture.

  case generazione_catture:
    assert(!(cep->attr & attributo::scacco));
    cep->fase = mosse_cattura;
    
    lm->clear();
    s->calcola_catture(lm);

    cep->m_attuale = lm->begin();

    // Notare che si procede sempre al punto successivo.

  case mosse_cattura:
    assert(!(cep->attr & attributo::scacco));

    if (cep->m_attuale != lm->end())
    {
      lista_mosse::iterator migliore(cep->m_attuale);

      for (lista_mosse::iterator attuale(migliore+1); 
           attuale != lm->end();
           attuale++)
        if (valutazione_cattura(attuale) > valutazione_cattura(migliore))
          migliore = attuale;

      lm->swap(cep->m_attuale,migliore);
      return true;
    }

    // Se si sono esaurite le mosse di cattura si passa alle altre mosse.

  case mossa_killer0:
    assert(!(cep->attr & attributo::scacco));
    cep->fase = mossa_killer1;

    if ( es.killer<0>(sr.ply()) != mossa::sentinella &&
         s->pseudo_legale(es.killer<0>(sr.ply()),&me) )
    {
      lm->push_back(me);
      return true;
    }

  case mossa_killer1:
    assert(!(cep->attr & attributo::scacco));
    cep->fase = generazione_non_catture;

    if ( es.killer<1>(sr.ply()) != mossa::sentinella &&
         s->pseudo_legale(es.killer<1>(sr.ply()),&me) )
    {
      lm->push_back(me);
      return true;
    }
    
  case generazione_non_catture:
    assert(!(cep->attr & attributo::scacco));
    cep->fase = altre_mosse;

    s->calcola_non_catture(lm);

    // Notare che si procede sempre al punto successivo.

  case altre_mosse:
    if (cep->m_attuale != lm->end())
    {
      lista_mosse::iterator migliore(cep->m_attuale);
      for (lista_mosse::iterator attuale(migliore+1);
           attuale != lm->end();
           attuale++)
        if (es[*attuale] > es[*migliore])
          migliore = attuale;

      lm->swap(cep->m_attuale,migliore);
      return true;
    }

    return false;

  case mossa_hash_evasione:
    assert(cep->attr & attributo::scacco);
    cep->fase = generazione_evasioni;

    // Se abbiamo una mossa valida suggerita dalla hash table si tenta di
    // evitare la generazione della lista completa di mosse.
    if (cep->m_migliore != mossa::sentinella && 
        s->pseudo_legale(cep->m_migliore,&me))
    {
      lm->push_back(me);

      cep->m_attuale = lm->begin();
      return true;
    }

    // Se la mossa hash non e' valida si passa alla generazione evasioni.

  case generazione_evasioni:
    assert(cep->attr & attributo::scacco);
    cep->fase = mosse_evasione;
    
    lm->clear();
    s->calcola_legali(lm);
    
    //if (lm->size() <= 1)
    //  cep->attr |= attributo::forzata;

    //if (cep->m_migliore != mossa::sentinella)
    //  lm->erase(cep->m_migliore);

    cep->m_attuale = lm->begin();

    // Notare che si procede sempre al punto successivo.

  default:
    assert((cep->attr & attributo::scacco) && cep->fase == mosse_evasione);
    if (cep->m_attuale != lm->end())
    {
      lista_mosse::iterator migliore(cep->m_attuale);
      unsigned migliore_e_cattura(migliore->e<cattura>());
  
      for (lista_mosse::iterator attuale(migliore+1);
           attuale != lm->end();
           attuale++)
      {
        const unsigned attuale_e_cattura(attuale->e<cattura>());
      
        if (attuale_e_cattura)
          if (migliore_e_cattura)
          {
            if (valutazione_cattura(attuale) > valutazione_cattura(migliore))
              migliore = attuale;
          }
          else
          {
            migliore_e_cattura =    true;
            migliore           = attuale;
          }
        else if (!migliore_e_cattura && es[*attuale] > es[*migliore])
        migliore = attuale;
      } 

      lm->swap(cep->m_attuale,migliore);
      return true;
    }

    return false;
  }
}

//-----------------------------------------------------------------------------
//  Sposta_in_testa:
//-----------------------------------------------------------------------------
void
giocatore::sposta_in_testa(lista_mosse::iterator m)
{
  //for (lista_mosse::iterator i(m); i != mosse_radice.begin(); i--)
  //  mosse_radice.swap(i-1,i);

#if defined(_DEBUG)
  lista_mosse back;
  for (lista_mosse::const_iterator i(mosse_radice.begin());
       i != mosse_radice.end();
       i++)
    back.push_back(*i);
#endif

  lista_mosse::iterator i(mosse_radice.begin());
  do
    mosse_radice.swap(i,m);
  while (i++ != m);

#if defined(_DEBUG)
  assert(mosse_radice.equivalente(back));
#endif
}

//-----------------------------------------------------------------------------
//  Ricerca_radice:
//-----------------------------------------------------------------------------
giocatore::risultato_ricerca
giocatore::ricerca_radice(valutazione alpha, valutazione beta, 
                          bool *pv_stampata)
{
  assert(get<iterazione>() >= 1 && alpha < beta);

  inc<nodi_ricerca>();

  ambiente_analisi *const cep = sr.cep();

  if (mosse_radice.empty())
  {
    val0 = (cep->attr & attributo::scacco) ? -matto : 0;
    return valutazione_esatta;
  }
  if (sr.patta(s))
  {
    val0 = 0;
    return valutazione_esatta;
  }

  lista_mosse lm(stack_m,stack_top);
  const dettaglio d(get<iterazione>()*unita);

  for (cep->m_migliore=mossa::sentinella, cep->m_attuale=mosse_radice.begin();
       cep->m_attuale != mosse_radice.end();
       cep->m_attuale++)
  {
    muovi(cep->m_attuale);

    const dettaglio d1(nuovo_d(d));

    valutazione val;
    if (cep->m_attuale == mosse_radice.begin())
      val = -ricerca(-beta,-alpha,d1,lm.begin());
    else
    {
      val = -ricerca(-alpha-1,-alpha,d1,lm.begin());
      if (alpha < val && val < beta)
        val = -ricerca(-beta,-alpha,d1,lm.begin());
    }

    ripristina(cep->m_attuale);

    if (interrompi())
      return tempo_scaduto;

    if ( (cep->m_attuale == mosse_radice.begin() && val <= alpha) ||
         val > alpha )
    {
      recupera_variante(*cep->m_attuale,&pv);

      // Stampa informazioni sulla ricerca in corso.
      if (stampa_info())
      {
        char commento(0);
        if (val <= alpha)
          commento = '?';
        else if (val >= beta)
          commento = '!';

        stampa_pv(val,commento);

        *pv_stampata = true;
      }
    }

    if (val > alpha)
    {
      // Aggiorna la miglior mossa.
      cep->m_migliore = *cep->m_attuale;

      // Bisogna fare attenzione al fatto che, dopo questa istruzione,
      // l'espressione cep->m_attuale non indica piu' la migliore mossa sino
      // ad ora individuata; tale mossa puo' comunque essere indicata mediante
      // l'espressione mosse_radice.begin() o cep->m_migliore.
      sposta_in_testa(cep->m_attuale);
      assert(cep->m_migliore==*mosse_radice.begin());
      
      // Notare che non si effettua la memorizzazione nella tabella hash.
      if (val >= beta)
        return fail_high;
      
      val0 = alpha = val;
    }
  }

  // Notare che non si effettua la memorizzazione nella tabella hash.
  return !cep->m_migliore ? fail_low : valutazione_esatta;
}

//-----------------------------------------------------------------------------
//  Null_d: questa funzione viene chiamata solo nelle posizioni in cui e' 
//          giudicato opportuno effettuare una null move e serve per stabilire
//          il livello di riduzione della profondita' di analisi. Gli argomenti
//          della funzione sono, nell'ordine, il dettaglio prima della
//          riduzione, il numero di pezzi di uno dei due giocatori. La 
//          riduzione e' calcolata in base al metodo Adaptive Null Move Depth
//          Reduction (E.A. Heinz, "Adaptive Null-Move Pruning", ICCA Journal,
//          Vol. 22, No. 3).
//-----------------------------------------------------------------------------
inline
dettaglio
null_d(dettaglio d, unsigned n)
{
  assert(d > 0 && n && n <= 16);

  dettaglio delta(null_depth);

  if ( (d > 8*unita) || ((d > 6*unita) && n >= 3) )
    delta += unita;

  return d-delta;

  //return d - (d <= 6*unita || n <= 3 ? null_depth : null_depth+unita);
}

//-----------------------------------------------------------------------------
//  Ricerca_standard:
//-----------------------------------------------------------------------------
valutazione
giocatore::ricerca_standard(valutazione alpha, valutazione beta, dettaglio d,
                            mossa_e *prima)
{
  assert(d >= unita && alpha < beta);

  // Si controlla se e' il momento di terminare la ricerca solo ad intervalli
  // di delta nodi. In questo modo non si spreca tempo con chiamate a 
  // read_clock(). 
  if (--delta <= 0)
  {
    controllo_interruzione();
    if (interrompi())
      return inf;

    delta = NODI_FRA_CONTROLLI;
  }

  inc<nodi_ricerca>();

  if (sr.patta(s))
    return 0;

  ambiente_analisi *const cep = sr.cep();

  // Si controlla se la posizione e' memorizzata nella tabella hash.
  valutazione val;
  switch (tt.find(s->chiave(),d,beta,&val,&cep->m_migliore,&cep->attr))
  {
  case trasposizioni::esatta:
    inc<hash_hit>();
    return val;

  case trasposizioni::superiore:
    inc<hash_hit>();
    if (val < beta)
      if (val <= alpha)
        return val;
      else
        beta = val;
    break;

  case trasposizioni::inferiore:
    inc<hash_hit>();
    if (val > alpha)
      if (val >= beta)
        return val;
      else
        alpha = val;
    break;
  }

  // I margini ci sono ma la prudenza non e' mai troppa...
  if (sr.profondita_massima())
    return beta;
  
  // Si tenta ora una NULL MOVE: il giocatore cui spetta la mossa passa la
  // mano e la posizione risultante viene esaminata a profondita' ridotta
  // rispetto al normale; se il, notevole, vantaggio di due mosse consecutive
  // non porta la valutazione ad un livello superiore ad alfa, si taglia in
  // quanto la posizione e' disperata. Si soprassiede alla null move solo se
  // - il giocatore che deve muovere e' sotto scacco (altrimenti si avrebbe
  //   una posizione illegale);
  // - la mossa precedente era una null move (una ricerca composta da sole
  //   null move consecutive e' una non ricerca);
  // - il giocatore che deve muovere non ha pezzi (nei finali di soli pedoni
  //   lo zugzwang e' la norma) o il suo avversario non ha pezzi (questa
  //   condizione, utilizzata in Darkthought sembra ridurre le irregolarita'
  //   della ricerca);
  // - il giocatore che deve muovere e' in zugzwang (ed, allora, il passaggio
  //   di mano all'avversario non e' significativo).
  // Notare che se la concessione di una mossa all'avversario lo porta alla
  // scacco matto, questa posizione e' critica e necessita di un'analisi piu'
  // approfondita (l'idea e' di Bruce Moreland). Questa condizione e'
  // registrata nella hash table in modo da non passare inosservata in caso di
  // mancata effettuazione della null move.
#if defined(NULL_MOVE)
  const unsigned min_uff( MIN(s->n_ufficiali(nero),s->n_ufficiali(bianco)) );

  if ( (cep-1)->m_attuale &&
       !cep->attr &&
       min_uff > 1 &&
       (s->delta_materiale().relativa(s->mossa_a()) > alpha || d > unita) )
  {
    muovinull();
    const dettaglio d1(null_d(d,min_uff));
    valutazione vnull(-ricerca(-beta,-beta+1,d1,prima));
    ripristinanull();
    
#if defined(VALIDATION_SEARCH)
    if (vnull >= beta)
      vnull = ricerca(beta-1,beta,d1,prima);
#endif

    if (interrompi())  return inf;

    if (vnull >= beta)
    {
      assert(!interrompi());
      tt.insert(s->chiave(),d,trasposizioni::inferiore,mossa::sentinella,vnull,
                cep->attr);
      return vnull;
    }

    if (vnull.e_matto_subito())
      cep->attr |= attributo::minaccia;
  }

  cep->attr &= ~attributo::evita_null;
#endif  // NULL_MOVE

  const bool          scacco(cep->attr & attributo::scacco);
  lista_mosse                           lm(prima,stack_top);
  trasposizioni::limitazione tipo(trasposizioni::superiore);
  unsigned                                     esaminate(0);

  cep->fase = f_estrazione(scacco);
  while ( estrai_mossa(&lm) )
  {
    if (muovi(cep->m_attuale))
    {
      const dettaglio d1(nuovo_d(d));
      
      if (!esaminate)
        val = -ricerca(-beta,-alpha,d1,lm.end());
      else
      {
        val = -ricerca(-alpha-1,-alpha,d1,lm.end());

        if (alpha < val && val < beta)
          val = -ricerca(-beta,-alpha,d1,lm.end());
      }

      ripristina(cep->m_attuale);

      if (interrompi())  return inf;

      if (val > alpha)
      {
        if (val >= beta)
        {
          assert(!interrompi());
          es.incrementa(scacco,sr.ply(),d,*cep->m_attuale);
          tt.insert(s->chiave(),d,trasposizioni::inferiore,*cep->m_attuale,val,
                    cep->attr);
          return val;
        }

        alpha           =                   val;
        cep->m_migliore =       *cep->m_attuale;
        tipo            = trasposizioni::esatta;
      }

      ++esaminate;
    }
  }

  assert(!interrompi());

  if (!esaminate)
    return (cep->attr & attributo::scacco) ? sr.ply()-matto : 0;
  if (esaminate == 1)
    cep->attr |= attributo::forzata;

  tt.insert(s->chiave(),d,tipo,cep->m_migliore,alpha,cep->attr);
  return alpha;
}

//-----------------------------------------------------------------------------
//  Quiescenza: esegue una semplice ricerca di quiescenza nella situazione
//              attuale. La funzione si basa sulla versione piu' semplice
//              dell'algoritmo alpha-beta. L'unica estensione utilizzata e' il
//              riordino delle mosse da analizzare in base alla tecnica 
//              MVT-LVA: vengono infatti analizzate solo mosse di cattura
//              (e quindi niente scacchi). Questo modo di procedere,
//              unitamente al fatto che nel corpo della funzione non si fa
//              riferimento a strutture dati esterne limitate in profondita', 
//              garantisce che la catena di chiamate ricorsive avra' termine e
//              permette di evitare il controllo:
//                  if (sr.profondita_massima()) ....
//              necessario nella funzione di ricerca principale.
//-----------------------------------------------------------------------------
valutazione
giocatore::quiescenza(valutazione alpha, valutazione beta, mossa_e *prima)
{
  assert(alpha < beta);

  inc<nodi_quiescenza>();

#if defined(LAZY_EVAL)
  const valutazione v1(s->valutazione_posizione(alpha,beta));
#else
  const valutazione v1(s->valutazione_posizione(-inf,+inf));
#endif
  
  if (v1 > alpha)
  {
    if (v1 >= beta)
      return v1;

    alpha = v1;
  }

  // Calcola le mosse di cattura...
  const valutazione gap(alpha.e_limitato()
   ? alpha - valore[pedone] - s->delta_materiale().relativa(s->mossa_a())
   : 0);

  lista_mosse lm(prima,stack_top);
  s->calcola_catture(&lm);

  for (lista_mosse::iterator i(lm.begin()); i != lm.end(); i++)
  {
    // ... e le riordina.
    lista_mosse::iterator migliore(i);

    for (lista_mosse::iterator j(i+1); j != lm.end(); j++)
      if (valutazione_cattura(j) > valutazione_cattura(migliore))
        migliore = j;

    if (valutazione_cattura(migliore) < gap)
      break;

    if (muovi(migliore))
    { 
      const valutazione val(-quiescenza(-beta,-alpha,lm.end()));
      ripristina(migliore);

      //if (interrompi())  return inf;

      if (val > alpha)
      {
        if (val >= beta)
          return val;

        alpha = val;
      }
    }

    // Notare che lo scambio e' ritardato in modo da venir evitato in caso di
    // interruzione della ricerca.
    lm.swap(i,migliore);
  }

  return alpha;
}

//-----------------------------------------------------------------------------
//  Controlla:
//-----------------------------------------------------------------------------
bool
giocatore::controlla() const
{
  return s->controlla() && pr.controlla() && es.controlla() && 
         statistico::controlla() && pv.controlla() && sr.controlla() && 
         tt.controlla() && val0.controlla() && 
         interfaccia::instance().controlla();
}

}  // Namespace Joker.

