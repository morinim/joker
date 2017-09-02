/*
 *  Libro_apertura.h
 */

#if !defined(LIBRO_APERTURA_H)
#define      LIBRO_APERTURA_H

#include "configurazione.h"
#include <ctime>
#include <fstream>
#include "interfaccia.h"
#include "nodo_apertura.h"
#include "san.h"
#include "situazione.h"

namespace Joker
{

class libro_apertura
{
public:
  enum modalita {apprendimento, casuale, torneo};

  static libro_apertura &instance();

  unsigned long posizioni() const;
  
  bool mosse_previste(const situazione &, lista_mosse *, mossa_e * = 0,
                      valutazione * = 0);

  bool aggiorna_variante(const lista_mosse &, colore, unsigned, bool, esito);

  void clear(bool, modalita);
  
  bool controlla() const;

private:
  explicit libro_apertura(const char [] = STANDARD_BOOK);

  bool find(const chiave_hash &, nodo_apertura *);
  bool find(const chiave_hash &);
  
  std::fstream  libro;
  bool         attivo;
  unsigned long nelem;
  modalita       mode;

  static libro_apertura _instance;
};

//-----------------------------------------------------------------------------
//  Instance:
//-----------------------------------------------------------------------------
inline
libro_apertura &
libro_apertura::instance()
{
  return _instance;
}

//-----------------------------------------------------------------------------
//  Libro_apertura:
//-----------------------------------------------------------------------------
inline
libro_apertura::libro_apertura(const char n[])
{
  libro.open(n,std::ios::in|std::ios::out|std::ios::binary);
  assert(libro.is_open());

  libro >> nelem;

  clear(true,apprendimento);
}

//-----------------------------------------------------------------------------
//  Posizioni:
//-----------------------------------------------------------------------------
inline
unsigned long
libro_apertura::posizioni() const
{
  return nelem;
}

//-----------------------------------------------------------------------------
//  Clear:
//-----------------------------------------------------------------------------
inline
void
libro_apertura::clear(bool a, modalita m)
{
  attivo = a;
  mode   = m;
}

}  // Namespace Joker.

#endif  // LIBRO_APERTURA_H
