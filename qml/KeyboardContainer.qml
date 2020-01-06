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
import QtQuick.Window 2.0
import "languages/"
import "keys/"
import UbuntuKeyboard 1.0
import eu.cpbm.okboard 1.0
import io.thp.pyotherside 1.0

Item {
    id: panel

    property var curve_ok: false
    property var curveIndex: 0
    property double scaling_ratio: 1

    property int keyWidth: 0
    property int keyHeight: 0

    property bool autoCapsTriggered: false
    property bool delayedAutoCaps: false

    property string activeKeypadState: "NORMAL"
    property alias popoverEnabled: extendedKeysSelector.enabled
    property bool switchBack: false // Switch back to the previous layout when changing fields
    property bool hideKeyLabels: false // Hide key labels when in cursor movement mode

    property Item lastKeyPressed // Used for determining double click validity in PressArea

    property var swypeAble: false
    property var correlationId: 0

    state: "CHARACTERS"

    function closeExtendedKeys()
    {
        extendedKeysSelector.closePopover();
    }

    function matching_done(candidates) {
        // XXX candidates is always empty.  why?
        console.warn("matching_done: starting")
        console.warn("matching_done: called with " + candidates)
        // The 'guess' stuff can't actually work until we get the
        // info about surrounding words.
        py.call("okboard.k.guess", [ candidates, correlationId], function(result) {
            if (result && result.length > 0) {
                console.warn("guess completed with result: " + result)
            } else {
                console.warn("guess completed with empty result")
            }
        })
    }

    CurveKB {
        id: curveimpl
        onMatchingDone: { matching_done(candidates); }
    }

    Loader {
        id: characterKeypadLoader
        objectName: "characterKeyPadLoader"
        anchors.fill: parent
        asynchronous: false
        source: panel.state === "CHARACTERS" ? internal.characterKeypadSource : internal.symbolKeypadSource
        onLoaded: {
            if (delayedAutoCaps) {
                activeKeypadState = "SHIFTED";
                delayedAutoCaps = false;
            } else {
                activeKeypadState = "NORMAL";
            }
        }
    }

    Python {
        id: py
        onError: { py.call("okboard.k.get_last_error", [], function(result) {
            if (result != undefined) {
                console.warn(result); 
            }
        }) }
    }

    ExtendedKeysSelector {
        id: extendedKeysSelector
        objectName: "extendedKeysSelector"
        anchors.fill: parent
    }

    Magnifier {
        id: magnifier
        shown: false
    }

    states: [
        State {
            name: "CHARACTERS"
        },
        State {
            name: "SYMBOLS"
        }
    ]

    onStateChanged: {
        maliit_input_method.keyboardState = state
    }

    QtObject {
        id: internal

        property Item activeKeypad: characterKeypadLoader.item
        property string characterKeypadSource: loadLayout(maliit_input_method.contentType,
                                                          maliit_input_method.activeLanguage)
        property string symbolKeypadSource: activeKeypad ? activeKeypad.symbols : ""

        onCharacterKeypadSourceChanged: {
            panel.state = "CHARACTERS";
        }

        function apply_configuration(conf) {
            console.warn("apply_configuration starting")
            console.warn("apply_configuration starting: conf: " + conf)
            if (conf && conf["unchanged"]) {
                // no configuration change
                console.warn("no configuration change")

            } else if (conf) {
                // curve matching plugin parameters
                console.warn("loading parameters: ")
                curveimpl.loadParameters(conf['curve_params']);
                console.warn("loaded parameters: " + conf['curve_params'])

                // language
                kb_lang = conf['kb_lang'];

            } else {
                conf_ok = false;
                console.warn("Error loading configuration")
            }
        }

        function setupCurve() {
            py.addImportPath(Qt.resolvedUrl('/usr/share/maliit/plugins/eu/cpbm/okboard'));
            py.importModule_sync('okboard');
            var orientation = "default"
            var layout = "en"
            py.call("okboard.k.get_config", [], function(result) {
                apply_configuration(result);
            })
            py.call("okboard.k.set_context", [ layout, orientation ]);
            curveimpl.setLogFile("/tmp/curve.log");
            console.warn("setting pixelsize to: " + fullScreenItem.width + "," + fullScreenItem.height)
            curveimpl.setScreenSizePixels(fullScreenItem.width, fullScreenItem.height)
            scaling_ratio = curveimpl.getScalingRatio();
            panel.curve_ok = curveimpl.loadTree("/usr/share/okboard/en.tre");
            console.warn("done loading tree")
        }

        function loadLayout(contentType, activeLanguage)
        {
            var language = activeLanguage.toLowerCase();
            if (!maliit_input_method.languageIsSupported(language)) {
                // If we don't have a layout for this specific locale 
                // check more generic locale
                language = language.slice(0,2);
            }

            if (!maliit_input_method.languageIsSupported(language)) {
                console.log("Language '" + language + "' not supported - using 'en' instead");
                maliit_input_method.activeLanguage = "en";
                language = "en";
            }

            // NumberContentType
            if (contentType === 1) {
                canvas.layoutId = "number";
                return "languages/Keyboard_numbers.qml";
            }

            // PhoneNumberContentType
            if (contentType === 2) {
                canvas.layoutId = "telephone";
                return "languages/Keyboard_telephone.qml";
            }

            if (language == "en" || language == "fr" || language == "nl") {
                panel.swypeAble = true
                setupCurve()
            }

            // EmailContentType
            if (contentType === 3) {
                canvas.layoutId = "email";
                return maliit_input_method.currentPluginPath + "/Keyboard_" + language + "_email.qml";
            }

            // UrlContentType
            if (contentType === 4) {
                canvas.layoutId = "url";
                return maliit_input_method.currentPluginPath + "/Keyboard_" + language + "_url_search.qml";
            }

            // FreeTextContentType used as fallback
            canvas.layoutId = "freetext";
            return maliit_input_method.currentPluginPath + "/Keyboard_" + language + ".qml";
        }
    }
}
