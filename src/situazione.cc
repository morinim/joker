/*
 *  Situazione.cc
 */

#include "situazione.h"


namespace Joker
{

//-----------------------------------------------------------------------------
//  Da_scacco:
//-----------------------------------------------------------------------------
bool
situazione::da_scacco(const mossa_e &m)
{
  bool ret(false);
  dati_r r;
  
  if (muovi(m,r))
  {
    ret = sotto_scacco();
    ripristina(m,r);
  }

  return ret;
}

//-----------------------------------------------------------------------------
//  Calcola_non_catture:
//-----------------------------------------------------------------------------
void
situazione::calcola_non_catture(lista_mosse *lm) const
{
  const colore c(mossa_a());
  const bitboard libere(~mappa());

  // En passant.
  bitboard bb(mappa(c,pedone));
  if (!p_en_passant().fuori())
  {
    posizione pos( p_en_passant() + (c==nero ? a_s : b_s) );
    if (bb[pos])
      lm->push_back(pos,p_en_passant(),pedone,pedone,
                    cattura|en_passant|di_pedone);

    pos = p_en_passant() + (c==nero ? a_d : b_d);
    if (bb[pos])
      lm->push_back(pos,p_en_passant(),pedone,pedone,
                    cattura|en_passant|di_pedone);
  }

  // Arrocchi.
  if (c == nero)
  {
    if ( (arrocco()&info_aus::O_On)
          && !(~libere & bitboard::raggio[e8][h8])
          && !attacca(bianco,e8) && !attacca(bianco,f8) )
      lm->push_back(e8,g8,re,vuoto,Joker::arrocco);
    if ( (arrocco()&info_aus::O_O_On)
          && !(~libere & bitboard::raggio[e8][a8])
          && !attacca(bianco,e8) && !attacca(bianco,d8) )
      lm->push_back(e8,c8,re,vuoto,Joker::arrocco);
  }
  else
  {
    if ( (arrocco()&info_aus::O_Ob)
          && !(~libere & bitboard::raggio[e1][h1])
          && !attacca(nero,e1) && !attacca(nero,f1) )
      lm->push_back(e1,g1,re,vuoto,Joker::arrocco);
    if ( (arrocco()&info_aus::O_O_Ob)
          && !(~libere & bitboard::raggio[e1][a1])
          && !attacca(nero,e1) && !attacca(nero,d1) )
      lm->push_back(e1,c1,re,vuoto,Joker::arrocco);
  }
  
  // Mosse di Pedone.
  // Le promozioni vengono considerate catture e quindi non generate.
  bb &= ~bitboard::traversa7[c];

  if (c == nero)
  {
    bitboard arrivi(avanza<nero>(bb) & libere);
    while (arrivi)
    {
      const posizione arrivo(arrivi.firstone());
      lm->push_back(arrivo+alto,arrivo,pedone,vuoto,di_pedone);
      arrivi.elimina(arrivo);
    }

    arrivi =    bb & bitboard::traversa[6];
    arrivi = avanza<nero>(arrivi) & libere;
    arrivi = avanza<nero>(arrivi) & libere;
    while (arrivi)
    {
      const posizione arrivo(arrivi.firstone());
      lm->push_back(arrivo+2*alto,arrivo,pedone,vuoto,doppio_p|di_pedone);
      arrivi.elimina(arrivo);
    }
  }
  else  // c == bianco
  {
    bitboard arrivi(avanza<bianco>(bb) & libere);
    while (arrivi)
    {
      const posizione arrivo(arrivi.firstone());
      lm->push_back(arrivo+basso,arrivo,pedone,vuoto,di_pedone);
      arrivi.elimina(arrivo);
    }

    arrivi =      bb & bitboard::traversa[1];
    arrivi = avanza<bianco>(arrivi) & libere;
    arrivi = avanza<bianco>(arrivi) & libere;
    while (arrivi)
    {
      const posizione arrivo(arrivi.firstone());
      lm->push_back(arrivo+2*basso,arrivo,pedone,vuoto,doppio_p|di_pedone);
      arrivi.elimina(arrivo);
    }
  }

  // Mosse di Cavallo.
  bb = mappa(c,cavallo);
  while (bb)
  {
    const posizione pos(bb.firstone());

    bitboard arrivi(mosse[cavallo][pos] & libere);
    while (arrivi)
    {
      const posizione arrivo(arrivi.firstone());
      lm->push_back(pos,arrivo,cavallo);
      arrivi.elimina(arrivo);
    }

    bb.elimina(pos);
  }  

  // Mosse di Alfiere, Torre, Donna.
  for (pezzo pz(alfiere); pz <= donna; ++pz)
  {
    bb = mappa(c,pz);
    while (bb)
    {
      const posizione pos(bb.firstone());
      
      for (const direzione *d = dir[pz]; *d; d++)
        for (posizione arrivo(pos + *d);
             !arrivo.fuori() && libere[arrivo];
             arrivo += *d)
          lm->push_back(pos,arrivo,pz,vuoto);
      
      bb.elimina(pos);
    }
  }
  
  // Mosse di Re.
  const posizione pos(mappa(c,re).firstone());

  bitboard arrivi(mosse[re][pos] & libere);
  while (arrivi)
  {
    const posizione arrivo(arrivi.firstone());
    lm->push_back(pos,arrivo,re);
    arrivi.elimina(arrivo);
  }
}

//-----------------------------------------------------------------------------
//  Calcola_catture: calcola e memorizza in lm tutte le mosse di cattura
//                   pseudolegali che il giocatore attivo puo' effetturare
//                   nella posizione corrente (per motivi di efficienza non si
//                   generano le catture en passant).
//-----------------------------------------------------------------------------
void
situazione::calcola_catture(lista_mosse *lm) const
{
  const colore c(mossa_a());
  const bitboard nemici(mappa(!c)), pezzi(mappa());
  
  // Catture/promozioni di Pedone.
  bitboard bb(mappa(c,pedone)), candidati(bb & bitboard::traversa7[c]);

  if (c==nero)
  {
    if (candidati)
    {
      candidati = avanza<nero>(candidati) & (~pezzi);
      while (candidati)
      {
        const posizione pos(candidati.firstone());
        lm->push_back(pos+alto,pos,pedone,vuoto,di_pedone|p_donna);
        candidati.elimina(pos);
      }
    }

    bitboard catture(attacchi_dx<nero>(bb) & nemici);
    while (catture)
    {
      const posizione arrivo(catture.firstone());
      const tipo_mossa tm(arrivo>h1 ? cattura|di_pedone
                                    : cattura|di_pedone|p_donna);

      lm->push_back(arrivo+a_s,arrivo,pedone,t_preda(bianco,arrivo),tm);

      catture.elimina(arrivo);
    }
    
    catture = attacchi_sx<nero>(bb) & nemici;
    while (catture)
    {
      const posizione arrivo(catture.firstone());
      const tipo_mossa tm(arrivo>h1 ? cattura|di_pedone
                                    : cattura|di_pedone|p_donna);

      lm->push_back(arrivo+a_d,arrivo,pedone,t_preda(bianco,arrivo),tm);

      catture.elimina(arrivo);
    }
  }
  else  // c==bianco
  {
    if (candidati)
    {
      candidati = avanza<bianco>(candidati) & (~pezzi);
      while (candidati)
      {
        const posizione pos(candidati.firstone());
        lm->push_back(pos+basso,pos,pedone,vuoto,di_pedone|p_donna);
        candidati.elimina(pos);
      }
    }

    bitboard catture(attacchi_dx<bianco>(bb) & nemici);
    while (catture)
    {
      const posizione arrivo(catture.firstone());
      const tipo_mossa tm(arrivo<a8 ? cattura|di_pedone 
                                    : cattura|di_pedone|p_donna);

      lm->push_back(arrivo+b_s,arrivo,pedone,t_preda(nero,arrivo),tm);

      catture.elimina(arrivo);
    }
    
    catture = attacchi_sx<bianco>(bb) & nemici;
    while (catture)
    {
      const posizione arrivo(catture.firstone());
      const tipo_mossa tm(arrivo<a8 ? cattura|di_pedone
                                    : cattura|di_pedone|p_donna);

      lm->push_back(arrivo+b_d,arrivo,pedone,t_preda(nero,arrivo),tm);

      catture.elimina(arrivo);
    }
  }

  // Catture di Cavallo.
  bb = mappa(c,cavallo);
  while (bb)
  {
    const posizione pos(bb.firstone());

    bitboard prede(mosse[cavallo][pos] & nemici);
    while (prede)
    {
      const posizione p_preda(prede.firstone());
      const pezzo preda(t_preda(!c,p_preda));

      lm->push_back(pos,p_preda,cavallo,preda,cattura);
      prede.elimina(p_preda);
    }

    bb.elimina(pos);
  }  

  // Catture di Alfiere, Torre, Donna.
  for (pezzo pz(alfiere); pz <= donna; ++pz)
  {
    bb = mappa(c,pz);
    while (bb)
    {
      const posizione pos(bb.firstone());

      const bitboard *const a = bitboard::raggio[pos];

      bitboard prede(mosse[pz][pos] & nemici);
      while (prede)
      {
        const posizione p_preda(prede.firstone());

        if ( !(a[p_preda] & pezzi) )
        {
          const pezzo preda(t_preda(!c,p_preda));

          lm->push_back(pos,p_preda,pz,preda,cattura);
        }
        
        prede.elimina(p_preda);
      }
      
      bb.elimina(pos);
    }
  }
  
  // Catture di Re.
  const posizione pos(mappa(c,re).firstone());
  bitboard prede(mosse[re][pos] & nemici);
  while (prede)
  {
    const posizione p_preda(prede.firstone());
    const pezzo preda(t_preda(!c,p_preda));

    lm->push_back(pos,p_preda,re,preda,cattura);    
    prede.elimina(p_preda);
  }
}

//-----------------------------------------------------------------------------
//  Calcola_legali: calcola tutte le mosse legali effettuabili (ad esclusione
//                  delle promozioni a pezzi minori) e le memorizza in lm.
//-----------------------------------------------------------------------------
void
situazione::calcola_legali(lista_mosse *lm)
{
  // Calcola tutte le mosse...
  calcola(lm);

  // ... ed elimina quelle illegali.
  lista_mosse::iterator i(lm->begin());
  while (i != lm->end())
  {
    dati_r r;

    if (!muovi(*i,r))
    {
      assert(!legale(*i));
      lm->erase(i);
    }
    else
    {
      ripristina(*i,r);
      assert(legale(*i));
      ++i;
    }
  }
}

//-----------------------------------------------------------------------------
//  Calcola_tutte: calcola tutte le mosse legali effettuabili comprese
//                 eventuali promozioni a pezzi minori. Non e' una funzione
//                 efficiente e, quindi, non andrebbe utilizzata durante la
//                 ricerca.
//-----------------------------------------------------------------------------
void
situazione::calcola_tutte(lista_mosse *lm)
{
  calcola_legali(lm);

  for (lista_mosse::const_iterator i(lm->begin()), sent(lm->end()); 
       i != sent;
       i++)
    if (i->tipo_mossa::e<promozione>())
    {
      const posizione da(i->da()), a(i->a());
      const tipo_mossa tm(tipo_mossa(*i) & ~p_donna);

      lm->push_back(da,a,pedone,i->preda(),tm | p_cavallo);
      lm->push_back(da,a,pedone,i->preda(),tm | p_alfiere);
      lm->push_back(da,a,pedone,i->preda(),tm |   p_torre);
    }
}

//-----------------------------------------------------------------------------
//  Pseudo_legale: memorizza in 'me' la mossa estesa corrispondente ad m nella
//                 situazione attuale. Questo potrebbe essere impossibile per
//                 le seguenti ragioni:
//                 - la mossa m e' illegale nell'attuale situazione;
//                 - la mossa m non e' univocamente estendibile (causa 
//                   promozione).
//                 Se la prima condizione si verifica restituisce false senza 
//                 modificare me; altrimenti restituisce true e memorizza in 
//                 me la mossa estesa (nel caso si verifichi la seconda 
//                 condizione la promozione me viene, arbitrariamente, assunta
//                 a Donna).
//-----------------------------------------------------------------------------
bool
situazione::pseudo_legale(mossa m, mossa_e *me) const
{
  const colore c(mossa_a());

  // Se la mossa da estendere non rispetta neanche le piu' elementari norme di
  // legalita' e' inutile proseguire.
  if (!controllo_rapido_legalita(c,m))
    return false;

  // Si e' ora sicuri che il pezzo che muove e' del colore corretto e che la
  // casella di destinazione e' vuota o contiene una preda valida (colore
  // corretto e tipo diverso da re).
  const bitboard pezzi(mappa());
  pezzo mosso(t_pezzo(c,m.da())), preda(vuoto);
  unsigned tipo(normale);

  switch (mosso)
  {
  case pedone:
    if (c == nero)
      // Doppio passo.
      if ( m.da() >= a7 && m.da()+basso+basso == m.a() && 
           !pezzi[m.da()+basso] && !pezzi[m.a()] )
        tipo |= doppio_p;
      else
      {
        // Cattura semplice o en_passant.
        if ( (m.da().colonna() != colA && m.da()+b_s == m.a()) ||
             (m.da().colonna() != colH && m.da()+b_d == m.a()) )
        {
          preda = t_pezzo(!c,m.a());

          if (!preda)
            if ( m.da().traversa() == 3 && m.a() == p_en_passant() &&
                 e_un(bianco,m.preda_en_passant(),pedone) )
            {
              preda =     pedone;
              tipo |= en_passant;
            }
            else
              return false;

          tipo |= cattura;
        }
        // L'unica mosse legale di pedone rimasta e' lo spostamento semplice.
        else if ( m.a() != m.da()+basso || pezzi[m.a()] )
          return false;

        if (m.a() <= h1)
          tipo |= p_donna;
      }
    else  // c == bianco
      // Doppio passo.
      if ( m.da() <= h2 && m.da()+alto+alto == m.a() && 
           !pezzi[m.da()+alto] && !pezzi[m.a()] )
        tipo |= doppio_p;
      else
      {
        // Cattura semplice o en_passant.
        if ( (m.da().colonna() != colA && m.da()+a_s == m.a()) ||
             (m.da().colonna() != colH && m.da()+a_d == m.a()) )
        {
          preda = t_pezzo(!c,m.a());

          if (!preda)
            if ( m.da().traversa() == 4 && m.a() == p_en_passant() && 
                 e_un(nero,m.preda_en_passant(),pedone) )
            {
              preda =     pedone;
              tipo |= en_passant;
            }
            else
              return false;

          tipo |= cattura;
        }
        // L'unica mosse legale di pedone rimasta e' lo spostamento semplice.
        else if (m.a() != m.da()+alto || pezzi[m.a()])
          return false;
        
        if (m.a() >= a8)
          tipo |= p_donna;
      }

    tipo |= di_pedone;
    if (me)
      *me = mossa_e(m,pedone,preda,tipo);
    break;

  case re:
    // Nel caso del re la mossa particolare da controllare e' l'arrocco.
    if (c == nero && m.da() == e8)
    {
      if (m.a() == g8)
      {
        if ( !(arrocco()&info_aus::O_On)
             || (pezzi & bitboard::raggio[e8][h8])
             || attacca(bianco,e8) || attacca(bianco,f8) )
          return false;

        tipo = Joker::arrocco;
      }
      else if (m.a() == c8)
      {
        if ( !(arrocco()&info_aus::O_O_On)
             || (pezzi & bitboard::raggio[e8][a8])
             || attacca(bianco,e8) || attacca(bianco,d8) )
          return false;

        tipo = Joker::arrocco;
      }
    }
    else if (c == bianco && m.da() == e1)
    {
      if (m.a() == g1)
      {
        if ( !(arrocco()&info_aus::O_Ob)
             || (pezzi & bitboard::raggio[e1][h1])
             || attacca(nero,e1) || attacca(nero,f1) )
          return false;

        tipo = Joker::arrocco;
      }
      else if (m.a() == c1)
      {
        if ( !(arrocco()&info_aus::O_O_Ob)
             || (pezzi & bitboard::raggio[e1][a1])
             || attacca(nero,e1) || attacca(nero,d1) )
          return false;

        tipo = Joker::arrocco;
      }
    }

    // Se e' un arrocco non c'e' bisogno di ulteriori controlli, altrimenti si
    // passa al caso generale (default).
    if (tipo == Joker::arrocco)
    {
      if (me)
        *me = mossa_e(m,re,preda,Joker::arrocco);
      break;
    }
  
  default:
    if ( mosse[mosso][m.da()][m.a()] )
    {
      // Controlla che la direttrice di spostamento sia sgombra.
      if ( scivola[mosso] && (bitboard::raggio[m.da()][m.a()] & pezzi) )
        return false;
        
      preda = t_pezzo(!c,m.a());
      if (preda)
        tipo |= cattura;        
    }
    else return false;

    if (me)
      *me = mossa_e(m,mosso,preda,tipo);
  }

  return true;
}

//-----------------------------------------------------------------------------
//  Legale: opera come la funzione pseudo_legale ed in piu' controlla che
//          la mossa m, una volta effettuata, non lasci il re sotto scacco.
//-----------------------------------------------------------------------------
bool
situazione::legale(mossa m, mossa_e *me)
{
  mossa_e tmp;

  if (!me)
    me = &tmp;

  if (pseudo_legale(m,me))
  {
    // Ora che abbiamo una mossa estesa non resta che controllare la sua
    // legalita' per quanto riguarda la sicurezza del re.
    dati_r r;
    if (muovi(*me,r))
    {
      ripristina(*me,r);
      return true;
    }
  }

  return false;
}

//-----------------------------------------------------------------------------
//  Carica:
//-----------------------------------------------------------------------------
bool
situazione::carica(std::istream &is, std::string *cmd)
{
  if (is.eof())
    return false;

  scacchiera s;
  info_aus  ia;

  if (!(is >> s >> ia))
    return false;

  *this = situazione(s,ia);

  if (cmd)
    getline(is,*cmd);
  else
  {
    std::string dummy;
    getline(is,dummy);
  }

  return !is.bad();
}

//-----------------------------------------------------------------------------
//  Operator>>:
//-----------------------------------------------------------------------------
std::istream &operator>>(std::istream &i, situazione &s)
{ 
  s.carica(i); 
  return i; 
}

//-----------------------------------------------------------------------------
//  Operator<<:
//-----------------------------------------------------------------------------
std::ostream &operator<<(std::ostream &o, const situazione &s)
{
  return o << scacchiera(s) << ' ' << info_aus(s);
}

//-----------------------------------------------------------------------------
//  Operator==:
//-----------------------------------------------------------------------------
bool
situazione::operator==(const situazione &s1) const
{
  return scacchiera(*this) == scacchiera(s1) &&
           info_aus(*this) ==   info_aus(s1) &&
                        hk ==         s1.hk;
}

//-----------------------------------------------------------------------------
//  Controlla:
//-----------------------------------------------------------------------------
bool
situazione::controlla() const
{
  // Se si puo' effettuare una cattura en passant deve essere presente il
  // pedone preda.
  const direzione d(mossa_a() == nero ? alto : basso);
  if ( !p_en_passant().fuori() && !e_un(!mossa_a(),p_en_passant()+d,pedone) )
    return false;

  // Se sono ancora possibili gli arrocchi, re e torri devono essere nella
  // posizione di partenza.
  if (arrocco() & info_aus::O_On)
    if (p_re(nero) != e8 || !e_un(nero,h8,torre))
      return false;
  if (arrocco() & info_aus::O_O_On)
    if (p_re(nero) != e8 || !e_un(nero,a8,torre))
      return false;
  if (arrocco() & info_aus::O_Ob)
    if (p_re(bianco) != e1 || !e_un(bianco,h1,torre))
      return false;
  if (arrocco() & info_aus::O_O_Ob)
    if (p_re(bianco) != e1 || !e_un(bianco,a1,torre))
      return false;
  
  return scacchiera::controlla() && info_aus::controlla() && hk.controlla();
}

}  // Namespace Joker.
