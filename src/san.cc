/*
 *  San.cc
 */

#include "san.h"

namespace Joker
{

//-----------------------------------------------------------------------------
//  Id: restituisce il codice del pezzo che ha come simbolo il carattere s.
//-----------------------------------------------------------------------------
pezzo san::id(char s)
{
  pezzo pz(pedone);
  while ( pz <= re && s != simbolo[pz] )
    ++pz;

  return pz <= re ? pz : vuoto;
}

//-----------------------------------------------------------------------------
//  San::a:
//-----------------------------------------------------------------------------
std::string san::a(situazione &s, mossa m)
{
  assert(s.controllo_rapido_legalita(s.mossa_a(),m));

  lista_mosse lm(lista_mosse::max_varianti);
  s.calcola_legali(&lm);

  // Controlla se ci sono piu' pezzi di tipo pz in grado di muovere in arrivo.
  mossa_e m_e;
  bool unico_su_colonna(true), unico_su_traversa(true);
  const pezzo mosso(s.t_pezzo(s.mossa_a(),m.da()));

  for (lista_mosse::const_iterator i(lm.begin()); i != lm.end(); i++)
  {
    if (m == *i)
      m_e = *i;

    if (mosso == s.t_pezzo(s.mossa_a(),i->da()) && 
        m.a() == i->a() && m.da() != i->da())
      if (m.da().colonna() == i->da().colonna())
        unico_su_colonna  = false;
      else
        unico_su_traversa = false;
  }

  // Effettua la vera e propria conversione.
  const std::string arrivo(m.a().str());
  std::string ret("");

  if (m_e.e<arrocco>())
    if (m.a() == g1 || m.a() == g8)  // Arrocco corto.
      ret = "O-O";
    else
      ret = "O-O-O";
  else
  {
    if (mosso != pedone)
    {
      ret += simbolo[mosso];

      if (!unico_su_colonna || !unico_su_traversa)
        ret += unico_su_colonna ? static_cast<char>('a' +  m.da().colonna())
                                : static_cast<char>('1' + m.da().traversa());
    }

    if (m_e.e<cattura>())
    {
      if (mosso == pedone)
        ret += static_cast<char>('a'+m.da().colonna());

      ret += 'x';
    }

    ret += arrivo;

    if (m_e.e<promozione>())
    {
      ret +=                         '=';
      ret += simbolo[m_e.promozione_a()];
    }
  }

  if (s.da_scacco(m_e))
    ret += '+';

  return ret;
}

//-----------------------------------------------------------------------------
//  San::da:
//-----------------------------------------------------------------------------
bool san::da(situazione &s, const std::string &str, mossa_e *m)
{
  const unsigned niente(100);
  pezzo pz(pedone), promosso(vuoto);
  posizione fine;
  unsigned colonna(niente), traversa(niente);

  const std::string s3(str,0,3), s5(str,0,5);
  
  // Notare che e' importante l'ordine di controllo fra arrocchi: se si 
  // controllasse per primo l'arrocco corto, il test risulterebbe positivo,
  // erroneamente, anche per l'arrocco lungo.
  if (s5 == "O-O-O" || s5 == "0-0-0")  // Arrocco lungo.  
  {
    pz   =                              re;
    fine = (s.mossa_a() == nero) ? c8 : c1;
  }
  else if (s3 == "O-O" || s3 == "0-0")  // Arrocco corto.
  {
    pz   =                              re;
    fine = (s.mossa_a() == nero) ? g8 : g1;
  }
  else if (!id(str[0]))  // Mossa di pedone.
    if (str[1] == 'x')
    {
      colonna =                       str[0]-'a';
      fine    = posizione(str[2]-'a',str[3]-'1');

      if (str.size() >= 6)
      {
        promosso = id(str[5]);
        assert(promosso);
      }
    }
    else
    {
      fine = posizione(str[0]-'a',str[1]-'1');

      if (str.size() >= 4)
      {
        promosso = id(str[3]);
        assert(promosso);
      }
    }
  else  // Tutte le altre mosse.
  {
    pz = id(str[0]);

    if (str[1] == 'x')
      fine = posizione(str[2]-'a',str[3]-'1');
    else if (str[2] == 'x')
    {
      if (isdigit(str[1]))
        traversa = str[1]-'1';
      else
        colonna  = str[1]-'a';
      fine = posizione(str[3]-'a',str[4]-'1');
    }
    else if (isdigit(str[3]))
    {
      if (isdigit(str[1]))
        traversa = str[1]-'1';
      else
        colonna  = str[1]-'a';
      fine = posizione(str[2]-'a',str[3]-'1');
    }
    else
      fine = posizione(str[1]-'a',str[2]-'1');
  }

  lista_mosse lm(lista_mosse::max_varianti);  
  s.calcola_tutte(&lm);
  
  for (lista_mosse::const_iterator i(lm.begin()); i != lm.end(); i++)
    if (s.t_pezzo(s.mossa_a(),i->da()) == pz && i->a() == fine &&
        (colonna  == niente ||  i->da().colonna() ==  colonna) &&
        (traversa == niente || i->da().traversa() == traversa) && 
        (!promosso || (i->e<promozione>() && i->promozione_a()==promosso)))
    {
      *m = *i;
      return true;
    }

  return false; 
}

//-----------------------------------------------------------------------------
//  Lista:
//-----------------------------------------------------------------------------
std::string san::lista(const situazione &s, const lista_mosse &lm,
                       char commento)
{
  dati_r r;
  situazione s1(s);
  std::string ret("");

  for (lista_mosse::const_iterator i(lm.begin()); i != lm.end(); i++)
  {
    ret += san::a(s1,*i);
    
    if (i == lm.begin() && commento)
      ret += commento;

    if (i+1 != lm.end())
    {
      ret += " ";
      s1.muovi(*i,r);
    }
  }

  return ret;
}

}  // Namespace Joker.
