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

#ifndef NOTE_SYNTAX_HIGHLIGHTER_H
#define NOTE_SYNTAX_HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegExp>

class SpellChecker;

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
		NoteSyntaxHighlighter(SpellChecker* checker, QTextDocument* parent = 0);

		void	SetSearchText(const QString& text);

		void	SetCheckSpelling(bool check);

protected:

		void	highlightBlock(const QString &text);

private:

		void	HighlightSpelling(const QString& text);

		SpellChecker*	fSpellChecker;
		bool			fCheckSpelling;
		QRegExp			fSearchExpr;
		QRegExp			fUrlExpr;
		QTextCharFormat	fFormat;
		QTextCharFormat fUrlFormat;

};

#endif
