/*
 *  Nonstd.h
 */

#if !defined(NONSTD_H)
#define      NONSTD_H

#include "configurazione.h"

#if defined(UNIX)
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#endif

#if defined(WIN32)
#include <sys/timeb.h>
#include <windows.h>
#endif

namespace Joker
{

#if defined(UNIX)
#  define read_clock_resolution .000001
#elif defined(WIN32)
#  define read_clock_resolution    .001
#else
#  error Costante read_clock_resolution ignota.
#endif



//  Function prototypes.
unsigned input_available();
unsigned MAX(unsigned, unsigned);
unsigned MIN(unsigned, unsigned);
double read_clock();



//-----------------------------------------------------------------------------
//  MAX:
//-----------------------------------------------------------------------------
inline
unsigned
MAX(unsigned x, unsigned y)
{
#if defined(ASSEMBLER) && defined(M_INTEL) && defined(_MSC_VER) 
  // Versione da me modificata della funzione MIN (si poteva anche utilizzare
  // lo stesso codice della funzione MIN con in piu' l'istruzione CMC).
  __asm
  {
    mov eax, dword ptr x    // Load x value
    mov edx, dword ptr y    // Load y value
    sub eax, edx            // Set carry flag if y is greater than x
    sbb ecx, ecx            // Get borrow out from previous SUB
    and ecx, eax            // If y > x, ECX = x-y else 0
    sub edx, ecx            // If y > x, EDX = 2y-x else y
    add eax, edx            // If y > x, EAX = y else x
  }
#elif defined(__GNUC__) || defined(__CYGWIN__)
  return x >? y;
#else
  return x >= y ? x : y;
#endif
}

//-----------------------------------------------------------------------------
//  MIN:
//-----------------------------------------------------------------------------
inline
unsigned
MIN(unsigned x, unsigned y)
{
#if defined(ASSEMBLER) && defined(M_INTEL) && defined(_MSC_VER)
  // Implementazione tratta dal manuale "AMD K6 Code Optimization" (latenza
  // statica di 4 cicli). L'idea si basa sul fatto che, operando con registri
  // a 32 bit, min(x,y) = y+((x-y)>>31)&(x-y).
  __asm
  {
    mov ecx, dword ptr x    // Load x value
    mov edx, dword ptr y    // Load y value
    sub ecx, edx            // Set carry flag if y is greater than x
    sbb eax, eax            // Get borrow out from previous SUB
    and eax, ecx            // if y > x, EAX = x-y else 0
    add eax, edx            // if y > x, return x-y+y=x else y
  }
#elif defined(__GNUC__) || defined(__CYGWIN__)
  return x <? y;
#else
  return x <= y ? x : y;
#endif
}

//-----------------------------------------------------------------------------
//  Input_available:
//-----------------------------------------------------------------------------
inline
unsigned
input_available()
{
#if defined(UNIX)
#
  fd_set readfds;
  timeval tv;
  
  FD_ZERO(&readfds);
  FD_SET(0,&readfds);
  tv.tv_sec  = 0;
  tv.tv_usec = 0;
  select(16,&readfds,0,0,&tv);
  return FD_ISSET(0,&readfds);
#
#elif defined(WIN32)
#
 DWORD dw;
 static bool init(false);
 static HANDLE inh(GetStdHandle(STD_INPUT_HANDLE));
 static int pipe(!GetConsoleMode(inh, &dw));

 if (!init)
 {
   init = true;

   if (!pipe)
   {
     SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT));
     FlushConsoleInputBuffer(inh);
     FlushConsoleInputBuffer(inh);
   }
  }

  if(pipe)
    return !PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL) ? 1 : dw;
  else
  {
    GetNumberOfConsoleInputEvents(inh, &dw);
    return dw <= 1 ? 0 : dw;
  }
#
#else
#
#  error Funzione input_available indefinita.
#
#endif
}

//-----------------------------------------------------------------------------
//  Read_clock:
//-----------------------------------------------------------------------------
inline
double
read_clock()
{
#if defined(UNIX)
#
  timeval tv;

  gettimeofday(&tv,0);
  return read_clock_resolution*static_cast<double>(tv.tv_usec) +
                               static_cast<double>(tv.tv_sec);
#
#elif defined(_MSC_VER)
#
  _timeb tv;

  _ftime(&tv);
  return read_clock_resolution*static_cast<double>(tv.millitm) +
                               static_cast<double>(tv.time);
#
#elif defined(__BORLANDC__) || defined(__CYGWIN__)
#
  timeb tv;

  ::ftime(&tv);
  return read_clock_resolution*static_cast<double>(tv.millitm) +
                               static_cast<double>(tv.time);
#
#else
#
#  error Bisogna definire una funzione per misurare lo scorrere del tempo.
#
#endif
}

}  // Namespace Joker.

#endif  // NONSTD_H
