TOP_BUILDDIR = $$OUT_PWD/../../..
TOP_SRCDIR = $$PWD/../../..

include($${TOP_SRCDIR}/config.pri)

TEMPLATE        = lib
TARGET          = dummy

# generate database for presage:
PLUGIN_INSTALL_PATH = $${UBUNTU_KEYBOARD_LIB_DIR}/fr/

lang_db_fr.commands += \
  rm -f $$PWD/database_fr.db && \
  text2ngram -n 1 -l -f sqlite -o $$PWD/database_fr.db $$PWD/les_trois_mousquetaires.txt && \
  text2ngram -n 2 -l -f sqlite -o $$PWD/database_fr.db $$PWD/les_trois_mousquetaires.txt && \
  text2ngram -n 3 -l -f sqlite -o $$PWD/database_fr.db $$PWD/les_trois_mousquetaires.txt
lang_db_fr.files += $$PWD/database_fr.db

lang_db_fr_install.files += $$PWD/database_fr.db
lang_db_fr_install.path = $$PLUGIN_INSTALL_PATH

QMAKE_EXTRA_TARGETS += lang_db_fr lang_db_fr_install

layout.files += $$PWD/keyboard_layout.json
layout.path += $$PLUGIN_INSTALL_PATH

dictionary.files += $$PWD/dictionary.txt
dictionary.path += $$PLUGIN_INSTALL_PATH

overrides.files += $$PWD/overrides.csv
overrides.path += $$PLUGIN_INSTALL_PATH

INSTALLS += layout dictionary lang_db_fr_install overrides

OTHER_FILES += \
    les_trois_mousquetaires.txt
