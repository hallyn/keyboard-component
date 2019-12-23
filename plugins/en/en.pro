CONFIG += ordered
TEMPLATE = subdirs
SUBDIRS = \
    src 

QMAKE_EXTRA_TARGETS += check
check.target = check
check.CONFIG = recursive
