/*
 *  Interfaccia.h
 */

#if !defined(INTERFACCIA_H)
#define      INTERFACCIA_H

#include <fstream>
#include <iomanip>
#include <sstream>
#include "configurazione.h"
#include "colore.h"
#include "esito.h"
#include "logger.h"
#include "secondi.h"
#include "valutazione.h"

namespace Joker
{

class interfaccia
{
public:
  enum codice_errore {comando_sconosciuto, mossa_illegale, 
                      protocollo_non_supportato};

  static interfaccia &instance();
  
  bool input_disponibile();
  bool leggi_input(std::string &, std::string []);

  void stampa_errore(codice_errore, const std::string &);
  void stampa_fail_high();
  void stampa_fail_low();
  void stampa_mossa(const std::string &);
  void stampa_pv(unsigned, valutazione, secondi, unsigned long,
                 const std::string &);
  void stampa_esito(esito);
  
  bool controlla() const;

private:
  interfaccia();

  void setup_protocol();
  void trasmetti(const std::string &, bool = true);
  
  bool mostra_pensiero;

  static interfaccia _instance;
};



//-----------------------------------------------------------------------------
//  Interfaccia:
//-----------------------------------------------------------------------------
inline
interfaccia::interfaccia()
  : mostra_pensiero(true)
{
  std::cout.setf(std::ios::unitbuf);
  std::cin.rdbuf()->pubsetbuf(0,0);
}

//-----------------------------------------------------------------------------
//  Stampa_fail:
//-----------------------------------------------------------------------------
inline
void
interfaccia::stampa_fail_high()
{
  trasmetti("++");
}

//-----------------------------------------------------------------------------
//  Stampa_low:
//-----------------------------------------------------------------------------
inline
void
interfaccia::stampa_fail_low()
{
  trasmetti("--");
}

//-----------------------------------------------------------------------------
//  Stampa_mossa:
//-----------------------------------------------------------------------------
inline
void
interfaccia::stampa_mossa(const std::string &m)
{
  trasmetti("move "+m);
}

//-----------------------------------------------------------------------------
//  Controlla:
//-----------------------------------------------------------------------------
inline
bool
interfaccia::controlla() const 
{
  return true;
}

//-----------------------------------------------------------------------------
//  Instance:
//-----------------------------------------------------------------------------
inline
interfaccia &
interfaccia::instance()
{
  return _instance;
}

//-----------------------------------------------------------------------------
//  Input_disponibile:
//-----------------------------------------------------------------------------
inline
bool
interfaccia::input_disponibile()
{
  return input_available();
}

}  // Namespace Joker.

#endif  // INTERFACCIA_H
