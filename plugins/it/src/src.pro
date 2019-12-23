TOP_BUILDDIR = $$OUT_PWD/../../..
TOP_SRCDIR = $$PWD/../../..

include($${TOP_SRCDIR}/config.pri)

TEMPLATE        = lib
TARGET          = dummy

# generate database for presage:
PLUGIN_INSTALL_PATH = $${UBUNTU_KEYBOARD_LIB_DIR}/it/

lang_db_it.commands += \
  rm -f $$PWD/database_it.db && \
  text2ngram -n 1 -l -f sqlite -o $$PWD/database_it.db $$PWD/la_francia_dal_primo_impero.txt && \
  text2ngram -n 2 -l -f sqlite -o $$PWD/database_it.db $$PWD/la_francia_dal_primo_impero.txt && \
  text2ngram -n 3 -l -f sqlite -o $$PWD/database_it.db $$PWD/la_francia_dal_primo_impero.txt
lang_db_it.files += $$PWD/database_it.db

lang_db_it_install.files += $$PWD/database_it.db
lang_db_it_install.path = $$PLUGIN_INSTALL_PATH

QMAKE_EXTRA_TARGETS += lang_db_it lang_db_it_install

overrides.files += $$PWD/overrides.csv
overrides.path += $$PLUGIN_INSTALL_PATH

layout.files += $$PWD/keyboard_layout.json
layout.path += $$PLUGIN_INSTALL_PATH

dictionary.files += $$PWD/dictionary.txt
dictionary.path += $$PLUGIN_INSTALL_PATH

INSTALLS += layout dictionary lang_db_it_install overrides

OTHER_FILES += \
    la_francia_dal_primo_impero.txt
