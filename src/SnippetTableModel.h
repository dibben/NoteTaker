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


#ifndef SNIPPET_TABLE_MODEL_H
#define SNIPPET_TABLE_MODEL_H

#include "SnippetCollection.h"

#include <QAbstractTableModel>
#include <QSharedPointer>

/*!
	\class  SnippetTableModel
	\author Author
	\date Sun Sep 27 2015

	\brief
*/

class SnippetTableModel : public QAbstractTableModel
{
		Q_OBJECT
public:
		SnippetTableModel(QSharedPointer<SnippetCollection> snippets, QObject* parent);

virtual int				rowCount(const QModelIndex& parent) const;
virtual int				columnCount(const QModelIndex& parent) const;
virtual QVariant		data(const QModelIndex& index, int role) const;
virtual bool			setData(const QModelIndex& index, const QVariant& value, int role);
virtual QVariant		headerData(int section, Qt::Orientation orientation, int role) const;
virtual Qt::ItemFlags	flags(const QModelIndex& index) const;

		QModelIndex		CreateSnippet();
		void			RemoveSnippet(const QModelIndex& index);
		void			UpdateSnippet(const QModelIndex& index, const Snippet& snippet);
		void			InsertSnippet(const Snippet& snippet);
private:

		QSharedPointer<SnippetCollection>	fSnippets;
};

#endif
