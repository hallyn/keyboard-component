#include "settingsmanager.h"

#include <QDir>
#include <QStandardPaths>

namespace Skeyer {

SettingsManager::SettingsManager(QObject *parent) : QObject(parent)
{

}

SettingsManager::~SettingsManager()
{

}

QString SettingsManager::currentLocale() const
{
    return m_settings.value("currentLocale", "en_US").toString();
}

void SettingsManager::setCurrentLocale(const QString &locale)
{
    if( currentLocale() != locale)
    {
        m_settings.setValue("currentLocale", locale);
        emit currentLocaleChanged(locale);
    }
}

bool SettingsManager::autoCapitalize() const
{
    return m_settings.value("autoCapitalize", true).toBool();
}

void SettingsManager::setAutoCapitalize(bool autoCapitalizeEnabled)
{
    if( autoCapitalize() != autoCapitalizeEnabled )
    {
        m_settings.setValue("autoCapitalize", autoCapitalizeEnabled);
        emit autoCapitalizeChanged(autoCapitalizeEnabled);
    }
}

bool SettingsManager::autoCorrect() const
{
    return m_settings.value("autoCorrect", "false").toBool();
}

void SettingsManager::setAutoCorrect(bool autoCorrectEnabled)
{
    if( autoCorrect() != autoCorrectEnabled )
    {
        m_settings.setValue("autoCorrect", autoCorrectEnabled);
        emit autoCorrectChanged(autoCorrectEnabled);
    }
}

QStringList SettingsManager::languagePacks()
{
    QDir d;
    d.cd(SKEYER_DATADIR);
    d.cd("language_packs");
    return d.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
}

QString SettingsManager::languagePackPath(QString locale, QString fileName)
{
    QDir d;
    d.cd(SKEYER_DATADIR);
    d.cd("language_packs");
    d.cd(locale);
    return d.filePath(fileName);
}

QString SettingsManager::userDataPath(QString fileName, QString subDir)
{
    QDir d;
#ifdef Q_OS_ANDROID
    d.mkpath("/mnt/sdcard/Skeyer/");
    d.cd("/mnt/sdcard/Skeyer/");
#else
    d.mkpath(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    d.cd(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
#endif

    if(!subDir.isEmpty())
    {
        d.mkpath(subDir);
        d.cd(subDir);
    }

    if(!fileName.isEmpty())
        return d.filePath(fileName);
    else return d.path();
}

SettingsManager *SettingsManager::instance()
{
    static SettingsManager settings;
    return &settings;
}

}
