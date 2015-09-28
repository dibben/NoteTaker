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

#ifndef NOTE_EDITOR_H
#define NOTE_EDITOR_H

#include <QTextEdit>
#include <QSharedPointer>

class SpellChecker;
class NoteSyntaxHighlighter;
class QCompleter;
class SnippetCollection;
class CompleterModel;

/*!
	\class  NoteEditor
	\author Author
	\date Wed Sep 23 2015

	\brief
*/

class NoteEditor : public QTextEdit
{
	Q_OBJECT
public:

		NoteEditor(QWidget *parent = 0);
		~NoteEditor();


		void	SetFont(const QFont& font, int tabSize);
		void	SetSearchText(const QString& text);
		void	SetSpellingCheckEnabled(bool enabled);
		void	SetSpellingLanguage(const QString& language);
		void	SetCompleter(QSharedPointer<SnippetCollection> snippets);

public slots:

		void	OpenURL(const QUrl& url);
		void	ShowContextMenu(const QPoint& pos);
		void	SnippetsUpdated();

		void	InsertCompletion(const QModelIndex& index);

protected:


virtual void	mousePressEvent(QMouseEvent* e);

		void	keyPressEvent(QKeyEvent* e);

private slots:

		void	ReplaceWithSuggestion();
		void	AddWordToUserWordlist();
		void	OnCursorPositionChanged();

private:

		QString FindUrl(const QTextCursor& cursor) const;
		QString FindUrlAtPosition(const QString& text, int pos) const;
		QMenu*	CreateSuggestionMenu(const QTextCursor& cursor) const;
		QString textUnderCursor() const;

		NoteSyntaxHighlighter*	fHiglighter;
		SpellChecker*			fSpellChecker;
		QCompleter*				fCompleter;
		CompleterModel*			fCompleterModel;

};

#endif
