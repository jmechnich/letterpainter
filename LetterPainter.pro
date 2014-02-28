TEMPLATE = app
CONFIG = qt warn_on
win32-borland:DEFINES = WIN32
HEADERS = \
	LetterPainter.h \
        LetterPainter.icc
SOURCES = \
	LetterPainter.cpp \
	main.cpp
win32:TARGET = bin-win32/LetterPainter
unix:TARGET = bin/LetterPainter
