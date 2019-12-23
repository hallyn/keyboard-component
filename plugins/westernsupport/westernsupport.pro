QT       -= gui

TARGET = westernsupport
TEMPLATE = lib
CONFIG += c++11

DEFINES += WESTERNSUPPORT_LIBRARY

TOP_BUILDDIR = $$OUT_PWD/../..
TOP_SRCDIR = $$PWD/../..
include($${TOP_SRCDIR}/config.pri)

INCLUDEPATH    += \
    $${TOP_SRCDIR}/src/ \
    $${TOP_SRCDIR}/src/lib/ \
    $${TOP_SRCDIR}/src/lib/logic/

DESTDIR         = $${TOP_BUILDDIR}/plugins/plugins

SOURCES += \
    westernsupport.cpp \
    westernlanguagefeatures.cpp \
    westernlanguagesplugin.cpp \
    candidatescallback.cpp \
    spellchecker.cpp \
    spellpredictworker.cpp \
    $${TOP_SRCDIR}/src/lib/logic/abstractlanguageplugin.cpp \

HEADERS += \
    westernsupport.h \
    westernsupport_global.h \
    westernlanguagefeatures.h \
    westernlanguagesplugin.h \
    candidatescallback.h \
    spellchecker.h \
    spellpredictworker.h \
    $${TOP_SRCDIR}/src/lib/logic/abstractlanguageplugin.h \


target.path = $${UBUNTU_KEYBOARD_LIB_DIR}
INSTALLS += target

# for plugins
API_HEADERS = westernlanguagesplugin.h

EXAMPLE_FILES = westernlanguageslugin.json

api_headers.files = $$API_HEADERS
api_headers.path = $$UBUNTU_KEYBOARD_HEADERS_DIR
INSTALLS += api_headers

DEFINES += SKEYER_DATADIR=\\\"$${UBUNTU_KEYBOARD_LIB_DIR}\\\"

# hunspell
CONFIG += link_pkgconfig
PKGCONFIG += hunspell
DEFINES += HAVE_HUNSPELL

# presage
LIBS += -lpresage -lskeyer
DEFINES += HUNSPELL_DICT_PATH=\\\"$$HUNSPELL_DICT_PATH\\\"

OTHER_FILES += westernlanguagesplugin.json
