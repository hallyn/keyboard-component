TOP_BUILDDIR = $$OUT_PWD/../../..
TOP_SRCDIR = $$PWD/../../..

include($${TOP_SRCDIR}/config.pri)

TEMPLATE        = lib
TARGET          = dummy

# generate database for presage:
PLUGIN_INSTALL_PATH = $${UBUNTU_KEYBOARD_LIB_DIR}/de/

lang_db_de.commands += \
  rm -f $$PWD/database_de.db && \
  text2ngram -n 1 -l -f sqlite -o $$PWD/database_de.db $$PWD/buddenbrooks.txt && \
  text2ngram -n 2 -l -f sqlite -o $$PWD/database_de.db $$PWD/buddenbrooks.txt && \
  text2ngram -n 3 -l -f sqlite -o $$PWD/database_de.db $$PWD/buddenbrooks.txt
lang_db_de.files += $$PWD/database_de.db

lang_db_de_install.path = $$PLUGIN_INSTALL_PATH
lang_db_de_install.files += $$PWD/database_de.db

QMAKE_EXTRA_TARGETS += lang_db_de lang_db_de_install

layout.files += $$PWD/keyboard_layout.json
layout.path += $$PLUGIN_INSTALL_PATH

dictionary.files += $$PWD/dictionary.txt
dictionary.path += $$PLUGIN_INSTALL_PATH

INSTALLS += layout dictionary lang_db_de_install

OTHER_FILES += \
    buddenbrooks.txt
