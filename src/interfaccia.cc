/*
 *  Interfaccia.cc
 */

#include "interfaccia.h"

namespace Joker
{

interfaccia interfaccia::_instance;

//-----------------------------------------------------------------------------
//  Setup_protocol:
//-----------------------------------------------------------------------------
void
interfaccia::setup_protocol()
{
  const std::string features[] =
  {
    "feature done=0",
    "feature ping=0",
    "feature setboard=1",
    "feature playother=0",
    "feature san=1",
    "feature usermove=1",
    "feature time=0",
    "feature draw=1",
    "feature sigint=0",
    "feature sigterm=0",
    "feature reuse=1",
    "feature analyze=1",
    "feature myname=\"Joker\"",
    "feature variants=\"normal\"",
    "feature colors=0",
    "feature ics=0",
    "feature name=0",
    //"feature pause=0",
    "feature done=1",
    ""
  };
  
  for (size_t i(0); !features[i].empty(); i++)
    trasmetti(features[i]);
}

//-----------------------------------------------------------------------------
//  Leggi_input:
//-----------------------------------------------------------------------------
bool
interfaccia::leggi_input(std::string &c, std::string arg[])
{
  bool ret(false);

  std::cin >> c;
  arg[0] = arg[1] = arg[2] = "";

  if (c == "xboard")
  {
    // Il comando "xboard" e' immediatamente seguito dalla versione del
    // protocollo di comunicazione.
    std::cin >> c;

    if (c == "protover")
    {
      std::cin >> arg[0];
      setup_protocol();
    }
    else
      stampa_errore(protocollo_non_supportato,arg[0]);
  }
  else if (c == "accepted")
    std::cin >> arg[0];  // Ignora il nome della features.
  else if (c == "rejected")
  {
    std::cin >> arg[0];
    stampa_errore(protocollo_non_supportato,"rejected feature "+arg[0]);
  }
  else if (c == "nopost")
    mostra_pensiero = false;
  else if (c == "post")
    mostra_pensiero = true;
  else
  {
    ret = true;

    // Comandi con argomento.
    if (c=="usermove" || c=="ping" || c=="time" || c=="otim" || c=="noise" ||
        c=="name" || c=="sd" || c=="st" || c=="variant")
      std::cin >> arg[0];
    else if (c=="result" || c=="rating")
      std::cin >> arg[0] >> arg[1];
    else if (c=="level")
      std::cin >> arg[0] >> arg[1] >> arg[2];
    else if (c=="setboard")
      std::cin >> arg[0] >> arg[1] >> arg[2] >> arg[3] >> arg[4] >> arg[5];
  }

  logger::instance() << "XBOARD> " << c << ' ' << arg[0] << ' ' << arg[1] 
                     << ' ' << arg[2] << '\n';

  return ret;
}

//-----------------------------------------------------------------------------
//  Stampa_errore:
//-----------------------------------------------------------------------------
void
interfaccia::stampa_errore(codice_errore ce, const std::string &s)
{
  switch (ce)
  {
  case comando_sconosciuto:
    trasmetti("Error (unknown command): "+s);
    break;

  case mossa_illegale:
    trasmetti("Illegal move: "+s);
    break;
    
  case protocollo_non_supportato:
    trasmetti("Illegal protocol version: "+s);
    break;
  }
}

//-----------------------------------------------------------------------------
//  Trasmetti: registra nel file di log il parametro s; se out==true (valore di
//             default) la invia anche sul canale di uscita.
//-----------------------------------------------------------------------------
void
interfaccia::trasmetti(const std::string &s, bool out)
{
  if (out) std::cout  << s << '\n';
  logger::instance() << "INTERFACE> " << s << '\n';
}

//-----------------------------------------------------------------------------
//  Stampa_pv:
//-----------------------------------------------------------------------------
void
interfaccia::stampa_pv(unsigned ply, valutazione v, secondi t, unsigned long n,
                       const std::string &pv)
{
  std::ostringstream ost;
  ost << ply << ' ' << v << ' ' << t.centisec() << ' ' << n << ' ' << pv;

  trasmetti(ost.str());
}

//-----------------------------------------------------------------------------
//  Stampa_risultato:
//-----------------------------------------------------------------------------
void
interfaccia::stampa_esito(esito e)
{
  switch (e)
  {
  case    resa_nero:  trasmetti("1-0 {Black resign}");  break;
  case vince_bianco:  trasmetti("1-0 {White mates}");   break;
  case        patta:  trasmetti("1/2-1/2 {Draw}");      break;
  case  resa_bianco:  trasmetti("0-1 {White resign}");  break;
  case   vince_nero:  trasmetti("0-1 {Black mates}");   break;
  case  sconosciuto:  trasmetti("*");
  }
}

}  // Namespace Joker.
