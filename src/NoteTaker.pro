
QT += core gui widgets network

TARGET = NoteTaker
TEMPLATE = app

MOC_DIR=.moc
OBJECTS_DIR=.obj
UI_DIR=.ui

unix {
  CONFIG += link_pkgconfig
}

SOURCES += main.cpp\
	MainWindow.cpp \
	Note.cpp \
	SettingsDialog.cpp \
	NoteSyntaxHighlighter.cpp \
	SimpleNote.cpp \
    ListDelegate.cpp \
    NoteEditor.cpp \
    NoteDatabase.cpp \
    MessageInterface.cpp \
    SpellChecker.cpp \
    SpellingDictionary.cpp

HEADERS  += MainWindow.h \
	Note.h \
	SettingsDialog.h \
	NoteSyntaxHighlighter.h \
	SimpleNote.h \
    ListDelegate.h \
    NoteEditor.h \
    NoteDatabase.h \
    MessageInterface.h \
    SpellChecker.h \
    SpellingDictionary.h

FORMS    += MainWindow.ui \
	SettingsDialog.ui

RESOURCES += \
    NoteTaker.qrc

OTHER_FILES += README.md \
			   LICENSE

unix: {
  PKGCONFIG += hunspell
}
