/*
 * Copyright 2013 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.4
import keys 1.0

KeyPad {
    anchors.fill: parent
    anchors.bottomMargin: 0

    Repeater {
        id: generalKeys
        anchors.fill: parent
        model: maliit_input_method.keyboardModel.rows - 1 // Handle bottom row separately

        ListView {
            id: row

            property int rowIndex: index

            height: panel.keyHeight
            width: panel.keyWidth * maliit_input_method.keyboardModel.currentGrid.rowWidth(rowIndex)

            anchors.horizontalCenter: parent.horizontalCenter
            y: rowIndex * panel.keyHeight

            model: maliit_input_method.keyboardModel.currentGrid.keyCount(rowIndex)

            onModelChanged: {
                calculateKeyWidth();
                calculateKeyHeight();
            }

            orientation: ListView.Horizontal
            interactive: false

            delegate: CharKey {
                property var keyModel: maliit_input_method.keyboardModel.keyModelAt(rowIndex, index, 0, maliit_input_method.keyboardModel.layout)
                property var keyModelShifted: maliit_input_method.keyboardModel.keyModelAt(rowIndex, index, 1, maliit_input_method.keyboardModel.layout)
                label: keyModel[0]
                shifted: keyModelShifted[0]
                extended: keyModel.length > 1 ? keyModel.slice(1) : null
                extendedShifted: keyModelShifted.length > 1 ? keyModelShifted.slice(1) : null
            }
        }

    }

    ShiftKey {
        visible: !oneTwo.visible
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: panel.keyHeight * 2
    }

    OneTwoKey { 
        id: oneTwo
        visible: maliit_input_method.keyboardModel.layout == 1
        label: "1/2";
        shifted: "2/2";
        fontSize: units.gu(UI.fontSize);
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: panel.keyHeight * 2
    }

    BackspaceKey {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: panel.keyHeight * 2
    }

    Item {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.topMargin: units.gu(UI.row_margin)

        height: panel.keyHeight

        SymbolShiftKey { id: symShiftKey;                            anchors.left: parent.left; height: parent.height; }

        LanguageKey    { id: languageMenuButton;                     anchors.left: symShiftKey.right; height: parent.height; }

        CharKey { 
            id: commaKey;
            property var keyModel: maliit_input_method.keyboardModel.keyModelAt(3, 0, 0, maliit_input_method.keyboardModel.layout)
            property var keyModelShifted: maliit_input_method.keyboardModel.keyModelAt(3, 0, 1, maliit_input_method.keyboardModel.layout)
            label: keyModel[0]
            shifted: keyModelShifted[0]
            extended: keyModel.length > 1 ? keyModel.slice(1) : null
            extendedShifted: keyModelShifted.length > 1 ? keyModelShifted.slice(1) : null
            anchors.left: languageMenuButton.right;
            height: parent.height;
        }

        SpaceKey { id: spaceKey;                               anchors.left: commaKey.right; anchors.right: extraKeys.left; noMagnifier: true; height: parent.height; }

        ListView {
            id: extraKeys
            anchors.right: enterKey.left

            height: parent.height
            width: contentWidth

            model: maliit_input_method.keyboardModel.currentGrid.keyCount(3) - 1 // Display first key before space bar

            orientation: ListView.Horizontal
            interactive: false

            delegate: CharKey {
                property var keyModel: maliit_input_method.keyboardModel.keyModelAt(3, index + 1, 0, maliit_input_method.keyboardModel.layout)
                property var keyModelShifted: maliit_input_method.keyboardModel.keyModelAt(3, index + 1, 1, maliit_input_method.keyboardModel.layout)
                label: keyModel[0]
                shifted: keyModelShifted[0]
                extended: keyModel.length > 1 ? keyModel.slice(1) : null
                extendedShifted: keyModelShifted.length > 1 ? keyModelShifted.slice(1) : null
                height: parent.height
            }
        }

        ReturnKey      { id: enterKey;                               anchors.right: parent.right; height: parent.height; }
    }
}
