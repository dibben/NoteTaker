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

#include "NoteSyntaxHighlighter.h"

#include <QTextDocument>

/*!
	\brief

*/

NoteSyntaxHighlighter::NoteSyntaxHighlighter(QTextDocument* parent) :
	QSyntaxHighlighter(parent)
{
	fFormat.setBackground(Qt::yellow);

	//http://stackoverflow.com/questions/3809401/what-is-a-good-regular-expression-to-match-a-url
	//http://regexr.com/39i0i
	fUrlExpr.setPattern("(http(s)?://.)?(www\\.)?[-a-zA-Z0-9@:%._\\+~#=]{2,256}\\.[a-z]{2,6}\\b([-a-zA-Z0-9@:%_\\+.~#?&//=]*)");

	QColor urlColor("#3366BB");
	fUrlFormat.setAnchor(true);
	fUrlFormat.setForeground(urlColor);
	fUrlFormat.setFontUnderline(true);
}

void NoteSyntaxHighlighter::SetSearchText(const QString& text)
{
	fSearchExpr = QRegExp(text);
	fSearchExpr.setMinimal(true);
	fSearchExpr.setPatternSyntax(QRegExp::Wildcard);
}

void NoteSyntaxHighlighter::highlightBlock(const QString& text)
{
	if (currentBlock().position() == 0) {
		int len = text.indexOf('\n');
		if (len == -1) len = text.length();
		QTextCharFormat format;
		format.setFontWeight(QFont::Bold);
		int textSize = document()->defaultFont().pointSize();
		format.setFontPointSize(textSize + 4);
		setFormat(0, len, format);
	}

	int index = fUrlExpr.indexIn(text);
	while (index >= 0) {
		int length = fUrlExpr.matchedLength();

		QTextCharFormat format = fUrlFormat;
		format.setAnchor(true);
		format.setAnchorHref(text.mid(index, length));
		setFormat(index, length, format);

		index = fUrlExpr.indexIn(text, index + length);
	}

	if (fSearchExpr.isEmpty() || fSearchExpr.pattern().startsWith('#')) return;

	/*int*/ index = fSearchExpr.indexIn(text);
	while (index >= 0) {
		int length = fSearchExpr.matchedLength();
		setFormat(index, length, fFormat);
		index = fSearchExpr.indexIn(text, index + length);
	}
}
