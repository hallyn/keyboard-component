#include "wordlist.h"
#include "settingsmanager.h"

#include <QDebug>
#include <QFile>
#include <QStringList>

namespace Skeyer {

WordList::WordList()
{
}

WordList::~WordList()
{
    saveUserData();
}

void WordList::loadDictionary(const QString &locale)
{
    m_locale = locale;

    if( !loadUserData() || m_data.isEmpty() ){
        qDebug() << "No prior cached data found for" << locale;
        qDebug() << "Now parsing the bundled dictionary file";
        parseDictionary();
    }
    analyzeWordsList();
}

bool WordList::loadUserData()
{

    QString userDataFileName = QString("%1.data").arg(m_locale);

    QFile userDataFile( SettingsManager::userDataPath(userDataFileName) );
    // Need to evaluate the difference between saving only the user used words vs. all the words in user's database
    // Saving only user's words means easy upload to cloud/backup.
    // Saving everything = less complicated code, faster startup time

    if ( userDataFile.open(QIODevice::ReadOnly) ) {
        QDataStream userDataStream(&userDataFile);
        userDataStream >> m_metadata;
        userDataStream >> m_data;
        return true;
    }

    return false;
}

bool WordList::parseDictionary()
{
    QString dictionaryFilePath = SettingsManager::languagePackPath(m_locale,"dictionary.txt");
    QFile dictionaryFile(dictionaryFilePath);
    QString line;

    QMap<QString, QString> attributes;
    m_data.clear();
    bool isHeader = true;

    if ( dictionaryFile.open(QFile::ReadOnly) ) {

        while( !(line = dictionaryFile.readLine()).isEmpty() ){

            if(line.startsWith("#"))
                continue;

            attributes = parseCSVLine(line);

            // First parse the header
            if(isHeader)
            {
                if( !attributes.contains("dictionary") )
                {
                    qDebug() << "Failed to parse the current dictionary" << attributes;
                    return false;
                }

                m_metadata = attributes;
                isHeader = false;
                continue;
            }

            if(!attributes.contains("shortcut") || !attributes.contains("bigram"))
                m_data << Word::fromAttributes(attributes);
        }

        qDebug() << "Read"<< m_data.count() << "words from" << dictionaryFilePath;
        qDebug() << "Dictionary details" << m_metadata;
        return true;
    }

    qDebug() << "Failed to load " << dictionaryFilePath;
    return false;
}

QMap<QString, QString> WordList::parseCSVLine(const QString &line)
{
    QMap<QString, QString> result;

    if(line.isEmpty())
        return result;

    QStringList kvPairs = line.split(',');

    for(const auto pair : kvPairs)
    {
        QStringList kv = pair.split('=');
        result[ kv[0].trimmed() ] = kv[1].trimmed();
    }

    return result;
}

bool WordList::saveUserData()
{

    QString userDataFileName = QString("%1.data").arg(m_locale);
    QFile userDataFile( SettingsManager::userDataPath(userDataFileName) );
    // Need to evaluate the difference between saving only the user used words vs. all the words in user's database
    // Saving only user's words means easy upload to cloud/backup.
    // Saving everything = less complicated code, faster startup time

    if ( userDataFile.open(QIODevice::WriteOnly) ) {
        QDataStream userDataStream(&userDataFile);
        userDataStream << m_metadata;
        userDataStream << m_data;
        return true;
    }

    return false;
}

Word WordList::word(const QString& w)
{
    auto result = qBinaryFind(m_data, Word(w));
    if( result != m_data.end() )
        return *result;

    // Add the word to user list
    return Word(w);
}

void WordList::incrementProbabilityOnce(const QString &w)
{
    auto result = qBinaryFind(m_data.begin(),m_data.end(), Word(w));
    if( result != m_data.end() )
        result->incrementProbabilityOnce();
    else addWord(Word(w));
}

bool WordList::addWord(const Word &w)
{
    auto location = qLowerBound(m_data.begin(), m_data.end(), w);

    if( location != m_data.end() )
    {
        //If the word already exists, simply update it.
        *location = w;
        return false;
    }
    m_data.insert(location, w);

    return true;
}

QList<Word>::iterator WordList::begin(const QChar &w)
{
    if(w.isNull() || ! m_alphabeticalIndex.contains(w))
        return m_data.begin();
    return m_data.begin() + m_alphabeticalIndex[w];
}

QList<Word>::iterator WordList::end(const QChar& w)
{
    auto keys = m_alphabeticalIndex.keys();
    int i = keys.indexOf(w);

    if(w.isNull() || i == -1 || i == keys.length() - 1)
        return m_data.end();

    return m_data.begin() + m_alphabeticalIndex[keys[i+1]];
}

QList<Word>::const_iterator WordList::constBegin(const QChar &w)
{
    if(w.isNull() || ! m_alphabeticalIndex.contains(w))
        return m_data.constBegin();
    return m_data.constBegin() + m_alphabeticalIndex[w];
}

QList<Word>::const_iterator WordList::constEnd(const QChar &w)
{
    auto keys = m_alphabeticalIndex.keys();
    int i = keys.indexOf(w);

    if(w.isNull() || i == -1 || i == keys.length() - 1)
        return m_data.constEnd();

    return m_data.constBegin() + m_alphabeticalIndex[keys[i+1]];
}

QMap<QString, QString> WordList::metadata() const
{
    return m_metadata;
}

void WordList::analyzeWordsList()
{

    qSort(m_data.begin(), m_data.end());

    for (int i = 0; i < m_data.count(); i++)
    {
        const Word& w = m_data[i];
        const QString& word = w.word();

        if(!word.isEmpty() && !m_alphabeticalIndex.contains(word[0]))
            m_alphabeticalIndex[word[0]] = i;
    }
}

int WordList::indentLevel(const QString &line)
{
    int result = 0;
    foreach (const QChar& c , line)
    {
        if( c.isSpace() )
            result++;
        else break;
    }
    return result;
}

bool WordList::contains(const QString &w) const
{
    auto result = qBinaryFind(m_data.begin(),m_data.end(), Word(w));
    if( result != m_data.end() )
    {
        return true;
    }
    return false;
}

}
