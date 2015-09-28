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

#include "NoteEditor.h"
#include "SpellChecker.h"
#include "NoteSyntaxHighlighter.h"
#include "CompleterModel.h"

#include <QMouseEvent>
#include <QMessageBox>
#include <QRegExp>
#include <QTextBlock>
#include <QUrl>
#include <QDesktopServices>
#include <QMenu>
#include <QCompleter>
#include <QAbstractItemView>
#include <QScrollBar>

/*!
	\brief

*/


NoteEditor::NoteEditor(QWidget *parent) :
	QTextEdit(parent)
{

	//TODO use counted pointer
	fSpellChecker = new SpellChecker;
	fHiglighter = new NoteSyntaxHighlighter(fSpellChecker, document());

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(QPoint)),
			this, SLOT(ShowContextMenu(QPoint)));

	connect(this, SIGNAL(cursorPositionChanged()),
			this, SLOT(OnCursorPositionChanged()));

	fCompleter = 0;
	fCompleterModel = 0;
}

NoteEditor::~NoteEditor()
{
	delete fSpellChecker;
}

void NoteEditor::SetFont(const QFont& font, int tabSize)
{
	document()->setDefaultFont(font);

	int width = QFontMetrics(font).averageCharWidth();
	setTabStopWidth(tabSize * width);
	fHiglighter->rehighlight();
}

void NoteEditor::SetSearchText(const QString& text)
{
	fHiglighter->SetSearchText(text);
	fHiglighter->rehighlight();
}

void NoteEditor::SetSpellingCheckEnabled(bool enabled)
{

	fHiglighter->SetCheckSpelling(enabled);
}

void NoteEditor::SetSpellingLanguage(const QString& language)
{
	fSpellChecker->LoadDictionary(language);
}

void NoteEditor::SetCompleter(QSharedPointer<SnippetCollection> snippets)
{
	fCompleter = new QCompleter(this);
	fCompleterModel = new CompleterModel(snippets, fCompleter);

	fCompleter->setModel(fCompleterModel);

	fCompleter->setWidget(this);
	fCompleter->setCompletionMode(QCompleter::PopupCompletion);
	fCompleter->setCaseSensitivity(Qt::CaseInsensitive);
	QObject::connect(fCompleter, SIGNAL(activated(QModelIndex)),
						this, SLOT(InsertCompletion(QModelIndex)));
}


void NoteEditor::mousePressEvent(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton) {
		QTextCursor cursor = cursorForPosition(e->pos());
		QString url = FindUrl(cursor);

		if (!url.isEmpty()) {
			OpenURL(url);
		}
	}

	QTextEdit::mousePressEvent(e);
}


QString NoteEditor::FindUrl(const QTextCursor& cursor) const
{
	int pos = cursor.positionInBlock();
	QString text = cursor.block().text();
	return FindUrlAtPosition(text, pos);
}

QString NoteEditor::FindUrlAtPosition(const QString& text, int pos) const
{
	QRegExp expr("(http(s)?://.)?(www\\.)?[-a-zA-Z0-9@:%._\\+~#=]{2,256}\\.[a-z]{2,6}\\b([-a-zA-Z0-9@:%_\\+.~#?&//=]*)");

	QString url;
	int index = expr.indexIn(text);
	while (index >= 0) {
		int length = expr.matchedLength();
		if (pos >= index && pos < index + length) {
			url = text.mid(index, length);
			break;
		} else if (pos < index + length) {
			break;
		}
		index = expr.indexIn(text, index + length);
	}

	return url;
}


void NoteEditor::OpenURL(const QUrl& url)
{
	if (!url.isValid()) return;

	QDesktopServices::openUrl(url);
}

void NoteEditor::ShowContextMenu(const QPoint& pos)
{
	QMenu* contextMenu = createStandardContextMenu();

	QTextCursor cursor = cursorForPosition(pos);
	cursor.select(QTextCursor::WordUnderCursor);

	if (cursor.hasSelection() && !fSpellChecker->IsCorrect(cursor.selectedText())) {

		QAction* separator = new QAction(contextMenu);
		separator->setSeparator(true);
		contextMenu->insertAction(contextMenu->actions()[0], separator);

		QMenu *subMenu = CreateSuggestionMenu(cursor);
		contextMenu->insertMenu(separator, subMenu);

		QAction *userWordlistAction = new QAction(tr("Add to User Dictionary"), contextMenu);
		userWordlistAction->setData(cursor.selectedText());
		connect(userWordlistAction, SIGNAL(triggered()),
				this, SLOT(AddWordToUserWordlist()));
		contextMenu->insertAction(separator, userWordlistAction);
	}

	contextMenu->exec(mapToGlobal(pos));
	delete contextMenu;
}

