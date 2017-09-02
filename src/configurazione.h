/*
 *  Configurazione.h
 */

#if !defined(CONFIGURAZIONE_H)
#define      CONFIGURAZIONE_H

#include <climits>


enum {M_i386, M_i486, M_iPentium, M_iPentiumII, M_amdK6, M_iPentiumIII, 
      M_amdAthlon, 
      M_alpha};

// Definizione valori derivati.
#if MICROPROCESSORE >= M_i386 && MICROPROCESSORE <= M_amdAthlon
#  define M_INTEL
#endif

// Settaggio degli opportuni livelli di warning in base al compilatore.
#if defined(_MSC_VER)
#  pragma warning(disable : 4786)
#endif

#if defined(__BORLANDC__)
#  pragma warn -8026
#  pragma warn -8027
#endif

namespace Joker
{

// Identificazione interi senza segno di dimensioni fissate.
typedef unsigned char unsigned8;
const unsigned8 u8_max = UCHAR_MAX;

#if UINT_MAX <= 65535ul
  typedef unsigned long unsigned32;
  const unsigned32 u32_max = ULONG_MAX;
#else
  typedef unsigned unsigned32;
  const unsigned32 u32_max = UINT_MAX;
#endif

#if defined(__GNUC__) || defined(__CYGWIN__)
# define  U64(x)                  x##ull
  typedef long long unsigned int unsigned64;
#elif defined(__BORLANDC__) || defined(_MSC_VER)
  typedef unsigned __int64 unsigned64;
#else
# error "Impossibile identificare un tipo intero senza segno su 64 bit."
#endif

}  // Namespace Joker.

#endif  // CONFIGURAZIONE_H
