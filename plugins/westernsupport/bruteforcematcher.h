#ifndef BRUTEFORCEMATCHER_H
#define BRUTEFORCEMATCHER_H

#include "abstractwordmatcher.h"
#include "wordlist.h"

#include <QObject>

namespace Skeyer {

class BruteForceMatcher : public AbstractWordMatcher
{
    Q_OBJECT
public:
    BruteForceMatcher(Engine* parent = 0);
    ~BruteForceMatcher();

    QList<QString> match(QString input, QList<QPointF> path  = QList<QPointF>(), int count = 5 , QString context = "");

private slots:
    void onWordSelected(const QString& word, const QString &input, const QList<QPointF> &inputPath);
    void updateLocale(const QString& locale);

private:
    const double m_threshold;
    WordList m_wordList;
};

}
#endif // BRUTEFORCEMATCHER_H
