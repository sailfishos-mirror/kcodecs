/*
    This file is part of the KDE libraries

    SPDX-FileCopyrightText: 2008 Wang Hoi <zealot.hoi@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KENCODINGPROBER_H
#define KENCODINGPROBER_H

// enable debug of private probers
// #define DEBUG_PROBE

#include <kcodecs_export.h>

#ifdef DEBUG_PROBE
#include <QDebug>
#endif

#include <QCoreApplication>
#include <QString>
#include <memory>

class KEncodingProberPrivate;

/*!
 * \class KEncodingProber
 * \inmodule KCodecs
 *
 * \brief Provides encoding detection(probe) capabilities.
 *
 * Probe the encoding of raw data only.
 * In the case it can't find it, return the most possible encoding it guessed.
 *
 * Always do Unicode probe regardless the ProberType
 *
 * Feed data to it several times with feed() until ProberState changes to FoundIt/NotMe,
 * or confidence() returns a value you find acceptable.
 *
 * Intended lifetime of the object: one instance per ProberType.
 *
 * Typical use:
 * \code
 * QByteArray data, moredata;
 * ...
 * KEncodingProber prober(KEncodingProber::Chinese);
 * prober.feed(data);
 * prober.feed(moredata);
 * if (prober.confidence() > 0.6)
 *    encoding  = prober.encoding();
 * \endcode
 *
 * At least 256 characters are needed to change the ProberState from Probing to FoundIt.
 * If you don't have so many characters to probe,
 * decide whether to accept the encoding it guessed so far according to the Confidence by yourself.
 *
 */
class KCODECS_EXPORT KEncodingProber
{
    Q_DECLARE_TR_FUNCTIONS(KEncodingProber)

public:
    /*!
     * \value FoundIt Sure find the encoding
     * \value NotMe Sure not included in current ProberType's all supported encodings
     * \value Probing Need more data to make a decision
     */
    enum ProberState {
        FoundIt,
        NotMe,
        Probing,
    };

    /*!
     * \value None
     * \value Universal
     * \value Arabic
     * \value Baltic
     * \value CentralEuropean
     * \value ChineseSimplified
     * \value ChineseTraditional
     * \value Cyrillic
     * \value Greek
     * \value Hebrew
     * \value Japanese
     * \value Korean
     * \value NorthernSaami
     * \value Other
     * \value SouthEasternEurope
     * \value Thai
     * \value Turkish
     * \value Unicode
     * \value WesternEuropean
     */
    enum ProberType {
        None,
        Universal,
        Arabic,
        Baltic,
        CentralEuropean,
        ChineseSimplified,
        ChineseTraditional,
        Cyrillic,
        Greek,
        Hebrew,
        Japanese,
        Korean,
        NorthernSaami,
        Other,
        SouthEasternEurope,
        Thai,
        Turkish,
        Unicode,
        WesternEuropean,
    };

    /*!
     * Default ProberType is Universal(detect all possible encodings)
     */
    KEncodingProber(ProberType proberType = Universal);

    ~KEncodingProber();

    KEncodingProber(const KEncodingProber &) = delete;
    KEncodingProber &operator=(const KEncodingProber &) = delete;

    /*!
     * reset the prober's internal state and data.
     */
    void reset();

    /*!
     * The main class method
     *
     * Feed \a data to the prober
     *
     * Returns the ProberState after probing the fed data.
     */
    ProberState feed(QByteArrayView data);
    // for API compatibility
    inline ProberState feed(const char *data, qsizetype len)
    {
        return feed({data, len});
    }

    /*!
     * Returns the prober's current ProberState
     *
     */
    ProberState state() const;

    /*!
     * Returns a QByteArray with the name of the best encoding it has guessed so far
     * \since 4.2.2
     */
    QByteArray encoding() const;

    /*!
     * Returns the confidence(sureness) of encoding it guessed so far (0.0 ~ 0.99), not very reliable for single byte encodings
     */
    float confidence() const;

    ProberType proberType() const;

    /*!
     * change current prober's ProberType and reset the prober
     *
     * \a proberType the new type
     */
    void setProberType(ProberType proberType);

    /*!
     * Returns the ProberType for \a lang (e.g. proberTypeForName("Chinese Simplified") will return KEncodingProber::ChineseSimplified
     */
    static ProberType proberTypeForName(const QString &lang);

    /*!
     * map ProberType to language string
     *
     * \a proberType the proper type
     *
     * Returns the language string
     */
    static QString nameForProberType(ProberType proberType);

private:
    std::unique_ptr<KEncodingProberPrivate> const d;
};

#endif
