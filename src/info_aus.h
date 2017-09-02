/*
 *  Info_aus.h
 */

#if !defined(INFO_AUS_H)
#define      INFO_AUS_H

#include <iostream>
#include <string>
#include "configurazione.h"
#include "colore.h"
#include "mossa.h"

namespace Joker
{

class info_aus
{
public:
  enum {O_Ob = 1, O_O_Ob = 2, O_On = 4, O_O_On = 8};

  info_aus(colore = bianco, unsigned = O_Ob|O_O_Ob|O_On|O_O_On,
           posizione = p_sent, unsigned = 0);

  unsigned arrocco() const { return ar; };
  unsigned cinquanta() const { return m50; };
  colore mossa_a() const { return attivo; };
  colore &mossa_a() { return attivo; };
  posizione p_en_passant() const { assert(ep.controlla()); return ep; };

  bool operator==(const info_aus &) const;
  bool operator!=(const info_aus &i) const { return !(*this == i); };

  bool controlla() const;

  friend std::istream &operator>>(std::istream &, info_aus &);
  friend std::ostream &operator<<(std::ostream &, const info_aus &);
  
protected:
  void muovi(const mossa_e &);
  void muovinull(info_aus *);
  void ripristina(const info_aus &);
  void ripristinanull(const info_aus &);

private:
  static const unsigned maschera_arrocco[64];

  colore attivo;
  posizione  ep;
  unsigned   ar;
  unsigned  m50;
};

//  Function prototypes per funzioni friend.
std::istream &operator>>(std::istream &, info_aus &);
std::ostream &operator<<(std::ostream &, const info_aus &);



//-----------------------------------------------------------------------------
//  Info_aus:
//-----------------------------------------------------------------------------
inline
info_aus::info_aus(colore c, unsigned a, posizione p, unsigned n) 
  : attivo(c), ep(p), ar(a), m50(n)
{
  assert(n <= 100 && p.controlla());
}

//-----------------------------------------------------------------------------
//  Muovi:
//-----------------------------------------------------------------------------
inline
void
info_aus::muovi(const mossa_e &m)
{
  ar &= maschera_arrocco[m.da()] & maschera_arrocco[m.a()];

  ep = m.e<doppio_p>() ? m.a() + (m.da() > m.a() ? alto : basso)
                       : posizione(p_sent);

  m50 = m.e<irreversibile>() ? 0 : m50+1;

  inverti(&attivo);
}

//-----------------------------------------------------------------------------
//  Muovinull:
//-----------------------------------------------------------------------------
inline
void
info_aus::muovinull(info_aus *r)
{ 
  r->ep = p_en_passant();
  ep    =         p_sent;

  inverti(&attivo);
}

//-----------------------------------------------------------------------------
//  Ripristina:
//-----------------------------------------------------------------------------
inline
void
info_aus::ripristina(const info_aus &r)
{
  *this = r;
}

//-----------------------------------------------------------------------------
// Ripristinanull:
//-----------------------------------------------------------------------------
inline
void
info_aus::ripristinanull(const info_aus &r)
{ 
  inverti(&attivo);

  ep = r.ep;
}

}  // Namespace Joker.

#endif  // INFO_AUS_H
