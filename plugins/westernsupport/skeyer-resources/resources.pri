# The application data needed for Skeyer to run.
# Currently this contains keyboard definition files and dictionaries needed for each language

include(../config.pri)

resources.files = $$PWD/language_packs
resources.path = $$DATADIR

OTHER_FILES += $$PWD/language_packs/en_US/keyboard_layout.json \
               $$PWD/language_packs/en_US/dictionary.txt \
               $$PWD/language_packs/nb_NO/keyboard_layout.json \
               $$PWD/language_packs/nb_NO/dictionary.txt \


DEFINES += SKEYER_DATADIR=\\\"$$DATADIR_CPPDEFINE\\\"

INSTALLS += resources
