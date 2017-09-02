/*
 *  Componenti_valutazione.h
 */

#if !defined(COMPONENTI_VALUTAZIONE_H)
#define      COMPONENTI_VALUTAZIONE_H

#include "configurazione.h"
#include <list>
#include <string>
#include <utility>

namespace Joker
{

class componenti_valutazione
{
public:
  typedef std::list< std::pair<std::string,std::string> > l_cv;
  typedef l_cv::const_iterator                  const_iterator;
   
  void push_back(const std::string &, const std::string &);
  
  const_iterator begin() const { return cv.begin(); };
  const_iterator   end() const { return   cv.end(); };

private:
  l_cv cv;
};

}  // Namespace Joker.

#endif  // COMPONENTI_VALUTAZIONE_H
