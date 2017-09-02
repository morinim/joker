/*
 *  PGN_stream.cc
 */

#include "pgn_stream.h"

namespace Joker
{

//-----------------------------------------------------------------------------
//  Open:
//-----------------------------------------------------------------------------
bool
pgn_stream::open(const char nome[])
{
  assert(nome);

  in.open(nome);
  return aperto = !(!in);
}

//-----------------------------------------------------------------------------
//  E_un_risultato:
//-----------------------------------------------------------------------------
bool
pgn_stream::e_un_risultato(const std::string &s) const
{
  return s=="1/2-1/2" || s=="1-0" || s=="0-1" || s=="*";
}

//-----------------------------------------------------------------------------
//  Leggi_intestazioni:
//-----------------------------------------------------------------------------
bool
pgn_stream::leggi_intestazioni(esito *e)
{
  while (!eof())
  {
    int c(in.get());

    if ( !isspace(c) )
      if (c == '[')
      {
        std::string tag;  in >> tag;

        if (tag == "Result")
        {
          in >> tag;
          if (tag == "\"1-0\"]")
            *e = vince_bianco;
          else if (tag == "\"1/2-1/2\"]")
            *e = patta;
          else if (tag == "\"0-1\"]")
            *e = vince_nero;
          else
            *e = sconosciuto;
        }
        else
          in.ignore(1024,']');
      }
      else
      {
        if (eof())
          return false;

        in.putback(char(c));
        break;
      }
  }

  return true;
}

//-----------------------------------------------------------------------------
//  Delta:
//-----------------------------------------------------------------------------
inline
valutazione
pgn_stream::delta(colore c, esito e)
{
  // Se nella posizione p vince il giocatore a cui spetta muovere, allora 
  // valutazione(p) deve essere negativa in modo che l'avversario non scelga di
  // raggiungere p.
  if (c==bianco && e == vince_bianco)
    return -1;
  if (c==  nero && e ==   vince_nero)
    return -1;
  if (c==bianco && e ==   vince_nero)
    return +1;
  if (c==  nero && e == vince_bianco)
    return +1;
  
  return 0;
}

//-----------------------------------------------------------------------------
//  Leggi_mosse: memorizza in bl un massimo di 'ancora' mosse, giocate a 
//               partire dalla posizione specificata in 's'. Se 'stat'==false
//               non memorizza nella variante  alcuna valutazione. Ritorna 
//               true se la lettura avviene senza inconvenienti, false in caso
//               contrario.
//-----------------------------------------------------------------------------
bool
pgn_stream::leggi_mosse(situazione s, buffer_libro *bl, unsigned ancora, 
                        esito e)
{
  static unsigned livello_variante(0);

  if (!eof())
  {  
    // Lettura del numero di mossa.
    std::string num;
    if ( !(in >> num) )
      return false;
  }

  while (!eof())
  {
    mossa_e m;
    dati_r dr;

    // Lettura della mossa del Bianco.
    std::string m1;
    if ( !(in >> m1) )
      return false;
    if (ancora && m1 != "...")
    {
      --ancora;

      if (san::da(s,m1,&m))
      {
        s.muovi(m,dr);

        bl->insert(s.chiave(),delta(s.mossa_a(),e));
      }
      else return false;
    }

    // Lettura di eventuali commenti alla mossa del Bianco.
    std::string c1;
    if ( !(in >> c1) )
      return false;
    if (c1[0] == '{')
      in.ignore(1024,'}');
      
    // Lettura di eventuali varianti del Bianco.
    std::string v1(c1);
    if (v1 == ")")
    {
      --livello_variante;
      return true;
    }
    while ( v1 == "(" )
    {
      situazione s1(s);  s1.ripristina(m,dr);
      
      ++livello_variante;
      if ( !leggi_mosse(s1,bl,ancora,e) || !(in >> v1) )
        return false;

      // Se non ci sono altre varianti, la mossa spetta ora al Nero. Bisogna
      // pero' ricordare che la sequenza di token prima della mossa attesa
      // sara': numero_mossa, punti_di_sospensione, mossa.
      if ( v1 != "(" )
        if ( !(in >> v1) || v1 != "..." || !(in >> v1) )
          return false;
    }

    // Lettura della mossa del Nero o del risultato.
    std::string m2(v1);
    if ( e_un_risultato(m2) )
      return !livello_variante;

    if (ancora)
    {
      --ancora;

      if (san::da(s,m2,&m))
      {
        s.muovi(m,dr);
        bl->insert(s.chiave(),delta(s.mossa_a(),e));
      }
      else return false;
    }
    
    // Lettura di eventuali commenti alla mossa del Nero.
    std::string c2;
    if ( !(in >> c2) )
      return false;
    if (c2[0] == '{')
      in.ignore(1024,'}');

    // Lettura di eventuali varianti del Nero.
    std::string v2(c2);
    if (v2 == ")")
    {
      --livello_variante;
      return true;
    }
    while ( v2 == "(" )
    {
      situazione s2(s);  s2.ripristina(m,dr);

      ++livello_variante;
      if ( !leggi_mosse(s2,bl,ancora,e) || !(in >> v2) )
        return false;
    }
  
    // Lettura del numero di mossa o del risultato.
    std::string num(v2);
    if ( e_un_risultato(num) )
      return !livello_variante;
  }

  return false;
}

//-----------------------------------------------------------------------------
//  Read:
//-----------------------------------------------------------------------------
bool
pgn_stream::read(buffer_libro *db, unsigned cut, bool stat)
{
  esito e;

  if (aperto && leggi_intestazioni(&e))
  {
    const situazione s;
    
    if (!cut)
      cut = UINT_MAX;

    if (!stat)
      e = sconosciuto;

    db->insert(s.chiave(),delta(bianco,e));

    return leggi_mosse(s,db,cut,e);
  }

  return false;
}

}  // Namespace Joker.
