/*
 *  Scacchiera.cc
 */

#include "scacchiera.h"

namespace Joker
{

//-----------------------------------------------------------------------------
//  Scacchiera:
//-----------------------------------------------------------------------------
scacchiera::scacchiera()
{ 
  clear();

  colore c(false);
  do
  {
    pezzi[c][  torre].aggiungi(flip[c][a1]);
    pezzi[c][cavallo].aggiungi(flip[c][b1]);
    pezzi[c][alfiere].aggiungi(flip[c][c1]);
    pezzi[c][  donna].aggiungi(flip[c][d1]);
    pezzi[c][     re].aggiungi(flip[c][e1]);
    pezzi[c][alfiere].aggiungi(flip[c][f1]);
    pezzi[c][cavallo].aggiungi(flip[c][g1]);
    pezzi[c][  torre].aggiungi(flip[c][h1]);

    posizione p(flip[c][a2]);
    for (unsigned i(0); i < 8; i++, ++p)
      pezzi[c][pedone].aggiungi(p);
  } while (c^=1);

  somma();
  assert(controlla());
}

//-----------------------------------------------------------------------------
//  Muovi:
//-----------------------------------------------------------------------------
void
scacchiera::muovi(colore c, const mossa_e &m)
{
  // Spostamento pezzo.
  pezzi[c][0].sposta(m.da(),m.a());

  pezzi[c][m.mosso()].elimina(m.da());
  pezzi[c][m.e<promozione>()? m.promozione_a() : m.mosso()].aggiungi(m.a());

  // Gestione casi particolari.
  if (m.e<cattura>())
  {
    const posizione p_preda(m.e<cattura_normale>() ? m.a()
                                                   : m.preda_en_passant());
    
    pezzi[!c][        0].elimina(p_preda);
    pezzi[!c][m.preda()].elimina(p_preda);
  }
  else if (m.e<arrocco>())
  {
    posizione ar_da, ar_a;

    if (flip[c][m.a()] == g1)  { ar_da = flip[c][h1];  ar_a = flip[c][f1]; }
    else                       { ar_da = flip[c][a1];  ar_a = flip[c][d1]; }

    pezzi[c][    0].sposta(ar_da,ar_a);
    pezzi[c][torre].sposta(ar_da,ar_a);
  }

  assert(controlla());
}

//-----------------------------------------------------------------------------
//  Ripristina:
//-----------------------------------------------------------------------------
void
scacchiera::ripristina(colore c, const mossa_e &m)
{
  // Spostamento pezzo.
  pezzi[c][0].sposta(m.a(),m.da());

  pezzi[c][m.mosso()].aggiungi(m.da());
  pezzi[c][m.e<promozione>() ? m.promozione_a() : m.mosso()].elimina(m.a());

  // Gestione casi particolari.
  if (m.e<cattura>())
  {
    const posizione p_preda(m.e<cattura_normale>() ? m.a()
                                                   : m.preda_en_passant());

    pezzi[!c][        0].aggiungi(p_preda);
    pezzi[!c][m.preda()].aggiungi(p_preda);
  }
  else if (m.e<arrocco>())
  {
    posizione ar_da, ar_a;

    if (flip[c][m.a()] == g1)  { ar_da = flip[c][h1];  ar_a = flip[c][f1]; }
    else                       { ar_da = flip[c][a1];  ar_a = flip[c][d1]; }

    pezzi[c][    0].sposta(ar_a,ar_da);
    pezzi[c][torre].sposta(ar_a,ar_da);
  }

  assert(controlla());
}

//-----------------------------------------------------------------------------
//  Attacca:
//-----------------------------------------------------------------------------
bool
scacchiera::attacca(colore c, posizione bersaglio) const
{
  assert(!bersaglio.fuori());

  // Attacchi di Cavallo.
  if (attacca<cavallo>(c,bersaglio))
    return true;

  // Attacchi di Pedone.
  if (attacca<pedone>(c,bersaglio))
    return true;

  // Attacchi di Alfieri e Donne.
  bitboard attaccanti( (mappa(c,alfiere)|mappa(c,donna)) & 
                       mosse[alfiere][bersaglio] );
  if (attaccanti)
  {
    const bitboard pezzi(mappa());

    do
    {
      const posizione pos(attaccanti.firstone());

      if (!(bitboard::raggio[bersaglio][pos] & pezzi))
        return true;

      attaccanti.elimina(pos);
    } while (attaccanti);
  }

  // Attacchi di Torri e Donne.
  attaccanti = (mappa(c,torre)|mappa(c,donna)) & mosse[torre][bersaglio];
  if (attaccanti)
  {
    const bitboard pezzi(mappa());

    do
    {
      const posizione pos(attaccanti.firstone());

      if (!(bitboard::raggio[bersaglio][pos] & pezzi))
        return true;

      attaccanti.elimina(pos);
    } while (attaccanti);
  }

  // Attacchi di Re.
  return attacca<re>(c,bersaglio);
}

//-----------------------------------------------------------------------------
//  Operator==:
//-----------------------------------------------------------------------------
bool
scacchiera::operator==(const scacchiera &s1) const
{
  colore c(false);
  do
    for (unsigned pz(vuoto); pz <= re; pz++)
      if (pezzi[c][pz] != s1.pezzi[c][pz])
        return false;
  while (c^=1);

  return true;
}

//-----------------------------------------------------------------------------
//  Clear:
//-----------------------------------------------------------------------------
void
scacchiera::clear()
{
  colore c(false);
  do
    for (unsigned pz(vuoto); pz <= re; pz++)
      pezzi[c][pz].clear();
  while (c^=1);
}

//-----------------------------------------------------------------------------
//  Somma:
//-----------------------------------------------------------------------------
void
scacchiera::somma()
{
  colore c(false); 
  do 
    for (unsigned pz(pedone); pz <= re; pz++)
      pezzi[c][0] |= pezzi[c][pz];
  while (c^=1);
}

//-----------------------------------------------------------------------------
//  T_pezzo:
//-----------------------------------------------------------------------------
pezzo
scacchiera::t_pezzo(colore c, posizione p) const
{
  assert(!p.fuori());

  if (!pezzi[c][0][p])
    return vuoto;

  for (pezzo pz(pedone); pz < re; ++pz)
    if (pezzi[c][pz][p])
      return pz;

  return re;
}

//-----------------------------------------------------------------------------
//  Operator>>:
//-----------------------------------------------------------------------------
std::istream &operator>>(std::istream &is, scacchiera &s)
{
  s.clear();

  int ch;
  while ( isspace(ch = is.get()) )
    ;

  for (int y(7); y >= 0; y--)
  {
    int x(0);

    do
    {
      const posizione p(x,y);
      switch(ch)
      {
      case 'p':    s.pezzi[  nero][ pedone].aggiungi(p);  break;
      case 'P':    s.pezzi[bianco][ pedone].aggiungi(p);  break;
      case 'n':    s.pezzi[  nero][cavallo].aggiungi(p);  break;
      case 'N':    s.pezzi[bianco][cavallo].aggiungi(p);  break;
      case 'b':    s.pezzi[  nero][alfiere].aggiungi(p);  break;
      case 'B':    s.pezzi[bianco][alfiere].aggiungi(p);  break;
      case 'r':    s.pezzi[  nero][  torre].aggiungi(p);  break;
      case 'R':    s.pezzi[bianco][  torre].aggiungi(p);  break;
      case 'q':    s.pezzi[  nero][  donna].aggiungi(p);  break;
      case 'Q':    s.pezzi[bianco][  donna].aggiungi(p);  break;
      case 'k':    s.pezzi[  nero][     re].aggiungi(p);  break;
      case 'K':    s.pezzi[bianco][     re].aggiungi(p);  break;
      default:
        if (isdigit(ch))
          x += ch-'1';
        else
        {
          is.clear(std::ios::badbit|is.rdstate());
          return is;
        }
      }

      ch = is.get();
    } while (++x < 8);

    if (ch == '/')
      ch = is.get();
    else if (y)
    {
      is.clear(std::ios::badbit|is.rdstate());
      return is;
    }
  }

  s.somma();

  assert(s.controlla());
  return is;
}

//-----------------------------------------------------------------------------
//  Operator<<:
//-----------------------------------------------------------------------------
std::ostream &operator<<(std::ostream &os, const scacchiera &s)
{
  const bitboard pezzi(s.mappa());
  int cont(0);

  for (int y(7); y >= 0; y--)
  {
    for (int x(0); x < 8; x++)
    {
      const posizione p(posizione(x,y));

      if ( !pezzi[p] )
        ++cont;
      else
      {
        if (cont)
        {
          os << cont; 
          cont = 0;
        }
        
        const colore    c( s.c_pezzo(  p) );
        const pezzo pz( s.t_pezzo(c,p) );
        
        os << char(c==nero ? tolower(simbolo[pz]) : toupper(simbolo[pz]));
      }
    }

    if (cont)
    {
      os << cont; 
      cont = 0;
    }

    if (y)
      os << '/';
  }

  return os;
}

//-----------------------------------------------------------------------------
//  Controlla:
//-----------------------------------------------------------------------------
bool
scacchiera::controlla() const
{
  colore c(false);

  // Controllo del numero di pezzi presenti sulla scacchiera.
  do
  {
    if (pezzi[c][0].count() > 16)
      return false;

    for (unsigned pz(pedone); pz <= donna; pz++)
      if (pezzi[c][pz].count() > 10)
        return false;

    if (pezzi[c][re].count() != 1)
      return false;
  } while (c^=1);

  // Controllo consistenza interna.
  do
  {
    bitboard maschera(pezzi[c][pedone]);

    for (unsigned pz(cavallo); pz <= re; pz++)
    {
      // Non ci possono essere sovrapposizioni.
      if ( !(maschera&pezzi[c][pz]).vuota() )
        return false;

      maschera |= pezzi[c][pz];
    }

    if (pezzi[c][0] != maschera)
      return false;
  } while (c^=1);

  // Controllo posizione pedoni.
  if ( !((pezzi[nero][pedone]|pezzi[bianco][pedone]) & 
         (bitboard::traversa[0]|bitboard::traversa[7])).vuota() )
    return false;

  return (pezzi[nero][0]&pezzi[bianco][0]).vuota();
}

}  // Namespace Joker.
