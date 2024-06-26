/*
 *--------------------------------------------------------------------------
 * A simple frequency model.
 *
 * This keeps a list of symbols and their frequencies, approximately
 * sorted by symbol frequency. We allow for a single symbol to periodically
 * move up the list when emitted, effectively doing a single step of
 * bubble sort periodically. This means it's largely the same complexity
 * irrespective of alphabet size.
 * It's more efficient on strongly biased distributions than random data.
 *
 * There is no escape symbol, so the model is tailored to relatively
 * stationary samples (although we do have occasional normalisation to
 * avoid frequency counters getting too high).
 *--------------------------------------------------------------------------
 */

#ifndef SIMPLE_MODEL_H
#define SIMPLE_MODEL_H

#include "clr.h"
#include <assert.h>
#include <stdio.h>
#include <cmath>
#include <array>

// Shrinking this to 1<<10 gives 2-3% smaller qualities, but 50% longer
#define MAX_FREQ ((1 << 16) - 32)

template <int NSYM>
struct SIMPLE_MODEL {
    enum {
        STEP = 8
    };

    SIMPLE_MODEL();

    SIMPLE_MODEL(const std::array<uint16_t,NSYM> & histogram);

    inline void reset();
    inline void reset(const std::array<uint16_t, NSYM> & histogram);
    
    inline void encodeSymbol(RangeCoder *rc, uint16_t sym, bool update, bool order);
    inline void encodeSymbolBinary(RangeCoder *rc, uint8_t sym);
    inline size_t encodeSymbolRegular(RangeCoder *rc, uint16_t sym);
    inline void encodeSymbolOrder(RangeCoder *rc, uint16_t sym);
    inline void encodeSymbolNoUpdate(RangeCoder *rc, uint16_t sym);

    inline void updateModelAccFrecs(bool decode);

    static int compare(const void *a, const void *b);

    inline void mix_array(void **models, uc len);

    inline uint16_t decodeSymbol(RangeCoder *rc, bool update, bool order);
    inline uint8_t decodeSymbolBinary(RangeCoder *rc);
    inline uint16_t decodeSymbolRegular(RangeCoder *rc);
    inline uint16_t decodeSymbolOrder(RangeCoder *rc);
    inline uint16_t decodeSymbolNoUpdate(RangeCoder *rc);

    inline uint16_t normalize_histogram(std::array<uint16_t, NSYM> & h, uint16_t max_freq);

    //protected:
    void normalize();

    void print_model();

    uint32_t TotFreq;  // Total frequency
    uint32_t BubCnt;   // Periodic counter for bubble sort step

    // Array of Symbols approximately sorted by Freq.
    struct SymFreqs {
        uint16_t Symbol;
        uint16_t Freq;
        uint32_t AccFreq;
    } sentinel, F[NSYM + 1];
};

template <int NSYM>
SIMPLE_MODEL<NSYM>::SIMPLE_MODEL() {
    for (int i = 0; i < NSYM; i++) {
        F[i].Symbol = i;
        F[i].Freq = 1;
    }

    TotFreq = NSYM;
    sentinel.Symbol = 0;
    sentinel.Freq = MAX_FREQ;  // Always first; simplifies sorting.
    BubCnt = 0;

    F[NSYM].Freq = 0;  // terminates normalize() loop. See below.
}
// TODO: Optimize this code
template <int NSYM>
inline uint16_t SIMPLE_MODEL<NSYM>::normalize_histogram(std::array<uint16_t, NSYM> & h, uint16_t max_freq)
{
    if (MAX_FREQ >= max_freq)
        return max_freq;
    uint16_t i = 0;
    while (MAX_FREQ < max_freq)
    {
        i++;
        max_freq >>= 1;
    }
    // TODO: This code is vectorizable
    uint16_t* ptr = h.data();
    uint16_t* end_ptr = ptr + NSYM;
    for (;ptr != end_ptr; ptr++)
    {
        *(ptr) >>= i;
    }
    return max_freq;
}

