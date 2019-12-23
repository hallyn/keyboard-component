#include "word.h"

#include <QStringList>
#include <QVariant>
#include <qmath.h>

namespace Skeyer {

Word::Word(const QString &w, quint8 frequency, quint8 originalFrequency, quint16 flags):
    m_word(w),
    m_frequency(frequency),
    m_originalFrequency(originalFrequency),
    m_flags(flags),
    m_swipeHintLength(0)
{
    updateProbability();
}

void Word::updateProbability()
{
    //TODO: pDictionary would have to account for other factors such as context, error rates etc...
    if(m_frequency > 15) m_probability = qLn( m_frequency )/qLn( 255 );
    else if(m_originalFrequency > 15) m_probability = qLn( m_originalFrequency )/qLn( 255 );
    else m_probability = -1;
}

Word Word::fromAttributes(const QMap<QString, QString> &attributes)
{
    Word result;

    if( attributes.contains("word") )
        result.m_word = attributes["word"];

    if( attributes.contains("f") )
        result.m_frequency = QVariant(attributes["f"]).toInt();

    if( attributes.contains("originalFreq") )
        result.m_frequency = QVariant(attributes["f"]).toInt();

    if(attributes.contains("flags"))
    {
        QString v = attributes["flags"];
        int flags = 0;

        if( v == "n" )
            flags = Word::n;
        else if( v == "s" )
            flags = Word::s;
        else if( v == "s" )
            flags = Word::r;
        else if( v == "r" )
            flags = Word::r;
        else if( v == "p" )
            flags = Word::p;
        else if( v == "e" )
            flags = Word::e;
        else if( v == "offensive" )
            flags = Word::offensive;
        else if( v == "babytalk" )
            flags = Word::babytalk;
        else if( v == "hand-added" )
            flags = Word::handadded;
        else if( v == "drugs" )
            flags = Word::drugs;
        else if( v == "medical" )
            flags = Word::medical;
        else if( v == "technical" )
            flags = Word::technical;
        else if( v == "abbreviation" )
            flags = Word::abbreviation;

        result.m_flags = flags;
    }

    result.updateProbability();

    return result;
}

const QString &Word::word() const
{
    return m_word;
}

double Word::probability() const
{
    return m_probability;
}

int Word::swipeHintLength() const
{
    return m_swipeHintLength;
}

void Word::setSwipeHintLength(const quint8 length)
{
    m_swipeHintLength = length;
}

QVector<QPair<quint8, quint8> > Word::swipeHintFrequencyVector() const
{
    return m_swipeHintVector;
}

void Word::setSwipeHintFrequencyVector(const QVector<QPair<quint8, quint8> > &vector)
{
    m_swipeHintVector = vector;
}

QVector<QPair<quint8, quint8> > Word::wordFrequencyVector() const
{
    return m_wordVector;
}

void Word::setWordFrequencyVector(const QVector<QPair<quint8, quint8> > &vector)
{
    m_wordVector = vector;
}

void Word::incrementProbabilityOnce()
{
    if(m_frequency < 255)
        m_frequency++;

    updateProbability();
}

QDataStream &operator<<(QDataStream &out, const Word &w)
{
    out << w.m_word
        << w.m_frequency
        << w.m_originalFrequency
        << w.m_flags
        << w.m_swipeHintLength
        << w.m_swipeHintVector
        << w.m_wordVector;

    return out;
}

QDataStream &operator>>(QDataStream &in, Word &w)
{
    in >> w.m_word
       >> w.m_frequency
       >> w.m_originalFrequency
       >> w.m_flags
       >> w.m_swipeHintLength
       >> w.m_swipeHintVector
       >> w.m_wordVector;

    w.updateProbability();

    return in;
}

}
