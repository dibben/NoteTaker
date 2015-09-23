#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "Note.h"
#include "NoteSyntaxHighlighter.h"
#include "SimpleNote.h"
#include "NoteDatabase.h"
#include "MessageInterface.h"
#include <QDialog>
#include <QList>

namespace Ui {
class MainWindow;
}

class QListWidgetItem;

class MainWindow : public QDialog,
					public MessageInterface
{
	Q_OBJECT
public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();


	void ClearCurrentNote();

public slots:

	void	OnAdd(const QString& text = QString());
	void	OnSettings();
	void	OnFavourite();
	void	OnExport();
	void	RemoveCurrent();
	void	FullSync();
	void	SetEditorFont(const QFont& font, int tabSize = 4);

private slots:

	void	UpdateNoteList();
	void	OnSelection(int row);
	void	OnTextChanged();
	void	OnTagsChanged();
	void	OnSearchChanged();
	void	SaveCurrent();
	void	SaveAll();
	void	OnEnter();
	void	OnTimeout();
	void	EditTags();

protected:

	void	closeEvent(QCloseEvent* ev);
	void	keyPressEvent(QKeyEvent* ev);

	void	SaveSettings();
	void	RestoreSettings();
	void	SetMessage(const QString& message);

private:

typedef QSharedPointer<Note>	NotePtr;

	SimpleNote* GetSynchroniser();
	void		UpdateFavouriteIcon();
	void		SetEditorEnabled(bool enable);
	void		LoadNotes();
	void		SetCurrentNote(const Note& note);
	void		UpdateListItem(NotePtr note, QListWidgetItem* item);

	Ui::MainWindow *ui;

	NoteDatabase		fNotes;
	int					fCurrent;
	QTimer*				fTimer;
	NoteSyntaxHighlighter*	fHiglighter;
	SimpleNote*		fSimpleNote;
};

#endif
