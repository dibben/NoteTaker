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

#include "SnippetTableModel.h"


/*!
	\brief

*/

SnippetTableModel::SnippetTableModel(QSharedPointer<SnippetCollection> snippets, QObject* parent) :
			QAbstractTableModel(parent)
{
	fSnippets = snippets;
}


int SnippetTableModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return fSnippets->Count();
}

int SnippetTableModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return 2;
}

QVariant SnippetTableModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())  return QVariant();

	Snippet snippet = fSnippets->GetSnippet(index.row());

	if (role == Qt::DisplayRole || role == Qt::EditRole) {
		if (index.column() == 0) {
			return snippet.Trigger();
		} else {
			return snippet.Description();
		}
	}

	return QVariant();
}

bool SnippetTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	 if (!index.isValid() || role != Qt::EditRole) return false;

	Snippet snippet = fSnippets->GetSnippet(index.row());

	if (index.column() == 0) {
		QString s = value.toString();
		if (s.isEmpty() && snippet.Trigger().isEmpty()) {
			RemoveSnippet(index);
			return false;
		}
		snippet.SetTrigger(s);
	} else {
		snippet.SetDesciption(value.toString());
	}

	UpdateSnippet(index, snippet);
	return true;
}

QVariant SnippetTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole || orientation != Qt::Horizontal) return QVariant();

	if (section == 0) {
		return tr("Trigger");
	} else {
		return tr("Snippet Description");
	}
}

Qt::ItemFlags SnippetTableModel::flags(const QModelIndex& index) const
{
	Qt::ItemFlags itemFlags = QAbstractTableModel::flags(index);
	if (index.isValid()) {
		Snippet snippet = fSnippets->GetSnippet(index.row());
		if (!snippet.IsBuiltIn()) {
			itemFlags |= Qt::ItemIsEditable;
		}
	}
	return itemFlags;
}


QModelIndex SnippetTableModel::CreateSnippet()
{
	Snippet snippet;
	beginInsertRows(QModelIndex(), 0, 0);
	int row = fSnippets->Insert(snippet);
	endInsertRows();

	return index(row, 0);
}

void SnippetTableModel::RemoveSnippet(const QModelIndex &index)
{
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	fSnippets->Remove(index.row());
	endRemoveRows();
}

void SnippetTableModel::UpdateSnippet(const QModelIndex &index, const Snippet &snippet)
{
	int row = index.row();

	fSnippets->Remove(index.row());

	int inserted = fSnippets->Insert(snippet);

	if (index.row() == inserted) {
		if (index.column() == 0) {
			emit dataChanged(index, index.sibling(row, 1));
		} else {
			emit dataChanged(index.sibling(row, 0), index);
		}
	} else {
		beginMoveRows(QModelIndex(), row, row, QModelIndex(), row < inserted ? inserted + 1 : inserted);
		endMoveRows();
	}
}

void SnippetTableModel::InsertSnippet(const Snippet& snippet)
{
	int inserted = fSnippets->Insert(snippet);

	beginInsertRows(QModelIndex(), inserted, inserted);
	endInsertRows();
}
