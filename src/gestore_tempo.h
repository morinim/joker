/*
 *  Gestore_tempo.h
 */

#if !defined(GESTORE_TEMPO_H)
#define      GESTORE_TEMPO_H

#include <cassert>
#include <exception>
#include <string>
#include "configurazione.h"
#include "secondi.h"

namespace Joker
{

class gestore_tempo
{
public:
  static const secondi tempo_minimo;
  
  enum eccezioni {argomenti};
  
  gestore_tempo(const std::string & = "40",
                const std::string & = "5",
                const std::string & = "0") throw(eccezioni);
  gestore_tempo(secondi);
  
  void aggiorna(secondi);
  secondi tempo_calcolo() const;

  bool controlla() const;

private:
  static const double margine_sicurezza_percentuale;

  class controllo
  {
  public:
    controllo(int m1 = 0, secondi t1 = 0.0) : m(m1), t(t1) {};

    int mosse() const { return m; };
    secondi ritmo() const { return t/m; };
    secondi tempo() const { return t; };

    void aggiorna(secondi t1) { --m; t -= t1; };
    void operator+=(const controllo &c) { m += c.m; t += c.t; };

    bool controlla() const { return t.controlla(); };

  private:
    int     m;
    secondi t;
  };

  const controllo    primo;
  const controllo  secondo;
  const secondi incremento;
  
  controllo residuo;
};

}  // Namespace Joker.

#endif  // GESTORE_TEMPO_H
