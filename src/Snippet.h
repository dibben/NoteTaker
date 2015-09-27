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

#ifndef SNIPPET_H
#define SNIPPET_H

#include <QString>
#include <QJsonObject>
#include <QVariant>

/*!
	\class  Snippet
	\author David Dibben
	\date Sun Sep 27 2015

	\brief A class to hold a text snippet
*/

class Snippet
{
public:
		Snippet();
		Snippet(const QJsonObject& obj);

friend	bool	operator==(const Snippet& arg1, const Snippet& arg2);
friend	bool	operator!=(const Snippet& arg1, const Snippet& arg2);

		QString	Trigger() const;
		QString	Description() const;
		QString	Contents() const;
		QString	InsertionText() const;
		int		CursorPos() const;

		bool	IsBuiltIn() const;

		void	SetContents(const QString& text);
		void	SetDesciption(const QString& text);
		void	SetTrigger(const QString& text);

		QJsonObject	AsJson() const;

private:

		QString ReplaceKeywords(const QString& text, int* pos = 0) const;
		QString Replacement(const QString& source) const;

		QString	fTrigger;
		QString	fDescription;
		QString	fContents;
		bool	fIsBuiltin;

};

Q_DECLARE_METATYPE(Snippet)


#endif
