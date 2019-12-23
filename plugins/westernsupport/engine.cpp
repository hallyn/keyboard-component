#include "engine.h"
#include "bruteforcematcher.h"
#include "keyboardmodel.h"
#include "settingsmanager.h"
#include <QDebug>

#ifdef ENABLE_HUNSPELL
#include "hunspellmatcher.h"
#endif

namespace Skeyer {

Engine::Engine(QObject *parent):
    QAbstractListModel(parent),
    m_keyboard(new KeyboardModel(parent)),
    m_settings(SettingsManager::instance())
{
#ifdef ENABLE_HUNSPELL
    m_wordMatcher = new HunspellMatcher(this);
#else
    m_wordMatcher = new BruteForceMatcher(this);
#endif
}

QHash<int, QByteArray> Engine::roleNames() const
{
    QHash<int, QByteArray> result;
    result[Qt::UserRole] = "word";
    return result;
}

QVariant Engine::data(const QModelIndex &index, int role) const
{
    if(index.isValid() && role == Qt::UserRole) {
        return m_currentMatches[index.row()];
    }
    return QVariant();
}

int Engine::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_currentMatches.count();
}

QString Engine::currentWord() const
{
    return m_currentWord;
}

void Engine::selectCurrentWord()
{
    if(m_currentWord != "")
        emit wordSelected(m_currentWord, m_currentInput, m_currentInputPath);

    clear();
}

void Engine::beginSwipe(const QString &start)
{
    Q_UNUSED(start)
//    qDebug() << "SwipeStarted:" << start;
    selectCurrentWord();
}

void Engine::endSwipe(const QVariantList &keys, const QVariantList &points)
{

    m_currentInput = "";
    m_currentInputPath.clear();

    for( const auto key : keys) m_currentInput += key.toString();
    for( const auto point : points) m_currentInputPath << point.toPointF();

//    qDebug() << "SwipeEnded: " << m_currentInput;
    updateMatches();
}

void Engine::acceptKeyPress(const QString &keyCode)
{
//    qDebug() << "Key Pressed:" << keyCode;

    if (keyCode == " " || keyCode == "." || keyCode == "!" || keyCode == "?") {
        selectCurrentWord();
    }
    else {
        setCurrentWord(m_currentWord + keyCode);
    }
}

void Engine::selectWord(const QString &word)
{
    qDebug()<<"You selected:"<< word;

    if(word != "")
        emit wordSelected(word, m_currentInput, m_currentInputPath);

    clear();
}

void Engine::setCurrentWord(const QString &word)
{
    if(m_currentWord != word)
    {
        m_currentWord = word;
        currentWordChanged(word);

        m_currentInput = word;
        m_currentInputPath.clear();
        updateMatches();
    }
}

KeyboardModel *Engine::keyboardModel() const
{
    return m_keyboard;
}

void Engine::updateMatches()
{
    QList<QString> matches = m_wordMatcher->match( m_currentInput, m_currentInputPath);

    // If this is a swipe, Use the first match as the current word
    if(!m_currentInputPath.isEmpty())
    {
        if(!matches.isEmpty())
        {
            m_currentWord = matches.takeFirst();
            emit currentWordChanged(m_currentWord);
        }
    }

    // Else the current word is already set, so remove it from the matches
    if(!matches.isEmpty())
        matches.removeOne(m_currentWord);

    //TODO: take care of the capitalization

    beginResetModel();
    m_currentMatches = matches;
    endResetModel();
}

void Engine::clear()
{
    beginResetModel();
    m_currentMatches.clear();
    endResetModel();

    m_currentWord = "";
    m_currentInput = "";
    m_currentInputPath.clear();
    emit currentWordChanged(m_currentWord);
}


void Engine::onLayoutChangerPressed()
{
    if(m_keyboard->layout() == KeyboardModel::AlphabeticLayout)
        m_keyboard->setKeyboardLayout(KeyboardModel::NumericLayout);
    else m_keyboard->setKeyboardLayout(KeyboardModel::AlphabeticLayout);

    if(m_keyboard->layout() == KeyboardModel::NumericLayout)
        m_keyboard->setShiftState(KeyboardModel::ShiftNone);
}

void Engine::onShiftPressed()
{
    switch(m_keyboard->shiftState()) {
        case KeyboardModel::ShiftNone:
            m_keyboard->setShiftState(KeyboardModel::ShiftPressed);
            break;
        case KeyboardModel::ShiftPressed:
            m_keyboard->setShiftState(KeyboardModel::ShiftLocked);
            break;
        case KeyboardModel::ShiftLocked:
            m_keyboard->setShiftState(KeyboardModel::ShiftNone);
            break;
    }

    emit specialKeyPressed(Qt::Key_Shift);
}

void Engine::onEnterPressed()
{

}

void Engine::onBackspacePressed()
{

}

void Engine::acceptSpecialKeyPress(int key)
{
    switch (key)
    {
        case KeyboardModel::LayoutChanger:
            onLayoutChangerPressed();
            break;

        case KeyboardModel::Shift:
            onShiftPressed();
            emit specialKeyPressed(Qt::Key_Shift);
            break;

        case KeyboardModel::Enter:
            onEnterPressed();
            emit specialKeyPressed(Qt::Key_Enter);
            break;

        case KeyboardModel::Backspace:
            onBackspacePressed();
            emit specialKeyPressed(Qt::Key_Backspace);
            break;
    }
}

QList<QString> Engine::match(QString word)
{
    return m_wordMatcher->match(word, QList<QPointF>(), 1);
}

}
