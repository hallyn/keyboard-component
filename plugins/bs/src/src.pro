TOP_BUILDDIR = $$OUT_PWD/../../..
TOP_SRCDIR = $$PWD/../../..

include($${TOP_SRCDIR}/config.pri)

TEMPLATE        = lib
TARGET          = dummy

# generate database for presage:
PLUGIN_INSTALL_PATH = $${UBUNTU_KEYBOARD_LIB_DIR}/bs/

lang_db_bs.commands += \
  rm -f $$PWD/database_bs.db && \
  text2ngram -n 1 -l -f sqlite -o $$PWD/database_bs.db $$PWD/free_ebook.txt && \
  text2ngram -n 2 -l -f sqlite -o $$PWD/database_bs.db $$PWD/free_ebook.txt && \
  text2ngram -n 3 -l -f sqlite -o $$PWD/database_bs.db $$PWD/free_ebook.txt
lang_db_bs.files += $$PWD/database_bs.db

lang_db_bs_install.files += $$PWD/database_bs.db
lang_db_bs_install.path = $$PLUGIN_INSTALL_PATH

QMAKE_EXTRA_TARGETS += lang_db_bs lang_db_bs_install

layout.files += $$PWD/keyboard_layout.json
layout.path += $$PLUGIN_INSTALL_PATH

dictionary.files += $$PWD/dictionary.txt
dictionary.path += $$PLUGIN_INSTALL_PATH

INSTALLS += layout dictionary lang_db_bs_install

OTHER_FILES += \
    free_ebook.txt
