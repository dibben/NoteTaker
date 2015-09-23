/*!
	\file  NoteSyntaxHighlighter.cpp
	\brief The NoteSyntaxHighlighter implementation
	\author Author
	\date \$Date: Mon Sep 21 2015 $
	\version \$Id: TSearchHighlighter.cpp $
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
