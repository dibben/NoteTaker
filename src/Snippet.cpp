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

#include "Snippet.h"

#include <QJsonObject>
#include <QJsonValue>
#include <QDateTime>
#include <QTime>
#include <QClipboard>
#include <QApplication>


Snippet::Snippet()
{
	fIsBuiltin = false;
}

Snippet::Snippet(const QJsonObject& obj)
{
	fTrigger = obj.value("trigger").toString();
	fDescription = obj.value("description").toString();
	fContents = obj.value("contents").toString();
	fIsBuiltin = obj.value("is_builtin").toBool();
}

bool operator==(const Snippet& arg1, const Snippet& arg2)
{
	return (arg1.fTrigger == arg2.fTrigger &&
			arg1.fDescription == arg2.fDescription &&
			arg1.fContents == arg2.fContents &&
			arg1.fIsBuiltin == arg2.fIsBuiltin);
}


bool operator!=(const Snippet& arg1, const Snippet& arg2)
{
	return !(arg1 == arg2);
}

QString Snippet::Trigger() const
{
	return fTrigger;
}

QString Snippet::Description() const
{
	return fDescription;
}

QString Snippet::Contents() const
{
	return fContents;
}


QString Snippet::InsertionText() const
{
	return ReplaceKeywords(fContents);
}


int Snippet::CursorPos() const
{
	int pos = -1;
	QString replacedText = ReplaceKeywords(fContents, &pos);
	if (pos == -1) {
		return replacedText.length();
	} else {
		return pos;
	}
}

QString Snippet::ReplaceKeywords(const QString& text, int* pos) const
{
	QString result = text;

	QRegExp exp("[$].*[$]");
	exp.setMinimal(true);
	int index = exp.indexIn(result);
	while (index >= 0) {
		int length = exp.matchedLength();
		QString var = exp.cap().mid(1, length - 2);

		if (var.length() == 0 && pos != 0) {
			*pos = index;
		}

		QString insert = Replacement(var);
		int shift = insert.length();
		result.replace(index, length, insert);

		index = exp.indexIn(result, index + shift);
	}

	return result;
}

QString Snippet::Replacement(const QString& source) const
{
	if (source.startsWith("date")) {
		return QDate::currentDate().toString();
	}
	if (source.startsWith("time")) {
		return QTime::currentTime().toString();
	}
	if (source.startsWith("now")) {
		return QTime::currentTime().toString();
	}
	if (source.startsWith("today")) {
		return QDate::currentDate().toString();
	}
	if (source == "clipboard") {
		QClipboard* clipboard = QApplication::clipboard();
		return clipboard->text();
	}

	return QString();
}

bool Snippet::IsBuiltIn() const
{
	return fIsBuiltin;
}

void Snippet::SetContents(const QString& text)
{
	fContents = text;
}

void Snippet::SetDesciption(const QString& text)
{
	fDescription = text;
}

void Snippet::SetTrigger(const QString& text)
{
	fTrigger = text;
}

QJsonObject Snippet::AsJson() const
{
	QJsonObject result;
	result.insert("trigger", fTrigger);
	result.insert("description", fDescription);
	result.insert("contents", fContents);
	result.insert("is_builtin", fIsBuiltin);

	return result;
}

