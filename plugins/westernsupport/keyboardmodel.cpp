#include "keyboardmodel.h"

#include "settingsmanager.h"

#include <QFile>
#include <QByteArray>
#include <QDebug>
#include <qmath.h>

namespace Skeyer {

KeyboardModel::KeyboardModel(QObject *parent):
    QObject(parent),
    m_spaceKeyWidth(4),
    m_currentLayout(AlphabeticLayout),
    m_currentModifier(ShiftModifier),
    m_shiftState(ShiftPressed)
{
    m_layoutNames["alphabetic"] = AlphabeticLayout;
    m_layoutNames["numeric"] = NumericLayout;

    m_modifierNames["default"] = DefaultModifier;
    m_modifierNames["shifted"] = ShiftModifier;

    connect(SettingsManager::instance(), SIGNAL(currentLocaleChanged(QString)),
            this, SLOT(loadLayout(const QString &)));

    loadLayout(SettingsManager::instance()->currentLocale());
}

KeyboardModel::~KeyboardModel()
{

}

int KeyboardModel::spaceKeyWidth() const
{
    return m_spaceKeyWidth;
}

QVariant KeyboardModel::rawData() const
{
    return m_rawData.toVariant();
}

int KeyboardModel::rows(KeyboardLayouts layout) const
{
    return m_layoutDimensions[layoutId(layout)].height();
}

int KeyboardModel::columns(KeyboardLayouts layout) const
{
    return m_layoutDimensions[layoutId(layout)].width();
}

int KeyboardModel::keyCount(int rowIndex, KeyboardModel::KeyboardLayouts layout) const
{
    const KeyGrid& grid = m_data[layoutId(layout)];
    return rowIndex >= 0 && rowIndex < grid.count()? grid[rowIndex].count() : 0;
}

int KeyboardModel::keyWidth(int row, int column, KeyboardModel::KeyboardLayouts layout) const
{
    return keyCodeAt(row, column, 0, DefaultModifier, layout) == " "? m_spaceKeyWidth : 1;
}

int KeyboardModel::rowWidth(int row, KeyboardModel::KeyboardLayouts layout) const
{
    layout = layoutId(layout);

    if(row < m_rowWidths[layout].count())
        return m_rowWidths[layout][row];

    return 0;
}

KeyboardModel::Key KeyboardModel::keyAt(int row, int column, KeyboardModel::KeyboardLayouts layout) const
{
    const KeyGrid& grid = m_data[layoutId(layout)];
    if(row < grid.count() && column < grid[row].count())
        return grid[row][column];

    return Key();
}

QStringList KeyboardModel::keyModelAt(int row, int column, KeyboardModel::KeyModifiers modifier, KeyboardModel::KeyboardLayouts layout) const
{
    return keyAt(row, column, layout)[modifierId(modifier)];
}

QString KeyboardModel::keyCodeAt(int row, int column, int index,
                                 KeyboardModel::KeyModifiers modifier,
                                 KeyboardModel::KeyboardLayouts layout) const
{
    QStringList keyModel = keyAt(row, column, layout)[modifierId(modifier)];
    return index >= 0 && index < keyModel.count() ? keyModel[index] : "";
}

KeyboardModel::Key KeyboardModel::keyAtPoint(double x, double y, KeyboardModel::KeyboardLayouts layout) const
{
    int r = qFloor(y);
    if( r < 0 || r >= rows(layout) )
        return Key();

    layout = layoutId(layout);

    const KeyRow row = m_data[layout][r];
    // Compensating for initial padding for horizontal alignment
    double initialPadding = (double)(m_layoutDimensions[layout].width() - m_rowWidths[layout][r])/2.0;

    if(x < initialPadding)
        return Key();

    x -= initialPadding;

    for(int i = 0; i < row.count(); i++)
    {
        double w = keyWidth(r, i, layout);
        if( x <= w )
            return row[i];

        x -= w;
    }

    return Key();
}

QString KeyboardModel::keyCodeAtPoint(double x, double y, int index,
                                      KeyboardModel::KeyModifiers modifier,
                                      KeyboardModel::KeyboardLayouts layout) const
{
    const Key k = keyAtPoint( x, y, layout );
    if(index >= 0 && index < k.size())
        return k[modifierId(modifier)][index];
    return "";
}

QPointF KeyboardModel::keyCodeLocation(QString keyCode, KeyboardModel::KeyboardLayouts layout) const
{
    if(!m_keyCodeDetails.contains(keyCode))
        return QPointF(-1,1);

    KeyDetails p0;
    const auto possibleKeys = m_keyCodeDetails.values(keyCode);

    for(const auto keyCodeDetails: possibleKeys)
        if(keyCodeDetails.layout == layout)
        {
            p0 = keyCodeDetails;
            break;
        }

    if(p0.x == -1)
        p0 = possibleKeys.first();

    return QPointF(p0.x, p0.y);
}

QString KeyboardModel::keyCodesBetween(QString startKeyCode, QString endKeyCode,
                                       KeyModifiers modifier,
                                       KeyboardModel::KeyboardLayouts layout) const
{
    QString result;

    if(!m_keyCodeDetails.contains(startKeyCode) || !m_keyCodeDetails.contains(endKeyCode))
        return result;

    KeyDetails p0;
    KeyDetails p1;
    for(const auto keyCodeDetails: m_keyCodeDetails.values(startKeyCode))
        if(keyCodeDetails.layout == layout)
        {
            p0 = keyCodeDetails;
            break;
        }

    for(const auto keyCodeDetails: m_keyCodeDetails.values(endKeyCode))
        if(keyCodeDetails.layout == layout)
        {
            p1 = keyCodeDetails;
            break;
        }

    // i.e You can't try to find keys between alphabetic keyboard and numeric keyboard
    // or shifted and default layouts.
    if(p0.x == -1 || p1.x == -1)
        return result;

    QString firstKeyCode = keyCodeAt(p0.row, p0.column, 0, modifier, p0.layout);
    QString lastKeyCode = keyCodeAt(p1.row, p1.column, 0, modifier, p1.layout);

    double x0 = p0.x, y0 = p0.y, x1 = p1.x, y1 = p1.y;

    if(x0 == x1)
    {
        double step = y1 > y0? 0.25 : - 0.25;
        int steps = (y1 - y0)/step;

        for(int i = 0; i < steps; i++)
        {
            double y = y0 + step*i;
            const QString currentKeyCode = keyCodeAtPoint(x0, y, 0, modifier, layout);

            if( result.lastIndexOf(currentKeyCode) == -1 && currentKeyCode != startKeyCode && currentKeyCode != endKeyCode)
                result += currentKeyCode;

        }
    }
    else
    {
        double m = (y1 - y0)/(x1 - x0);
        double c = y0 - m*x0;

        //TODO: look into computing the best possible stepSize for a given layout
        double step = x1 > x0? 0.25 : -0.25;
        int steps = (x1 - x0)/step;

        for(int i = 0; i < steps; i++)
        {
            double x = x0 + step*i;
            double y = m*x + c;
            const QString currentKeyCode = keyCodeAtPoint(x, y, 0, modifier, layout);

            if( result.lastIndexOf( currentKeyCode ) == -1 && currentKeyCode != firstKeyCode && currentKeyCode != lastKeyCode)
                result += currentKeyCode;
        }
    }

    return result;

}

QString KeyboardModel::neighbours(QString keyCode,
                                  KeyboardModel::KeyModifiers modifier,
                                  KeyboardModel::KeyboardLayouts layout) const
{
    QString result;

    QPointF p0 = keyCodeLocation(keyCode, layout);

    for(int i = -1; i <= 1; i++)
    {
        for(int j = -1; j <= 1; j++)
        {
            QString currentKeyCode = keyCodeAtPoint( p0.x() + i, p0.y() +j, 0, modifier, layout);
            if(result.lastIndexOf(currentKeyCode) == -1 && currentKeyCode != keyCode)
                result += currentKeyCode;
        }
    }
    return result;
}


bool KeyboardModel::loadLayout(const QString &locale)
{
    QString keyboardFilePath = SettingsManager::languagePackPath(locale,"keyboard_layout.json");
    QFile keyboardFile(keyboardFilePath);

    if ( keyboardFile.open(QFile::ReadOnly) ) {
        QByteArray keyboardData = keyboardFile.readAll();
        m_rawData = QJsonDocument::fromJson(keyboardData);
        parseKeyboard();
        updateGeometry();
        setKeyboardLayout(AlphabeticLayout);
        emit keyboardChanged(locale);
        return true;
    }

    else qDebug()<<"Cant open" << keyboardFilePath;
    return false;
}

void KeyboardModel::setKeyboardLayout(KeyboardModel::KeyboardLayouts layout)
{
    if(m_currentLayout != layout)
    {       
        m_currentLayout = layout;
        emit layoutChanged(m_currentLayout);
    }
}

void KeyboardModel::setKeyModifier(KeyboardModel::KeyModifiers modifier)
{
    if(m_currentModifier != modifier)
    {
        m_currentModifier = modifier;
        emit keyModifierChanged(m_currentModifier);
    }
}

void KeyboardModel::parseKeyboard()
{
    const QVariantMap& keyboardMap = m_rawData.toVariant().toMap();

    m_data.clear();

    for(const auto layoutName : m_layoutNames.keys()){
        KeyboardLayouts layoutId = m_layoutNames[layoutName];
        if(!keyboardMap.contains(layoutName))
        {
            m_data[layoutId] = KeyGrid();
            qDebug()<< layoutName << "is not a currently supported layout. Skipping...";
            continue;
        }


        QVariantList gridData= keyboardMap[layoutName].toList();
        KeyGrid grid;

        for(const auto currentRow : gridData)
        {
            QVariantList rowData = currentRow.toList();
            KeyRow row;

            for(const auto currentKey : rowData)
            {
                QVariantMap keyData = currentKey.toMap();
                Key key;

                for(const auto modifier : m_modifierNames.keys())
                {
                    if(keyData.contains(modifier))
                        key[m_modifierNames[modifier]] = keyData[modifier].toStringList();
                    else key[m_modifierNames[modifier]] = QStringList();

                }

                row << key;
            }

            grid << row;
        }

        m_data[layoutId] = grid;
    }
}

void KeyboardModel::updateGeometry()
{
    m_rowWidths.clear();
    m_layoutDimensions.clear();

    for(const auto layoutId: m_data.keys())
    {
        const auto layout = m_data[layoutId];
        int layoutHeight = layout.count();
        int layoutWidth = 0;

        //First update each row's dimensions - mainly width
        for(int r = 0; r < layoutHeight; r++)
        {
            const auto row = layout[r];
            double rw = 0;

            for(int c = 0; c < row.length(); c++ )
                rw += keyWidth(r,c, layoutId);

            if(layoutWidth < rw)
                layoutWidth = rw;

            m_rowWidths[layoutId] << rw;
        }


        //Now insert the details for each key...
        for(int r = 0; r < layoutHeight; r++)
        {
            const auto row = layout[r];

            //For horizontally aligning each row at the center
            double initialPadding = (double)(layoutWidth - m_rowWidths[layoutId][r])/2;

            for(int c = 0; c < row.length(); c++)
            {

                Key key = keyAt(r, c, layoutId);
                double kw = keyWidth(r,c, layoutId);

                for(const auto modifier : m_modifierNames.values())
                {
                    for(int i = 0; i < key[modifier].count(); i++ )
                        m_keyCodeDetails.insert( key[modifier][i], KeyDetails(r, c, i, initialPadding+kw/2, r + 0.5 , modifier, layoutId) );
                }

//                qDebug() << key[DefaultModifier][0] << r << c << initialPadding + kw/2.0 << r + 0.5 ;
                initialPadding += kw;
            }
        }

        m_layoutDimensions[layoutId] = QSize(layoutWidth, layoutHeight);
    }
}

KeyboardModel::KeyboardLayouts KeyboardModel::layoutId(KeyboardModel::KeyboardLayouts layout) const
{
    if(layout == CurrentLayout) return m_currentLayout;
    return layout;
}

KeyboardModel::KeyModifiers KeyboardModel::modifierId(KeyboardModel::KeyModifiers modifier) const
{
    if(modifier == CurrentModifier) return m_currentModifier;
    return modifier;
}


KeyboardModel::ShiftState KeyboardModel::shiftState() const
{
    return m_shiftState;
}

void KeyboardModel::setShiftState(KeyboardModel::ShiftState mode)
{
    if(m_shiftState != mode)
    {
        m_shiftState = mode;
        emit shiftStateChanged(m_shiftState);

        if(mode == ShiftPressed || mode == ShiftLocked)
            setKeyModifier(ShiftModifier);
        else setKeyModifier(DefaultModifier);
    }
}

bool KeyboardModel::swipeable() const
{
    return m_currentLayout == AlphabeticLayout;
}

}
