TOP_BUILDDIR = $$OUT_PWD/../../..
TOP_SRCDIR = $$PWD/../../..

include($${TOP_SRCDIR}/config.pri)

TEMPLATE        = lib
TARGET          = dummy

# generate database for presage:
PLUGIN_INSTALL_PATH = $${UBUNTU_KEYBOARD_LIB_DIR}/en/

lang_db_en.commands += \
  rm -f $$PWD/database_en.db && \
  text2ngram -n 1 -l -f sqlite -o $$PWD/database_en.db $$PWD/the_picture_of_dorian_gray.txt && \
  text2ngram -n 2 -l -f sqlite -o $$PWD/database_en.db $$PWD/the_picture_of_dorian_gray.txt && \
  text2ngram -n 3 -l -f sqlite -o $$PWD/database_en.db $$PWD/the_picture_of_dorian_gray.txt
lang_db_en.files += $$PWD/database_en.db

lang_db_en_install.files += $$PWD/database_en.db
lang_db_en_install.path = $$PLUGIN_INSTALL_PATH

QMAKE_EXTRA_TARGETS += lang_db_en lang_db_en_install

layout.files += $$PWD/keyboard_layout.json
layout.path += $$PLUGIN_INSTALL_PATH

dictionary.files += $$PWD/dictionary.txt
dictionary.path += $$PLUGIN_INSTALL_PATH

overrides.files += $$PWD/overrides.csv
overrides.path += $$PLUGIN_INSTALL_PATH

INSTALLS += layout dictionary lang_db_en_install overrides

OTHER_FILES += \
    the_picture_of_dorian_gray.txt
