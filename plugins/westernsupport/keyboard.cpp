#include "keyboard.h"
#include <QFile>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>

Keyboard::Keyboard(QObject *parent) : QObject(parent)
{
}

Keyboard::~Keyboard()
{
}

const QJsonDocument &Keyboard::getJson() const
{
    return m_jsonDocument;
}

bool Keyboard::load(QString locale)
{
    QString keyboardFileName = QString("%1/language_packs/%2/keyboard_layout.json").arg(SKEYER_DATADIR).arg(locale);
    QFile keyboardFile(keyboardFileName);

    if (!keyboardFile.open(QFile::ReadOnly) ) {
        qWarning() << "unable to open keyboard file:" << keyboardFileName;
        return false;
    }

    QByteArray keyboardData = keyboardFile.readAll();
    m_jsonDocument = QJsonDocument::fromJson(keyboardData);

    const QJsonArray rows = m_jsonDocument.object()["alphabetical"].toArray();
    foreach(const QJsonValue &rowValue, rows) {
        const QJsonArray row = rowValue.toArray();
        foreach(const QJsonValue &keyValue, row) {
            const QJsonArray unshiftedKeys = keyValue.toArray()[0].toArray();
            const QJsonArray shiftedKeys = keyValue.toArray()[1].toArray();

            for (int i=0; i<unshiftedKeys.size(); i++) {
                if (shiftedKeys.size() < i) {
                    break;
                }

                const QString unshifted = unshiftedKeys[i].toString();
                const QString shifted = shiftedKeys[i].toString();

                if (unshifted.isEmpty() || shifted.isEmpty()) {
                    continue;
                }

                m_shiftMappings[unshifted[0]] = shifted[0];
            }
        }
    }

    return true;
}

QChar Keyboard::getKeyShifted(const QChar key) const
{
    if (m_shiftMappings.contains(key)) {
        return m_shiftMappings[key];
    } else {
        // Maybe use QChar::toUpper() instead?
        return key;
    }
}
