TOP_BUILDDIR = $$OUT_PWD/../../..
TOP_SRCDIR = $$PWD/../../..

include($${TOP_SRCDIR}/config.pri)

TEMPLATE        = lib
TARGET          = dummy

# generate database for presage:
PLUGIN_INSTALL_PATH = $${UBUNTU_KEYBOARD_LIB_DIR}/da/

lang_db_da.commands += \
  rm -f $$PWD/database_da.db && \
  text2ngram -n 1 -l -f sqlite -o $$PWD/database_da.db $$PWD/free_ebook.txt && \
  text2ngram -n 2 -l -f sqlite -o $$PWD/database_da.db $$PWD/free_ebook.txt && \
  text2ngram -n 3 -l -f sqlite -o $$PWD/database_da.db $$PWD/free_ebook.txt
lang_db_da.files += $$PWD/database_da.db

lang_db_da_install.files += $$PWD/database_da.db
lang_db_da_install.path = $$PLUGIN_INSTALL_PATH

layout.files += $$PWD/keyboard_layout.json
layout.path += $$PLUGIN_INSTALL_PATH

dictionary.files += $$PWD/dictionary.txt
dictionary.path += $$PLUGIN_INSTALL_PATH

overrides.files += $$PWD/overrides.csv
overrides.path += $$PLUGIN_INSTALL_PATH

QMAKE_EXTRA_TARGETS += lang_db_da lang_db_da_install

INSTALLS += layout dictionary lang_db_da_install overrides

OTHER_FILES += \
    free_ebook.txt
