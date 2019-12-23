#ifndef WORDLIST_H
#define WORDLIST_H

#include <QList>
#include <QMap>

#include "word.h"

namespace Skeyer {

class WordList
{
    /**
      * This class holds the list of words contained in the dictionary.
      * It also contains the code to serialize and de-serialize it's contents
      * and code to parse the dictionary files.
      *
      * TODO: Extend this to hold the User Error Frequency Values too.
      * TODO: See if directly parsing Android dictionary files makes sense.
      **/
public:

    explicit WordList();
    ~WordList();

    void loadDictionary(const QString& locale);

    bool loadUserData();
    bool saveUserData();

    Word word(const QString &w);
    bool addWord(const Word& w);

    void incrementProbabilityOnce(const QString &w);

//    inline QString swipeHint(const QString& w);
//    inline bool setSwipeHint(const QString& w, const QString& swipeHint);

    inline int count() const { return m_data.length(); }
    inline bool contains(const QString& w ) const;

    QList<Word>::iterator begin(const QChar& w = QChar());
    QList<Word>::iterator end(const QChar& w = QChar());
    QList<Word>::const_iterator constBegin(const QChar& w);
    QList<Word>::const_iterator constEnd(const QChar& w);

    // Holds the android dictionary's header file. Mainly useful for it's version and date of creation
    QMap<QString, QString> metadata() const;

private:
    bool parseDictionary();
    QMap<QString, QString> parseCSVLine(const QString& line);

    void analyzeWordsList();

    int indentLevel(const QString& line);

    QList<Word> m_data;
    QMap<QString, QString> m_metadata;

    QString m_locale;
    QMap<QChar, int> m_alphabeticalIndex;
};

}

#endif // WORDLIST_H
