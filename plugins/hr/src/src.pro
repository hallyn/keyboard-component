TOP_BUILDDIR = $$OUT_PWD/../../..
TOP_SRCDIR = $$PWD/../../..

include($${TOP_SRCDIR}/config.pri)

TEMPLATE        = lib
TARGET          = dummy

# generate database for presage:
PLUGIN_INSTALL_PATH = $${UBUNTU_KEYBOARD_LIB_DIR}/hr/

lang_db_hr.commands += \
  rm -f $$PWD/database_hr.db && \
  text2ngram -n 1 -l -f sqlite -o $$PWD/database_hr.db $$PWD/knjiga.txt && \
  text2ngram -n 2 -l -f sqlite -o $$PWD/database_hr.db $$PWD/knjiga.txt && \
  text2ngram -n 3 -l -f sqlite -o $$PWD/database_hr.db $$PWD/knjiga.txt
lang_db_hr.files += $$PWD/database_hr.db

lang_db_hr_install.files += $$PWD/database_hr.db
lang_db_hr_install.path = $$PLUGIN_INSTALL_PATH

QMAKE_EXTRA_TARGETS += lang_db_hr lang_db_hr_install

layout.files += $$PWD/keyboard_layout.json
layout.path += $$PLUGIN_INSTALL_PATH

dictionary.files += $$PWD/dictionary.txt
dictionary.path += $$PLUGIN_INSTALL_PATH

target.path = $$PLUGIN_INSTALL_PATH
INSTALLS += layout dictionary lang_db_hr_install

OTHER_FILES += \
    knjiga.txt
