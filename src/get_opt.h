/*
 *  Get_opt.h
 */

#if !defined(GET_OPT_H)
#define      GET_OPT_H

#include <string>

const char SW = '-';

extern char *arg_corrente;
extern unsigned opt_ind;

struct opzione
{
  opzione(const std::string &, bool, int);

  const std::string nome;
  bool         argomento;
  int                val;
};

//-----------------------------------------------------------------------------
//  Opzione:
//-----------------------------------------------------------------------------
inline
opzione::opzione(const std::string &s, bool a, int v) 
  : nome(s), argomento(a), val(v) 
{
}



int get_opt(int, char *const [], const opzione [], unsigned &);

#endif  // GET_OPT_H
