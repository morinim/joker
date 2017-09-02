/*
 *  Partita.h
 */

#if !defined(PARTITA_H)
#define      PARTITA_H

#include "configurazione.h"
#include "gestore_tempo.h"
#include "giocatore.h"

namespace Joker
{

class partita
{
public:
  explicit partita(const parametri_ricerca & = parametri_ricerca());
  ~partita();

  void gioca();

  bool controlla() const;

private:
  enum {sup_mosse=500};
  enum stato_joker {analizza, attivo, inattivo};

  void calcola();
  void muovi(const mossa_e &);
  
  void analyze();
  void computer();
  void force();
  void go();
  void level(const std::string []);
  void name(const std::string &);
  void new_game(const situazione_valutata & = situazione_valutata());
  void playother();
  void remove();
  void result(const std::string &);
  void undo();
  void usermove(const std::string &);

  bool fine_partita(valutazione) const;

  const parametri_ricerca  pr;

  std::string nome_avversario;
  unsigned     elo_avversario;
  bool    computer_avversario;

  stato_joker           stato;
  colore                joker;
  
  dati_r   rimuovi[sup_mosse];
  lista_mosse           mosse;
  situazione_valutata       s;

  gestore_tempo           *gt;
  giocatore                *g;
};

//-----------------------------------------------------------------------------
//  ~Partita:
//-----------------------------------------------------------------------------
inline
partita::~partita()
{
  delete gt;
  delete  g;
}

}  // Namespace Joker.

#endif  // PARTITA_H
