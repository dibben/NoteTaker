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

#ifndef NOTE_DATABASE_H
#define NOTE_DATABASE_H

#include <Note.h>

#include <QSharedPointer>
#include <QList>

class MessageInterface;
class SimpleNote;
class QProgressDialog;

/*!
	\class  NoteDatabase
	\author Author
	\date Wed Sep 23 2015

	\brief
*/

class NoteDatabase
{
public:

typedef QSharedPointer<Note>	NotePtr;

	NoteDatabase();

	int		Size() const;
	NotePtr	GetNote(int index) const;

	int		FindIndex(const QString& title) const;

	QHash<QString, int>	AllTags() const;

	bool	UpdateNote(int index, const QString& text);
	void	DeleteNote(int index);
	void	AddNote(const QString& text, const QString& tag = QString());
	void	AddNote(NotePtr note);



	void	RemoveNotes(const QList<int>& indices);
	void	RemoveNote(int index);

	void	SortNotes();

	void	LoadNotes();
	void	SaveNotes() const;

	void	SyncLocalNotes(SimpleNote* synchroniser, QProgressDialog* progress = 0);
	void	FullSync(SimpleNote* synchroniser, QProgressDialog* progress = 0);

	void	SetMessageReceiver(MessageInterface* interface);

private:

	QHash<QString,int> NoteKeys() const;
	void	SetMessage(const QString& message);

	QList<NotePtr>		fNotes;
	MessageInterface*	fMessageReceiver;

};

#endif
