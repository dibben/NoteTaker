/*!
	\file   NoteSyntaxHighlighter.h
	\brief The NoteSyntaxHighlighter class
	\author Author
	\date $Date: Mon Sep 21 2015 $
	\version $Id: TSearchHighlighter.h $
*/

#ifndef NOTE_SYNTAX_HIGHLIGHTER_H
#define NOTE_SYNTAX_HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegExp>

/*!
	\class  NoteSyntaxHighlighter
	\author Author
	\date Mon Sep 21 2015

	\brief
*/

class NoteSyntaxHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT
public:
		NoteSyntaxHighlighter(QTextDocument* parent = 0);

		void	SetSearchText(const QString& text);

protected:

		void	highlightBlock(const QString &text);

private:

		QRegExp			fSearchExpr;
		QRegExp			fUrlExpr;
		QTextCharFormat	fFormat;
		QTextCharFormat fUrlFormat;

};

#endif
