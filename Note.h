/*!
	\file   Note.h
	\brief The Note class
	\author Author
	\date $Date: Sun Sep 20 2015 $
	\version $Id: TNote.h $
*/

#ifndef NOTE_H
#define NOTE_H

#include <QString>
#include <QJsonObject>
#include <QDateTime>
#include <QSharedPointer>

class QRegExp;
class SimpleNote;

/*!
	\class  TNote
	\author Author
	\date Sun Sep 20 2015

	\brief
*/

class Note
{
public:

typedef QSharedPointer<Note>	NotePtr;

static	NotePtr	Create(SimpleNote* server, const QString& id);

		Note();
		Note(const QString& contents);

		bool		IsValid() const;
		QString		ID() const;
		QString		Title() const;
		QString		Text() const;
		int			SyncNumber() const;
		bool		NeedsSave() const;
		bool		NeedsSync() const;
		bool		IsLocalKey() const;
		bool		IsDeleted() const;

		bool		IsFavourite() const;
		QDateTime	CreatedDate() const;
		QDateTime	ModifiedDate() const;
		QDateTime	SyncDate() const;
		QStringList	Tags() const;
		QStringList	SystemTags() const;


		bool		Contains(const QRegExp& expr) const;

		void		SetText(const QString& text);
		void		SetTags(const QStringList& tags);
		void		SetFavourite(bool isFavourite);
		void		MarkDeleted(bool isDeleted);

		QString		ToJson() const;

		void		Sync(SimpleNote* server);
		void		Save();
		void		Remove();

static	Note		FromFile(const QString& filename);

private:

		Note(const QJsonObject& obj);

		bool		fNeedsSave;
		QJsonObject	fObject;
};

#endif
