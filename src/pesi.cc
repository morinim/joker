/*
 *  Pesi.cc
 */

#include "pesi.h"

namespace Joker
{

const pesi::associazione pesi::nome_indice[n_pesi] = 
{
  {           "Bishop_(bad)",             alfiere_cattivo},
  {  "Bishop_(king_tropism)",             alfiere_tropism},
  {          "Bishop_(pair)",              alfiere_coppia},
  {        "Bishop_(strong)",               alfiere_forte},
  {       "Bishop_(unmoved)",      alfiere_non_sviluppato},
  {          "King_(center)",                re_al_centro},
  {          "King_(corner)",                re_in_angolo},
  {   "King_(off_back_rank)",                  re_esposto},
  {"King_(safety_open_file)",        re_su_colonna_aperta},
  {      "Knight_(blockade)",          cavallo_bloccatore},
  {        "Knight_(center)",           cavallo_al_centro},
  {  "Knight_(king_tropism)",             cavallo_tropism},
  {      "Knight_(outpost*)",           cavallo_avamposto},
  {"Knight_(strong_square*)",          cavallo_casa_forte},
  {       "Knight_(unmoved)",      cavallo_non_sviluppato},
  {         "Pawn_(advance)",             pedone_avanzata},
  {           "Pawn_(baseA)",                pedone_baseA},
  {           "Pawn_(baseB)",                pedone_baseB},
  {           "Pawn_(baseC)",                pedone_baseC},
  {           "Pawn_(baseD)",                pedone_baseD},
  {           "Pawn_(baseE)",                pedone_baseE},
  {           "Pawn_(baseF)",                pedone_baseF},
  {           "Pawn_(baseG)",                pedone_baseG},
  {           "Pawn_(baseH)",                pedone_baseH},
  {          "Pawn_(center)",            pedone_al_centro},
  {"Pawn_(connected_passed)",     pedone_passato_connesso},
  {         "Pawn_(doubled)",             pedone_doppiato},
  {        "Pawn_(isolated)",              pedone_isolato},
  {     "Pawn_(near_center)",        pedone_vicino_centro},
  {         "Pawn_(passed*)",              pedone_passato},
  {      "Pawn_(shield_far)",        pedone_scudo_lontano},
  {     "Pawn_(shield_near)",         pedone_scudo_vicino},
  {      "Pawn_(supported*)",           pedone_supportato},
  {            "Pawn_(weak)",               pedone_debole},
  {     "Queen_(early_exit)",      donna_sviluppo_precoce},
  {   "Queen_(king_tropism)",               donna_tropism},
  {         "Queen_(strong)",                 donna_forte},
  {    "Rook_(absolute_7th)",        torre_in_7a_assoluta},
  {   "Rook_(behind_passed)",        torre_dietro_passato},
  {  "Rook_(half_open_file)", torre_su_colonna_semiaperta},
  {    "Rook_(king_tropism)",               torre_tropism},
  {         "Rook_(limited)",              torre_limitata},
  {          "Rook_(on_7th)",                 torre_in_7a},
  {       "Rook_(open_file)",     torre_su_colonna_aperta},
  {         "Rook_(trapped)",          torre_intrappolata}
};



//-----------------------------------------------------------------------------
//  Ind:
//-----------------------------------------------------------------------------
inline pesi::indice_peso pesi::ind(const std::string &nome) throw(Joker::pesi::eccezioni)
{
  size_t min_i(0), max_i(n_pesi-1);

  while (min_i <= max_i)
  {
    const size_t i((min_i+max_i)/2);

    if (nome_indice[i].nome < nome)
      min_i = i+1;
    else if (nome < nome_indice[i].nome)
      max_i = i-1;
    else
      return nome_indice[i].ind;
  }

  throw(formato_scorretto);  
}

//-----------------------------------------------------------------------------
//  Pesi:
//-----------------------------------------------------------------------------
pesi::pesi() throw(Joker::pesi::eccezioni)
{
  std::ifstream in(STANDARD_WEIGHTS);
    
  if (!in)
    throw lettura_dati_impossibile;

  size_t      i(0);
  std::string nome;

  while (i < n_pesi && in >> nome && in >> w[ind(nome)])
    ++i;
    
  if (i != n_pesi)
    throw lettura_dati_impossibile;
}

//-----------------------------------------------------------------------------
//  Controlla:
//-----------------------------------------------------------------------------
bool pesi::controlla() const
{
  size_t i(1);
  while (i < n_pesi &&
         std::string(nome_indice[i-1].nome) < std::string(nome_indice[i].nome))
    ++i;

  return i == n_pesi;
}

}  // Namespace Joker.
