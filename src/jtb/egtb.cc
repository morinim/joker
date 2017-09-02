/*
 *  Egtb.cc
 */

#include "egtb.h"
#include "tbindex.cc"

namespace Joker
{

/*
 *  Egtb:
 */
egtb::egtb(char n[]) : cache(new char[1<<20]), num(IInitializeTb(n))
{
  if (num)
  {
    Log::joker << "\nEGTB: " << num << " piece tablebase files found.\n";
    FTbSetCacheSize(cache,1<<20);
    Log::joker << "EGTB: cache initialized.\n";
  }
}

/*
 *  ~Egtb:
 */
egtb::~egtb()
{
  delete [] cache;
  VTbCloseFiles();
}

/*
 *  Compila:
 */
void egtb::compila(const situazione &s, int *const pc[2], square *const psq[2]) const
{
  colore c(false);

  do
    for (size_t i(0); i < 5; i++)
      pc[c][i] = 0;
  while (++c);

  do
  {
    for (lista_pezzi::const_iterator i(s.begin(c)+1); i != s.end(c); i++)
    {
      const unsigned ip(i->t_pezzo()-1);

      psq[c][ip*MAX_P + pc[c][ip]] = i->p_pezzo().ind64();
      ++pc[c][ip];
    }

    psq[c][(re-1)*MAX_P] = s.p_re(c).ind64();
  } while (++c);
}

/*
 *  Find:
 */
bool egtb::find(const situazione &s, valutazione *v) const
{
  int rgiCounters[10];
  int *const pc[2] = { rgiCounters+5, rgiCounters };
  square rgsq[2][MAX_P*5+1];
  square *psq[2] = { rgsq[0], rgsq[1] };
  
  compila(s,pc,psq);

  // Verifica se esite una tabella riguardante l'attuale insieme di pezzi.
  int iTb(IDescFindFromCounters(rgiCounters));
  if (!iTb)  return false; // No such TB

  bool fInvert;
  color   side;
  if (iTb > 0)
  {
    side    = (s.mossa_a()==colore::nero) ? x_colorBlack : x_colorWhite;
    fInvert =                                                     false;
    psq[colore::bianco] = rgsq[colore::bianco];
    psq[  colore::nero] = rgsq[  colore::nero];
  }
  else
  {
    side    = (s.mossa_a()==colore::nero) ? x_colorWhite : x_colorBlack;
    fInvert =                                                      true;
    psq[colore::bianco] = rgsq[  colore::nero];
    psq[  colore::nero] = rgsq[colore::bianco];
    iTb                 =                 -iTb;
  }

  if (!FRegistered(iTb, side))
    return false;

  const square sqEnP(s.p_en_passant());
  INDEX ind(PfnIndCalc(iTb,side)(psq[colore::bianco],psq[colore::nero],sqEnP,
                                 fInvert));

  const int tbv(L_TbtProbeTable(iTb,side,ind));
  if (tbv == L_bev_broken)  return false;

  if (tbv > 0)
    *v = matto-L_bev_mi1+tbv-1;
  else if (tbv < 0)
    *v = -matto+2*(L_bev_mi1+tbv);
  else
    *v = 0;
  return true;
}

}  // Namespace Joker.
