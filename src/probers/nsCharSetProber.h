/*  -*- C++ -*-
    SPDX-FileCopyrightText: 1998 Netscape Communications Corporation <developer@mozilla.org>

    SPDX-License-Identifier: MIT
*/

#ifndef nsCharSetProber_h__
#define nsCharSetProber_h__

#include "kencodingprober.h"

namespace kencodingprober
{
typedef enum {
    eDetecting = 0, // We are still detecting, no sure answer yet, but caller can ask for confidence.
    eFoundIt = 1, // That's a positive answer
    eNotMe = 2, // Negative answer
} nsProbingState;

#define SHORTCUT_THRESHOLD (float)0.95

class KCODECS_NO_EXPORT nsCharSetProber
{
public:
    virtual ~nsCharSetProber()
    {
    }
    virtual const char *GetCharSetName() = 0;
    virtual nsProbingState HandleData(const char *aBuf, unsigned int aLen) = 0;
    virtual nsProbingState GetState(void) = 0;
    virtual void Reset(void) = 0;
    virtual float GetConfidence(void) = 0;

#ifdef DEBUG_PROBE
    void DumpStatus() override
    {
    }
#endif

    // Helper functions used in the Latin1 and Group probers.
    // both functions Allocate a new buffer for newBuf. This buffer should be
    // freed by the caller using PR_FREEIF.
    // Both functions return false in case of memory allocation failure.
    static bool FilterWithoutEnglishLetters(const char *aBuf, unsigned int aLen, char **newBuf, unsigned int &newLen);
    static bool FilterWithEnglishLetters(const char *aBuf, unsigned int aLen, char **newBuf, unsigned int &newLen);
};
}
#endif /* nsCharSetProber_h__ */
