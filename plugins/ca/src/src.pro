TOP_BUILDDIR = $$OUT_PWD/../../..
TOP_SRCDIR = $$PWD/../../..

include($${TOP_SRCDIR}/config.pri)

TEMPLATE        = lib
TARGET          = dummy

# generate database for presage:
PLUGIN_INSTALL_PATH = $${UBUNTU_KEYBOARD_LIB_DIR}/ca/

lang_db_ca.commands += \
  rm -f $$PWD/database_ca.db && \
  text2ngram -n 1 -l -f sqlite -o $$PWD/database_ca.db $$PWD/paulina_buxareu.txt && \
  text2ngram -n 2 -l -f sqlite -o $$PWD/database_ca.db $$PWD/paulina_buxareu.txt && \
  text2ngram -n 3 -l -f sqlite -o $$PWD/database_ca.db $$PWD/paulina_buxareu.txt
lang_db_ca.files += $$PWD/database_ca.db

lang_db_ca_install.files += $$PWD/database_ca.db
lang_db_ca_install.path = $$PLUGIN_INSTALL_PATH

QMAKE_EXTRA_TARGETS += lang_db_ca lang_db_ca_install

layout.files += $$PWD/keyboard_layout.json
layout.path += $$PLUGIN_INSTALL_PATH

dictionary.files += $$PWD/dictionary.txt
dictionary.path += $$PLUGIN_INSTALL_PATH

overrides.files += $$PWD/overrides.csv
overrides.path += $$PLUGIN_INSTALL_PATH

INSTALLS += layout dictionary lang_db_ca_install overrides

OTHER_FILES += \
    paulina_buxareu.txt
