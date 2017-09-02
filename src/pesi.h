/*
 *  Pesi.h
 */

#if !defined(PESI_H)
#define      PESI_H

#include <fstream>
#include <string>
#include "configurazione.h"
#include "valutazione.h"

namespace Joker
{

class pesi
{
public:
  enum eccezioni {formato_scorretto, lettura_dati_impossibile};

  enum indice_peso {re_esposto = 0, re_in_angolo, re_su_colonna_aperta,
                    re_al_centro,
                    donna_forte, donna_tropism, donna_sviluppo_precoce,
                    torre_intrappolata, torre_limitata, torre_tropism,
                    torre_su_colonna_aperta, torre_su_colonna_semiaperta,
                    torre_in_7a, torre_in_7a_assoluta, torre_dietro_passato,
                    cavallo_al_centro, cavallo_casa_forte, cavallo_bloccatore,
                    cavallo_non_sviluppato, cavallo_tropism, cavallo_avamposto,
                    alfiere_coppia, alfiere_forte, alfiere_non_sviluppato,
                    alfiere_tropism, alfiere_cattivo,
                    pedone_al_centro, pedone_vicino_centro,
                    pedone_debole, pedone_doppiato, pedone_isolato,
                    pedone_passato_connesso, pedone_passato, pedone_supportato,
                    pedone_avanzata, pedone_scudo_vicino, pedone_scudo_lontano,
                    pedone_baseA, pedone_baseB, pedone_baseC, pedone_baseD, 
                    pedone_baseE, pedone_baseF, pedone_baseG, pedone_baseH,
                    n_pesi};
  
  pesi() throw(eccezioni);
  
  valutazione operator[](indice_peso) const;

  bool controlla() const;

private:
  struct associazione { char *nome; indice_peso ind; };
  static const associazione nome_indice[n_pesi];
  static indice_peso ind(const std::string &) throw(eccezioni);

  valutazione w[n_pesi];
};

//-----------------------------------------------------------------------------
//  Operator[]: restituisce il valore del i-esimo peso.
//-----------------------------------------------------------------------------
inline valutazione pesi::operator[](indice_peso i) const
{
  assert(i < n_pesi);
  return w[i];
}

}  // Namespace Joker.

#endif  // PESI_H
