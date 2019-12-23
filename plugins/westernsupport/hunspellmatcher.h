#ifndef HUNSPELLMATCHER_H
#define HUNSPELLMATCHER_H
#include "abstractwordmatcher.h"
#include "wordlist.h"

#include <QMultiMap>
#include <QScopedPointer>

namespace Skeyer {

class Hunspell;
class HunspellMatcher : public AbstractWordMatcher
{
    Q_OBJECT
public:
    HunspellMatcher(Engine* parent = 0);
    ~HunspellMatcher();

    QList<QString> match(QString input, QList<QPointF> path  = QList<QPointF>(), int count = 5 , QString context = "");

private slots:
    void updateLocale(const QString& locale);

private:
    QMultiMap<QString, QString> m_swipeHintWordMap;

    WordList m_dictionary;

    QScopedPointer<Hunspell> m_hunspellWords;
    QScopedPointer<Hunspell> m_hunspellSwipeHints;
};

}

#endif // HUNSPELLMATCHER_H
