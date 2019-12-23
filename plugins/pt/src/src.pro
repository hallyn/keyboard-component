TOP_BUILDDIR = $$OUT_PWD/../../..
TOP_SRCDIR = $$PWD/../../..

include($${TOP_SRCDIR}/config.pri)

TEMPLATE        = lib
TARGET          = dummy

# generate database for presage:
PLUGIN_INSTALL_PATH = $$UBUNTU_KEYBOARD_LIB_DIR/pt/

lang_db_pt.commands += \
  rm -f $$PWD/database_pt.db && \
  text2ngram -n 1 -l -f sqlite -o $$PWD/database_pt.db $$PWD/historias_sem_data.txt && \
  text2ngram -n 2 -l -f sqlite -o $$PWD/database_pt.db $$PWD/historias_sem_data.txt && \
  text2ngram -n 3 -l -f sqlite -o $$PWD/database_pt.db $$PWD/historias_sem_data.txt
lang_db_pt.files += $$PWD/database_pt.db

lang_db_pt_install.files += $$PWD/database_pt.db
lang_db_pt_install.path = $$PLUGIN_INSTALL_PATH

QMAKE_EXTRA_TARGETS += lang_db_pt lang_db_pt_install

overrides.files += $$PWD/overrides.csv
overrides.path += $$PLUGIN_INSTALL_PATH

layout.files += $$PWD/keyboard_layout.json
layout.path += $$PLUGIN_INSTALL_PATH

dictionary.files += $$PWD/dictionary.txt
dictionary.path += $$PLUGIN_INSTALL_PATH

INSTALLS += layout dictionary lang_db_pt_install overrides

OTHER_FILES += \
    historias_sem_data.txt
