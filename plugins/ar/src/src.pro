TOP_BUILDDIR = $$OUT_PWD/../../..
TOP_SRCDIR = $$PWD/../../..

include($${TOP_SRCDIR}/config.pri)

TEMPLATE        = lib
TARGET          = dummy

# generate database for presage:
PLUGIN_INSTALL_PATH = $${UBUNTU_KEYBOARD_LIB_DIR}/ar/

lang_db_ar.commands += \
  rm -f $$PWD/database_ar.db && \
  text2ngram -n 1 -l -f sqlite -o $$PWD/database_ar.db $$PWD/free_ebook.txt && \
  text2ngram -n 2 -l -f sqlite -o $$PWD/database_ar.db $$PWD/free_ebook.txt && \
  text2ngram -n 3 -l -f sqlite -o $$PWD/database_ar.db $$PWD/free_ebook.txt
lang_db_ar.files += $$PWD/database_ar.db
lang_db_ar_install.path = $$PLUGIN_INSTALL_PATH
lang_db_ar_install.files += $$PWD/database_ar.db

QMAKE_EXTRA_TARGETS += lang_db_ar lang_db_ar_install

layout.files += $$PWD/keyboard_layout.json
layout.path += $$PLUGIN_INSTALL_PATH

dictionary.files += $$PWD/dictionary.txt
dictionary.path += $$PLUGIN_INSTALL_PATH

INSTALLS += lang_db_ar_install

OTHER_FILES += \
    free_ebook.txt
