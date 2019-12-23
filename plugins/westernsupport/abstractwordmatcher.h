#ifndef ABSTRACTWORDMATCHER_H
#define ABSTRACTWORDMATCHER_H

#include <QObject>
#include <QString>
#include <QPointF>
#include <QHash>
#include <QMap>

#define MAX_WORD_LENGTH 512

namespace Skeyer {

class Engine;
class KeyboardModel;
class AbstractWordMatcher : public QObject
{
    Q_OBJECT
public:
    explicit AbstractWordMatcher(Engine *parent = 0);
    virtual ~AbstractWordMatcher(){}


    /**
     * @brief The main match() method, which does all of the heavy lifting.
     *
     * @param input - sequence of keys that are activated for this prediction
     * @param count - number of results to fetch. Default: 5
     * @param context - for future use for better predictions based on the last word(s) - using bi/tri/n-grams.
     * @return
     */
    virtual QList<QString> match(QString input, QList<QPointF> path  = QList<QPointF>(), int count = 5 , QString context = "") = 0;

protected:
    /**
     * @brief Returns the modified Levenstein's distance to convert word1 to word2
     *        by considering the keyboard layout.
     * @param source
     * @param destination
     * @param k - cut off distance : -1 means no cutoff distance
     * @return min(actual distance, k+1) (when k >= 0)
     */
    double editDistance(const QString& source, const QString& destination, int k = -1);

    double editDistance(const QChar* source, const int sourceLength,
                        const QChar* destination, const int destinationLength,
                        int k = -1);

    double pCurveDistance(const QString& source, const QList<QPointF>& path, const QString& destination);

    double confidence(const QString& source, const QString& destination);

    inline double cost(const QChar& source, const QChar& destination)
    {
        m_cost++;
        //TODO: Look if this needs to be normalized to lie in between 0 and 1.
        if(source == destination) return 0;
        if(m_keyCodeLocation.contains(source)) return m_keyCodeDistances[source].value(destination, 1);
        else return 1;
    }

    /**
     * @brief calculates a possible input a user can swipe the given word by summing all the
     * keys in between the characters of the given word.
     * @param word
     * @return
     */
    QString swipeHint(const QString& word) const;

    QVector<QPair<quint8, quint8> > unigramFrequencyVector(const QString& word);
    int unigramfrequencyDistance(const QVector< QPair<quint8,quint8> >& source, const QVector< QPair<quint8, quint8> >& destination);

signals:
    void keyboardModelUpdated();

protected:
    Engine *m_engine;
    QMap< QChar, int > m_keyCodeIndices;
    QHash< QChar, QPointF > m_keyCodeLocation;
    QHash< QChar, QString > m_keyCodeNeighbours;
    QHash< QChar, QHash < QChar, double > > m_keyCodeDistances;
    QHash< QChar, QHash < QChar, QString > > m_keyCodesBetween;
    int m_cost;

private slots:
    void updateKeyboardModel(const QString& locale);
private:
    KeyboardModel *m_keyboard;
};

}

#endif // ABSTRACTWORDMATCHER_H
