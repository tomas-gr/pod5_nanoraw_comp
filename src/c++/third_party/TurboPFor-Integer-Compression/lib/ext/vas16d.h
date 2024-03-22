// Optimized Simple16 decode implementation
#define S16DEC(__in, _p, __action) { register unsigned _rw = *(unsigned *)__in; __in+=4/sizeof(__in[0]); \
  switch(_rw>>28)  {\
    case 0:\
      _p[ 0] = (_rw    ) & 1;\
      _p[ 1] = (_rw>> 1) & 1;\
      _p[ 2] = (_rw>> 2) & 1;\
      _p[ 3] = (_rw>> 3) & 1;\
      _p[ 4] = (_rw>> 4) & 1;\
      _p[ 5] = (_rw>> 5) & 1;\
      _p[ 6] = (_rw>> 6) & 1;\
      _p[ 7] = (_rw>> 7) & 1;\
      _p[ 8] = (_rw>> 8) & 1;\
      _p[ 9] = (_rw>> 9) & 1;\
      _p[10] = (_rw>>10) & 1;\
      _p[11] = (_rw>>11) & 1;\
      _p[12] = (_rw>>12) & 1;\
      _p[13] = (_rw>>13) & 1;\
      _p[14] = (_rw>>14) & 1;\
      _p[15] = (_rw>>15) & 1;\
      _p[16] = (_rw>>16) & 1;\
      _p[17] = (_rw>>17) & 1;\
      _p[18] = (_rw>>18) & 1;\
      _p[19] = (_rw>>19) & 1;\
      _p[20] = (_rw>>20) & 1;\
      _p[21] = (_rw>>21) & 1;\
      _p[22] = (_rw>>22) & 1;\
      _p[23] = (_rw>>23) & 1;\
      _p[24] = (_rw>>24) & 1;\
      _p[25] = (_rw>>25) & 1;\
      _p[26] = (_rw>>26) & 1;\
      _p[27] = (_rw>>27) & 1;  _p += 28;\
      break;\
    case 1: \
      _p[ 0] = (_rw    ) & 3;\
      _p[ 1] = (_rw>> 2) & 3;\
      _p[ 2] = (_rw>> 4) & 3;\
      _p[ 3] = (_rw>> 6) & 3;\
      _p[ 4] = (_rw>> 8) & 3;\
      _p[ 5] = (_rw>>10) & 3;\
      _p[ 6] = (_rw>>12) & 3;\
      _p[ 7] = (_rw>>14) & 1;\
      _p[ 8] = (_rw>>15) & 1;\
      _p[ 9] = (_rw>>16) & 1;\
      _p[10] = (_rw>>17) & 1;\
      _p[11] = (_rw>>18) & 1;\
      _p[12] = (_rw>>19) & 1;\
      _p[13] = (_rw>>20) & 1;\
      _p[14] = (_rw>>21) & 1;\
      _p[15] = (_rw>>22) & 1;\
      _p[16] = (_rw>>23) & 1;\
      _p[17] = (_rw>>24) & 1;\
      _p[18] = (_rw>>25) & 1;\
      _p[19] = (_rw>>26) & 1;\
      _p[20] = (_rw>>27) & 1;  _p += 21; \
      break; \
    case 2: \
      _p[0] = (_rw) & 1;     \
      _p[1] = (_rw>>1) & 1;\
      _p[2] = (_rw>>2) & 1;\
      _p[3] = (_rw>>3) & 1;\
      _p[4] = (_rw>>4) & 1;\
      _p[5] = (_rw>>5) & 1;\
      _p[6] = (_rw>>6) & 1;\
      _p[7] = (_rw>>7) & 3;\
      _p[8] = (_rw>>9) & 3;\
      _p[9] = (_rw>>11) & 3;\
      _p[10] = (_rw>>13) & 3;\
      _p[11] = (_rw>>15) & 3;\
      _p[12] = (_rw>>17) & 3;\
      _p[13] = (_rw>>19) & 3;\
      _p[14] = (_rw>>21) & 1;\
      _p[15] = (_rw>>22) & 1;\
      _p[16] = (_rw>>23) & 1;\
      _p[17] = (_rw>>24) & 1;\
      _p[18] = (_rw>>25) & 1;\
      _p[19] = (_rw>>26) & 1;\
      _p[20] = (_rw>>27) & 1;  _p += 21;\
      break; \
    case 3: \
      _p[0] = (_rw) & 1;     \
      _p[1] = (_rw>>1) & 1;\
      _p[2] = (_rw>>2) & 1;\
      _p[3] = (_rw>>3) & 1;\
      _p[4] = (_rw>>4) & 1;\
      _p[5] = (_rw>>5) & 1;\
      _p[6] = (_rw>>6) & 1;\
      _p[7] = (_rw>>7) & 1;\
      _p[8] = (_rw>>8) & 1;\
      _p[9] = (_rw>>9) & 1;\
      _p[10] = (_rw>>10) & 1;\
      _p[11] = (_rw>>11) & 1;\
      _p[12] = (_rw>>12) & 1;\
      _p[13] = (_rw>>13) & 1;\
      _p[14] = (_rw>>14) & 3;\
      _p[15] = (_rw>>16) & 3;\
      _p[16] = (_rw>>18) & 3;\
      _p[17] = (_rw>>20) & 3;\
      _p[18] = (_rw>>22) & 3;\
      _p[19] = (_rw>>24) & 3;\
      _p[20] = (_rw>>26) & 3;  _p += 21;\
      break; \
    case 4: \
      _p[ 0] = (_rw    ) & 3;\
      _p[ 1] = (_rw>> 2) & 3;\
      _p[ 2] = (_rw>> 4) & 3;\
      _p[ 3] = (_rw>> 6) & 3;\
      _p[ 4] = (_rw>> 8) & 3;\
      _p[ 5] = (_rw>>10) & 3;\
      _p[ 6] = (_rw>>12) & 3;\
      _p[ 7] = (_rw>>14) & 3;\
      _p[ 8] = (_rw>>16) & 3;\
      _p[ 9] = (_rw>>18) & 3;\
      _p[10] = (_rw>>20) & 3;\
      _p[11] = (_rw>>22) & 3;\
      _p[12] = (_rw>>24) & 3;\
      _p[13] = (_rw>>26) & 3;  _p += 14;\
      break; \
    case 5: \
      _p[0] = (_rw) & 15;     \
      _p[1] = (_rw>>4) & 7;\
      _p[2] = (_rw>>7) & 7;\
      _p[3] = (_rw>>10) & 7;\
      _p[4] = (_rw>>13) & 7;\
      _p[5] = (_rw>>16) & 7;\
      _p[6] = (_rw>>19) & 7;\
      _p[7] = (_rw>>22) & 7;\
      _p[8] = (_rw>>25) & 7;  _p += 9;\
      break; \
    case 6: \
      _p[0] = (_rw) & 7;     \
      _p[1] = (_rw>>3) & 15;\
      _p[2] = (_rw>>7) & 15;\
      _p[3] = (_rw>>11) & 15;\
      _p[4] = (_rw>>15) & 15;\
      _p[5] = (_rw>>19) & 7;\
      _p[6] = (_rw>>22) & 7;\
      _p[7] = (_rw>>25) & 7;  _p += 8;\
      break; \
    case 7: \
      _p[0] = (_rw) & 15;     \
      _p[1] = (_rw>>4) & 15;\
      _p[2] = (_rw>>8) & 15;\
      _p[3] = (_rw>>12) & 15;\
      _p[4] = (_rw>>16) & 15;\
      _p[5] = (_rw>>20) & 15;\
      _p[6] = (_rw>>24) & 15;  _p += 7;\
      break; \
    case 8: \
      _p[0] = (_rw    ) & 31;\
      _p[1] = (_rw>> 5) & 31;\
      _p[2] = (_rw>>10) & 31;\
      _p[3] = (_rw>>15) & 31;\
      _p[4] = (_rw>>20) & 15;\
      _p[5] = (_rw>>24) & 15;  _p += 6;\
      break; \
    case 9: \
      _p[0] = (_rw) & 15;     \
      _p[1] = (_rw>>4) & 15;\
      _p[2] = (_rw>>8) & 31;\
      _p[3] = (_rw>>13) & 31;\
      _p[4] = (_rw>>18) & 31;\
      _p[5] = (_rw>>23) & 31;  _p += 6;\
      break; \
    case 10: \
      _p[0] = (_rw) & 63;     \
      _p[1] = (_rw>>6) & 63;\
      _p[2] = (_rw>>12) & 63;\
      _p[3] = (_rw>>18) & 31;\
      _p[4] = (_rw>>23) & 31;  _p += 5;\
      break; \
    case 11: \
      _p[0] = (_rw) & 31;     \
      _p[1] = (_rw>>5) & 31;\
      _p[2] = (_rw>>10) & 63;\
      _p[3] = (_rw>>16) & 63;\
      _p[4] = (_rw>>22) & 63;  _p += 5;\
      break; \
    case 12: \
      _p[0] = (_rw) & 127;     \
      _p[1] = (_rw>>7) & 127;\
      _p[2] = (_rw>>14) & 127;\
      _p[3] = (_rw>>21) & 127;  _p += 4;\
      break; \
    case 13: \
      _p[0] = (_rw) & 1023;     \
      _p[1] = (_rw>>10) & 511;\
      _p[2] = (_rw>>19) & 511;  _p += 3;\
      break; \
    case 14: \
      _p[0] = (_rw) & 16383;     \
      _p[1] = (_rw>>14) & 16383;  _p += 2;\
      break; \
    case 15: \
      _p[0] = (_rw) & ((1<<28)-1);     _p++; \
      break; \
  } \
}
static inline unsigned char *vs16dec(unsigned *__restrict in, int n, unsigned *__restrict out) { unsigned *out_ = out+n; while(out < out_) S16DEC(in, out, ;); return (unsigned char *)in; }

