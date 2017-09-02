/*
 *  Fase_estrazione.h
 */

#if !defined(FASE_ESTRAZIONE_H)
#define      FASE_ESTRAZIONE_H

namespace Joker
{

enum fase_estrazione { 
                       mossa_hash, 
                       generazione_catture, mosse_cattura, 
                       mossa_killer0, mossa_killer1, 
                       generazione_non_catture, altre_mosse, 
                       mossa_hash_evasione,
                       generazione_evasioni, mosse_evasione 
                     };

//-----------------------------------------------------------------------------
//  F_estrazione:
//-----------------------------------------------------------------------------
inline
fase_estrazione
f_estrazione(bool sotto_scacco)
{
  return !sotto_scacco ? mossa_hash : mossa_hash_evasione;
}

} // Namespace Joker.

#endif  // FASE_ESTRAZIONE_H
