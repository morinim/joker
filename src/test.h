/*
 *  Test.h
 */

#if !defined(TEST_H)
#define      TEST_H

#include "configurazione.h"
#include <iostream>
#include <list>
#include "giocatore.h"

namespace Joker
{

// Function prototypes per funzioni friend.
class risultato;
class test;
std::ostream &operator<<(std::ostream &, const risultato &);



//-----------------------------------------------------------------------------
//  Mossa_corretta:
//-----------------------------------------------------------------------------
inline bool mossa_corretta(const lista_mosse &migliori, mossa suggerita)
{
  return migliori.empty() || migliori.find(suggerita);
}



class risultato
{
public:
  risultato::risultato(const std::string &,
                       const lista_mosse &, mossa,
                       unsigned long, unsigned long,
                       unsigned long, unsigned long, 
                       unsigned long, unsigned long,
                       unsigned, secondi);

  bool operator==(const risultato &r) const { return id == r.id && corretta == r.corretta; };
  bool operator!=(const risultato &r) const { return !(*this == r); };
  bool operator<(const risultato &r) const { return id < r.id; };
  bool operator>(const risultato &r) const { return id > r.id; };

  bool controlla() const;

  friend std::ostream &operator<<(std::ostream &, const risultato &);
  friend class test;

private:
  const std::string  id;

  const mossa suggerita;
  const bool   corretta;
  
  const unsigned long                nodir;
  const unsigned long                nodiq;
  const unsigned long                  nps;
  const unsigned long                 hits;
  const unsigned long             anomalie;
  const unsigned long           estensioni;
  const unsigned                      prof;
  const secondi                      tempo;
};

//-----------------------------------------------------------------------------
//  Risultato:risultato:
//-----------------------------------------------------------------------------
inline risultato::risultato(const std::string &i,
                            const lista_mosse &lm, mossa s,
                            unsigned long nr, unsigned long nq,
                            unsigned long nps1, unsigned long nh,
                            unsigned long ext, unsigned long an,
                            unsigned prof1, secondi tempo1)
  : id(i),
    suggerita(s), corretta(mossa_corretta(lm,s)),
    nodir(nr), nodiq(nq), nps(nps1), hits(nh), anomalie(an), estensioni(ext),
    prof(prof1), tempo(tempo1)
{
  assert(controlla());
}

//-----------------------------------------------------------------------------
//  Risultato::controlla:
//-----------------------------------------------------------------------------
inline bool risultato::controlla() const
{
  return hits <= nodir && estensioni <= nodir && suggerita.controlla() && 
         tempo.controlla();
}



class test
{
public:
  explicit test(const char [], const char [] = 0);  
  operator bool() const;

  bool esegui(secondi, unsigned, const parametri_ricerca &, bool);
  void stampa_risultati();

  bool controlla() const;

private:
  typedef std::list<risultato> lista_risultati;

  enum tipo_token {tag, argomento};
  static std::string token(std::string *, tipo_token);

  static void analizza_codici_epd(situazione &, std::string, std::string *, 
                                  lista_mosse *);

  std::ifstream   ift;
  std::ofstream   oft;
  
  lista_risultati ris;
};

//-----------------------------------------------------------------------------
//  Test::test:
//-----------------------------------------------------------------------------
inline test::test(const char nf_in[], const char nf_out[]) 
  : ift(nf_in), oft(nf_out)
{
  assert(nf_in && nf_out);
}

//-----------------------------------------------------------------------------
//  Test::operator bool:
//-----------------------------------------------------------------------------
inline test::operator bool() const
{
  return ift != 0;
}

}  // Namespace Joker.

#endif  // TEST_H
