#include "hunspellmatcher.h"
#include "settingsmanager.h"
#include "toplist.h"

#include <hunspell/hunspell.hxx>
#include <QDebug>
#include <QtMath>
#include <QFile>
#include <QTextStream>

namespace Skeyer {

HunspellMatcher::HunspellMatcher(Engine *parent):
    AbstractWordMatcher(parent),
    m_hunspellWords(0),
    m_hunspellSwipeHints(0)
{
    connect( SettingsManager::instance(), SIGNAL(currentLocaleChanged(const QString&)),
             this, SLOT(updateLocale(const QString&)) );

    updateLocale(SettingsManager::instance()->currentLocale());
}

HunspellMatcher::~HunspellMatcher()
{

}

QList<QString> HunspellMatcher::match(QString input, QList<QPointF> path, int count, QString context)
{
    Q_UNUSED(context)
    TopList<QString, double> result(count);

    if(!m_hunspellWords || !m_hunspellSwipeHints)
    {
        qWarning() << "Hunspell not ready!";
        return result.get();
    }

    char **suggestions;
    int suggestionCount;
    double pDistance, pDictionary;

    input = input.toLower();

    if(path.isEmpty())
    {
        suggestionCount = m_hunspellWords->suggest(&suggestions, input.toUtf8().constData());

        for(int i = 0; i < suggestionCount; i++)
        {
            const QString& suggestion = QString::fromUtf8(suggestions[i]);
            pDistance = 1.0 - editDistance(input, suggestion)/qMax(input.length(), suggestion.length());
            pDictionary = 1;
            result.add( suggestion, pDistance*pDictionary );
        }

        m_hunspellWords->free_list(&suggestions, suggestionCount);
    }
    else
    {
        suggestionCount = m_hunspellSwipeHints->suggest(&suggestions, input.toUtf8().constData());
        for(int i = 0; i < suggestionCount; i++)
        {
            const QString& suggestion = QString::fromUtf8(suggestions[i]);
            if(!m_swipeHintWordMap.contains(suggestion))
            {
                qDebug() << suggestion << "is invalid swipehint";
                continue;
            }

            for(const auto word : m_swipeHintWordMap.values(suggestion) )
            {
                pDistance = 1.0 - editDistance( input, suggestion )/qMax(input.length(), suggestion.length());
                pDictionary = m_dictionary.word(word).probability();
                result.add( word, pDistance*pDictionary );
            }
        }

        m_hunspellSwipeHints->free_list(&suggestions, suggestionCount);
    }

    return result.get();
}

void HunspellMatcher::updateLocale(const QString &locale)
{
    m_dictionary.loadDictionary(locale);

    for(auto cursor = m_dictionary.begin(); cursor != m_dictionary.end(); cursor++)
        m_swipeHintWordMap.insert( swipeHint( cursor->word().toLower() ), cursor->word() );

    QString wordsFilePath = SettingsManager::userDataPath(QString("%1.words").arg(locale), "HunspellMatcher");
    QString swipeHintFilePath = SettingsManager::userDataPath(QString("%1.swipehints").arg(locale),"HunspellMatcher");
    QString emptyAffixFilePath = SettingsManager::userDataPath("empty.aff", "HunspellMatcher");

    QFile wordsFile( wordsFilePath );
    if(!wordsFile.exists())
    {
        if(!wordsFile.open(QFile::WriteOnly))
        {
            qWarning() << wordsFilePath << "not writeable";
            return;
        }
        QTextStream stream(&wordsFile);

        stream << QString::number(m_dictionary.count()) << "\n";

        for(auto cursor = m_dictionary.begin(); cursor != m_dictionary.end(); cursor++)
            stream << cursor->word() << "\n";

        stream << "\n";
    }
    wordsFile.close();

    QFile swipeHintFile(swipeHintFilePath);
    if(!swipeHintFile.exists())
    {
        if(!swipeHintFile.open(QFile::WriteOnly))
        {
            qWarning() << swipeHintFilePath << "not writeable";
            return;
        }

        QTextStream stream(&swipeHintFile);

        stream << QString::number(m_dictionary.count()) << "\n";

        for(auto cursor = m_dictionary.begin(); cursor != m_dictionary.end(); cursor++)
            stream << swipeHint(cursor->word().toLower()) << "\n";

        stream << "\n";
    }
    swipeHintFile.close();

    QFile emptyAffixFile(emptyAffixFilePath);
    if(!emptyAffixFile.exists())
    {
        if(!emptyAffixFile.open(QFile::WriteOnly))
        {
            qWarning() << emptyAffixFilePath << "not writeable";
            return;
        }
        QTextStream stream(&emptyAffixFile);
        stream << "\n";
    }
    emptyAffixFile.close();

    m_hunspellWords.reset( new Hunspell(emptyAffixFilePath.toUtf8().data(), wordsFilePath.toUtf8().data() ) );
    m_hunspellSwipeHints.reset( new Hunspell(emptyAffixFilePath.toUtf8().data(), swipeHintFilePath.toUtf8().data()) );
}

}
