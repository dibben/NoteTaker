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

#include "NoteSyntaxHighlighter.h"
#include "SpellChecker.h"

#include <QTextDocument>

/*!
	\brief

*/

NoteSyntaxHighlighter::NoteSyntaxHighlighter(SpellChecker* checker, QTextDocument* parent) :
	QSyntaxHighlighter(parent),
	fSpellChecker(checker)
{
	fFormat.setBackground(Qt::yellow);
	fCheckSpelling = true;

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

void NoteSyntaxHighlighter::SetCheckSpelling(bool check)
{
	fCheckSpelling = check;
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

	HighlightSpelling(text);

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

void NoteSyntaxHighlighter::HighlightSpelling(const QString& text)
{
	if (!fCheckSpelling || fSpellChecker == 0) return;

	QStringList wordList = text.split(QRegExp("\\W+"), QString::SkipEmptyParts);
	int index = 0;
	foreach (QString word, wordList) {
		index = text.indexOf(word, index);

		QTextCharFormat spellFormat = format(index);
		spellFormat.setUnderlineStyle(QTextCharFormat::WaveUnderline);
		spellFormat.setUnderlineColor(Qt::red);

		if (!fSpellChecker->IsCorrect(word)) {
			setFormat(index, word.length(), spellFormat);
		}
		index += word.length();
	}
}
