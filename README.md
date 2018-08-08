# Joker

**This software is not developed anymore**

Joker is a [Chess Engine Communication Protocol][1] compliant chess engine developed from 1998 until 2001, written in C++ with some conditional x86 inline assembly concerning *population count* and *bitscan of bitboards*.

During its lifetime, around Version 0.6.7 in 1999, Joker used MTD(f) along with ETC, but finally stuck with PVS and aspiration. It initially used the 0x88 board representation but evolved to bitboards. Quiet move generation of sliding pieces is still done with a nested loop over directions with pre-increments of origin until target squares are empty.

It's offered without support, as "historical" artifact, under the terms of the MIT license.

Further details at http://www.eosdev.it/joker/

---

If you like computer chess take a look at http://computer-chess.org and https://www.chessprogramming.org, you'll love them!


[1]: https://www.gnu.org/software/xboard/engine-intf.html
