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

#include "NoteListModel.h"
#include "NoteDatabase.h"

#include <QColor>

typedef QSharedPointer<Note>	NotePtr;

/*!
	\brief

*/

NoteListModel::NoteListModel(NoteDatabase* noteDatabase, QObject* parent) :
		QAbstractListModel(parent)
{
	fNoteDB = noteDatabase;
}


int NoteListModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return fNoteDB->Size();
}


QVariant NoteListModel::data(const QModelIndex& index, int role) const
{
	int row = index.row();
	if (row < 0 || row >= rowCount()) return QVariant();

	NotePtr note = fNoteDB->GetNote(row);

	switch (role) {
		case Qt::DisplayRole:
			return note->Title();
		case Qt::ToolTipRole:
			return QVariant();

		case Qt::UserRole:
			return row;

		case Qt::UserRole + 1:
		{
			QString title = note->Title();
			QString previewText = note->Text().mid(title.length(), 200);
			previewText = previewText.replace('\n', ' ').trimmed();
			return previewText;
		}
		case Qt::UserRole + 2:
			return note->ModifiedDate();

		case Qt::ForegroundRole:
			if (note->Text().isEmpty()) {
				return QColor(Qt::gray);
			} else {
				return QVariant();
			}
		break;
	}
	return QVariant();

}

void NoteListModel::NoteChanged(int row)
{
	emit dataChanged(index(row), index(row));
}

void NoteListModel::ResetModel()
{
	beginResetModel();
	endResetModel();
}
