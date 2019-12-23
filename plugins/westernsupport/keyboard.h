#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QObject>
#include <QJsonDocument>
#include <QHash>

class Keyboard : public QObject
{
    Q_OBJECT
public:
    explicit Keyboard(QObject *parent = 0);
    ~Keyboard();

    const QJsonDocument &getJson() const;
    bool load(QString locale);

    QChar getKeyShifted(const QChar key) const;

signals:

public slots:
private:
    QJsonDocument m_jsonDocument;
    QHash<QChar, QChar> m_shiftMappings;
};

#endif // KEYBOARD_H
