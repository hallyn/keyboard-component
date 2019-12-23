TOP_BUILDDIR = $$OUT_PWD/../../..
TOP_SRCDIR = $$PWD/../../..

include($${TOP_SRCDIR}/config.pri)

TEMPLATE        = lib
TARGET          = dummy

# generate database for presage:
PLUGIN_INSTALL_PATH = $${UBUNTU_KEYBOARD_TEST_DIR}/testlayout/

lang_db_testlayout.commands += \
  rm -f $$PWD/database_testlayout.db && \
  text2ngram -n 1 -l -f sqlite -o $$PWD/database_testlayout.db $$PWD/empty.txt && \
  text2ngram -n 2 -l -f sqlite -o $$PWD/database_testlayout.db $$PWD/empty.txt && \
  text2ngram -n 3 -l -f sqlite -o $$PWD/database_testlayout.db $$PWD/empty.txt
lang_db_testlayout.files += $$PWD/database_testlayout.db

lang_db_testlayout_install.files += $$PWD/database_testlayout.db
lang_db_testlayout_install.path = $$PLUGIN_INSTALL_PATH

QMAKE_EXTRA_TARGETS += lang_db_testlayout lang_db_testlayout_install

layout.files += $$PWD/keyboard_layout.json
layout.path += $$PLUGIN_INSTALL_PATH

dictionary.files += $$PWD/dictionary.txt
dictionary.path += $$PLUGIN_INSTALL_PATH

INSTALLS += layout dictionary lang_db_testlayout_install

OTHER_FILES += \
    empty.txt
