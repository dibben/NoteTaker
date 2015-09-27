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

#include "CompleterModel.h"

#include "Snippet.h"
#include <QVariant>


CompleterModel::CompleterModel(QSharedPointer<SnippetCollection> snippets, QObject* parent) :
	QAbstractListModel(parent)
{
	fSnippets = snippets;
}


int CompleterModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return fSnippets->Count();
}

QVariant CompleterModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid() || index.row() > rowCount()) return QVariant();

	const Snippet snippet = fSnippets->GetSnippet(index.row());

	switch (role) {
		case Qt::DisplayRole:
			return QString("%1 %2").arg(snippet.Trigger(), -10).arg(snippet.Description());

		case Qt::EditRole:
			return snippet.Trigger();

		case Qt::ToolTipRole:
			return snippet.InsertionText().toHtmlEscaped();

		case Qt::UserRole:
			return QVariant::fromValue(snippet);

		break;
	}
	return QVariant();
}

Snippet CompleterModel::GetSnippet(const QModelIndex& index) const
{
	return fSnippets->GetSnippet(index.row());
}

void CompleterModel::ResetModel()
{
	beginResetModel();
	endResetModel();
}


