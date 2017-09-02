/*
 *  Libro_apertura.cc
 */

#include "libro_apertura.h"

namespace Joker
{

libro_apertura libro_apertura::_instance;



//-----------------------------------------------------------------------------
//  Find: memorizza in n le informazioni relative al nodo del libro di apertura
//        avente chiave k (se esiste). Se si individua un nodo con chiave k la
//        posizione (tellp), nel file in cui e' memorizzato il libro di
//        apertura, corrisponde a quella di inizio memorizzazione del nodo di
//        interesse.
//-----------------------------------------------------------------------------
bool
libro_apertura::find(const chiave_hash &k, nodo_apertura *n)
{
  assert(posizioni());
  
  const unsigned long dim(nodo_apertura::dimensione), hd_size(11);

  *n = nodo_apertura(k);

  unsigned long s(0);
  libro.seekg(hd_size,std::ios::beg);
  nodo_apertura ns(libro);

  unsigned long d(posizioni()-1);
  libro.seekg(d*dim+hd_size,std::ios::beg);
  nodo_apertura nd(libro);

  if (ns <= *n && *n <= nd)
    while (s <= d)
    {
      unsigned long x(s);

      if (s < d)
      {
        x += static_cast<unsigned long>( distanza(*n,ns) / 
                                         (distanza(nd,ns)/(d-s)) );
        assert(s <= x && x <= d);
      }
    
      libro.seekg(x*dim+hd_size,std::ios::beg);
      const nodo_apertura nx(libro);

      if (*n == nx)
      {
        *n = nx;
        libro.seekg(x*dim+hd_size,std::ios::beg);
        return true;
      }
      else if (*n < nx)
      {
        d = x-1;
        libro.seekg(static_cast<long>(d)*dim+hd_size,std::ios::beg);
        libro >> nd;

        if (nd < *n)
          return false;
      }
      else  // *n > nx
      {
        s = x+1;
        libro.seekg(static_cast<long>(s)*dim+hd_size,std::ios::beg);
        libro >> ns;

        if (ns > *n)
          return false;
      }
    }

  return false;
}

bool
libro_apertura::find(const chiave_hash &k)
{
  nodo_apertura n;
  return find(k,&n);
}

//-----------------------------------------------------------------------------
//  Mosse_previste:
//-----------------------------------------------------------------------------
bool
libro_apertura::mosse_previste(const situazione &s1, lista_mosse *lm,
                               mossa_e *suggerita, valutazione *score)
{
  // Notare che la posizione di partenza deve essere inclusa nel libro di
  // apertura altrimenti, supponendo che Joker sia il Nero, si possono
  // verificare situazioni tipo:
  //   1. e4 e5  2. Cf3 Cc6  3. Ab5 a6
  // il Bianco esce dal libro di apertura con 4. Cc3; se il Nero continuasse ad
  // affidarsi al libro di aperture, probabilmente, giocherebbe Cf6? al posto
  // di proseguire autonomamente con axb5.
  if (!attivo || !posizioni() || !find(s1.chiave()))
    return false;

  srand( static_cast<unsigned long>(time(0)) );

  valutazione tmp[lista_mosse::max_varianti];
  
  // ** Registrazione delle mosse di apertura e della relativa valutazione **
  situazione s(s1);
  lm->clear();  s.calcola_tutte(lm);

  lista_mosse::iterator i(lm->begin());
  while (i != lm->end())
  {
    dati_r r;
    nodo_apertura n;

    s.muovi(*i,r);
    const bool trovata(find(s.chiave(),&n));
    s.ripristina(*i,r);

    if (trovata)
    {
      switch (mode)
      {
      case apprendimento:  tmp[i - lm->begin()] = inf - n.giocate();  break;
      case casuale:        tmp[i - lm->begin()] =     rand() % 1000;  break;
      case torneo:         tmp[i - lm->begin()] =     n.punteggio();  break;
      }

      ++i;
    }
    else
      lm->erase(i);
  }
    
  // Se nella libreria non e' presente almeno una posizione derivante dalla
  // attuale e' inutile proseguire.
  if (!lm->size())
    return false;

  // ** Selezione della miglior mossa in base al criterio specificato **
  lm->sort(tmp);

  if (suggerita)  *suggerita = (*lm)[0];
  if (    score)  *score     =   tmp[0];

  return true;
}

//-----------------------------------------------------------------------------
//  Aggiorna_variante:
//-----------------------------------------------------------------------------
bool
libro_apertura::aggiorna_variante(const lista_mosse &variante, 
                                  colore joker, unsigned elo_avversario,
                                  bool computer_avversario, esito e)
{
  if (!attivo || !posizioni())
    return false;

  // La variante del giocatore vincente viene premiata (e quella del perdente
  // penalizzate) se:
  // - joker e' il giocatore perdente;
  // - joker vince e l'avversario e' un computer;
  // - joker vince l'avversario ha un punteggio Elo superiore a 2500.

  bool punti_a[2] = {false,false};
  punti_a[  nero] = e & (vince_nero|resa_bianco|patta);
  punti_a[bianco] = e & (vince_bianco|resa_nero|patta);
  
  bool favorevole_a[2] = {false,false};
  favorevole_a[ joker] = punti_a[joker] && (computer_avversario || 
                                            elo_avversario >= 2500);
  favorevole_a[!joker] = !punti_a[joker] || 
                         (punti_a[!joker] && elo_avversario < 2500);
    
  const bool aggiorna(favorevole_a[nero] || favorevole_a[bianco]);

  situazione s;
  // Notare che non si interrompe il ciclo appena individuata la prima 
  // posizione fuori libro: sono infatti possibili rientri successivi nel libro
  // di apertura.
  for (lista_mosse::const_iterator i(variante.begin());
       i != variante.end();
       i++)
  {
    nodo_apertura n;
    if ( find(s.chiave(),&n) )
    {
      const unsigned ply(i-variante.begin());
      const colore c_attuale(ply%2 ? nero : bianco);

      valutazione delta(0);

      // Notare come viene assegnato il punteggio:
      // SEGNO) Se la posizione attuale e' favorevole al giocatore cui spetta
      //        la mossa, il segno e' negativo perche' l'avversario deve essere
      //        scoraggiato dallo scegliere questa mossa;
      // MODULO) Il risultato finale dipende piu' direttamente dall'ultima
      //         posizione del libro di apertura rispetto alla posizione 
      //         iniziale
      if (aggiorna)
      {
        delta = MIN(1+ply/11,3);

        if (favorevole_a[c_attuale])
          delta = -delta;
      }

      const unsigned long       giocate(n.giocate()+1);
      const valutazione punteggio(n.punteggio()+delta);

      // Notare che bisogna comunque aggiornare il numero di partite giocate.
      libro << nodo_apertura(s.chiave(),giocate,punteggio);
      logger::instance() << "OPENING_BOOK> " << scacchiera(s) << ": games "
                         << giocate << ", score " << punteggio << " (delta " 
                         << delta << ")\n";
    }

    dati_r r;
    s.muovi(*i,r);
  }

  return true;
}

//-----------------------------------------------------------------------------
//  Controlla:
//-----------------------------------------------------------------------------
bool
libro_apertura::controlla() const
{
  return true;
}

}  // Namespace Joker.
