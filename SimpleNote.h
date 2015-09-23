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

#ifndef SIMPLE_NOTE_H
#define SIMPLE_NOTE_H

#include <QString>
#include <QObject>

#include <QNetworkAccessManager>
#include <QNetworkReply>

class QJsonArray;

/*!
	\class  TSimpleNote
	\author Author
	\date Mon Sep 21 2015

	\brief
*/

class SimpleNote : public QObject
{
	Q_OBJECT
public:
	SimpleNote(const QString& user, const QString& password);

	QString		Token();

	bool		HasError() const;
	QString		ErrorMessage() const;

	void		Authenticate(const QString& user, const QString& password);

	QJsonArray	GetNoteList();
	QJsonObject	GetNote(const QString& noteId);
	QJsonObject UpdateNote(const QJsonObject& note);
	bool		DeleteNote(const QString& noteId);

	void		ClearError();

private:


	QUrlQuery	CreateQuery();
	QByteArray	GetRequestWithRetry(const QString& urlString, const QUrlQuery& query);
	QByteArray	PostRequestWithRetry(const QString& urlString, const QUrlQuery& query, const QByteArray& data);
	QByteArray	GetRequest(const QString& urlString, const QUrlQuery& query);
	QByteArray	PostRequest(const QString& urlString, const QUrlQuery& query, const QByteArray& data);


	QNetworkAccessManager*	fAccessManager;
	QString					fUser;
	QString					fPassword;
	QString					fToken;
	QString					fError;
};

#endif
