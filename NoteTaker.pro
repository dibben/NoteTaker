

QT += core gui widgets network

TARGET = NoteTaker
TEMPLATE = app

MOC_DIR=.moc
OBJECTS_DIR=.obj
UI_DIR=.ui


SOURCES += main.cpp\
		MainWindow.cpp \
	Note.cpp \
	SettingsDialog.cpp \
	NoteSyntaxHighlighter.cpp \
	SimpleNote.cpp \
    ListDelegate.cpp \
    NoteEditor.cpp \
    NoteDatabase.cpp \
    MessageInterface.cpp

HEADERS  += MainWindow.h \
	Note.h \
	SettingsDialog.h \
	NoteSyntaxHighlighter.h \
	SimpleNote.h \
    ListDelegate.h \
    NoteEditor.h \
    NoteDatabase.h \
    MessageInterface.h

FORMS    += MainWindow.ui \
	SettingsDialog.ui

RESOURCES += \
    NoteTaker.qrc