void NoteEditor::SnippetsUpdated()
{
	if (fCompleterModel == 0) return;
	fCompleterModel->ResetModel();
}


QMenu* NoteEditor::CreateSuggestionMenu(const QTextCursor& cursor) const
{
	QMenu* result = new QMenu(tr("Suggestions"));

	int cursorPosition = cursor.position();
	QStringList suggestions = fSpellChecker->Suggestions(cursor.selectedText());

	foreach (const QString& suggestion, suggestions) {
		QAction* action = result->addAction(suggestion);
		action->setData(cursorPosition);
		connect(action, SIGNAL(triggered()),
				this, SLOT(ReplaceWithSuggestion()));
	}

	if (suggestions.isEmpty()) {
		result->setEnabled(false);
	}

	return result;
}

void NoteEditor::ReplaceWithSuggestion()
{
	QAction *action = qobject_cast<QAction*>(sender());

	QTextCursor cursor = textCursor();
	cursor.beginEditBlock();

	cursor.setPosition(action->data().toInt());
	cursor.select(QTextCursor::WordUnderCursor);
	cursor.insertText(action->text());

	cursor.endEditBlock();
}

void NoteEditor::AddWordToUserWordlist()
{
	QAction *action = qobject_cast<QAction*>(sender());
	QString word = action->data().toString();
	fSpellChecker->AddToUserDictionary(word);
	fHiglighter->rehighlight();
}

void NoteEditor::OnCursorPositionChanged()
{
	fHiglighter->OnPositionChanged(textCursor());
}



void NoteEditor::InsertCompletion(const QModelIndex& index)
{
	if (!index.isValid() || fCompleter->widget() != this) return;

	Snippet snippet = fCompleter->completionModel()->data(index, Qt::UserRole).value<Snippet>();

	QTextCursor tc = textCursor();

	tc.clearSelection();
	tc.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, fCompleter->completionPrefix().length());

	int pos = tc.position();
	tc.insertText(snippet.InsertionText());
	tc.setPosition(pos);
	tc.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, snippet.CursorPos());

	setTextCursor(tc);
}

QString NoteEditor::textUnderCursor() const
{
	QTextCursor tc = textCursor();
	tc.select(QTextCursor::WordUnderCursor);
	return tc.selectedText();
}


void NoteEditor::keyPressEvent(QKeyEvent *e)
{
	if (fCompleter && fCompleter->popup()->isVisible()) {
		// The following keys are forwarded by the completer to the widget
	   switch (e->key()) {
	   case Qt::Key_Enter:
	   case Qt::Key_Return:
	   case Qt::Key_Escape:
	   case Qt::Key_Tab:
	   case Qt::Key_Backtab:
			e->ignore();
			return; // let the completer do default behavior
	   default:
		   break;
	   }
	}

	bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
	if (!fCompleter || !isShortcut) // do not process the shortcut when we have a completer
		QTextEdit::keyPressEvent(e);

	const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
	if (!fCompleter || (ctrlOrShift && e->text().isEmpty()))
		return;

	//hide propup if at end of the word
	static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
	bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
	QString completionPrefix = textUnderCursor();

	if (!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 3
					  || eow.contains(e->text().right(1)))) {
		fCompleter->popup()->hide();
		return;
	}

	//set prefix in completer
	if (completionPrefix != fCompleter->completionPrefix()) {
		fCompleter->setCompletionPrefix(completionPrefix);
		fCompleter->popup()->setCurrentIndex(fCompleter->completionModel()->index(0, 0));
	}
	//show the completer
	QRect cr = cursorRect();
	cr.setWidth(fCompleter->popup()->sizeHintForColumn(0)
				+ fCompleter->popup()->verticalScrollBar()->sizeHint().width());
	fCompleter->complete(cr); // popup it up!
}

