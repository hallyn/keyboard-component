#ifndef KEYBOARDMODEL_H
#define KEYBOARDMODEL_H

#include <QObject>
#include <QJsonDocument>
#include <QVariant>
#include <QPointF>
#include <QSize>
#include <QStringList>

namespace Skeyer {

/**
 * @brief The KeyboardModel class:
 * Assumptions: keys are unit sized, and each row is center aligned in a grid.
 * Space key is a "special key", whose width can be specified. For obvious reasons.
 */
class KeyboardModel : public QObject
{
    Q_OBJECT

    Q_ENUMS(ShiftState)
    Q_ENUMS(KeyModifiers)
    Q_ENUMS(KeyboardLayouts)
    Q_ENUMS(SpecialKeys)

    // Convenience property to make the QML bindings work like
    // keyboardModel.currentGrid.keyModelAt(row,column)
    Q_PROPERTY(KeyboardModel* currentGrid READ currentGrid NOTIFY layoutChanged)

    Q_PROPERTY(bool swipeable READ swipeable NOTIFY layoutChanged)

    Q_PROPERTY(KeyModifiers keyModifier READ keyModifier NOTIFY keyModifierChanged)
    Q_PROPERTY(KeyboardLayouts layout READ layout  NOTIFY layoutChanged)
    Q_PROPERTY(int rows READ rows NOTIFY layoutChanged)
    Q_PROPERTY(int columns READ columns NOTIFY layoutChanged)

    Q_PROPERTY(ShiftState shiftState READ shiftState NOTIFY shiftStateChanged)

    Q_PROPERTY(int spaceKeyWidth READ spaceKeyWidth CONSTANT)

    Q_PROPERTY(QVariant rawData READ rawData CONSTANT)

public:
    explicit KeyboardModel(QObject *parent = 0);
    ~KeyboardModel();

    enum ShiftState {
        ShiftNone,
        ShiftPressed,
        ShiftLocked
    };

    enum KeyModifiers {
        DefaultModifier = 0,
        ShiftModifier,
        CurrentModifier
    };

    enum KeyboardLayouts {
        AlphabeticLayout = 0,
        NumericLayout,
        CurrentLayout
    };

    enum SpecialKeys {
        Shift = Qt::Key_Shift,
        Enter = Qt::Key_Enter,
        Backspace = Qt::Key_Backspace,
        LayoutChanger
    };

    KeyboardModel* currentGrid() { return this; }


    ShiftState shiftState() const;
    void setShiftState(ShiftState mode);

    KeyboardLayouts layout() const { return m_currentLayout; }
    KeyModifiers keyModifier() const { return m_currentModifier; }

    /**
     * @brief whether or not the current layout is swipeable
     * @return
     */
    bool swipeable() const;

    /**
     * @brief returns the width of the space key in terms of number of gridCells.
     * @return
     */
    int spaceKeyWidth() const;

    /**
     * @brief returns the raw JSON of the currently loaded keyboardlayout
     * @return
     */
    QVariant rawData() const;

    /**
     * @brief Number of Rows in the current keyGrid
     * @param modifier
     * @param layout
     * @return
     */
    int rows(KeyboardLayouts layout = CurrentLayout) const;

    /**
     * @brief Number of Columns in the current keyGrid
     * @param modifier
     * @param layout
     * @return
     */
    int columns(KeyboardLayouts layout = CurrentLayout) const;

    /**
     * @brief Number of Keys in the given row of the current current layout
     * @param row
     * @param layout
     * @return
     */
    Q_INVOKABLE int keyCount(int rowIndex, KeyboardLayouts layout = CurrentLayout) const;

    /**
     * @brief keyWidth - returns the width of key at (row, column).
     *        keyWidth for space = 4. For the rest it is 1
     * @param row
     * @param column
     * @param layout
     * @return
     */
    Q_INVOKABLE int keyWidth(int row, int column, KeyboardLayouts layout = CurrentLayout) const;

    /**
     * @brief rowWidth - returns the total width of all the keys in this row
     * @param row
     * @param column
     * @param layout
     * @return
     */
    Q_INVOKABLE int rowWidth(int row, KeyboardLayouts layout = CurrentLayout) const;

    /**
     * @brief keyModelAt - list of all keyCodes to be shown in the given key
     * @param row
     * @param column
     * @param modifier
     * @param layout
     * @return
     */
    Q_INVOKABLE QStringList keyModelAt(int row, int column,
                                       KeyModifiers keyModifier = CurrentModifier,
                                       KeyboardLayouts layout = CurrentLayout) const;

    /**
     * @brief keyAt - returns the keyCode at the given indice of the row/column of the layout
     * @param row
     * @param column
     * @param index - returns the first key of the keyModel if unspecified
     * @param modifier
     * @param layout
     * @return
     */
    Q_INVOKABLE QString keyCodeAt(int row, int column,
                                  int index = 0,
                                  KeyModifiers keyModifier = CurrentModifier,
                                  KeyboardLayouts layout = CurrentLayout) const;