#if 0
#define BREAK _rw = *_in++; goto *_lab[__out<_oute?((_rw)>>28):16]

#define s16dec(__in, __n, __pout) ({\
  __label__ _lab0,_lab1,_lab2,_lab3,_lab4,_lab5,_lab6,_lab7,_lab8,_lab9,_lab10,_lab11,_lab12,_lab13,_lab14,_lab15,_labend;\
  static void *_lab[] = { &&_lab0, &&_lab1, &&_lab2, &&_lab3, &&_lab4, &&_lab5, &&_lab6, &&_lab7, &&_lab8, &&_lab9, &&_lab10, &&_lab11, &&_lab12, &&_lab13, &&_lab14, &&_lab15, &&_labend };\
  unsigned *_in = __in; typeof(__pout[0]) *__out = __pout, *_oute = __out+(__n); register unsigned _rw = *_in++; goto *_lab[(_rw)>>28];\
    _lab0:\
      __out[0] = (_rw) & 1;     \
      __out[1] = (_rw>>1) & 1;  \
      __out[2] = (_rw>>2) & 1;  \
      __out[3] = (_rw>>3) & 1;  \
      __out[4] = (_rw>>4) & 1;  \
      __out[5] = (_rw>>5) & 1;  \
      __out[6] = (_rw>>6) & 1;  \
      __out[7] = (_rw>>7) & 1;  \
      __out[8] = (_rw>>8) & 1;  \
      __out[9] = (_rw>>9) & 1;  \
      __out[10] = (_rw>>10) & 1;  \
      __out[11] = (_rw>>11) & 1;  \
      __out[12] = (_rw>>12) & 1;  \
      __out[13] = (_rw>>13) & 1;  \
      __out[14] = (_rw>>14) & 1;  \
      __out[15] = (_rw>>15) & 1;  \
      __out[16] = (_rw>>16) & 1;  \
      __out[17] = (_rw>>17) & 1;  \
      __out[18] = (_rw>>18) & 1;  \
      __out[19] = (_rw>>19) & 1;  \
      __out[20] = (_rw>>20) & 1;  \
      __out[21] = (_rw>>21) & 1;  \
      __out[22] = (_rw>>22) & 1;  \
      __out[23] = (_rw>>23) & 1;  \
      __out[24] = (_rw>>24) & 1;  \
      __out[25] = (_rw>>25) & 1;  \
      __out[26] = (_rw>>26) & 1;  \
      __out[27] = (_rw>>27) & 1;  __out += 28;\
      BREAK; \
    _lab1: \
      __out[0] = (_rw) & 3;     \
      __out[1] = (_rw>>2) & 3;  \
      __out[2] = (_rw>>4) & 3;  \
      __out[3] = (_rw>>6) & 3;  \
      __out[4] = (_rw>>8) & 3;  \
      __out[5] = (_rw>>10) & 3; \
      __out[6] = (_rw>>12) & 3;  \
      __out[7] = (_rw>>14) & 1;  \
      __out[8] = (_rw>>15) & 1;  \
      __out[9] = (_rw>>16) & 1;  \
      __out[10] = (_rw>>17) & 1; \
      __out[11] = (_rw>>18) & 1;  \
      __out[12] = (_rw>>19) & 1;  \
      __out[13] = (_rw>>20) & 1;  \
      __out[14] = (_rw>>21) & 1;  \
      __out[15] = (_rw>>22) & 1;  \
      __out[16] = (_rw>>23) & 1;  \
      __out[17] = (_rw>>24) & 1;  \
      __out[18] = (_rw>>25) & 1;  \
      __out[19] = (_rw>>26) & 1;  \
      __out[20] = (_rw>>27) & 1;  __out += 21; \
      BREAK; \
    _lab2: \
      __out[0] = (_rw) & 1;     \
      __out[1] = (_rw>>1) & 1;  \
      __out[2] = (_rw>>2) & 1;  \
      __out[3] = (_rw>>3) & 1;  \
      __out[4] = (_rw>>4) & 1;  \
      __out[5] = (_rw>>5) & 1;  \
      __out[6] = (_rw>>6) & 1;  \
      __out[7] = (_rw>>7) & 3;  \
      __out[8] = (_rw>>9) & 3;  \
      __out[9] = (_rw>>11) & 3;  \
      __out[10] = (_rw>>13) & 3;  \
      __out[11] = (_rw>>15) & 3;  \
      __out[12] = (_rw>>17) & 3;  \
      __out[13] = (_rw>>19) & 3;  \
      __out[14] = (_rw>>21) & 1;  \
      __out[15] = (_rw>>22) & 1;  \
      __out[16] = (_rw>>23) & 1;  \
      __out[17] = (_rw>>24) & 1;  \
      __out[18] = (_rw>>25) & 1;  \
      __out[19] = (_rw>>26) & 1;  \
      __out[20] = (_rw>>27) & 1;  __out += 21;\
      BREAK; \
    _lab3: \
      __out[0] = (_rw) & 1;     \
      __out[1] = (_rw>>1) & 1;  \
      __out[2] = (_rw>>2) & 1;  \
      __out[3] = (_rw>>3) & 1;  \
      __out[4] = (_rw>>4) & 1;  \
      __out[5] = (_rw>>5) & 1;  \
      __out[6] = (_rw>>6) & 1;  \
      __out[7] = (_rw>>7) & 1;  \
      __out[8] = (_rw>>8) & 1;  \
      __out[9] = (_rw>>9) & 1;  \
      __out[10] = (_rw>>10) & 1;  \
      __out[11] = (_rw>>11) & 1;  \
      __out[12] = (_rw>>12) & 1;  \
      __out[13] = (_rw>>13) & 1;  \
      __out[14] = (_rw>>14) & 3;  \
      __out[15] = (_rw>>16) & 3;  \
      __out[16] = (_rw>>18) & 3;  \
      __out[17] = (_rw>>20) & 3;  \
      __out[18] = (_rw>>22) & 3;  \
      __out[19] = (_rw>>24) & 3;  \
      __out[20] = (_rw>>26) & 3;  __out += 21;\
      BREAK; \
    _lab4: \
      __out[0] = (_rw) & 3;     \
      __out[1] = (_rw>>2) & 3;  \
      __out[2] = (_rw>>4) & 3;  \
      __out[3] = (_rw>>6) & 3;  \
      __out[4] = (_rw>>8) & 3;  \
      __out[5] = (_rw>>10) & 3;  \
      __out[6] = (_rw>>12) & 3;  \
      __out[7] = (_rw>>14) & 3;  \
      __out[8] = (_rw>>16) & 3;  \
      __out[9] = (_rw>>18) & 3;  \
      __out[10] = (_rw>>20) & 3;  \
      __out[11] = (_rw>>22) & 3;  \
      __out[12] = (_rw>>24) & 3;  \
      __out[13] = (_rw>>26) & 3;  __out += 14;\
      BREAK; \
    _lab5: \
      __out[0] = (_rw) & 15;     \
      __out[1] = (_rw>>4) & 7;  \
      __out[2] = (_rw>>7) & 7;  \
      __out[3] = (_rw>>10) & 7;  \
      __out[4] = (_rw>>13) & 7;  \
      __out[5] = (_rw>>16) & 7;  \
      __out[6] = (_rw>>19) & 7;  \
      __out[7] = (_rw>>22) & 7;  \
      __out[8] = (_rw>>25) & 7;  __out += 9;\
      BREAK; \
    _lab6: \
      __out[0] = (_rw) & 7;     \
      __out[1] = (_rw>>3) & 15;  \
      __out[2] = (_rw>>7) & 15;  \
      __out[3] = (_rw>>11) & 15;  \
      __out[4] = (_rw>>15) & 15;  \
      __out[5] = (_rw>>19) & 7;  \
      __out[6] = (_rw>>22) & 7;  \
      __out[7] = (_rw>>25) & 7;  __out += 8;\
      BREAK; \
    _lab7: \
      __out[0] = (_rw) & 15;     \
      __out[1] = (_rw>>4) & 15;  \
      __out[2] = (_rw>>8) & 15;  \
      __out[3] = (_rw>>12) & 15;  \
      __out[4] = (_rw>>16) & 15;  \
      __out[5] = (_rw>>20) & 15;  \
      __out[6] = (_rw>>24) & 15;  __out += 7;\
      BREAK; \
    _lab8: \
      __out[0] = (_rw) & 31;     \
      __out[1] = (_rw>>5) & 31;  \
      __out[2] = (_rw>>10) & 31;  \
      __out[3] = (_rw>>15) & 31;  \
      __out[4] = (_rw>>20) & 15;  \
      __out[5] = (_rw>>24) & 15;  __out += 6;\
      BREAK; \
    _lab9: \
      __out[0] = (_rw) & 15;     \
      __out[1] = (_rw>>4) & 15;  \
      __out[2] = (_rw>>8) & 31;  \
      __out[3] = (_rw>>13) & 31;  \
      __out[4] = (_rw>>18) & 31;  \
      __out[5] = (_rw>>23) & 31;  __out += 6;\
      BREAK; \
    _lab10: \
      __out[0] = (_rw) & 63;     \
      __out[1] = (_rw>>6) & 63;  \
      __out[2] = (_rw>>12) & 63;  \
      __out[3] = (_rw>>18) & 31;  \
      __out[4] = (_rw>>23) & 31;  __out += 5;\
      BREAK; \
    _lab11: \
      __out[0] = (_rw) & 31;     \
      __out[1] = (_rw>>5) & 31;  \
      __out[2] = (_rw>>10) & 63;  \
      __out[3] = (_rw>>16) & 63;  \
      __out[4] = (_rw>>22) & 63;  __out += 5;\
      BREAK; \
    _lab12: \
      __out[0] = (_rw) & 127;     \
      __out[1] = (_rw>>7) & 127;  \
      __out[2] = (_rw>>14) & 127;  \
      __out[3] = (_rw>>21) & 127;  __out += 4;\
      BREAK; \
    _lab13: \
      __out[0] = (_rw) & 1023;     \
      __out[1] = (_rw>>10) & 511;  \
      __out[2] = (_rw>>19) & 511;  __out += 3;\
      BREAK; \
    _lab14:\
      __out[0] = (_rw) & 16383;     \
      __out[1] = (_rw>>14) & 16383;  __out += 2;\
      BREAK; \
    _lab15:\
      __out[0] = (_rw) & ((1<<28)-1);     __out++; \
      BREAK;\
    _labend:;(_in-1);\
})
#endif
