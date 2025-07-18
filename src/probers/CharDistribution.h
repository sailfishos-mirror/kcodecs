/*  -*- C++ -*-
    SPDX-FileCopyrightText: 1998 Netscape Communications Corporation <developer@mozilla.org>

    SPDX-License-Identifier: MIT
*/

#ifndef CharDistribution_h__
#define CharDistribution_h__

#include "kcodecs_export.h"

#include <qcompilerdetection.h>

#define ENOUGH_DATA_THRESHOLD 256

namespace kencodingprober
{
class KCODECS_NO_EXPORT CharDistributionAnalysis
{
public:
    CharDistributionAnalysis()
    {
        Reset();
    }
    virtual ~CharDistributionAnalysis()
    {
    }

    // feed a block of data and do distribution analysis
    void HandleData(const char * /* aBuf */, unsigned int /* aLen */)
    {
    }

    // Feed a character with known length
    void HandleOneChar(const char *aStr, unsigned int aCharLen)
    {
        int order;

        // we only care about 2-bytes character in our distribution analysis
        order = (aCharLen == 2) ? GetOrder(aStr) : -1;

        if (order >= 0) {
            mTotalChars++;
            // order is valid
            if ((unsigned int)order < mTableSize) {
                if (512 > mCharToFreqOrder[order]) {
                    mFreqChars++;
                }
            }
        }
    }

    // return confidence base on existing data
    float GetConfidence();

    // Reset analyser, clear any state
    void Reset(void)
    {
        mDone = false;
        mTotalChars = 0;
        mFreqChars = 0;
    }

    // It is not necessary to receive all data to draw conclusion. For charset detection,
    // certain amount of data is enough
    bool GotEnoughData()
    {
        return mTotalChars > ENOUGH_DATA_THRESHOLD;
    }

protected:
    // we do not handle character base on its original encoding string, but
    // convert this encoding string to a number, here called order.
    // This allows multiple encodings of a language to share one frequency table
    virtual int GetOrder(const char * /* str */)
    {
        return -1;
    }

    // If this flag is set to true, detection is done and conclusion has been made
    bool mDone;

    // The number of characters whose frequency order is less than 512
    unsigned int mFreqChars;

    // Total character encountered.
    unsigned int mTotalChars;

    // Mapping table to get frequency order from char order (get from GetOrder())
    const short *mCharToFreqOrder;

    // Size of above table
    unsigned int mTableSize;

    // This is a constant value varies from language to language, it is used in
    // calculating confidence. See my paper for further detail.
    float mTypicalDistributionRatio;
};

class KCODECS_NO_EXPORT EUCKRDistributionAnalysis : public CharDistributionAnalysis
{
public:
    EUCKRDistributionAnalysis();

protected:
    // for euc-KR encoding, we are interested
    //  first  byte range: 0xb0 -- 0xfe
    //  second byte range: 0xa1 -- 0xfe
    // no validation needed here. State machine has done that
    int GetOrder(const char *str) override
    {
        if ((unsigned char)*str >= (unsigned char)0xb0) {
            return 94 * ((unsigned char)str[0] - (unsigned char)0xb0) + (unsigned char)str[1] - (unsigned char)0xa1;
        } else {
            return -1;
        }
    }
};

class KCODECS_NO_EXPORT GB2312DistributionAnalysis : public CharDistributionAnalysis
{
public:
    GB2312DistributionAnalysis();

protected:
    // for GB2312 encoding, we are interested
    //  first  byte range: 0xb0 -- 0xfe
    //  second byte range: 0xa1 -- 0xfe
    // no validation needed here. State machine has done that
    int GetOrder(const char *str) override
    {
        if ((unsigned char)*str >= (unsigned char)0xb0 && (unsigned char)str[1] >= (unsigned char)0xa1) {
            return 94 * ((unsigned char)str[0] - (unsigned char)0xb0) + (unsigned char)str[1] - (unsigned char)0xa1;
        } else {
            return -1;
        }
    }
};

class KCODECS_NO_EXPORT Big5DistributionAnalysis : public CharDistributionAnalysis
{
public:
    Big5DistributionAnalysis();

protected:
    // for big5 encoding, we are interested
    //  first  byte range: 0xa4 -- 0xfe
    //  second byte range: 0x40 -- 0x7e , 0xa1 -- 0xfe
    // no validation needed here. State machine has done that
    int GetOrder(const char *str) override
    {
        if ((unsigned char)*str >= (unsigned char)0xa4)
            if ((unsigned char)str[1] >= (unsigned char)0xa1) {
                return 157 * ((unsigned char)str[0] - (unsigned char)0xa4) + (unsigned char)str[1] - (unsigned char)0xa1 + 63;
            } else {
                return 157 * ((unsigned char)str[0] - (unsigned char)0xa4) + (unsigned char)str[1] - (unsigned char)0x40;
            }
        else {
            return -1;
        }
    }
};

class KCODECS_NO_EXPORT SJISDistributionAnalysis : public CharDistributionAnalysis
{
public:
    SJISDistributionAnalysis();

protected:
    // for sjis encoding, we are interested
    //  first  byte range: 0x81 -- 0x9f , 0xe0 -- 0xfe
    //  second byte range: 0x40 -- 0x7e,  0x81 -- oxfe
    // no validation needed here. State machine has done that
    int GetOrder(const char *str) override
    {
        int order;
        if ((unsigned char)*str >= (unsigned char)0x81 && (unsigned char)*str <= (unsigned char)0x9f) {
            order = 188 * ((unsigned char)str[0] - (unsigned char)0x81);
        } else if ((unsigned char)*str >= (unsigned char)0xe0 && (unsigned char)*str <= (unsigned char)0xef) {
            order = 188 * ((unsigned char)str[0] - (unsigned char)0xe0 + 31);
        } else {
            return -1;
        }
        order += (unsigned char)*(str + 1) - 0x40;
        if ((unsigned char)str[1] > (unsigned char)0x7f) {
            order--;
        }
        return order;
    }
};

class KCODECS_NO_EXPORT EUCJPDistributionAnalysis : public CharDistributionAnalysis
{
public:
    EUCJPDistributionAnalysis();

protected:
    // for euc-JP encoding, we are interested
    //  first  byte range: 0xa0 -- 0xfe
    //  second byte range: 0xa1 -- 0xfe
    // no validation needed here. State machine has done that
    int GetOrder(const char *str) override
    {
        if ((unsigned char)*str >= (unsigned char)0xa0) {
            return 94 * ((unsigned char)str[0] - (unsigned char)0xa1) + (unsigned char)str[1] - (unsigned char)0xa1;
        } else {
            return -1;
        }
    }
};
}
#endif // CharDistribution_h__
