/*
 *  Egtb_def.h
 */

#if !defined(EGTB_DEF_H)
#define      EGTB_DEF_H

#define NEW
#define MAX_P                          3
#define XX  Joker::posizione::sentinella

typedef unsigned long INDEX;
typedef int          square;

#define SqFindKing(psq)                     (psq[MAX_P*5])
#define SqFindOne(psq,pi)              (psq[MAX_P*(pi-1)])
#define SqFindFirst(psq,pi)            (psq[MAX_P*(pi-1)])
#define SqFindSecond(psq,pi)         (psq[MAX_P*(pi-1)+1])
#define SqFindThird(psq,pi)          (psq[MAX_P*(pi-1)+2])

#endif  // EGTB_DEF_H
