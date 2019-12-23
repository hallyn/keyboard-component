#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>
#include <QFile>

namespace Skeyer {

class SettingsManager : public QObject
{
    Q_OBJECT
public:
    explicit SettingsManager(QObject *parent = 0);
    ~SettingsManager();


    /**
    * @brief Returns the current corpus locale and keyboard used by the UI.
    *        It is of the format <language>_<country code>
    *        eg. en_US
    *
    * @return QString
    */
    Q_PROPERTY(QString currentLocale
               READ currentLocale
               WRITE setCurrentLocale
               NOTIFY currentLocaleChanged)
    QString currentLocale() const;
    void setCurrentLocale(const QString& locale);

    /**
     * @brief Whether or not to automatically capitalize words.
     * @return
     */
    Q_PROPERTY(bool autoCapitalize
               READ autoCapitalize
               WRITE setAutoCapitalize
               NOTIFY autoCapitalizeChanged)

    bool autoCapitalize() const;
    void setAutoCapitalize(bool autoCapitalizeEnabled);

    /**
     * @brief Whether or not to automatically correct the typed words.
     * */
    Q_PROPERTY(bool autoCorrect
               READ autoCorrect
               WRITE setAutoCorrect
               NOTIFY autoCorrectChanged)

    bool autoCorrect() const;
    void setAutoCorrect( bool autoCorrectEnabled );

    static QStringList languagePacks();
    static QString languagePackPath(QString locale, QString fileName);
    static QString userDataPath(QString fileName = "", QString subDir = "");
    static SettingsManager* instance();

signals:
    void currentLocaleChanged(const QString& locale);
    void autoCapitalizeChanged(bool autoCapitalize);
    void autoCorrectChanged(bool autoCorrectEnabled);

public slots:

private:
    QSettings m_settings;
};

}

#endif // SETTINGSMANAGER_H
