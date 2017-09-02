/*
 *  Trasposizioni.cc
 */

#include "trasposizioni.h"

namespace Joker
{

//-----------------------------------------------------------------------------
//  Clear:
//-----------------------------------------------------------------------------
void
trasposizioni::clear()
{
  const unsigned long nelem(maschera+1);
 
  // Notare che inizializzando la tabella con questa info, anche nel caso
  // malaugurato in cui la ricerca nella stessa porti a scegliere un elemento
  // vuoto, questo non viene utilizzato (dettaglio==0) e la conversione in
  // valutazione (differenza v-inf), cosi' come quella in mossa, non genera una
  // valutazione fuori dal range ammesso.
  const unsigned64 info(impacchetta(0,inutile,attributo::nessuno,0,
                                    mossa::sentinella,0));

  for (unsigned long i(0); i < nelem; i++)
  {
    base[i].setta<0>(chiave_hash(0),info);
    base[i].setta<1>(chiave_hash(0),info);
  }

  eta_attuale = 1;
}

//-----------------------------------------------------------------------------
//  Find:
//-----------------------------------------------------------------------------
trasposizioni::limitazione
trasposizioni::find(const chiave_hash &k, dettaglio d_richiesto, 
                    valutazione beta, valutazione *val, mossa *m,
                    attributi *attr) const
{
  const elemento *const elem = at(k);

  bool livello(0);

  if (k != elem->chiave[livello] && k != elem->chiave[++livello])
  {
    *m = mossa::sentinella;
    return inutile;
  }
  
  *val = elem->valutazione(livello);
  *m   =       elem->mossa(livello);
  
  const limitazione tipo(elem->tipo(livello));

#if defined(NULL_MOVE)
  // Notare che bisognerebbe anche controllare il dettaglio di analisi. In ogni
  // caso il peggio che puo' capitare e' di effettuare una null move che non
  // produce un taglio.
  if (tipo!=inferiore && *val < beta)
    *attr |= attributo::evita_null;
#endif

  return elem->dettaglio(livello) < d_richiesto ? inutile : tipo;
}

//-----------------------------------------------------------------------------
//  Insert:
//-----------------------------------------------------------------------------
void
trasposizioni::insert(const chiave_hash &k, dettaglio d, limitazione tipo,
                      mossa m, valutazione val, attributi attr)
{
  assert(d >= 0);

  if (val.e_matto())
    if (val > 0 && tipo != superiore)
      { tipo = inferiore; val =  max_matto; }
    else if (val < 0 && tipo != inferiore)
      { tipo = superiore; val = -max_matto; }
    else
      return;

  elemento *const elem = at(k);
  
  const bool collisione(k==elem->chiave[0]);
  
  if (elem->dettaglio(0) <= d || elem->eta(0) != eta_attuale || val.e_matto())
  {
    if (collisione)
    {
      if (!m)  m = elem->mossa(0);
    }
    else  elem->abbassa_livello_0();

    elem->setta<0>(k,impacchetta(eta_attuale,tipo,attr,val,m,d));

    assert(elem->eta(0)==eta_attuale && elem->tipo(0)==tipo &&
           elem->attributi(0)==attr && elem->valutazione(0)==val && 
           elem->mossa(0)==m && elem->dettaglio(0)==d);
  }
  else if (!collisione)
  {
    // if (!m && k==elem->chiave[1])  m = elem->mossa(1);

    elem->setta<1>(k,impacchetta(eta_attuale,tipo,attr,val,m,d));
  
    assert(elem->eta(1)==eta_attuale && elem->tipo(1)==tipo &&
           elem->attributi(1)==attr && elem->valutazione(1)==val &&
           elem->mossa(1)==m && elem->dettaglio(1)==d);
  } 

  assert( elem->chiave[0] != elem->chiave[1] );
}

//-----------------------------------------------------------------------------
//  Controlla:
//-----------------------------------------------------------------------------
bool
trasposizioni::controlla() const
{
  return true;
}

}  // Namespace Joker.
