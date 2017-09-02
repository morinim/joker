/*
 *  Bitboard.h
 */

#if !defined(BITBOARD_H)
#define      BITBOARD_H

#include "configurazione.h"
#include "colore.h"
#include "posizione.h"

namespace Joker
{

class bitboard
{
public:
  static const bitboard          caselle[2];
  static const bitboard    grande_centro[2];
  static const bitboard          colonna[8];
  static const bitboard         traversa[8];
  static const bitboard        traversa7[2];
  static const bitboard     traversa7abs[2];
  static const bitboard         fianchi[64];
  static bitboard             avanti[2][64];
  static const bitboard        m_isolato[8];
  static bitboard          m_passato[2][64];
  static bitboard           m_scudo1[2][64];
  static bitboard           m_scudo2[2][64];
  static bitboard            raggio[64][64];
  static bitboard torre_intrappolata[2][64];

  bitboard() {};
  explicit bitboard(unsigned64);

  unsigned count() const;
  unsigned firstone() const;
  bool operator[](posizione) const;
  bool vuota() const;

  void clear();
  void aggiungi(posizione);
  void elimina(posizione);
  void sposta(posizione, posizione);

  bitboard operator<<(unsigned) const;
  bitboard operator>>(unsigned) const;

  operator unsigned64() const;

  bitboard operator|(bitboard) const;
  bitboard operator|=(bitboard);
  bitboard operator&(bitboard) const;
  bitboard operator&=(bitboard);
  bitboard operator~() const;

  bool operator==(bitboard) const;
  bool operator!=(bitboard) const;

  bool controlla() const;

protected:
  static void calcola_array();
  
private:
  static const bitboard     colonnaA;
  static const bitboard maschera[65];

  bitboard(posizione) { assert(false); };