//FIXME: If the max. frequency of the histogram is higer than the one of the coder, UB arises
// Precondition: normalize_histogram called before
template <int NSYM>
SIMPLE_MODEL<NSYM>::SIMPLE_MODEL(const std::array<uint16_t, NSYM> & histogram)
{
    assert(histogram.size() == NSYM);
    TotFreq = 0;
    for (uint16_t i = 0; i < NSYM; i++)
    {
        F[i].Symbol = i;
        F[i].Freq = histogram[i];
        TotFreq += F[i].Freq;
    }

    sentinel.Symbol = 0;
    sentinel.Freq = MAX_FREQ;
    BubCnt = 0;

    F[NSYM].Freq = 0;
}

template <int NSYM>
inline void SIMPLE_MODEL<NSYM>::reset(const std::array<uint16_t, NSYM> & histogram) {
    assert(histogram.size() == NSYM);
    TotFreq = 0;
    for (uint16_t i = 0; i < NSYM; i++)
    {
        F[i].Symbol = i;
        F[i].Freq = histogram[i];
        TotFreq += F[i].Freq;
    }

    sentinel.Symbol = 0;
    sentinel.Freq = MAX_FREQ;
    BubCnt = 0;

    F[NSYM].Freq = 0;
}

template <int NSYM>
void SIMPLE_MODEL<NSYM>::reset() {
    for (int i = 0; i < NSYM; i++) {
        F[i].Symbol = i;
        F[i].Freq = 1;
    }
    TotFreq = NSYM;
    sentinel.Symbol = 0;
    sentinel.Freq = MAX_FREQ;  // Always first; simplifies sorting.
    BubCnt = 0;

    F[NSYM].Freq = 0;  // terminates normalize() loop. See below.
}

template <int NSYM>
void SIMPLE_MODEL<NSYM>::normalize() {
    /* Faster than F[i].Freq for 0 <= i < NSYM */
    TotFreq = 0;
    for (SymFreqs *s = F; s->Freq; s++) {
        s->Freq -= s->Freq >> 1;
        TotFreq += s->Freq;
    }
}

template <int NSYM>
void SIMPLE_MODEL<NSYM>::print_model() {
    printf("NSYM: %d \n", NSYM);
    for (int i = 0; i < NSYM; i++) {
        printf(" (%d, %d, %d),", F[i].Symbol, F[i].Freq, F[i].AccFreq);
    }
    printf("\n");
}

template <int NSYM>
inline void SIMPLE_MODEL<NSYM>::encodeSymbolBinary(RangeCoder *rc, uint8_t sym) {
    assert(sym < NSYM);
    rc->EncodeBinary(sym);
}


template <int NSYM>
inline void SIMPLE_MODEL<NSYM>::encodeSymbol(RangeCoder *rc, uint16_t sym, bool update, bool order) {
    if (update)
        if (order)
            encodeSymbolOrder(rc, sym);
        else
            encodeSymbolRegular(rc, sym);
    else
        encodeSymbolNoUpdate(rc, sym);
}

template <int NSYM>
inline size_t SIMPLE_MODEL<NSYM>::encodeSymbolRegular(RangeCoder *rc, uint16_t sym) {
    assert(sym < NSYM);
    SymFreqs *s = F;
    uint32_t AccFreq = 0;

    while (s->Symbol != sym)
        AccFreq += s++->Freq;

    size_t bytes_written = rc->Encode(AccFreq, s->Freq, TotFreq);
    s->Freq += STEP;
    TotFreq += STEP;

    if (TotFreq > MAX_FREQ)
        normalize();
    return bytes_written;
}

template <int NSYM>
inline void SIMPLE_MODEL<NSYM>::encodeSymbolOrder(RangeCoder *rc, uint16_t sym) {
    assert(sym < NSYM);
    SymFreqs *s = F;
    uint32_t AccFreq = 0;

    while (s->Symbol != sym)
        AccFreq += s++->Freq;

    rc->Encode(AccFreq, s->Freq, TotFreq);
    s->Freq += STEP;
    TotFreq += STEP;

    if (TotFreq > MAX_FREQ)
        normalize();

    /* Keep approx sorted */
    if (((++BubCnt & 15) == 0) && s[0].Freq > s[-1].Freq) {
        SymFreqs t = s[0];
        s[0] = s[-1];
        s[-1] = t;
    }
}

template <int NSYM>
inline void SIMPLE_MODEL<NSYM>::encodeSymbolNoUpdate(RangeCoder *rc, uint16_t sym) {
#ifdef __ORDER_SYMBOLS__
    SymFreqs *s = F;

    while (s->Symbol != sym)
        s++;

    rc->Encode(s->AccFreq, s->Freq, TotFreq);
#else
    assert(sym < NSYM);
    rc->Encode(F[sym].AccFreq, F[sym].Freq, TotFreq);
#endif
}

