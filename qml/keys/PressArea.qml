/*
 * Copyright 2013 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.4
import eu.cpbm.okboard 1.0

/*!
  MultiPointTouchArea is similar to the MouseArea
  But to enable multiple PressAreas to be touched at the same time, this is based
  on MultiPointTouchArea
 */
MultiPointTouchArea {
    id: root

    /// Is true while the area is touched, and the finger did not yet lift
    property bool pressed: false
    // Track whether we've swiped out of a key press to dismiss the keyboard
    property bool swipedOut: false
    property bool hanging: false
    property bool horizontalSwipe: false
    property bool held: false
    property alias mouseX: point.x
    property alias mouseY: point.y
    property var firstX: 0
    property var firstY: 0
    // Keep track of the touch start position ourselves instead of using
    // point.startY, as this always reports 0 for mouse interaction 
    // (https://bugreports.qt.io/browse/QTBUG-41692)
    property real startY

    property bool acceptDoubleClick: false
    maximumTouchPoints: 1

    /// Same as MouseArea pressAndHold()
    signal pressAndHold()

    signal doubleClicked()

    /// Cancels the current pressed state of the mouse are
    function cancelPress() {
        pressed = false;
        holdTimer.stop();
    }

    property var keys_ok: false
    property var keyMap: []
    function walkKeyChildren(obj) {
        for (var i = 0; i < obj.children.length; i++) {
            var child = obj.children[i];
            if (child.label && child.label.length == 1) {
                var p = child.mapToItem(null, child.x, child.y, child.width, child.height)
                var keyItem = new Object;
                keyItem.x = p.x;
                keyItem.y = p.y;
                console.warn("adding key " + child.label + " at point " + p.x + ", " + p.y)
                keyItem.width = p.width;
                keyItem.height = p.height;
                keyItem.caption = child.label;
                root.keyMap.push(keyItem);
            } else {
                walkKeyChildren(child);
            }
        }
    }

    function buildKeyMap() {
        if (!root.keys_ok && root.keyMap.length == 0) {
            walkKeyChildren(panel);
            console.warn("calling curveimpl.loadkeys with " + root.keyMap)
            curveimpl.loadKeys(root.keyMap);
            console.warn("curveimpl.loadkeys returned")
            root.keys_ok = true;
        }

    }

    CurveKB {
        id: curveimpl
        onMatchingDone: { matching_done(candidates); }
    }

    function addPoint(obj, x, y) {
        var p = obj.parent.mapToItem(null, x, y)
        curveimpl.addPoint(p.x, p.y, panel.curveIndex)
    }

    function setupCurve() {
        curveimpl.setLogFile("/tmp/curve.log");
        curveimpl.setDebug(true);
        console.warn("setting pixelsize to: " + fullScreenItem.width + "," + fullScreenItem.height)
        curveimpl.setScreenSizePixels(fullScreenItem.width, fullScreenItem.height)
        panel.curve_ok = curveimpl.loadTree("/usr/share/okboard/en.tre");
        console.warn("done loading tree")
    }

    touchPoints: [
        TouchPoint { 
            id: point

            onXChanged: {
                if (root.swipedOut) {
                    root.addPoint(root, point.x, point.y)
                    return
                }

                if (root.hanging)
                    return;

                if (point.x < root.x || point.x > root.x + root.width) {
                    if (!root.swipedOut) {
                        // We've swiped out of the key
                        root.swipedOut = true;
                        if (!panel.curve_ok) {
                            setupCurve()
                        }
                        if (root.keyMap.length == 0) {
                            console.warn("building map")
                            buildKeyMap()
                            console.warn("done building map")
                        }

                        curveimpl.resetCurve()
                        panel.curveIndex++;
                        curveimpl.startCurve()
                        root.addPoint(root, firstX, firstY);
                        cancelPress();
                    }

                    root.addPoint(root, point.x, point.y)
                }
            }

            // Dragging implemented here rather than in higher level
            // mouse area to avoid conflict with swipe selection
            // of extended keys
            onYChanged: {
                if (root.swipedOut) {
                    root.addPoint(root, point.x, point.y)
                    return
                }

                if (root.hanging)
                    return;

                if (point.y < root.y || point.y > root.y + root.height) {
                    if (!root.swipedOut) {
                        // We've swiped out of the key
                        root.swipedOut = true;
                        if (!panel.curve_ok) {
                            setupCurve()
                        }
                        if (root.keyMap.length == 0) {
                            console.warn("building map")
                            buildKeyMap()
                            console.warn("done building map")
                        }

                        curveimpl.resetCurve()
                        panel.curveIndex++;
                        curveimpl.startCurve()
                        root.addPoint(root, firstX, firstY);
                        cancelPress();
                    }
                    
                    root.addPoint(root, point.x, point.y)
                    // Dirty hack, see onReleased below
                    if (point.y > root.height) {
                        // Touch point released past height of keyboard.
                        return;
                    }
                }
            }
        }
    ]

    Timer {
        id: holdTimer
        interval: 300
        onTriggered: {
            if (root.pressed) {
                root.pressAndHold();
                held = true;
            }
        }
    }

    Timer {
        id: doubleClickTimer
        interval: 400 // Default Qt double click interval
    }

    onPressed: {
        firstX = point.x
        firstY = point.y
        pressed = true;
        held = false;
        root.swipedOut = false;
        startY = point.y;
        holdTimer.restart();

        // We keep a global view of whether any other keys have been
        // pressed in between the first tap on this key and the second
        // one, so that quickly tapping one key, then tapping another,
        // then tapping the first again doesn't trigger a double click.

        if (doubleClickTimer.running) {
            if (panel.lastKeyPressed == root) {
                doubleClicked();
            }
        } else {
            if (acceptDoubleClick) {
                doubleClickTimer.restart();
            }
            panel.lastKeyPressed = root
        }
    }

    onReleased: {

        if (root.swipedOut) {
            root.swipedOut = false
            root.hanging = true
            root.addPoint(root, point.x, point.y)
            console.warn("calling endOneCurve")
            curveimpl.endOneCurve(panel.curveIndex)
            //curveimpl.endCurve(panel.curveIndex)
            console.warn("endOneCurve returned")
            console.warn("calling getCandidates")
            var x = curveimpl.getCandidates()
            console.warn("getCandidates returned")
            console.warn("getCandidates returned " + x)
            root.hanging = false
        }

        pressed = false;
        held = false;
        holdTimer.stop();
    }
}
