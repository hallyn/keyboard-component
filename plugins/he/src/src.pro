TOP_BUILDDIR = $$OUT_PWD/../../..
TOP_SRCDIR = $$PWD/../../..

include($${TOP_SRCDIR}/config.pri)

TEMPLATE        = lib
TARGET          = dummy

# generate database for presage:
PLUGIN_INSTALL_PATH = $${UBUNTU_KEYBOARD_LIB_DIR}/he/

lang_db_he.commands += \
  rm -f $$PWD/database_he.db && \
  text2ngram -n 1 -l -f sqlite -o $$PWD/database_he.db $$PWD/free_ebook.txt && \
  text2ngram -n 2 -l -f sqlite -o $$PWD/database_he.db $$PWD/free_ebook.txt && \
  text2ngram -n 3 -l -f sqlite -o $$PWD/database_he.db $$PWD/free_ebook.txt
lang_db_he.files += $$PWD/database_he.db

lang_db_he_files.files += $$PWD/database_he.db
lang_db_he_files.path = $$PLUGIN_INSTALL_PATH

QMAKE_EXTRA_TARGETS += lang_db_he lang_db_he_files

layout.files += $$PWD/keyboard_layout.json
layout.path += $$PLUGIN_INSTALL_PATH

dictionary.files += $$PWD/dictionary.txt
dictionary.path += $$PLUGIN_INSTALL_PATH

overrides.files += $$PWD/overrides.csv
overrides.path += $$PLUGIN_INSTALL_PATH

INSTALLS += layout dictionary lang_db_he_files overrides

OTHER_FILES += \
    free_ebook.txt
