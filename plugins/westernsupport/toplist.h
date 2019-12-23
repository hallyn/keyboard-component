#ifndef TOPLIST_H
#define TOPLIST_H

#include <QList>

namespace Skeyer {

template <class K, class V>
class TopList{
    class Node {
    public:
        K key;
        V value;
        Node(K k, V v): key(k), value(v) {}
        inline bool operator< (const Node& other) const { return value < other.value; }
        inline bool operator> (const Node& other) const { return value > other.value; }
        inline bool operator== (const Node& other) const { return key == other.key; }
    };

    int m_maxSize;

    //Sorted list of nodes. Smallest first.
    QList<Node> m_data;

public:
    TopList(int maxSize): m_maxSize(maxSize){}

    bool add(K k, V v)
    {
        Node current{k,v};

        int i = 0;

        //Insert it at the first position if possible
        if( m_data.count() < m_maxSize ) m_data.insert( i, current );
        else if( m_data.first() < current ) m_data.replace( i, current );
        else return false;

        //And bubble it up to it's position
        while( i + 1 < m_data.count() && m_data[i] > m_data[i+1] )
        {
            qSwap(m_data[i], m_data[i+1]);
            i++;
        }

        return true;
    }

    QList<K> get()
    {
        QList<K> result;

        // Sort the values in reverse order
        qSort(m_data.begin(), m_data.end(),  qGreater<Node>());

        for(auto node : m_data)
            result.append(node.key);

        return result;
    }

    inline int count()
    {
        return m_data.count();
    }

    inline V minValue()
    {
        return m_data.count() > 0? m_data.first().value : V(0);
    }
};

}

#endif // TOPLIST_H
