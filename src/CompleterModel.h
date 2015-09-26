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

#ifndef COMPLETER_MODEL_H
#define COMPLETER_MODEL_H

#include "SnippetCollection.h"

#include <QSharedPointer>
#include <QAbstractListModel>

/*!
	\class  CompleterModel
	\author Author
	\date Sun Sep 27 2015

	\brief
*/

class CompleterModel: public QAbstractListModel
{
		Q_OBJECT
public:
		CompleterModel(QSharedPointer<SnippetCollection> snippets, QObject* parent = 0);

virtual int			rowCount(const QModelIndex& parent = QModelIndex()) const;
virtual QVariant	data(const QModelIndex& index, int role) const;

		Snippet		GetSnippet(const QModelIndex& index) const;

		void		ResetModel();

private:

		QSharedPointer<SnippetCollection>	fSnippets;
};

#endif
