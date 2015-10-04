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

#ifndef NOTE_LIST_MODEL_H
#define NOTE_LIST_MODEL_H


#include <QAbstractListModel>

class NoteDatabase;
class Note;

/*!
	\class  NoteListModel
	\author Author
	\date Sun Oct 4 2015

	\brief
*/

class NoteListModel : public QAbstractListModel
{
public:

enum {
	kIndex = Qt::UserRole,
	kPreviewText,
	kDate,
	kDeleted,
	kTags,
	kIsFavourite,
	kID
};

typedef QSharedPointer<Note>	NotePtr;

		NoteListModel(NoteDatabase* noteDatabase, QObject *parent = 0);


virtual int			rowCount(const QModelIndex& parent = QModelIndex()) const;
virtual QVariant	data(const QModelIndex& index, int role) const;

		QModelIndex	index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const;

		void		NoteChanged(int row);

		void		ResetModel();

public slots:

		void		AddNote(const QString& text, const QString& tag = QString());
		void		AddNote(NotePtr note);
		void		RemoveNote(int index);

private:

		NoteDatabase*	fNoteDB;
};

#endif