template <int NSYM>
inline void SIMPLE_MODEL<NSYM>::mix_array(void **models, uc len) {
    TotFreq = 0;
    for (int i = 0; i < NSYM; i++) {
        uint fmix = 0;
        for (uc c = 0; c < len; c++) {
            fmix += ((SIMPLE_MODEL<NSYM> *)models[c])->F[i].Freq;
        }
        fmix = round((double)fmix / len);
        F[i].Freq = fmix;
        TotFreq += fmix;
    }
}

template <int NSYM>
int SIMPLE_MODEL<NSYM>::compare(const void *a, const void *b) {
    SymFreqs f_a = *((SymFreqs *)a);
    SymFreqs f_b = *((SymFreqs *)b);

    if (f_a.Freq == f_b.Freq)
        return 0;
    else if (f_a.Freq > f_b.Freq)
        return -1;
    else
        return 1;
}

template <int NSYM>
inline void SIMPLE_MODEL<NSYM>::updateModelAccFrecs(bool decode) {
#ifdef __ORDER_SYMBOLS__
    qsort(F, NSYM, sizeof(SymFreqs), compare);
#endif
    if (decode) {
        F[0].AccFreq = F[0].Freq;
        for (int i = 1; i < NSYM; i++) {
            F[i].AccFreq = F[i - 1].AccFreq + F[i].Freq;
        }
    } else {
        F[0].AccFreq = 0;
        for (int i = 1; i < NSYM; i++) {
            F[i].AccFreq = F[i - 1].AccFreq + F[i - 1].Freq;
        }
    }
}

template <int NSYM>
inline uint16_t SIMPLE_MODEL<NSYM>::decodeSymbol(RangeCoder *rc, bool update, bool order) {
    if (update)
        if (order)
            return decodeSymbolOrder(rc);
        else
            return decodeSymbolRegular(rc);
    else
        return decodeSymbolNoUpdate(rc);
}

template <int NSYM>
inline uint8_t SIMPLE_MODEL<NSYM>::decodeSymbolBinary(RangeCoder *rc) {
    int8_t ret = rc->DecodeBinary();
    return ret;
}

template <int NSYM>
inline uint16_t SIMPLE_MODEL<NSYM>::decodeSymbolRegular(RangeCoder *rc) {
    SymFreqs *s = F;
    uint32_t freq = rc->GetFreq(TotFreq);
    uint32_t AccFreq;

    for (AccFreq = 0; (AccFreq += s->Freq) <= freq; s++)
        ;
    AccFreq -= s->Freq;

    rc->Decode(AccFreq, s->Freq);
    s->Freq += STEP;
    TotFreq += STEP;

    if (TotFreq > MAX_FREQ)
        normalize();

    assert(s->Symbol < NSYM);
    return s->Symbol;
}

template <int NSYM>
inline uint16_t SIMPLE_MODEL<NSYM>::decodeSymbolOrder(RangeCoder *rc) {
    SymFreqs *s = F;
    uint32_t freq = rc->GetFreq(TotFreq);
    uint32_t AccFreq;

    for (AccFreq = 0; (AccFreq += s->Freq) <= freq; s++)
        ;
    AccFreq -= s->Freq;

    rc->Decode(AccFreq, s->Freq);
    s->Freq += STEP;
    TotFreq += STEP;

    if (TotFreq > MAX_FREQ)
        normalize();

    /* Keep approx sorted */
    if (((++BubCnt & 15) == 0) && s[0].Freq > s[-1].Freq) {
        SymFreqs t = s[0];
        s[0] = s[-1];
        s[-1] = t;
        return t.Symbol;
    }
    assert(s->Symbol < NSYM);
    return s->Symbol;
}

template <int NSYM>
inline uint16_t SIMPLE_MODEL<NSYM>::decodeSymbolNoUpdate(RangeCoder *rc) {
    uint32_t freq = rc->GetFreq(TotFreq);
    SymFreqs *s = F;

    for (; s->AccFreq <= freq; s++)
        ;

    rc->Decode(s->AccFreq - s->Freq, s->Freq);

    assert(s->Symbol < NSYM);
    return s->Symbol;
}

#endif