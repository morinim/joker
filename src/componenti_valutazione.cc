/*
 *  Componenti_valutazione.cc
 */

#include "componenti_valutazione.h"

/*
 *  Push_back:
 */
void Joker::componenti_valutazione::push_back(const std::string &s1, 
                                              const std::string &s2)
{
  cv.push_back(std::make_pair(s1,s2));
}
