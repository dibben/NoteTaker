/*
	Copyright 2015 David Dibben <dibben@ieee.org>

	NoteTaker is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	NoteTaker is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with NoteTaker.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "Note.h"
#include "NoteSyntaxHighlighter.h"
#include "SimpleNote.h"
#include "NoteDatabase.h"
#include "MessageInterface.h"
#include "SnippetCollection.h"

#include <QDialog>
#include <QList>
#include <QSharedPointer>

namespace Ui {
class MainWindow;
}

class QListWidgetItem;
class SpellChecker;

class MainWindow : public QDialog,
					public MessageInterface
{
	Q_OBJECT
public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();


	void		ClearCurrentNote();
	QString		CurrentTag() const;


public slots:

	void	OnAdd(const QString& text = QString());
	void	OnSettings();
	void	OnFavourite();
	void	OnExport();
	void	RemoveCurrent();

	void	UpdateTagSelector();

	void	FullSync();
	void	SaveCurrent();

	void	SetEditorFont(const QFont& font, int tabSize = 4);

	void	OpenNote(const QString& title);

private slots:

	void	UpdateNoteList();
	void	OnSelection(int row);
	void	OnTextChanged();
	void	OnTagsChanged();
	void	OnSearchChanged();
	void	OnTagSelected();
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

	void	SetProxy();
	void	SetDictionary();

private:

typedef QSharedPointer<Note>	NotePtr;

	SimpleNote* GetSynchroniser();
	void		UpdateFavouriteIcon();
	void		SetEditorEnabled(bool enable);
	void		LoadNotes();
	void		SetCurrentNote(const Note& note);
	void		UpdateListItem(NotePtr note, QListWidgetItem* item);
	void		LoadSnippets();
	void		SaveSnippets() const;
	QString		UserSnippetFile() const;
	int			FindRow(int index) const;

	Ui::MainWindow*		ui;
	NoteDatabase		fNotes;
	int					fCurrent;
	QTimer*				fTimer;
	SimpleNote*			fSimpleNote;
	QSharedPointer<SnippetCollection>	fSnippets;
};

#endif
