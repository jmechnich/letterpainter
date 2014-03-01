TEMPLATE = app
CONFIG = qt warn_on
win32-borland:DEFINES = WIN32
HEADERS = \
	LetterPainter.h \
        LetterPainter.icc
SOURCES = \
	LetterPainter.cpp \
	main.cpp
win32:TARGET = letterpainter.exe
unix:TARGET  = letterpainter
