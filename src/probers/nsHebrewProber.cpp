/*  -*- C++ -*-
    SPDX-FileCopyrightText: 1998 Netscape Communications Corporation <developer@mozilla.org>

    SPDX-License-Identifier: MIT
*/

#include "nsHebrewProber.h"
#include <stdio.h>

// windows-1255 / ISO-8859-8 code points of interest
#define FINAL_KAF ('\xea')
#define NORMAL_KAF ('\xeb')
#define FINAL_MEM ('\xed')
#define NORMAL_MEM ('\xee')
#define FINAL_NUN ('\xef')
#define NORMAL_NUN ('\xf0')
#define FINAL_PE ('\xf3')
#define NORMAL_PE ('\xf4')
#define FINAL_TSADI ('\xf5')
#define NORMAL_TSADI ('\xf6')

// Minimum Visual vs Logical final letter score difference.
// If the difference is below this, don't rely solely on the final letter score distance.
#define MIN_FINAL_CHAR_DISTANCE (5)

// Minimum Visual vs Logical model score difference.
// If the difference is below this, don't rely at all on the model score distance.
#define MIN_MODEL_DISTANCE (0.01)

#define VISUAL_HEBREW_NAME ("ISO-8859-8")
#define LOGICAL_HEBREW_NAME ("windows-1255")

namespace kencodingprober
{
bool nsHebrewProber::isFinal(char c)
{
    return ((c == FINAL_KAF) || (c == FINAL_MEM) || (c == FINAL_NUN) || (c == FINAL_PE) || (c == FINAL_TSADI));
}

bool nsHebrewProber::isNonFinal(char c)
{
    return ((c == NORMAL_KAF) || (c == NORMAL_MEM) || (c == NORMAL_NUN) || (c == NORMAL_PE));
    // The normal Tsadi is not a good Non-Final letter due to words like
    // 'lechotet' (to chat) containing an apostrophe after the tsadi. This
    // apostrophe is converted to a space in FilterWithoutEnglishLetters causing
    // the Non-Final tsadi to appear at an end of a word even though this is not
    // the case in the original text.
    // The letters Pe and Kaf rarely display a related behavior of not being a
    // good Non-Final letter. Words like 'Pop', 'Winamp' and 'Mubarak' for
    // example legally end with a Non-Final Pe or Kaf. However, the benefit of
    // these letters as Non-Final letters outweighs the damage since these words
    // are quite rare.
}

/** HandleData
 * Final letter analysis for logical-visual decision.
 * Look for evidence that the received buffer is either logical Hebrew or
 * visual Hebrew.
 * The following cases are checked:
 * 1) A word longer than 1 letter, ending with a final letter. This is an
 *    indication that the text is laid out "naturally" since the final letter
 *    really appears at the end. +1 for logical score.
 * 2) A word longer than 1 letter, ending with a Non-Final letter. In normal
 *    Hebrew, words ending with Kaf, Mem, Nun, Pe or Tsadi, should not end with
 *    the Non-Final form of that letter. Exceptions to this rule are mentioned
 *    above in isNonFinal(). This is an indication that the text is laid out
 *    backwards. +1 for visual score
 * 3) A word longer than 1 letter, starting with a final letter. Final letters
 *    should not appear at the beginning of a word. This is an indication that
 *    the text is laid out backwards. +1 for visual score.
 *
 * The visual score and logical score are accumulated throughout the text and
 * are finally checked against each other in GetCharSetName().
 * No checking for final letters in the middle of words is done since that case
 * is not an indication for either Logical or Visual text.
 *
 * The input buffer should not contain any white spaces that are not (' ')
 * or any low-ascii punctuation marks.
 */
nsProbingState nsHebrewProber::HandleData(const char *aBuf, unsigned int aLen)
{
    // Both model probers say it's not them. No reason to continue.
    if (GetState() == eNotMe) {
        return eNotMe;
    }

    const char *curPtr;
    const char *endPtr = aBuf + aLen;

    for (curPtr = (char *)aBuf; curPtr < endPtr; ++curPtr) {
        char cur = *curPtr;
        if (cur == ' ') { // We stand on a space - a word just ended
            if (mBeforePrev != ' ') { // *(curPtr-2) was not a space so prev is not a 1 letter word
                if (isFinal(mPrev)) { // case (1) [-2:not space][-1:final letter][cur:space]
                    ++mFinalCharLogicalScore;
                } else if (isNonFinal(mPrev)) { // case (2) [-2:not space][-1:Non-Final letter][cur:space]
                    ++mFinalCharVisualScore;
                }
            }
        } else { // Not standing on a space
            if ((mBeforePrev == ' ') && (isFinal(mPrev)) && (cur != ' ')) { // case (3) [-2:space][-1:final letter][cur:not space]
                ++mFinalCharVisualScore;
            }
        }
        mBeforePrev = mPrev;
        mPrev = cur;
    }

    // Forever detecting, till the end or until both model probers return eNotMe (handled above).
    return eDetecting;
}

// Make the decision: is it Logical or Visual?
const char *nsHebrewProber::GetCharSetName()
{
    // If the final letter score distance is dominant enough, rely on it.
    int finalsub = mFinalCharLogicalScore - mFinalCharVisualScore;
    if (finalsub >= MIN_FINAL_CHAR_DISTANCE) {
        return LOGICAL_HEBREW_NAME;
    }
    if (finalsub <= -(MIN_FINAL_CHAR_DISTANCE)) {
        return VISUAL_HEBREW_NAME;
    }

    // It's not dominant enough, try to rely on the model scores instead.
    float modelsub = mLogicalProb->GetConfidence() - mVisualProb->GetConfidence();
    if (modelsub > MIN_MODEL_DISTANCE) {
        return LOGICAL_HEBREW_NAME;
    }
    if (modelsub < -(MIN_MODEL_DISTANCE)) {
        return VISUAL_HEBREW_NAME;
    }

    // Still no good, back to final letter distance, maybe it'll save the day.
    if (finalsub < 0) {
        return VISUAL_HEBREW_NAME;
    }

    // (finalsub > 0 - Logical) or (don't know what to do) default to Logical.
    return LOGICAL_HEBREW_NAME;
}

void nsHebrewProber::Reset(void)
{
    mFinalCharLogicalScore = 0;
    mFinalCharVisualScore = 0;

    // mPrev and mBeforePrev are initialized to space in order to simulate a word
    // delimiter at the beginning of the data
    mPrev = ' ';
    mBeforePrev = ' ';
}

nsProbingState nsHebrewProber::GetState(void)
{
    // Remain active as long as any of the model probers are active.
    if ((mLogicalProb->GetState() == eNotMe) && (mVisualProb->GetState() == eNotMe)) {
        return eNotMe;
    }
    return eDetecting;
}

#ifdef DEBUG_PROBE
void nsHebrewProber::DumpStatus()
{
    printf("  HEB: %d - %d [Logical-Visual score]\r\n", mFinalCharLogicalScore, mFinalCharVisualScore);
}
#endif
}