    /**
     * @brief keyAtPoint - returns the keyCode at the given indice of the row/column of the keyGrid
     * @param x
     * @param y
     * @param index - returns the first key of the keyModel if unspecified
     * @param modifier
     * @param layout
     * @return
     */
    Q_INVOKABLE QString keyCodeAtPoint(double x,double y,
                                       int index = 0,
                                       KeyModifiers keyModifier = CurrentModifier,
                                       KeyboardLayouts layout = CurrentLayout) const;

    /**
     * @brief keyCodeLocation returns the location of a keyCode in the specified layout
     * @param keyCode
     * @param layout - if the specified layout is not found, it returns it from wherever found.
     * @return returns QPointF(-1,-1) if not found
     */
    QPointF keyCodeLocation(QString keyCode, KeyboardLayouts layout = CurrentLayout ) const;


    /**
     * @brief keyCodesBetween find the keyCodes between startKeyCode and endKeyCode, excluding the endpoints.
     * @param startKeyCode
     * @param endKeyCode
     * @param modifier - if not specified, returns the result keyCodes using the DefaultModifiers.
     * @param layout - if not specified, assumes the keyCodes supplied are in the AlphabeticLayout
     * @return returns an empty string on failure
     */
    QString keyCodesBetween(QString start,QString end,
                            KeyModifiers keyModifier = DefaultModifier,
                            KeyboardLayouts layout = AlphabeticLayout) const;


    /**
     * @brief neighbours
     * @param keyCode
     * @param modifier- returns the neighbour keyCodes from using this KeyModifier
     * @param layout - prefers to look for the keyCode in the specified layout. Else returns from first found layout
     *
     * @return
     */
    QString neighbours(QString keyCode,
                       KeyModifiers keyModifier = DefaultModifier,
                       KeyboardLayouts layout = AlphabeticLayout) const;

signals:
    void shiftStateChanged(ShiftState shiftState);

    void keyboardChanged(const QString& locale);
    void layoutChanged(KeyboardLayouts layout);
    void keyModifierChanged(KeyModifiers keyModifier);


public slots:
    bool loadLayout(const QString &locale);

    void setKeyboardLayout(KeyboardLayouts layout);
    void setKeyModifier(KeyModifiers keyModifier);

private:
    typedef QMap<KeyModifiers, QStringList> Key;
    typedef QList<Key> KeyRow;
    typedef QList<KeyRow> KeyGrid; // Essentially a layout to be rendered for us
    typedef QMap<KeyboardLayouts, KeyGrid> Keyboard; // Various available layouts - such as alphabetical, symbolic, etc...

    /**
     * @brief keyAt - returns the current keyModel at any given row/column of the keyGrid
     * @param row
     * @param column
     * @param layout
     * @return
     */
    Key keyAt(int row, int column, KeyboardLayouts layout = CurrentLayout) const;

    /**
     * @brief keyAtPoint - returns the current KeyModel at a given point in the cartesian plane.
     * Assumes the keys are horizontally aligned and also takes the dimensions of the space key into account.
     *
     * @param x
     * @param y
     * @param layout
     * @return
     */
    Key keyAtPoint(double x, double y, KeyboardLayouts layout = CurrentLayout) const;

    void parseKeyboard();
    void updateGeometry();

private:

    struct KeyDetails{
        int row, column, index;
        double x, y;
        KeyModifiers keyModifier;
        KeyboardLayouts layout;
        KeyDetails(int r = -1, int c = -1, int i = -1,
                   double px = -1, double py = -1,
                   KeyModifiers m = DefaultModifier, KeyboardLayouts l = AlphabeticLayout):
                    row(r), column(c), index(i), x(px), y(py),
                    keyModifier(m), layout(l)
        {}
    };

    typedef struct KeyDetails KeyDetails;

    inline KeyboardLayouts layoutId(KeyboardLayouts layout) const;
    inline KeyModifiers modifierId(KeyModifiers keyModifier) const;

    QJsonDocument m_rawData;
    Keyboard m_data;

    const int m_spaceKeyWidth;

    KeyboardLayouts m_currentLayout;
    KeyModifiers m_currentModifier;

    ShiftState m_shiftState;

    QMap< QString, KeyModifiers > m_modifierNames;
    QMap< QString, KeyboardLayouts > m_layoutNames;

    QMap< KeyboardLayouts, QSize > m_layoutDimensions;
    QMap< KeyboardLayouts, QList<int> > m_rowWidths;

    QMultiMap< QString, KeyDetails > m_keyCodeDetails;
};

}

#endif // KEYBOARDMODEL_H
