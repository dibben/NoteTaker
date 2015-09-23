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

#include "SimpleNote.h"

#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QEventLoop>
#include <QTimer>
#include <QNetworkCookie>
#include <QNetworkCookieJar>

namespace {
	const char* kAuthUrl  = "https://simple-note.appspot.com/api/login";
	const char* kDataUrl  = "https://simple-note.appspot.com/api2/data";
	const char* kIndexUrl = "https://simple-note.appspot.com/api2/index";
}


void WaitForReply(QNetworkReply* reply)
{
	if (reply == 0) return;

	QEventLoop loop;

	QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
	if (reply->isRunning()) {
		loop.exec(QEventLoop::ExcludeUserInputEvents);
	}
}


/*!
	\brief
*/
SimpleNote::SimpleNote(const QString& user, const QString& password)
{
	fAccessManager = new QNetworkAccessManager(this);

	fUser = user;
	fPassword = password;
}

QString SimpleNote::Token()
{
	if (fToken.isEmpty()) {
		Authenticate(fUser, fPassword);
	}
	return fToken;
}

bool SimpleNote::HasError() const
{
	return !fError.isEmpty();
}

QString SimpleNote::ErrorMessage() const
{
	return fError;
}

void SimpleNote::Authenticate(const QString& user, const QString& password)
{
	QString values = QString("email=%1&password=%2").arg(user.trimmed()).arg(password.trimmed());
	QByteArray result = PostRequest(kAuthUrl, QUrlQuery(), values.toUtf8().toBase64());

	fToken = result;
	if (fToken.isEmpty()) {
		fError = tr("Authentication Failed");
	}
}


QJsonArray SimpleNote::GetNoteList()
{
	ClearError();

	QJsonArray notes;

	QString mark;
	do {
		QUrlQuery query = CreateQuery();
		query.addQueryItem("length", "20");
		if (!mark.isEmpty()) {
			query.addQueryItem("mark", mark);
		}

		QByteArray response = GetRequestWithRetry(kIndexUrl, query);

		if (HasError()) break;

		QJsonDocument document = QJsonDocument::fromJson(response);
		QJsonArray data = document.object()["data"].toArray();
		for (int i = 0; i < data.size(); ++i) {
			notes.append(data[i]);
		}

		mark = document.object()["mark"].toString();
	} while (!mark.isEmpty());

	return notes;
}



QJsonObject SimpleNote::GetNote(const QString& noteId)
{
	QByteArray response = GetRequestWithRetry(QString(kDataUrl) + "/" + noteId, CreateQuery());

	QJsonDocument document = QJsonDocument::fromJson(response);
	return document.object();
}

QJsonObject SimpleNote::UpdateNote(const QJsonObject& note)
{
	QString url = kDataUrl;
	if (note.contains("key") && !note["key"].toString().isEmpty()) {
		url += "/" + note["key"].toString();
	}

	QByteArray response = PostRequestWithRetry(url, CreateQuery(), QJsonDocument(note).toJson());

	QJsonDocument document = QJsonDocument::fromJson(response);
	return document.object();
}


QUrlQuery SimpleNote::CreateQuery()
{
	QUrlQuery query;
	query.addQueryItem("email", fUser.trimmed());
	query.addQueryItem("auth", Token());
	return query;
}

QByteArray SimpleNote::GetRequestWithRetry(const QString& urlString, const QUrlQuery& query)
{
	QByteArray result = GetRequest(urlString, query);
	if (HasError() && fToken.isEmpty()) {
		Token();
		result = GetRequest(urlString, query);
	}
	return result;
}

QByteArray SimpleNote::PostRequestWithRetry(const QString& urlString, const QUrlQuery& query, const QByteArray& data)
{
	QByteArray result = PostRequest(urlString, query, data);
	if (HasError() && fToken.isEmpty()) {
		Token();
		result = PostRequest(urlString, query, data);
	}
	return result;
}

QByteArray SimpleNote::GetRequest(const QString& urlString, const QUrlQuery& query)
{
	ClearError();

	QUrl url(urlString);
	url.setQuery(query);
	QNetworkRequest request(url);
	request.setRawHeader("User-Agent", "NoteTaker 1.0");

	QNetworkReply* reply = fAccessManager->get(request);
	WaitForReply(reply);

	QByteArray result;
	if (reply == 0 || reply->error() != QNetworkReply::NoError) {
		if (reply->error() == 401) {
			fError = tr("Failed Authorisation (Get)");
			fToken = QString();
		} else {
			fError = tr("A server error occured (%1)").arg(reply->error());
		}
	} else {
		result = reply->readAll();
	}

	if (reply) {
		delete reply;
	}

	return result;
}

QByteArray SimpleNote::PostRequest(const QString& urlString, const QUrlQuery& query, const QByteArray& data)
{
	ClearError();
	QUrl url(urlString);
	if (!query.isEmpty()) {
		url.setQuery(query);
	}
	QNetworkRequest request(url);
	request.setRawHeader("User-Agent", "NoteTaker 1.0");
	request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");

	QNetworkReply* reply = fAccessManager->post(request, data);
	WaitForReply(reply);

	QByteArray result;
	if (reply == 0 || reply->error() != QNetworkReply::NoError) {
		if (reply->error() == 401) {
			fError = tr("Failed Authorisation (post)");
			fToken = QString();
		} else {
			fError = tr("A server error occured (%1)").arg(reply->error());
		}
	} else {
		result =  reply->readAll();
	}

	if (reply) {
		delete reply;
	}

	return result;
}


void SimpleNote::ClearError()
{
	fError.clear();
}
