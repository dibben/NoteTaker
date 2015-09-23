/*
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

#include <QMouseEvent>
#include <QMessageBox>
#include <QRegExp>
#include <QTextBlock>
#include <QUrl>
#include <QDesktopServices>

/*!
	\brief

*/

NoteEditor::NoteEditor(QWidget *parent) :
	QTextEdit(parent)
{
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
