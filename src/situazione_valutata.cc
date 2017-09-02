/*
 *  Situazione_valutata.cc
 */

#include "situazione_valutata.h"

namespace Joker
{

const pesi                      situazione_valutata::w; 
situazione_valutata::ecache situazione_valutata::cache;



//-----------------------------------------------------------------------------
//  Case_attaccabili: restituisce una bitboard contenente l'insieme delle
//                    caselle direttamente attaccate od attaccabili dopo una
//                    mossa (uno o due passi) da parte dei Pedoni di colore C.
//                    Gli unici ostacoli considerati sono i Pedoni di colore 
//                    opposto.
//-----------------------------------------------------------------------------
template<colore C>
inline
bitboard
situazione_valutata::case_attaccabili() const
{
  // Caselle non occupate da Pedoni.
  const bitboard libere(~(mappa(nero,pedone)|mappa(bianco,pedone)));

  // Tutti i Pedoni di colore C.
  const bitboard p(mappa(C,pedone));

  // Caselle raggiungibili con un passo di Pedone.
  const bitboard p1(avanza<C>(p) & libere);

  // Caselle raggiungibili con due passi di Pedone.
  const bitboard p2(avanza<C>(p1) & libere & bitboard::traversa[4-C]);

  // Si ha:
  // - attacchi<C>(p): caselle direttamente attaccate;
  // - attacchi<C>(p1|p2): caselle attaccate dopo una mossa di Pedone.
  return attacchi<C>(p|p1|p2);
}

//-----------------------------------------------------------------------------
//  Case_forti: restituisce una bitboard contenente l'insieme delle case forti
//              per il giocatore C; vengono considerate case forti quelle non
//              attaccabili da Pedoni avversari.
//-----------------------------------------------------------------------------
template<colore C>
inline
bitboard
situazione_valutata::case_forti() const
{
  return ~case_attaccabili<!C>();
  // & attacchi<C>(mappa(C,pedone))   <-- se si vogliono solo quelle fissate.
}

//-----------------------------------------------------------------------------
//  Pedoni_deboli: un Pedone viene considerato debole se:  
//                 - non e' difeso da propri Pedoni;
//                 - non c'e' spinta di propri Pedoni in grado di difenderlo;
//                 - non si puo' spostare su case difese da propri Pedoni.
//-----------------------------------------------------------------------------
template<colore C>
inline
bitboard
situazione_valutata::pedoni_deboli() const
{
  // Caselle non occupate da Pedoni.
  const bitboard libere(~(mappa(nero,pedone)|mappa(bianco,pedone)));

  // Caselle difese o difendibili.
  const bitboard difese(case_attaccabili<C>());

  // Pedoni che possono portarsi su caselle protette.
  const bitboard pl(difese&libere);
  const bitboard altri(avanza<!C>(pl) | avanza<!C>(pl &
                                                   bitboard::traversa[4-C] &
                                                   avanza<C>(libere),2));

  return mappa(C,pedone) & ~(altri|difese);
}

//-----------------------------------------------------------------------------
//  Decodifica:
//-----------------------------------------------------------------------------
std::string
situazione_valutata::decodifica(const bitboard bb[2]) const
{
  std::string caselle("black={");

  for (posizione p(a1); !p.fuori(); ++p)
    if ( bb[nero][p] )
      caselle += ' '+p.str();

  caselle += " } white={";

  for (posizione p(a1); !p.fuori(); ++p)
    if ( bb[bianco][p] )
      caselle += ' '+p.str();

  return caselle + " }";
}

//-----------------------------------------------------------------------------
//  Colonna_aperta: 
//-----------------------------------------------------------------------------
inline
bool
situazione_valutata::colonna_aperta(unsigned n) const
{  
  assert(colA <= n && n <= colH);
  return !( bitboard::colonna[n] & (mappa(nero,pedone)|mappa(bianco,pedone)) );
}

//-----------------------------------------------------------------------------
//  Fianchi_bloccati: 
//-----------------------------------------------------------------------------
inline
bool
situazione_valutata::fianchi_bloccati(posizione pos) const
{
  return (bitboard::fianchi[pos] & mappa()) == bitboard::fianchi[pos];
}

//-----------------------------------------------------------------------------
//  Torre_intrappolata:
//-----------------------------------------------------------------------------
inline
bool
situazione_valutata::torre_intrappolata(colore c, posizione pos) const
{
  return bitboard::torre_intrappolata[c][p_re(c)][pos];
}

//-----------------------------------------------------------------------------
//  V_sviluppo:
//-----------------------------------------------------------------------------
inline
valutazione
situazione_valutata::v_sviluppo() const
{
  assert(fase_gioco() == fase::apertura);

  valutazione v(0);

  colore c(false);
  do
  {
    v = -v;

    bool minori_non_sviluppati(false);

    if (e_un(c,flip[c][b1],cavallo))
      { v += w[pesi::cavallo_non_sviluppato]; minori_non_sviluppati = true; }
    if (e_un(c,flip[c][g1],cavallo))
      { v += w[pesi::cavallo_non_sviluppato]; minori_non_sviluppati = true; }

    if (e_un(c,flip[c][c1],alfiere))
      { v += w[pesi::alfiere_non_sviluppato]; minori_non_sviluppati = true; }
    if (e_un(c,flip[c][f1],alfiere))
      { v += w[pesi::alfiere_non_sviluppato]; minori_non_sviluppati = true; }

    if (minori_non_sviluppati && !e_un(c,flip[c][d1],donna))
      v += w[pesi::donna_sviluppo_precoce];      
  } while (c^=1);
  
  return v;
}

//-----------------------------------------------------------------------------
//  V_disposizione_pezzi:
//-----------------------------------------------------------------------------
inline
valutazione
situazione_valutata::v_disposizione_pezzi() const
{
  const posizione pos_re[2] = { p_re(0), p_re(1) };

  valutazione v(0);

  colore c(false);
  do
  {
    v = -v;

    // Valutazione Cavalli.
    bitboard bb(mappa(c,cavallo));
    while (bb)
    {
      const posizione pos(bb.firstone());

      v += bonus_cavallo[c][pos] + tropism_cavallo[distanza(pos,pos_re[!c])];

      if (info->forti[c][pos])
      {
        // Un avamposto e' una casa forte fissata.
        if (mosse[!c][pos] & mappa(c,pedone))
          v += bonus_cavallo_forte[c][pos];

        if (info->passati[!c][avanti1[c][pos]])
        {
          assert(pos.distanza_da_base(c) < 7);
          v += cavallo_bloccatore[pos.distanza_da_base(c)];
        }
      }
      
      bb.elimina(pos);
    }

    // Valutazione Alfieri.
    bb = mappa(c,alfiere);
    if (bb)
    {
      if (bb & (bb-1))
        v += w[pesi::alfiere_coppia];

      if (fase_gioco() == fase::finale && info->passati[!c].count() > 1)
        v += w[pesi::alfiere_forte];

      do
      {
        const posizione pos(bb.firstone());
        const int ostacoli( (bitboard::grande_centro[pos.colore()] & 
                             mappa(c,pedone)).count() );

        v += alfiere_cattivo[ostacoli] + 
             tropism_alfiere[distanza(pos,pos_re[!c])];

        bb.elimina(pos);
      } while (bb);
    }

    // Valutazione Torri.
    bb = mappa(c,torre);
    while (bb)
    {
      const posizione pos(bb.firstone());

      if (pos.distanza_da_base(c) == 6 && pos_re[!c].distanza_da_base(!c) == 0)
      {
        v += w[pesi::torre_in_7a];

        if ( info->passati[c] && !(bitboard::traversa7abs[c] & mappa(!c)) )
          v += w[pesi::torre_in_7a_assoluta];
      }

      if (colonna_aperta(pos.colonna()))
      {
        v +=        w[pesi::torre_su_colonna_aperta];
        v += tropism_torre[distanza_x(pos,pos_re[!c])];
      }
      else if ( !(bitboard::colonna[pos.colonna()] & mappa(c,pedone)) )
      {
        v +=    w[pesi::torre_su_colonna_semiaperta];
        v += tropism_torre[distanza_x(pos,pos_re[!c])];
      }
      else
      {
        if (torre_intrappolata(c,pos))
          v += w[pesi::torre_intrappolata];
        if (bitboard::avanti[c][pos] & info->passati[c])
          v += w[pesi::torre_dietro_passato];
      }

      if (fianchi_bloccati(pos))
        v += w[pesi::torre_limitata];

      bb.elimina(pos);
    }

    // Valutazione Donne.
    bb = mappa(c,donna);
    while (bb)
    {
      const posizione pos(bb.firstone());

      if (fs != fase::finale && info->v_schermo.relativa(c) > 0)
        v += w[pesi::donna_forte];

      v += tropism_donna[distanza(pos,pos_re[!c])];
      
      bb.elimina(pos);
    }
  } while (c^=1);

  return v;
}

//-----------------------------------------------------------------------------
//  V_struttura_pedonale:
//-----------------------------------------------------------------------------
inline
valutazione
situazione_valutata::v_struttura_pedonale() const
{
  const ecache::tipo_chiave k(ecache::calcola_chiave(*this));

  info = cache.find(k,&info_ok); 

#if defined(_DEBUG)
  static unsigned collision(0);
  const valutazione   v_back_schermo(info_ok ?   info->v_schermo : inf);
  const valutazione v_back_struttura(info_ok ? info->v_struttura : inf);
#else
  // Se il risultato e' gia' memorizzato nella cache si evita tutto il lavoro.
  if (info_ok)
    return info->v_struttura+info->v_schermo;
#endif

  // ... altrimenti bisogna rimboccarsi le maniche.
  const bitboard deboli[] = {pedoni_deboli<0>(), pedoni_deboli<1>()};

  info->forti[  nero] = case_forti<  nero>();
  info->forti[bianco] = case_forti<bianco>();

  info->passati[  nero].clear();
  info->passati[bianco].clear();

  info->v_struttura = 0;
  info->v_schermo   = 0;

  colore c(false);
  do
  {
    info->v_struttura = -info->v_struttura;
    info->v_schermo   =   -info->v_schermo;

    const posizione pos_re(p_re(c));

    // ********* Valutazione posizione dei singoli Pedoni *********
    bitboard bb(mappa(c,pedone));
    while (bb)
    {
      const posizione pos(bb.firstone());
      valutazione score(bonus_pedone[c][pos]);
    
      // Si stabilisce se il Pedone e' debole (ed in quale grado)...
      assert( (bitboard::m_isolato[pos.colonna()]&mappa(c,pedone)) ||
              deboli[c][pos] );
      if (deboli[c][pos])
      {
        info->v_struttura += w[pesi::pedone_debole];

        const unsigned colonna(pos.colonna());
        if ( (bitboard::m_isolato[colonna] & mappa(c,pedone)) == 0 )
          if (colonna==colA || colonna==colH)
            info->v_struttura += w[pesi::pedone_isolato]/2;
          else
            info->v_struttura += w[pesi::pedone_isolato];
      }

      // ... doppiato...
      const bool doppiato(bitboard::avanti[c][pos] & mappa(c,pedone));
      if (doppiato)
        score += w[pesi::pedone_doppiato];

      // ... passato.
      if (p_passato(c,pos))
      {
        info->passati[c].aggiungi(pos);

        if (fase_gioco() != fase::apertura)
        {
          // Se questo Pedone e' passato, doppiato ed arretrato non gli si
          // puo' certo assegnare il bonus completo per Pedone passato.
          score *= doppiato ? valutazione(w[pesi::pedone_passato]/2) 
                            : w[pesi::pedone_passato];

          // I Pedoni passati supportati dal Re sono molto piu' forti.
          if (fase_gioco() == fase::finale && distanza(pos_re,pos)==1)
            score *= w[pesi::pedone_supportato];

          // Pedoni connessi, passati, avanzati sono difficilmente arrestabili.
          if (info->passati[c] & bitboard::fianchi[pos])
            score += bonus_coppia_passata[pos.distanza_da_base(c)];
        }
      }

      info->v_struttura += score;

      bb.elimina(pos);
    }

    // ********* Valutazione schermo pedonale *********
    info->v_schermo += bonus_re[c][pos_re];

    if (fase_gioco() != fase::finale)
    {
      if (bitboard::m_scudo1[c][pos_re])
      {
        const bitboard scudo1(bitboard::m_scudo1[c][pos_re] & mappa(c,pedone));
        const bitboard scudo2(bitboard::m_scudo2[c][pos_re] & mappa(c,pedone));

        const unsigned  n1(                scudo1.count() );
        const unsigned n1d( (scudo1 &  deboli[c]).count() );
        const unsigned n2f( (scudo2 & ~deboli[c]).count() );

        info->v_schermo += w[ pesi::pedone_scudo_vicino] * ( n1 - n1d);
        info->v_schermo += w[pesi::pedone_scudo_lontano] * (n2f + n1d);
      }
      /*
      switch (pos_re.colonna())
      {
      case colD:
      case colC:
      case colB:
        if ((bitboard::avanti[c][pos_re-1] & mappa(c,pedone)) == 0)
          info->v_schermo += w[pesi::re_su_colonna_aperta]/2;

      case colA:
        if ((bitboard::avanti[c][pos_re+1] & mappa(c,pedone)) == 0)
          info->v_schermo += w[pesi::re_su_colonna_aperta]/2;
        break;

      case colE:
      case colF:
      case colG:
        if ((bitboard::avanti[c][pos_re+1] & mappa(c,pedone)) == 0)
          info->v_schermo += w[pesi::re_su_colonna_aperta]/2;

      case colH:
        if ((bitboard::avanti[c][pos_re-1] & mappa(c,pedone)) == 0)
          info->v_schermo += w[pesi::re_su_colonna_aperta]/2;
      }

      if ( (bitboard::avanti[c][pos_re] & mappa(c,pedone)) == 0)
        info->v_schermo += w[pesi::re_su_colonna_aperta];
      */

      if ( (bitboard::avanti[c][pos_re] & mappa(c,pedone)) == 0)
        info->v_schermo += w[pesi::re_su_colonna_aperta];
      if  ( pos_re.colonna() > colA &&
            (bitboard::avanti[c][pos_re-1] & mappa(c,pedone)) == 0)
        info->v_schermo += w[pesi::re_su_colonna_aperta]/2;
      if  ( pos_re.colonna() < colH &&
            (bitboard::avanti[c][pos_re+1] & mappa(c,pedone)) == 0)
        info->v_schermo += w[pesi::re_su_colonna_aperta]/2;
    }
  } while (c^=1);

#if defined(_DEBUG)
  if (info_ok)
    if (  v_back_schermo !=   info->v_schermo ||
        v_back_struttura != info->v_struttura )
      std::cout << "******** Collision: " << ++collision << ".\n";
#endif

  cache.update(k,&info_ok);
  return info->v_struttura+info->v_schermo;
}

//-----------------------------------------------------------------------------
//  Valutazione posizione:
//-----------------------------------------------------------------------------
valutazione
situazione_valutata::valutazione_posizione(valutazione alpha,
                                           valutazione beta) const
{
  assert(alpha < beta);

  valutazione v_totale(delta_materiale()), v_rel(v_totale.relativa(mossa_a()));
  
  if (v_rel + residuo[0] < alpha || v_rel - residuo[0] > beta)
    return v_rel;

  unsigned i(0);
  do
  {
    v_totale +=           (this->*fval[i])();
    v_rel     = v_totale.relativa(mossa_a());
  } while (fval[++i] && 
           v_rel + residuo[i] >= alpha &&
           v_rel - residuo[i] <=  beta);

  return v_rel;
}

//-----------------------------------------------------------------------------
//  Resetta_array:
//-----------------------------------------------------------------------------
void
situazione_valutata::reset_array()
{
  for (unsigned i(0); i < 2; i++)
    for (unsigned j(0); j < 64; j++)
    {
      bonus_pedone[i][j]  = 0;
      bonus_cavallo[i][j] = 0;
      bonus_re[i][j]      = 0;
    }
}

//-----------------------------------------------------------------------------
//  Calcola_array:
//-----------------------------------------------------------------------------
template<>
void
situazione_valutata::calcola_array<fase::apertura>()
{
  reset_array();

  colore c(false);
  do
  {
    // E', in genere, un vantaggio possedere Pedoni al centro od in grado di 
    // effettuare un controllo sul centro.
    bonus_pedone[c][flip[c][d5]] += w[pesi::pedone_vicino_centro];
    bonus_pedone[c][flip[c][e5]] += w[pesi::pedone_vicino_centro];
    bonus_pedone[c][flip[c][d4]] += w[    pesi::pedone_al_centro];
    bonus_pedone[c][flip[c][e4]] += w[    pesi::pedone_al_centro];
    bonus_pedone[c][flip[c][e3]] += w[pesi::pedone_vicino_centro];
    bonus_pedone[c][flip[c][d3]] += w[pesi::pedone_vicino_centro];

    posizione p;

    // Alfieri e Cavalli non sono, in genere, ben posti sul bordo scacchiera.
    for (p = a1; !p.fuori(); ++p)
    {
      const int d1(p.distanza_da_centro()+1);
      bonus_cavallo[c][p] += (d1 != 4) ? w[pesi::cavallo_al_centro]/d1 : 0;
    }

    // E' bene che il Re stia nelle retrovie.
    for (p = a1; !p.fuori(); ++p)
      bonus_re[c][p] += w[pesi::re_esposto] * p.distanza_da_base(c);

    // I Pedoni centrali sono potenzialmente piu' utili di quelli laterali.
    const valutazione p_base[8]={w[pesi::pedone_baseA],w[pesi::pedone_baseB],
                                 w[pesi::pedone_baseC],w[pesi::pedone_baseD],
                                 w[pesi::pedone_baseE],w[pesi::pedone_baseF],
                                 w[pesi::pedone_baseG],w[pesi::pedone_baseH]};
    for (p = a2; p != a8; ++p)
      bonus_pedone[c][p] += p_base[p.colonna()];

    // Avamposti e case forti sono punti importanti.
    for (p = a1; !p.fuori(); ++p)
    {
      const valutazione mul(p.campo() == c ? w[pesi::cavallo_casa_forte]
                                           : w[ pesi::cavallo_avamposto]);
      
      if (p.distanza_da_centro() < 3)
        bonus_cavallo_forte[c][p] = bonus_cavallo[c][p] * mul;
      else
        bonus_cavallo_forte[c][p] =                         0;
    }
  } while (c^=1);

  alfiere_cattivo[0] =                          0;
  alfiere_cattivo[1] = 1*w[pesi::alfiere_cattivo];
  alfiere_cattivo[2] = 2*w[pesi::alfiere_cattivo];
  alfiere_cattivo[3] = 3*w[pesi::alfiere_cattivo];
  alfiere_cattivo[4] = 4*w[pesi::alfiere_cattivo];
  alfiere_cattivo[5] = 5*w[pesi::alfiere_cattivo];
  alfiere_cattivo[6] = 6*w[pesi::alfiere_cattivo];
  alfiere_cattivo[7] = 7*w[pesi::alfiere_cattivo];
  alfiere_cattivo[8] = 8*w[pesi::alfiere_cattivo];

  cavallo_bloccatore[0] =                                    0;
  cavallo_bloccatore[1] = (10*w[pesi::cavallo_bloccatore]);
  cavallo_bloccatore[2] = (15*w[pesi::cavallo_bloccatore]) / 2;
  cavallo_bloccatore[3] = ( 5*w[pesi::cavallo_bloccatore]);
  cavallo_bloccatore[4] = ( 8*w[pesi::cavallo_bloccatore]) / 3;
  cavallo_bloccatore[5] = ( 2*w[pesi::cavallo_bloccatore]);
  cavallo_bloccatore[6] = ( 4*w[pesi::cavallo_bloccatore]) / 3;
  cavallo_bloccatore[7] =                                    0;

  bonus_coppia_passata[0] =                                   0;
  bonus_coppia_passata[1] =                                   1;
  bonus_coppia_passata[2] =                                   1;
  bonus_coppia_passata[3] = w[pesi::pedone_passato_connesso]/16;
  bonus_coppia_passata[4] =  w[pesi::pedone_passato_connesso]/8;
  bonus_coppia_passata[5] =  w[pesi::pedone_passato_connesso]/2;
  bonus_coppia_passata[6] =    w[pesi::pedone_passato_connesso];
  bonus_coppia_passata[7] =                                   0;

  tropism_donna[0] =   w[pesi::donna_tropism];
  tropism_donna[1] =   w[pesi::donna_tropism];
  tropism_donna[2] =   w[pesi::donna_tropism];
  tropism_donna[3] = w[pesi::donna_tropism]/2;
  tropism_donna[4] = w[pesi::donna_tropism]/4;
  tropism_donna[5] =                        0;
  tropism_donna[6] =                        0;
  tropism_donna[7] =                        0;

  tropism_torre[0] =   w[pesi::torre_tropism];
  tropism_torre[1] =   w[pesi::torre_tropism];
  tropism_torre[2] = w[pesi::torre_tropism]/2;
  tropism_torre[3] =                        0;
  tropism_torre[4] =                        0;
  tropism_torre[5] =                        0;
  tropism_torre[6] =                        0;
  tropism_torre[7] =                        0;

  tropism_alfiere[0] =   w[pesi::alfiere_tropism];
  tropism_alfiere[1] =   w[pesi::alfiere_tropism];
  tropism_alfiere[2] =   w[pesi::alfiere_tropism];
  tropism_alfiere[3] = w[pesi::alfiere_tropism]/2;
  tropism_alfiere[4] = w[pesi::alfiere_tropism]/4;
  tropism_alfiere[5] =                          0;
  tropism_alfiere[6] =                          0;
  tropism_alfiere[7] =                          0;

  tropism_cavallo[0] =   w[pesi::cavallo_tropism];
  tropism_cavallo[1] =   w[pesi::cavallo_tropism];
  tropism_cavallo[2] =   w[pesi::cavallo_tropism];
  tropism_cavallo[3] = w[pesi::cavallo_tropism]/2;
  tropism_cavallo[4] = w[pesi::cavallo_tropism]/4;
  tropism_cavallo[5] =                          0;
  tropism_cavallo[6] =                          0;
  tropism_cavallo[7] =                          0;
}

template<>
void
situazione_valutata::calcola_array<fase::mediogioco>()
{
  reset_array();

  colore c(false);

  do
  {
    // Nel mediogioco i Pedoni possono, solitamente, spingersi più avanti che
    // in apertura.
    bonus_pedone[c][flip[c][d5]] += w[pesi::pedone_al_centro];
    bonus_pedone[c][flip[c][e5]] += w[pesi::pedone_al_centro];
    bonus_pedone[c][flip[c][d4]] += w[pesi::pedone_al_centro];
    bonus_pedone[c][flip[c][e4]] += w[pesi::pedone_al_centro];

    posizione p;
  
    // Alfieri e Cavalli non sono, in genere, ben posti sul bordo scacchiera.
    for (p = a1; !p.fuori(); ++p)
    {
      const int d1(p.distanza_da_centro()+1);
      bonus_cavallo[c][p] += (d1 != 4) ? w[pesi::cavallo_al_centro]/d1 : 0;
    }

    // E' bene che il Re stia nelle retrovie.
    for (p = a1; !p.fuori(); ++p)
      bonus_re[c][p] += w[pesi::re_esposto] * p.distanza_da_base(c);

    // I Pedoni centrali sono potenzialmente piu' utili di quelli laterali.
    const valutazione p_base[8]={w[pesi::pedone_baseA],w[pesi::pedone_baseB],
                                 w[pesi::pedone_baseC],w[pesi::pedone_baseD],
                                 w[pesi::pedone_baseE],w[pesi::pedone_baseF],
                                 w[pesi::pedone_baseG],w[pesi::pedone_baseH]};
    for (p = a2; p != a8; ++p)
      bonus_pedone[c][p] += p_base[p.colonna()];

    // Avamposti e case forti sono punti importanti.
    for (p = a1; !p.fuori(); ++p)
    {
      const valutazione mul(p.campo() == c ? w[pesi::cavallo_casa_forte]
                                            : w[ pesi::cavallo_avamposto]);
      
      if (p.distanza_da_centro() < 3)
        bonus_cavallo_forte[c][p] = bonus_cavallo[c][p] * mul;
      else
        bonus_cavallo_forte[c][p] =                         0;
    }
  } while (c^=1);

  alfiere_cattivo[0] =                          0;
  alfiere_cattivo[1] = 1*w[pesi::alfiere_cattivo];
  alfiere_cattivo[2] = 2*w[pesi::alfiere_cattivo];
  alfiere_cattivo[3] = 3*w[pesi::alfiere_cattivo];
  alfiere_cattivo[4] = 4*w[pesi::alfiere_cattivo];
  alfiere_cattivo[5] = 5*w[pesi::alfiere_cattivo];
  alfiere_cattivo[6] = 6*w[pesi::alfiere_cattivo];
  alfiere_cattivo[7] = 7*w[pesi::alfiere_cattivo];
  alfiere_cattivo[8] = 8*w[pesi::alfiere_cattivo];

  cavallo_bloccatore[0] =                                    0;
  cavallo_bloccatore[1] = (10*w[pesi::cavallo_bloccatore]);
  cavallo_bloccatore[2] = (15*w[pesi::cavallo_bloccatore]) / 2;
  cavallo_bloccatore[3] = ( 5*w[pesi::cavallo_bloccatore]);
  cavallo_bloccatore[4] = ( 8*w[pesi::cavallo_bloccatore]) / 3;
  cavallo_bloccatore[5] = ( 2*w[pesi::cavallo_bloccatore]);
  cavallo_bloccatore[6] = ( 4*w[pesi::cavallo_bloccatore]) / 3;
  cavallo_bloccatore[7] =                                    0;

  bonus_coppia_passata[0] =                                   0;
  bonus_coppia_passata[1] =                                   1;
  bonus_coppia_passata[2] = w[pesi::pedone_passato_connesso]/16;
  bonus_coppia_passata[3] =  w[pesi::pedone_passato_connesso]/8;
  bonus_coppia_passata[4] =  w[pesi::pedone_passato_connesso]/4;
  bonus_coppia_passata[5] =    w[pesi::pedone_passato_connesso];
  bonus_coppia_passata[6] =  w[pesi::pedone_passato_connesso]*2;
  bonus_coppia_passata[7] =                                   0;

  tropism_donna[0] =   w[pesi::donna_tropism];
  tropism_donna[1] =   w[pesi::donna_tropism];
  tropism_donna[2] =   w[pesi::donna_tropism];
  tropism_donna[3] = w[pesi::donna_tropism]/2;
  tropism_donna[4] = w[pesi::donna_tropism]/4;
  tropism_donna[5] =                        0;
  tropism_donna[6] =                        0;
  tropism_donna[7] =                        0;

  tropism_torre[0] =   w[pesi::torre_tropism];
  tropism_torre[1] =   w[pesi::torre_tropism];
  tropism_torre[2] = w[pesi::torre_tropism]/2;
  tropism_torre[3] =                        0;
  tropism_torre[4] =                        0;
  tropism_torre[5] =                        0;
  tropism_torre[6] =                        0;
  tropism_torre[7] =                        0;

  tropism_alfiere[0] =   w[pesi::alfiere_tropism];
  tropism_alfiere[1] =   w[pesi::alfiere_tropism];
  tropism_alfiere[2] =   w[pesi::alfiere_tropism];
  tropism_alfiere[3] = w[pesi::alfiere_tropism]/2;
  tropism_alfiere[4] = w[pesi::alfiere_tropism]/4;
  tropism_alfiere[5] =                          0;
  tropism_alfiere[6] =                          0;
  tropism_alfiere[7] =                          0;

  tropism_cavallo[0] =   w[pesi::cavallo_tropism];
  tropism_cavallo[1] =   w[pesi::cavallo_tropism];
  tropism_cavallo[2] =   w[pesi::cavallo_tropism];
  tropism_cavallo[3] = w[pesi::cavallo_tropism]/2;
  tropism_cavallo[4] = w[pesi::cavallo_tropism]/4;
  tropism_cavallo[5] =                          0;
  tropism_cavallo[6] =                          0;
  tropism_cavallo[7] =                          0;
}

template<>
void
situazione_valutata::calcola_array<fase::finale>()
{
  reset_array();

  colore c(false);
  do
  {
    posizione p;

    // I Cavalli non sono, in genere, ben posti sul bordo scacchiera.
    for (p = a1; !p.fuori(); ++p)
    {
      const int d1(p.distanza_da_centro()+1);
      bonus_cavallo[c][p] += d1 != 4 ? w[pesi::cavallo_al_centro]/d1 : 0;
    }

    // Il Re deve portarsi verso il centro della scacchiera.
    for (p = a1; !p.fuori(); ++p)
    {
      const int d1(p.distanza_da_centro()+1);

      bonus_re[c][p] += w[pesi::re_al_centro]/d1;

      // Quando il Re e' sul bordo scacchiera e' posto tanto peggio quanto piu'
      // e' vicino agli angoli (utile euristica per individuare piu' 
      // velocemente i matti in semplici finali).
      if ( p.colonna() == colA ||  p.colonna() == colH ||
          p.traversa() ==    0 || p.traversa() ==    7)
      {
        const unsigned d_a1(distanza(p,a1)), d_a8(distanza(p,a8)),
                       d_h1(distanza(p,h1)), d_h8(distanza(p,h8)),
                       d_min( MIN( MIN(d_a1,d_a8), MIN(d_h1,d_h8) ) );

        bonus_re[c][p] += w[pesi::re_in_angolo] - (3-d_min);
      }
    }

    // I Pedoni centrali sono potenzialmente piu' utili di quelli laterali.
    const valutazione p_base[8]={w[pesi::pedone_baseA],w[pesi::pedone_baseB],
                                 w[pesi::pedone_baseC],w[pesi::pedone_baseD],
                                 w[pesi::pedone_baseE],w[pesi::pedone_baseF],
                                 w[pesi::pedone_baseG],w[pesi::pedone_baseH]};

    for (p = a2; p != a8; ++p)
      bonus_pedone[c][p] += p_base[p.colonna()];

    // I Pedoni avanzati sono piu' pericolosi di quelli arretrati.
    for (p = a2; p != a8; ++p)
      bonus_pedone[c][p] += w[pesi::pedone_avanzata] *
                            (p.distanza_da_base(c)-1);

    // Avamposti e case forti sono punti importanti.
    for (p = a1; !p.fuori(); ++p)
    {
      const valutazione mul(p.campo() == c ? w[pesi::cavallo_casa_forte]
                                           : w[ pesi::cavallo_avamposto]);
      
      if (p.distanza_da_centro() < 3)
        bonus_cavallo_forte[c][p] = bonus_cavallo[c][p] * mul;
      else
        bonus_cavallo_forte[c][p] =                         0;
    }
  } while (c^=1);

  alfiere_cattivo[0] =                          0;
  alfiere_cattivo[1] = 1*w[pesi::alfiere_cattivo];
  alfiere_cattivo[2] = 2*w[pesi::alfiere_cattivo];
  alfiere_cattivo[3] = 3*w[pesi::alfiere_cattivo];
  alfiere_cattivo[4] = 4*w[pesi::alfiere_cattivo];
  alfiere_cattivo[5] = 5*w[pesi::alfiere_cattivo];
  alfiere_cattivo[6] = 6*w[pesi::alfiere_cattivo];
  alfiere_cattivo[7] = 7*w[pesi::alfiere_cattivo];
  alfiere_cattivo[8] = 8*w[pesi::alfiere_cattivo];

  cavallo_bloccatore[0] =                                    0;
  cavallo_bloccatore[1] = (10*w[pesi::cavallo_bloccatore]);
  cavallo_bloccatore[2] = (15*w[pesi::cavallo_bloccatore]) / 2;
  cavallo_bloccatore[3] = ( 5*w[pesi::cavallo_bloccatore]);
  cavallo_bloccatore[4] = ( 8*w[pesi::cavallo_bloccatore]) / 3;
  cavallo_bloccatore[5] = ( 2*w[pesi::cavallo_bloccatore]);
  cavallo_bloccatore[6] = ( 4*w[pesi::cavallo_bloccatore]) / 3;
  cavallo_bloccatore[7] =                                    0;

  bonus_coppia_passata[0] =                                   0;
  bonus_coppia_passata[1] = w[pesi::pedone_passato_connesso]/16;
  bonus_coppia_passata[2] =  w[pesi::pedone_passato_connesso]/8;
  bonus_coppia_passata[3] =  w[pesi::pedone_passato_connesso]/4;
  bonus_coppia_passata[4] =  w[pesi::pedone_passato_connesso]/2;
  bonus_coppia_passata[5] =    w[pesi::pedone_passato_connesso];
  bonus_coppia_passata[6] =  w[pesi::pedone_passato_connesso]*2;
  bonus_coppia_passata[7] =                                   0;

  tropism_donna[0] =   w[pesi::donna_tropism];
  tropism_donna[1] =   w[pesi::donna_tropism];
  tropism_donna[2] =   w[pesi::donna_tropism];
  tropism_donna[3] = w[pesi::donna_tropism]/2;
  tropism_donna[4] = w[pesi::donna_tropism]/4;
  tropism_donna[5] =                        0;
  tropism_donna[6] =                        0;
  tropism_donna[7] =                        0;

  tropism_torre[0] =   w[pesi::torre_tropism];
  tropism_torre[1] =   w[pesi::torre_tropism];
  tropism_torre[2] = w[pesi::torre_tropism]/2;
  tropism_torre[3] =                        0;
  tropism_torre[4] =                        0;
  tropism_torre[5] =                        0;
  tropism_torre[6] =                        0;
  tropism_torre[7] =                        0;

  tropism_alfiere[0] =   w[pesi::alfiere_tropism];
  tropism_alfiere[1] =   w[pesi::alfiere_tropism];
  tropism_alfiere[2] =   w[pesi::alfiere_tropism];
  tropism_alfiere[3] = w[pesi::alfiere_tropism]/2;
  tropism_alfiere[4] = w[pesi::alfiere_tropism]/4;
  tropism_alfiere[5] =                          0;
  tropism_alfiere[6] =                          0;
  tropism_alfiere[7] =                          0;

  tropism_cavallo[0] =   w[pesi::cavallo_tropism];
  tropism_cavallo[1] =   w[pesi::cavallo_tropism];
  tropism_cavallo[2] =   w[pesi::cavallo_tropism];
  tropism_cavallo[3] = w[pesi::cavallo_tropism]/2;
  tropism_cavallo[4] = w[pesi::cavallo_tropism]/4;
  tropism_cavallo[5] =                          0;
  tropism_cavallo[6] =                          0;
  tropism_cavallo[7] =                          0;
}

//-----------------------------------------------------------------------------
//  V_differenza_materiale:
//-----------------------------------------------------------------------------
valutazione
situazione_valutata::v_differenza_materiale() const
{
  valutazione v(0);

  for (pezzo pz(pedone); pz < re; ++pz)
    v -= mappa(nero,pz).count() * valore[pz];

  for (pezzo pz(pedone); pz < re; ++pz)
    v += mappa(bianco,pz).count() * valore[pz];

  return v;
}

//-----------------------------------------------------------------------------
//  Elementi_di_valutazione:
//-----------------------------------------------------------------------------
void
situazione_valutata::elementi_di_valutazione(componenti_valutazione &l) const
{
  // ********* Fase di gioco *********
  l.push_back("Phase",fase_gioco().str());
  
  // ********* Valutazione differenza di materiale *********
  l.push_back("Material delta", ::itos(delta_materiale()));
  

  // ********* Valutazione struttura pedonale e sicurezza Re *********
  v_struttura_pedonale();

  l.push_back("Pawn structure", ::itos(info->v_struttura));
  l.push_back("King security", ::itos(info->v_schermo));

  // ********* Valutazione disposizione pezzi *********
  l.push_back("Piece disposition", ::itos(v_disposizione_pezzi()));

  // ********* Lista Pedoni deboli *********
  bitboard bb[2];
  bb[nero] = pedoni_deboli<nero>();  bb[bianco] = pedoni_deboli<bianco>();
  l.push_back("Weak Pawns",decodifica(bb));

  // ********* Lista case forti *********
  bb[nero] = case_forti<nero>();  bb[bianco] = case_forti<bianco>();
  l.push_back("Strong squares",decodifica(bb));

  // ********* Lista Pedoni passati *********
  bb[nero] = info->passati[nero];  bb[bianco] = info->passati[bianco];
  l.push_back("Passed Pawns",decodifica(bb));

  // ********* Lista Torri bloccate *********
  bb[0].clear();  bb[1].clear();
  colore c(false);
  do
  {  
    bitboard torri(mappa(c,torre));

    while (torri)
    {
      const posizione pos(torri.firstone());

      if (torre_intrappolata(c,pos))
        bb[c].aggiungi(pos);
      
      torri.elimina(pos);
    }
  } while (c^=1);

  l.push_back("Blocked Rooks",decodifica(bb));

  // ********* Lista Torri limitate *********
  bb[0].clear();  bb[1].clear();
  do
  {  
    bitboard torri(mappa(c,torre));

    while (torri)
    {
      const posizione pos(torri.firstone());

      if (fianchi_bloccati(pos))
        bb[c].aggiungi(pos);
      
      torri.elimina(pos);
    }
  } while (c^=1);

  l.push_back("Limited Rooks",decodifica(bb));
}

//-----------------------------------------------------------------------------
//  Setta_funzioni_valutazione:
//-----------------------------------------------------------------------------
void
situazione_valutata::setta_funzioni_valutazione()
{
  cache.clear();

  switch (fase_gioco())
  {
  case fase::apertura:
    calcola_array<fase::apertura>();

    fval[0]    = &situazione_valutata::v_struttura_pedonale;
    residuo[0] =                                        300;
    fval[1]    = &situazione_valutata::v_disposizione_pezzi;
    residuo[1] =                                        100;
    fval[2]    =           &situazione_valutata::v_sviluppo;
    residuo[2] =                                         20;
    fval[3]    =                                          0;
    residuo[3] =                                          0;
    break;

  case fase::mediogioco:
    calcola_array<fase::mediogioco>();

    fval[0]    = &situazione_valutata::v_struttura_pedonale;
    residuo[0] =                                        300;
    fval[1]    = &situazione_valutata::v_disposizione_pezzi;
    residuo[1] =                                        100;
    fval[2]    =                                          0;
    residuo[2] =                                          0;
    break;
      
  case fase::finale:
    calcola_array<fase::finale>();

    fval[0]    = &situazione_valutata::v_struttura_pedonale;
    residuo[0] =                                        200;
    fval[1]    = &situazione_valutata::v_disposizione_pezzi;
    residuo[1] =                                        100;
    fval[2]    =                                          0;
    residuo[2] =                                          0;
  }
}

//-----------------------------------------------------------------------------
//  Aggiorna_fase_gioco:
//-----------------------------------------------------------------------------
bool
situazione_valutata::aggiorna_fase_gioco()
{
  const fase attuale(*this), precedente(fase_gioco());

  if (attuale != precedente)
  {
    fs = attuale;

    setta_funzioni_valutazione();
  }

  return attuale != precedente;
}

//-----------------------------------------------------------------------------
//  Situazione_valutata:
//-----------------------------------------------------------------------------
situazione_valutata::situazione_valutata(const situazione &s1) 
  : situazione(s1), fs(s1)
{
  setta_funzioni_valutazione();

  dm = v_differenza_materiale();
}

//-----------------------------------------------------------------------------
//  Carica:
//-----------------------------------------------------------------------------
bool
situazione_valutata::carica(std::istream &i, std::string *cmd)
{
  situazione s;

  if (s.carica(i,cmd))
  {
    *this = situazione_valutata(s);
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
//  Controlla:
//-----------------------------------------------------------------------------
bool
situazione_valutata::controlla() const
{ 
  return situazione::controlla() && dm.controlla() && fs.controlla() && 
         delta_materiale() == v_differenza_materiale() && w.controlla();
}

}  // Namespace Joker.