  unsigned64 bb;
};

//-----------------------------------------------------------------------------
//  Bitboard:
//-----------------------------------------------------------------------------
inline
bitboard::bitboard(unsigned64 b) 
  : bb(b)
{
}

//-----------------------------------------------------------------------------
//  Count:
//-----------------------------------------------------------------------------
inline
unsigned
bitboard::count() const
{
#if defined(ASSEMBLER) && defined(M_INTEL) && defined(_MSC_VER)
  __asm
  {
      mov     ecx, dword ptr bb
      mov     eax, 0
      test    ecx, ecx
      jz      l1
  l0: lea     edx, [ecx-1]
      inc     eax
      and     ecx, edx
      jnz     l0
  l1: mov     ecx, dword ptr bb+4
      test    ecx, ecx
      jz      l3
  l2: lea     edx, [ecx-1]
      inc     eax
      and     ecx, edx
      jnz     l2
  l3: 
  }
#elif defined(__BORLANDC__)
  static const unsigned8 bits[256] =
  {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8 
  };

  return bits[unsigned32(bb       & 255)] + bits[unsigned32((bb>> 8) & 255)] + 
         bits[unsigned32((bb>>16) & 255)] + bits[unsigned32((bb>>24) & 255)] +
         bits[unsigned32((bb>>32) & 255)] + bits[unsigned32((bb>>40) & 255)] +
         bits[unsigned32((bb>>48) & 255)] + bits[unsigned32((bb>>56) & 255)];
/*#elif defined(ASSEMBLER) && defined(M_INTEL) && defined(__GNUC__)
  unsigned n;

  __asm__
  ("movl        $0,    %0\n\
    testl    %%eax, %%eax\n\
    jz                 1f\n\
   0:\n\
    leal -1(%%eax), %%ecx\n\
    incl               %0\n\
    andl     %%ecx, %%eax\n\
    jnz                0b\n\
   1:\n\
    testl    %%edx, %%edx\n\
    jz                 3f\n\
   2:\n\
    leal -1(%%edx), %%ecx\n\
    incl               %0\n\
    andl     %%ecx, %%edx\n\
    jnz                2b\n\
   3:"
    : "=r" (n)
    : "A" (bb)
    : "ecx"
  );

  return n;*/
#else
  //static const unsigned64 m1(0x5555555555555555ull);
  //static const unsigned64 m2(0x3333333333333333ull);
  //const unsigned64 a( bb - ((bb>>1) & m1) );
  //const unsigned64 c( (a&m2) + ((a>>2) & m2) );
  //unsigned32 n(unsigned32(c) + unsigned32(c>>32));
  //n = (n&0x0F0F0F0F) + ((n>> 4) & 0x0F0F0F0F);
  //n = (n&    0xFFFF) +  (n>>16);
  //n = (n&      0xFF) +  (n>> 8);
  //return n;

  unsigned n(0);
  unsigned64 x(bb);

  while (x)
  {
    ++n;
    x &= x-1;
  }
  return n;
#endif
}

//-----------------------------------------------------------------------------
//  Vuota:
//-----------------------------------------------------------------------------
inline
bool
bitboard::vuota() const
{
  return !bb;
}

//-----------------------------------------------------------------------------
//  Operator[]:
//-----------------------------------------------------------------------------
inline
bool
bitboard::operator[](posizione p) const
{
  return bool(bb & maschera[p]);
}

//-----------------------------------------------------------------------------
//  Firstone:
//-----------------------------------------------------------------------------
inline
unsigned
bitboard::firstone() const
{
  assert(bb);

#if defined(ASSEMBLER) && defined(__GNUC__) && defined(M_INTEL) && \
    MICROPROCESSORE>=M_iPentiumII
  unsigned n;

  __asm__("testl %%eax, %%eax\n\
           jz              0f\n\
           bsfl  %%eax,    %0\n\
           jmp             1f\n\
          0:\n\
           bsfl  %%edx,    %0\n\
           addl    $32,    %0\n\
          1:"
          : "=r" (n)
          : "A" (bb)
         );

  return n;
#else
#include "tabelle/bsf.tab"

  if (bb & 65535ul)
    return posizione(bsf[unsigned32(bb & 65535ul);
  if ((bb>>16) & 65535ul)
    return posizione(bsf[unsigned32((bb>>16) & 65535ul)]+a3);
  if ((bb>>32) & 65535ul)
    return posizione(bsf[unsigned32((bb>>32) & 65535ul)]+a5);
  return posizione(bsf[unsigned32((bb>>48) & 65535ul)]+a7);
#endif
}

//-----------------------------------------------------------------------------
//  Clear:
//-----------------------------------------------------------------------------
inline
void
bitboard::clear()
{
  bb = unsigned64(0);
}

//-----------------------------------------------------------------------------
//  Aggiungi:
//-----------------------------------------------------------------------------
inline
void
bitboard::aggiungi(posizione p)
{
  assert(!(*this)[p]);
  bb |= maschera[p]; 
}

//-----------------------------------------------------------------------------
//  Elimina:
//-----------------------------------------------------------------------------
inline
void
bitboard::elimina(posizione p)
{ 
  assert((*this)[p]); 
  bb ^= maschera[p]; 
}

//-----------------------------------------------------------------------------
//  Sposta:
//-----------------------------------------------------------------------------
inline
void
bitboard::sposta(posizione da, posizione a)
{
  elimina(da);
  aggiungi(a);
}

//-----------------------------------------------------------------------------
//  Operator Unsigned64:
//-----------------------------------------------------------------------------
inline
bitboard::operator unsigned64() const
{
  return bb;
}

//-----------------------------------------------------------------------------
//  Operator<<:
//-----------------------------------------------------------------------------
inline
bitboard
bitboard::operator<<(unsigned n) const
{
  assert(n < 64);
  return bitboard(bb << n);
}

//-----------------------------------------------------------------------------
//  Operator>>:
//-----------------------------------------------------------------------------
inline
bitboard
bitboard::operator>>(unsigned n) const
{
  assert(n < 64);
  return bitboard(bb >> n);
}

//-----------------------------------------------------------------------------
//  Operator|:
//-----------------------------------------------------------------------------
inline
bitboard
bitboard::operator|(bitboard b) const
{
  return bitboard(bb | b.bb);
}

//-----------------------------------------------------------------------------
//  Operator|=:
//-----------------------------------------------------------------------------
inline
bitboard
bitboard::operator|=(bitboard b)
{
  bb |= b.bb;
  return *this;
}

//-----------------------------------------------------------------------------
//  Operator&:
//-----------------------------------------------------------------------------
inline
bitboard
bitboard::operator&(bitboard b) const
{
  return bitboard(bb & b.bb);
}

//-----------------------------------------------------------------------------
//  Operator&=:
//-----------------------------------------------------------------------------
inline
bitboard
bitboard::operator&=(bitboard b)
{
  bb &= b.bb;
  return *this;
}

//-----------------------------------------------------------------------------
//  Operator~:
//-----------------------------------------------------------------------------
inline
bitboard
bitboard::operator~() const
{
  return bitboard(~bb);
}

//-----------------------------------------------------------------------------
//  Operator==:
//-----------------------------------------------------------------------------
inline
bool
bitboard::operator==(bitboard b) const
{
  return bb == b.bb;
}

//-----------------------------------------------------------------------------
//  Operator!=:
//-----------------------------------------------------------------------------
inline
bool
bitboard::operator!=(bitboard b) const
{
  return bb != b.bb;
}

}  // Namespace Joker.

#endif  // BITBOARD_H
