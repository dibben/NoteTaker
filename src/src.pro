
QT += core gui widgets network

TARGET = notetaker
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
    SpellingDictionary.cpp \
    Snippet.cpp \
    SnippetCollection.cpp \
    SnippetTableModel.cpp \
    CompleterModel.cpp \
    NoteListModel.cpp \
    NoteSortFilterModel.cpp

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
    SpellingDictionary.h \
    Snippet.h \
    SnippetCollection.h \
    SnippetTableModel.h \
    CompleterModel.h \
    NoteListModel.h \
    NoteSortFilterModel.h

FORMS    += MainWindow.ui \
	SettingsDialog.ui

RESOURCES += \
    NoteTaker.qrc

OTHER_FILES += README.md \
			   LICENSE

unix: {
  PKGCONFIG += hunspell
}

RC_FILE = notetaker.rc

## INSTALLATION

unix {
   isEmpty(PREFIX): PREFIX = /usr
   DATADIR = $${PREFIX}/share

   # install desktop file
   desktop.path = $${DATADIR}/applications
   desktop.files += notetaker.desktop

   # install application
   target.path = $${PREFIX}/bin

   INSTALLS += target desktop
   message("The project will be installed in prefix $${PREFIX}")
}

DISTFILES += \
    notetaker.desktop \
    notetaker.rc \
    builtin-snippets.json
