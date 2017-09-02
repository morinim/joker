/*
 *  PGN_stream.h
 */

#if !defined(PGN_H)
#define      PGN_H

#include <fstream>
#include <string>
#include "buffer_libro.h"
#include "esito.h"
#include "san.h"
#include "situazione.h"

namespace Joker
{

class pgn_stream
{
public:
  pgn_stream();
  pgn_stream(const char []);

  bool open(const char []);
  
  bool eof() const;
  bool read(buffer_libro *, unsigned = 0, bool = true);
  
  bool operator!() const;

private:
  static valutazione delta(colore, esito);

  bool e_un_risultato(const std::string &) const;
  bool leggi_intestazioni(esito *);
  bool leggi_mosse(situazione, buffer_libro *, unsigned, esito);
  
  bool      aperto;
  std::ifstream in;
};

//-----------------------------------------------------------------------------
//  Pgn_stream:
//-----------------------------------------------------------------------------
inline
pgn_stream::pgn_stream() 
  : aperto(false)
{
}

inline
pgn_stream::pgn_stream(const char nome[])
{
  assert(nome);
  
  aperto = open(nome);
}

//-----------------------------------------------------------------------------
//  Eof:
//-----------------------------------------------------------------------------
inline
bool
pgn_stream::eof() const
{
  return !aperto || in.eof();
}

//-----------------------------------------------------------------------------
//  Operator!:
//-----------------------------------------------------------------------------
inline
bool
pgn_stream::operator!() const
{
  return !aperto;
}

}  // Namespace Joker.

#endif
