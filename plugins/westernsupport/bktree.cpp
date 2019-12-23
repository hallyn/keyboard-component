#include "bktree.h"
#include <QDebug>

BKTree::BKTree() :
    m_rootNode(QString()),
    m_nodeCount(0)
{
//    insert("book");
//    insert("books");
//    insert("cake");
//    insert("boo");
//    insert("cape");
//    insert("boon");
//    insert("cook");
//    insert("cart");

//    m_rootNode.dumpTree();

//    qDebug() << searchMatch("caqe");
//    qDebug() << m_rootNode.children.count();
}

BKTree::~BKTree()
{

}

void BKTree::insert(const QString word)
{
    if (word.isEmpty()) {
        qWarning() << Q_FUNC_INFO << "given empty word";
        return;
    }

    // no root yet
    if (m_rootNode.word.isEmpty()) {
        m_rootNode.word = word;
        return;
    }

    if(m_rootNode.insert(word))
        m_nodeCount++;
}

QMap<int, QString> BKTree::searchMatch(const QString word)
{
    int count = 0;
    QMap<int, QString> suggestions;

    QList<BKNode*> toVisit;
    toVisit.append(&m_rootNode);
    while(!toVisit.isEmpty()) {
        BKNode *node = toVisit.takeFirst();

        int distance = editDistance(word, node->word, SEARCH_TOLERANCE);

        if (distance < SEARCH_TOLERANCE) {
            suggestions.insert(distance, node->word);
        }

        foreach (int key, node->children.keys()) {
            if (key > distance - SEARCH_TOLERANCE || key < distance + SEARCH_TOLERANCE) {
                toVisit.append(&node->children[key]);
            }
        }
        count++;
    };
    //qDebug() << count << "/" << m_nodeCount << "nodes searched for word" << word << "for" << suggestions.count() << "results.";
    return suggestions;
}


double BKTree::editDistance(const QString& source, const QString& destination, int k)
{
    int i,j;
    int previousRow = 0, currentRow = 1;
    double deletionCost, insertionCost, substitutionCost;
    double rowMin;


    if(k < 0) k = qMax(source.length(), destination.length());

    if(source.isEmpty() || destination.isEmpty())
        return qMin(source.length() + destination.length(), k + 1);

    // The diff will always be at least the difference between the word lengths
    if( qAbs(source.length() - destination.length()) > k )
        return k + 1;

    QString first;
    QString second;
    if(source.length() > destination.length())
    {
        first = source;
        second = destination;
    }
    else
    {
        first = destination;
        second = source;
    }
    int jMin, jMax;

    double scratchPad[2][second.length() + 1];

    for(j = 0; j <= second.length(); j++)
        scratchPad[previousRow][j] = j;

    for(i = 1; i <= first.length(); i++)
    {
        previousRow = (i-1)%2;
        currentRow = (i)%2;
        scratchPad[currentRow][0] = i;

        jMin = qMax(1 , i - k - 1);
        jMax = qMin( second.length(), i + k + 1);

        rowMin = second.length();
        for(j = jMin; j <= jMax; j++)
        {
            deletionCost =  scratchPad[previousRow][j] + 1;
            insertionCost = scratchPad[currentRow][j-1] + 1;
            // Substitution comes free of cost if we are not substituting anything
            substitutionCost = scratchPad[previousRow][j-1] + cost( first[i-1], second[j-1]);

            scratchPad[currentRow][j] = qMin( deletionCost, qMin( substitutionCost, insertionCost) );
            rowMin = qMin(rowMin, scratchPad[currentRow][j]);
        }

        if(rowMin > k)
            return k + 1;
    }


    return scratchPad[currentRow][second.length()];
}

double BKTree::cost(const QChar &source, const QChar &destination)
{
    if(source == destination) return 0;
    else return 1;
}

bool BKTree::BKNode::insert(const QString newWord)
{
    if(word == newWord)
        return false;

    const int distance = editDistance(word, newWord);


    if (children.contains(distance)) {
        return children[distance].insert(newWord);
    } else {
        children.insert(distance, newWord);
    }

    return true;
}
