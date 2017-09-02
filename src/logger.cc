/*
 *  Logger.cc
 */

#include "logger.h"

namespace Joker
{

logger logger::_instance;

//-----------------------------------------------------------------------------
//  logger:
//-----------------------------------------------------------------------------
logger::logger(const char n[], bool on) throw(Joker::logger::eccezioni)
  : nome(n), attivo(on)
{
  if (attivo)
  { 
    out.open(n);
    if (!out)  throw(impossibile_aprire_file);
  }
}

//-----------------------------------------------------------------------------
//  Stato:
//-----------------------------------------------------------------------------
void
logger::stato(bool nuovo) throw(Joker::logger::eccezioni)
{
  if (attivo != nuovo)
  {
    switch (nuovo)
    {
    case true:
      out.open(nome.c_str());
      if (!out)  throw(impossibile_aprire_file);
      break;

    case false:
      out.close();
    }

    attivo = !attivo;
  }
}

//-----------------------------------------------------------------------------
//  Stato_finale:
//-----------------------------------------------------------------------------
void
logger::stato_finale(double sec, unsigned long nhit)
{
  if (attivo)
  {
    out << "Total elapsed time: " << sec;
    out << "\nHash hits: " << nhit << '\n';
  }
}

//-----------------------------------------------------------------------------
//  Stato_iniziale:
//-----------------------------------------------------------------------------
void
logger::stato_iniziale(double sec, const componenti_valutazione &l)
{
  if (attivo)
  {
    out << "\nSearch time: " << sec;
    out << "\nPosition score/features\n-----------------------";
    for (componenti_valutazione::const_iterator i(l.begin()); i!=l.end(); i++)
      out << '\n' << i->first << ": " << i->second;
    out << "\n-----------------------\n";
  }
}

//-----------------------------------------------------------------------------
//  Controlla:
//-----------------------------------------------------------------------------
bool
logger::controlla() const
{
  return (!attivo || out.good());
}

}  // Namespace Joker.
