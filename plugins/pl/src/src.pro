TOP_BUILDDIR = $$OUT_PWD/../../..
TOP_SRCDIR = $$PWD/../../..

include($${TOP_SRCDIR}/config.pri)

TEMPLATE        = lib
TARGET          = dummy

# generate database for presage:
PLUGIN_INSTALL_PATH = $${UBUNTU_KEYBOARD_LIB_DIR}/pl/

lang_db_pl.commands += \
  rm -f $$PWD/database_pl.db && \
  text2ngram -n 1 -l -f sqlite -o $$PWD/database_pl.db $$PWD/ziemia_obiecana_tom_pierwszy_4.txt && \
  text2ngram -n 2 -l -f sqlite -o $$PWD/database_pl.db $$PWD/ziemia_obiecana_tom_pierwszy_4.txt && \
  text2ngram -n 3 -l -f sqlite -o $$PWD/database_pl.db $$PWD/ziemia_obiecana_tom_pierwszy_4.txt
lang_db_pl.files += $$PWD/database_pl.db

lang_db_pl_install.files += $$PWD/database_pl.db
lang_db_pl_install.path = $$PLUGIN_INSTALL_PATH

QMAKE_EXTRA_TARGETS += lang_db_pl lang_db_pl_install

layout.files += $$PWD/keyboard_layout.json
layout.path += $$PLUGIN_INSTALL_PATH

dictionary.files += $$PWD/dictionary.txt
dictionary.path += $$PLUGIN_INSTALL_PATH

INSTALLS += layout dictionary lang_db_pl_install

OTHER_FILES += \
    ziemia_obiecana_tom_pierwszy_4.txt
