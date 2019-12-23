#include "bruteforcematcher.h"
#include "settingsmanager.h"
#include "toplist.h"
#include "engine.h"

#include <QDebug>
#include <qmath.h>

namespace Skeyer {

BruteForceMatcher::BruteForceMatcher(Engine *parent):
    AbstractWordMatcher(parent),
    m_threshold(0.5)
{
    connect( m_engine, SIGNAL(wordSelected(const QString&, const QString&, const QList<QPointF>&)),
             this, SLOT(onWordSelected(const QString &, const QString &, const QList<QPointF> &)) );

    connect( SettingsManager::instance(), SIGNAL(currentLocaleChanged(const QString&)),
             this, SLOT(updateLocale(const QString&)) );

    updateLocale(SettingsManager::instance()->currentLocale());
}

BruteForceMatcher::~BruteForceMatcher()
{

}

QList<QString> BruteForceMatcher::match(QString input, QList<QPointF> path, int count, QString context)
{
    Q_UNUSED(context)
    input = input.toLower();

    TopList<QString, double> result( count );

    double pTotal, pEditDistance, distance, pDictionary;

    //    qDebug() << QString("Input = %1 ( %2 ) points").arg( input ).arg( path.length() );
    int wordsMatched = 0;
    int totalWords = 0;
    int skippedWords = 0;

    QString word;

    int maxLength;

    int k = qMax( 5.0, m_threshold*input.length() );
    double minValue = 0;

    const QVector< QPair<quint8, quint8> > inputUnigramFrequency = unigramFrequencyVector(input);

    m_cost = 0;

    //    QChar c =  input[0]; // Just search for the words starting with the first alphabet
    for( const auto c : m_keyCodeNeighbours[input[0]] ){
        const auto cursorBegin = m_wordList.begin( c );
        const auto cursorEnd = m_wordList.end( c );
        for( auto cursor = cursorBegin; cursor < cursorEnd; cursor++)
        {
            totalWords++;

            pDictionary = cursor->probability();

            if( pDictionary < minValue )
            {
                skippedWords++;
                continue;
            }

            if(!path.isEmpty())
            {
                if( ( cursor->swipeHintLength() > 0 && qAbs(input.length() - cursor->swipeHintLength() ) > k ) ||
                    unigramfrequencyDistance( inputUnigramFrequency, cursor->swipeHintFrequencyVector() ) > k  )
                {
                    skippedWords++;
                    continue;
                }

                word = swipeHint( cursor->word().toLower() );

                if(cursor->swipeHintFrequencyVector().isEmpty())
                {
                    cursor->setSwipeHintLength( word.length() );
                    cursor->setSwipeHintFrequencyVector( unigramFrequencyVector(word) );
                }
            }
            else
            {
                if( qAbs(input.length() - cursor->word().length()) > k ||
                    unigramfrequencyDistance( inputUnigramFrequency, cursor->wordFrequencyVector() ) > k )
                {
                    skippedWords++;
                    continue;
                }

                word = cursor->word().toLower();

                if( cursor->wordFrequencyVector().isEmpty() )
                    cursor->setWordFrequencyVector( unigramFrequencyVector(word) );
            }

            maxLength = qMax( input.length(), word.length() );

            distance = editDistance(input, word, k);
            pEditDistance = 1.0 - distance/maxLength;

            // TODO: Find a better metric for a non match of two words
            // Skipping the words which are beyond our threshold
            if(distance > k)
                continue;

            //TODO: Account for perplexity..
            //            pTotal = pEditDistance*qPow(pDictionary, 1.0 - pEditDistance);
            pTotal = pEditDistance*pDictionary;// Bayes Theorem
            result.add( cursor->word(), pTotal );

            // Once we have enough words, adjust our search params, to speed things up
            // Compute the distance for such k that this can make it to toplist
            if( result.count() == count )
            {
                minValue = result.minValue();
                k = qMin( k, (int)( maxLength*( 1.0 - minValue/pDictionary ) + 2 )  );
            }

            wordsMatched++;
        }
    }

//    qDebug() << QString("Skipped %1 words").arg(skippedWords);
//    qDebug() <<  QString("Considered %1/%2 words").arg(wordsMatched).arg(totalWords) << "@" << m_cost;
    return result.get();
}

void BruteForceMatcher::onWordSelected(const QString &word, const QString &input, const QList<QPointF> &inputPath)
{
    Q_UNUSED(input)
    Q_UNUSED(inputPath)

    m_wordList.incrementProbabilityOnce(word);
}

void BruteForceMatcher::updateLocale(const QString &locale)
{
    m_wordList.loadDictionary( locale );

    // Check and Generate Cached data...
    const auto cursorBegin = m_wordList.begin();
    const auto cursorEnd = m_wordList.end();

    for( auto cursor = cursorBegin; cursor < cursorEnd; cursor++)
    {
        if(cursor->swipeHintFrequencyVector().isEmpty())
        {
            if(! ( cursor->swipeHintFrequencyVector().isEmpty() ||
                   cursor->wordFrequencyVector().isEmpty() ) )
                continue;

            QString word = cursor->word().toLower();
            QString hint = swipeHint( word );

            cursor->setWordFrequencyVector( unigramFrequencyVector(word) );
            cursor->setSwipeHintFrequencyVector( unigramFrequencyVector(hint) );
            cursor->setSwipeHintLength( hint.length() );
        }
    }
}

}
