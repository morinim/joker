/*
 *  Linea_apertura.h
 */

#if !defined(LINEA_APERTURA_H)
#define      LINEA_APERTURA_H

#include <list>
#include "lista_mosse.h"
#include "nodo_apertura.h"
#include "situazione.h"

namespace Joker
{

/*
 *  Function prototypes per funzioni friend.
 */
class linea_apertura;
std::ostream &operator<<(std::ostream &, const linea_apertura &);



class linea_apertura
{
public:
  enum risultato {vittoria_bianco, vittoria_nero, patta, indefinito};

  linea_apertura();

  void pop();
  void push_back(const chiave_hash &);
  void setta_risultato(risultato);

  void clear();
  void merge(linea_apertura &);
  
  friend std::ostream &operator<<(std::ostream &, const linea_apertura &);

  bool controlla() const;

private:
  typedef std::list<nodo_apertura> linea_ap;

  void sort();

  linea_ap linea;
  bool  ordinata;
};

/*
 *  Linea_apertura:
 */
inline linea_apertura::linea_apertura() : ordinata(true)
{
}

/*
 *  Pop:
 */
inline void linea_apertura::pop()
{
  linea.pop_back();
}

/*
 *  Push_back:
 */
inline void linea_apertura::push_back(const chiave_hash &c)
{
  ordinata = false;
  linea.push_back(nodo_apertura(c));
}

/*
 *  Clear:
 */
inline void linea_apertura::clear()
{
  ordinata = true;
  linea.clear();
}

}  // Namespace Joker.

#endif
