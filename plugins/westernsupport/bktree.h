#ifndef BKTREE_H
#define BKTREE_H

#include <QString>
#include <QMap>
#include <QHash>
#include <QDebug>
#include <iostream>

#define SEARCH_TOLERANCE 5

class BKTree
{
public:
    BKTree();
    ~BKTree();

    static double editDistance(const QString& source, const QString& destination, int k = -1);
    inline static double cost(const QChar& source, const QChar& destination);

    void insert(const QString word);
    void dumpTree() { m_rootNode.dumpTree(); }
    QMap<int, QString> searchMatch(const QString word);

private:
    struct BKNode {
        BKNode(QString newWord = QString()) : word(newWord) {}

        bool insert(const QString newWord);

        void dumpTree() {
            static int indent = 0;
            std::cout << word.toStdString() << "\n";

            if (children.size() > 0) {
                indent++;
                foreach(const int &key, children.keys()) {
                    for (int i=0; i<indent; i++) {
                        std::cout << " ";
                    }
                    std::cout << key << ": ";
                    children[key].dumpTree();
                }
                indent--;
            }
        }

        QString word;

        // edit distance -> node
        QMap<int, BKNode> children;
    };


    BKNode m_rootNode;
    int m_nodeCount;
};

#endif // BKTREE_H
