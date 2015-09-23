/*!
	\file   NoteDatabase.h
	\brief The NoteDatabase class
	\author Author
	\date $Date: Wed Sep 23 2015 $
	\version $Id: NoteDatabase.h $
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

	bool	UpdateNote(int index, const QString& text);
	void	DeleteNote(int index);
	void	AddNote(const QString& text);
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
