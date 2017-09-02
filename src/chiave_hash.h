/*
 *  Chiave_hash.h
 */

#if !defined(CHIAVE_HASH_H)
#define      CHIAVE_HASH_H

#include <fstream>
#include <iomanip>
#include "configurazione.h"
#include "random.h"
#include "scacchiera.h"

namespace Joker
{

//  Function prototypes per funzioni friend.
class chiave_hash;
std::istream &operator>>(std::istream &, chiave_hash &);
std::ostream &operator<<(std::ostream &, const chiave_hash &);



class chiave_hash
{
public:
  explicit chiave_hash(const unsigned64 & = 0);
  chiave_hash(const scacchiera &, colore);

  bool operator<(const chiave_hash &) const;
  bool operator>(const chiave_hash &) const;
  bool operator==(const chiave_hash &) const;
  bool operator!=(const chiave_hash &) const;
  
  friend unsigned64 distanza(const chiave_hash &, const chiave_hash &);
  unsigned64 maschera(const unsigned64 &) const;
    
  void muovi(colore, const mossa_e &);
  void muovinull();
  void ripristina(const chiave_hash &);
  void ripristinanull();

  friend std::istream &operator>>(std::istream &, chiave_hash &);
  friend std::ostream &operator<<(std::ostream &, const chiave_hash &);

  bool controlla() const;

#if defined(_DEBUG)
  bool verifica(const scacchiera &, colore) const;
#endif

protected:
  static void calcola_array();

private:
  static unsigned64 codice_posizione[2][7][64];

  void sposta(colore, pezzo, mossa);
  void inverti_colore();

  unsigned64 chiave;
};

//-----------------------------------------------------------------------------
//  Chiave_hash:
//-----------------------------------------------------------------------------
inline
chiave_hash::chiave_hash(const unsigned64 &k) 
  : chiave(k)
{
}

//-----------------------------------------------------------------------------
//  Operator<:
//-----------------------------------------------------------------------------
inline
bool
chiave_hash::operator<(const chiave_hash &k) const
{
  return chiave < k.chiave;
}

//-----------------------------------------------------------------------------
//  Operator>:
//-----------------------------------------------------------------------------
inline
bool
chiave_hash::operator>(const chiave_hash &k) const
{
  return chiave > k.chiave;
}

//-----------------------------------------------------------------------------
//  Operator==:
//-----------------------------------------------------------------------------
inline
bool
chiave_hash::operator==(const chiave_hash &k) const
{
  return chiave == k.chiave;
}

//-----------------------------------------------------------------------------
//  Operator!=:
//-----------------------------------------------------------------------------
inline
bool
chiave_hash::operator!=(const chiave_hash &k) const
{
  return chiave != k.chiave;
}

//-----------------------------------------------------------------------------
//  Sposta:
//-----------------------------------------------------------------------------
inline
void
chiave_hash::sposta(colore c, pezzo pz, mossa m)
{
  chiave ^= codice_posizione[c][pz][m.da()];
  chiave ^= codice_posizione[c][pz][ m.a()];
}

//-----------------------------------------------------------------------------
//  Inverti_colore:
//-----------------------------------------------------------------------------
inline
void
chiave_hash::inverti_colore()
{
  chiave = ~chiave;
}

//-----------------------------------------------------------------------------
//  Muovi:
//-----------------------------------------------------------------------------
inline
void
chiave_hash::muovi(colore c, const mossa_e &m)
{
  // Sposta il pezzo che muove.
  sposta(c,m.mosso(),m);
    
  // Effettua le modifiche della chiave legate al tipo di mossa.
  if (m.e<cattura>())
    if (!m.e<en_passant>())
      chiave ^= codice_posizione[!c][m.preda()][m.a()];
    else
      chiave ^= codice_posizione[!c][pedone][m.preda_en_passant()];
   
  if (m.e<arrocco>())
    switch (m.a())
    {
    case c1:  sposta(bianco,torre,mossa(a1,d1)); break;
    case g1:  sposta(bianco,torre,mossa(h1,f1)); break;
    case c8:  sposta(  nero,torre,mossa(a8,d8)); break;
    default:  sposta(  nero,torre,mossa(h8,f8)); assert(m.a() == g8);
    }
  else if (m.e<promozione>())
  {
    chiave ^= codice_posizione[c][pedone][m.a()];
    chiave ^= codice_posizione[c][m.promozione_a()][m.a()];
  }

  // Alla fine si cambia il giocatore attivo.
  inverti_colore();
}

//-----------------------------------------------------------------------------
//  Muovinull:
//-----------------------------------------------------------------------------
inline
void
chiave_hash::muovinull()
{
  inverti_colore();
}

//-----------------------------------------------------------------------------
//  Ripristina:
//-----------------------------------------------------------------------------
inline
void
chiave_hash::ripristina(const chiave_hash &k)
{
  *this = k;
}

//-----------------------------------------------------------------------------
//  Ripristinanull:
//-----------------------------------------------------------------------------
inline
void
chiave_hash::ripristinanull()
{
  inverti_colore();  
}

//-----------------------------------------------------------------------------
//  Controlla:
//-----------------------------------------------------------------------------
inline
bool
chiave_hash::controlla() const
{
  return true;
}

//-----------------------------------------------------------------------------
//  Distanza:
//-----------------------------------------------------------------------------
inline
unsigned64
distanza(const chiave_hash &c1, const chiave_hash &c2)
{
  return c1 > c2 ? c1.chiave-c2.chiave : c2.chiave-c1.chiave;
}

//-----------------------------------------------------------------------------
//  Maschera:
//-----------------------------------------------------------------------------
inline
unsigned64
chiave_hash::maschera(const unsigned64 &m) const
{
  return chiave & m;
}

}  // Namespace Joker.

#endif  // CHIAVE_HASH_H
