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

#ifndef SNIPPET_COLLECTION_H
#define SNIPPET_COLLECTION_H

#include "Snippet.h"
#include <QMultiMap>


/*!
	\class  SnippetCollection
	\author David Dibben
	\date Sun Sep 27 2015

	\brief
*/

class SnippetCollection
{
public:

	SnippetCollection();

	int				Count() const;

	bool			Contains(const QString& trigger) const;
	bool			Contains(const Snippet& snippet) const;
	QList<Snippet>	Find(const QString& trigger) const;
	Snippet			GetSnippet(int index) const;

	int				Insert(const Snippet& snippet);
	void			Remove(const Snippet& snippet);
	void			Remove(int index);
	void			Update(int index, const Snippet& snippet);
	void			Clear();

	void			Load(const QString& filename);
	void			Save(const QString& filename);
	void			RestoreBuiltIn();


private:

	QMultiMap<QString, Snippet>	fSnippets;
};

#endif
