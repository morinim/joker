/*
 *  Linea_apertura.cc
 */

#include "linea_apertura.h"

//-----------------------------------------------------------------------------
//  Setta_risultato:
//-----------------------------------------------------------------------------
void
Joker::linea_apertura::setta_risultato(risultato r)
{
  // Se la linea di apertura e' gia' stata riordinata, non c'e' modo di
  // individuare il giocatore attivo in una posizione.
  assert(!ordinata);

  unsigned n(0);
  
  for (linea_ap::iterator i(linea.begin()); i != linea.end(); i++, n++)
    switch (r)
    {
    case vittoria_bianco:
      if (n%2)
        i->sconfitta();
      else
        i->vittoria();
      break;

    case vittoria_nero:
      if (n%2)
        i->vittoria();
      else
        i->sconfitta();
      break;

    case patta:
      i->patta();
      break;

    default:
      ;
    }
}

//-----------------------------------------------------------------------------
//  Merge:
//-----------------------------------------------------------------------------
void
Joker::linea_apertura::merge(linea_apertura &l2)
{
  if (!ordinata)
    sort();

  if (!l2.ordinata)
    l2.sort();

  linea_ap::iterator i1(linea.begin());

  for (linea_ap::const_iterator i2(l2.linea.begin());
       i2 != l2.linea.end();
       i2++)
  {
    while (i1 != linea.end() && *i1 < *i2)
      ++i1;

    if (i1 == linea.end() || *i2 < *i1)
      linea.insert(i1,*i2);
    else
      *i1 = nodo_apertura(*i1 + *i2);
  }
}

//-----------------------------------------------------------------------------
//  Sort:
//-----------------------------------------------------------------------------
void
Joker::linea_apertura::sort()
{
  // Riordina la linea di apertura...
  linea.sort();
  ordinata = true;

  // ... ed unifica eventuali posizioni uguali.
  if (linea.size() > 1)
  {
    linea_ap::iterator prec(linea.begin()), i(prec);

    ++i;
    for (; i != linea.end(); prec=i, i++)
      if (*prec == *i)
      {
        *i = nodo_apertura(*prec+*i);
        linea.erase(prec);
      }
  }
}

//-----------------------------------------------------------------------------
//  Controlla():
//-----------------------------------------------------------------------------
bool
Joker::linea_apertura::controlla() const
{
  if (ordinata) 
    // Se la classe dice di essere ordinata... lo deve essere realmente!
    if (linea.size() > 1)  
    {
      linea_ap::const_iterator prec(linea.begin()), i(prec);
      
      ++i;
      for (; i != linea.end(); prec=i, i++)
        if ( !(*i > *prec) )
          return false;
    }

  for (linea_ap::const_iterator p(linea.begin()); p != linea.end(); p++)
  {
    unsigned n(0);

    if ((*p).n_patte() > 0)
      ++n;
    if ((*p).n_sconfitte() > 0)
      ++n;
    if ((*p).n_vittorie() > 0)
      ++n;

    // E' possibile che, in seguito ad una fusione di una posizione, una linea
    // di apertura abbia un campo con valore maggiore di uno; non e' pero'
    // possibile che una unica linea di apertura porti a due o piu' risultati
    // differenti.
    if (n > 1)
      return false;
  }

  return true;
}

//-----------------------------------------------------------------------------
//  Operator<<:
//-----------------------------------------------------------------------------
std::ostream &
Joker::operator<<(std::ostream &o, const linea_apertura &l)
{
  assert(l.ordinata);

  o << std::setw(10) << l.linea.size() << '\n';

  for (linea_apertura::linea_ap::const_iterator i(l.linea.begin());
       i != l.linea.end();
       i++)
    o << *i;

  return o;
}
