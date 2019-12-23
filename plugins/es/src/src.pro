TOP_BUILDDIR = $$OUT_PWD/../../..
TOP_SRCDIR = $$PWD/../../..

include($${TOP_SRCDIR}/config.pri)

TEMPLATE        = lib
TARGET          = dummy

# generate database for presage:
PLUGIN_INSTALL_PATH = $${UBUNTU_KEYBOARD_LIB_DIR}/es/

lang_db_es.commands += \
  rm -f $$PWD/database_es.db && \
  text2ngram -n 1 -l -f sqlite -o $$PWD/database_es.db $$PWD/el_quijote.txt && \
  text2ngram -n 2 -l -f sqlite -o $$PWD/database_es.db $$PWD/el_quijote.txt && \
  text2ngram -n 3 -l -f sqlite -o $$PWD/database_es.db $$PWD/el_quijote.txt
lang_db_es.files += $$PWD/database_es.db

lang_db_es_install.files += $$PWD/database_es.db
lang_db_es_install.path = $$PLUGIN_INSTALL_PATH

QMAKE_EXTRA_TARGETS += lang_db_es lang_db_es_install

layout.files += $$PWD/keyboard_layout.json
layout.path += $$PLUGIN_INSTALL_PATH

dictionary.files += $$PWD/dictionary.txt
dictionary.path += $$PLUGIN_INSTALL_PATH

INSTALLS += layout dictionary lang_db_es_install

OTHER_FILES += \
    el_quijote.txt
