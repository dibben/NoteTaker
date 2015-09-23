/*
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

		void		Export(const QString& filename);

static	Note		FromFile(const QString& filename);

private:

		Note(const QJsonObject& obj);

		bool		fNeedsSave;
		QJsonObject	fObject;
};

#endif
