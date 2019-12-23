#include "abstractwordmatcher.h"
#include "engine.h"
#include "keyboardmodel.h"

#include <QDebug>
#include <QStringList>
#include <qmath.h>

namespace Skeyer {

AbstractWordMatcher::AbstractWordMatcher(Engine *parent) :
    QObject(parent),
    m_engine(parent),
    m_cost(0),
    m_keyboard(m_engine->keyboardModel())
{
    connect( m_keyboard, SIGNAL(keyboardChanged(const QString&)),
             this, SLOT(updateKeyboardModel(const QString&)) );

    updateKeyboardModel("");
}

double AbstractWordMatcher::editDistance(const QString &source, const QString &destination, int k)
{
    return editDistance(source.constData(), source.length(), destination.constData(), destination.length(), k);
}

double AbstractWordMatcher::editDistance(const QChar *source, const int sourceLength, const QChar *destination, const int destinationLength, int k)
{
    int i,j;
    int previousRow = 0, currentRow = 1;
    double deletionCost, insertionCost, substitutionCost;
    double rowMin;
    double scratchPad[2][destinationLength + 1];

    if(k < 0) k = qMax(sourceLength, destinationLength);

    if(sourceLength < 1 || destinationLength < 1)
        return qMin(sourceLength + destinationLength, k + 1);

    // The diff will always be at least the difference between the word lengths
    if( qAbs(sourceLength - destinationLength) > k )
        return k + 1;

    for(j = 0; j <= destinationLength; j++)
        scratchPad[previousRow][j] = j;

    for(i = 1; i <= sourceLength; i++)
    {
        previousRow = (i-1)%2;
        currentRow = (i)%2;
        scratchPad[currentRow][0] = i;

        rowMin = destinationLength;
        for(j = 1; j <= destinationLength; j++)
        {
            deletionCost =  scratchPad[previousRow][j] + 1;
            insertionCost = scratchPad[currentRow][j-1] + 1;
            // Substitution comes free of cost if we are not substituting anything
            substitutionCost = scratchPad[previousRow][j-1] + cost( source[i-1], destination[j-1]);

            scratchPad[currentRow][j] = qMin( deletionCost, qMin( substitutionCost, insertionCost) );

            rowMin = qMin(rowMin, scratchPad[currentRow][j]);
        }

        if(rowMin > k)
            return k + 1;
    }


    return scratchPad[currentRow][destinationLength];
}

double AbstractWordMatcher::pCurveDistance(const QString &source, const QList<QPointF>& path, const QString &destination)
{
    // Kind of implements:
    // http://stackoverflow.com/questions/6418042/how-do-i-calculate-the-difference-between-two-sequences-of-points
    // TODO: look into quadratic and other kinds of interpolations

    if( source.count() < 2 || path.count() < 2 || destination.count() < 2 )
        return 0;

    double distance = 0;
    double epsilon = 0;
    double x1, x2, y1, y2;

    int nPoints1 = path.length();
    int nPoints2 = destination.length();
    int step = 5;
    int tmp;

    QVector<QPointF> path2( nPoints2 );
    for( int i = 0; i < nPoints2; i++ )
        path2[i] = m_keyboard->keyCodeLocation(destination[i]);

    for( int i = 0; i < nPoints1; i += step )
    {
        x1 = path[i].x();
        y1 = path[i].y();

        epsilon = nPoints2*((double)i/nPoints1);
        tmp = epsilon; epsilon -= tmp;

        if(tmp < nPoints2 - 1)
        {
            x2 =  (1 - epsilon)*path2[tmp].x() + epsilon*path2[tmp + 1].x();
            y2 =  (1 - epsilon)*path2[tmp].y() + epsilon*path2[tmp + 1].y();
        }
        else
        {
            x2 = path2.last().x();
            y2 = path2.last().y();
        }

        distance += qSqrt((y2- y1)*(y2 - y1) + (x2 - x1)*(x2 - x1));
    }

    return qAtan(1.0/distance)/qAsin(1);
}

double AbstractWordMatcher::confidence(const QString &source, const QString &destination)
{
    // TODO: A better measure of confidence could be the number of common characters
    return qAbs( source.length() - destination.length() )/qMax( source.length() , destination.length() );
}





QString AbstractWordMatcher::swipeHint(const QString &word) const
{
    if( word.length() < 2 )
        return "";

    QChar result[MAX_WORD_LENGTH];
    int resultLength = 0;

    for( int i = 1; i < word.length(); i++ )
    {
        result[resultLength++] = word[i-1];

        const auto keysBetween = m_keyCodesBetween[word[i-1]][word[i]];
        for(const auto c : keysBetween)
            result[resultLength++] = c;
    }

    result[resultLength++] = word[word.length() - 1];

    return QString(result, resultLength);
}

void AbstractWordMatcher::updateKeyboardModel(const QString &locale)
{
    Q_UNUSED(locale)
    m_keyCodeLocation.clear();
    m_keyCodeDistances.clear();
    m_keyCodeNeighbours.clear();

    for(int i = 0; i < m_keyboard->rows(KeyboardModel::AlphabeticLayout); i++)
    {
        for(int j = 0; j < m_keyboard->columns(KeyboardModel::AlphabeticLayout); j++)
        {
            QString currentKey = m_keyboard->keyCodeAt(i, j, 0, KeyboardModel::DefaultModifier, KeyboardModel::AlphabeticLayout);
            if(!currentKey.isEmpty())
                m_keyCodeLocation[currentKey[0]] = m_keyboard->keyCodeLocation(currentKey);

            if(!m_keyCodeIndices.contains(currentKey[0]))
                m_keyCodeIndices[currentKey[0]] = m_keyCodeIndices.count();
        }
    }

    for(auto from : m_keyCodeLocation.keys())
    {
        if(from.isLetter())
            m_keyCodeNeighbours[from] = QString(from);

        for(auto to : m_keyCodeLocation.keys())
        {
            QPointF p0 = m_keyCodeLocation[from];
            QPointF p1 = m_keyCodeLocation[to];
            if(p0 != QPointF(-1,-1) && p1 != QPointF(-1,-1))
            {
                m_keyCodeDistances[from][to] = qSqrt( (p0.x() - p1.x())*(p0.x() - p1.x()) + (p0.y() - p1.y())*(p0.y() - p1.y()) );

                if( (int)m_keyCodeDistances[from][to] == 1 && !m_keyCodeNeighbours[from].contains(to))
                    if(from.isLetter() && to.isLetter())
                    m_keyCodeNeighbours[from] += to;
                m_keyCodesBetween[from][to] = m_keyboard->keyCodesBetween(from, to);
//                qDebug() << from << "-------->" << to << m_keyCodesBetween[from][to];
            }
        }
    }


    emit keyboardModelUpdated();
}

QVector<QPair<quint8, quint8> > AbstractWordMatcher::unigramFrequencyVector(const QString &word)
{

    QMap<quint8, quint8> temp;
    for(const auto c : word)
    {
        int ci = m_keyCodeIndices[c];
        temp[ci] = temp.value(ci, 0) + 1;
    }

    QVector< QPair<quint8, quint8> > result = QVector< QPair<quint8,quint8> >(temp.size());
    int count = 0;
    for(auto i = temp.constBegin(); i != temp.constEnd(); i++, count++)
        result[count] = { i.key(), i.value() };

    return result;
}

int AbstractWordMatcher::unigramfrequencyDistance(const QVector< QPair<quint8, quint8> >& source, const QVector<QPair<quint8, quint8> >& destination)
{
    if(source.isEmpty() || destination.isEmpty())
        return -1;

    int intersection = 0;
    int S = 0, D = 0;

    int i = 0, j = 0;

    while( i < source.size() && j < destination.size() )
    {
        if( destination[j].first == source[i].first )
        {
            intersection += qAbs(source[i].second - destination[j].second);
            i++;
            j++;
        }
        else if(source[i].first > destination[j].first)
        {
            D += destination[j].second;
            j++;
        }
        else
        {
            S += source[i].second;
            i++;
        }
    }

    while( i < source.size() )
    {
        S += source[i].second;
        i++;
    }

    while( j < destination.size() )
    {
        D += destination[j].second;
        j++;
    }

    return qMax(intersection + S, intersection + D);
}

}
