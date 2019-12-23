#ifndef ENGINE_H
#define ENGINE_H

#include <QAbstractListModel>
#include <QPointF>

namespace Skeyer {

class AbstractWordMatcher;
class KeyboardModel;
class SettingsManager;

/**
 * @brief The Engine class, which currently collects the input from the UI and processes it to:
 * 1) Generates word suggestions for the user
 * 2) Find out when a new word has been selected by the user to send it to the backend
 *
 */
class Engine : public QAbstractListModel
{
    Q_OBJECT

    // The currentWord is the word being typed or the first word from the list of matches for a swipe
    Q_PROPERTY(QString currentWord READ currentWord WRITE setCurrentWord NOTIFY currentWordChanged)

public:
    explicit Engine(QObject* parent = 0);

    // Pretty much standard stuff while deriving from the QAbstractListModel
    QHash<int, QByteArray> roleNames() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /**
     * @brief currentWord returns the currentWord being edited by the user
     * TODO: Add a setting to enable/disable editing the words before they are comitted for a keyPress.
     * This stuff is layout dependent too.
     * @return
     */
    QString currentWord() const;

    /**
     * @brief keyboardModel - instance of the current KeyboardModel being used
     * @return
     */
    KeyboardModel* keyboardModel() const;

signals:
    void currentWordChanged(const QString word);
    void wordSelected(const QString& word, const QString &input, const QList<QPointF> inputPath);
    void specialKeyPressed(Qt::Key key);

public slots:

    /**
     * @brief This slot lets the engine know when a swipe starts.
     * This can be useful for the engine to
     * 1) Automatically add Spaces, Punctuation etc...
     * 2) Do the necessary preprocessing for this swipe ( currently NADA )
     */
    void beginSwipe(const QString &start);

    /**
     * @brief This slot lets the engine know when a swipe ends.
     * We can then updateMatches() based on currentInput and currentInputPath
     */
    void endSwipe(const QVariantList &keys, const QVariantList &points);

    /**
     * @brief If the keyCode pressed isn't a space/punctuation then it is added to the
     * currentWord and the matches are updated.
     *
     * @param keyCode
     * @return bool
     */
    void acceptKeyPress(const QString &keyCode);

    /**
     * @brief acceptSpecialKeyPress
     * @param key
     */
    void acceptSpecialKeyPress(int key);

    /**
     * @brief selects current word
     */
    void selectCurrentWord();

    /**
     * @brief selects the word - mainly called from the UI.
     *
     */
    void selectWord(const QString& word);

    /**
     * @brief setCurrentWord
     * @param word
     */
    void setCurrentWord(const QString& word);

    QList<QString> match(QString word);

private:

    void updateMatches();
    void clear();

    void onLayoutChangerPressed();
    void onEnterPressed();
    void onShiftPressed();
    void onBackspacePressed();

    KeyboardModel* m_keyboard;
    AbstractWordMatcher* m_wordMatcher;
    SettingsManager* m_settings;

    QList<QString> m_currentMatches;
    QString m_currentWord;

    // For generating matches for the current word and to analyze things later on
    QString m_currentInput;
    QList<QPointF> m_currentInputPath; // If this is empty, it means the user didn't swipe this word.
};

}

#endif // ENGINE_H
