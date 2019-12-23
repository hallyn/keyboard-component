#ifndef WORD_H
#define WORD_H

#include <QMap>
#include <QPair>
#include <QString>
#include <QVector>
#include <QDataStream>

namespace Skeyer {

class Word {
    /**
      * Represents a word
      *
      * Currently, we are generating the dictionary from the Android Latin IME, where,
      *
      * Keys = [' word', 'f', 'whitelist', '  shortcut', 'not_a_word', 'flags', 'originalFreq']
      * Flags = ['', 'hand-added', 'nonword', 'e', 'australian:nonword', 'drugs', 'medical',
      * 'nonword:offensive', 'offensive', 'babytalk', 'n', 'abbreviation', 'technical',
      * 's', 'r', 'australian', 'offensive:nonword', 'p']
      *
      *
      * f = userCount
      * originalFreq = corpusCount
      *
      * TODO: Implement support for shortcuts.
      * */
    QString m_word; //The actual word

    quint8 m_frequency; // is an integer from 15 - > 255
    quint8 m_originalFrequency;

    quint16 m_flags;

    //Cached values
    quint8 m_swipeHintLength; //Swipe hint when this word is swiped

    QVector < QPair<quint8, quint8> > m_swipeHintVector;
    QVector < QPair<quint8, quint8> > m_wordVector;

    qreal m_probability;


    void updateProbability();

public:

    enum Flags {
        n = 1,
        s = 1 << 1,
        r = 1 << 2,
        p = 1 << 3,
        e = 1 << 4,
        offensive = 1 << 5,
        babytalk = 1 << 6,
        handadded = 1 << 7,
        drugs = 1 << 8,
        medical = 1 << 9,
        technical = 1 << 10,
        abbreviation = 1 << 11
    };

    Word(const QString& w = "",
         quint8 frequency = 1, quint8 m_originalFrequency = 0,
         quint16 flags = 0);

    static Word fromAttributes(const QMap<QString, QString>& attributes);

    const QString& word() const;

    double probability() const;

    int swipeHintLength() const;
    void setSwipeHintLength(const quint8 length);

    QVector< QPair<quint8, quint8> > swipeHintFrequencyVector() const;
    void setSwipeHintFrequencyVector(const QVector< QPair<quint8, quint8> >& vector);

    QVector< QPair<quint8, quint8> >  wordFrequencyVector() const;
    void setWordFrequencyVector(const QVector< QPair<quint8, quint8> >& vector);

    void incrementProbabilityOnce();


    inline bool operator< (const QString& other) const { return m_word < other; }
    inline bool operator< (const Word& other) const { return m_word < other.m_word; }

    inline bool operator> (const QString& other) const { return m_word > other; }
    inline bool operator> (const Word& other) const { return m_word > other.m_word; }

    inline bool operator== (const Word& other) const { return m_word == other.m_word; }

    friend QDataStream& operator<<(QDataStream &out, const Word &w);
    friend QDataStream& operator>>(QDataStream &in, Word &w);
};


QDataStream& operator<<(QDataStream &out, const Word &w);
QDataStream& operator>>(QDataStream &in, Word &w);

}

#endif // WORD_H
