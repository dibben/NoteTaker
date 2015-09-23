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

#include "Note.h"
#include "SimpleNote.h"

#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QUuid>
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QByteArray>

double ToTimestamp(const QDateTime& time)
{
	return (double)(time.toMSecsSinceEpoch()) / 1000.0;
}

QDateTime FromTimestamp(double timestamp)
{
	qint64 msec = (qint64)(timestamp * 1000);
	return QDateTime::fromMSecsSinceEpoch(msec);
}

QString CreateID()
{
	QString result = QUuid::createUuid().toString();
	result = result.remove("{");
	result = result.remove("}");
	result = result.remove("-");
	return "local-" + result;
}

void UpdateObject(QJsonObject& target, const QJsonObject& source)
{
	for (QJsonObject::const_iterator it = source.constBegin(); it != source.constEnd(); it++) {
		target.insert(it.key(), it.value());
	}
}

/*!
	\brief

*/

Note::NotePtr Note::Create(SimpleNote* server, const QString& id)
{
	QJsonObject obj;
	if (server) {
		obj = server->GetNote(id);
	}

	obj["syncdate"] = ToTimestamp(QDateTime::currentDateTime());
	NotePtr result(new Note(obj));
	result->fNeedsSave = true;

	return result;
}

Note::Note()
{
	fObject["favourite"] = false;
	fObject["key"] = CreateID();
	fObject["createdate"] = QString::number(ToTimestamp(QDateTime::currentDateTime()), 'f', 6);
	fObject["content"] = QString();
	fNeedsSave = true;
}

Note::Note(const QString& contents)
{
	fObject["favourite"] = false;
	fObject["key"] = CreateID();
	fObject["createdate"] = QString::number(ToTimestamp(QDateTime::currentDateTime()), 'f', 6);
	fObject["content"] = contents;
	fNeedsSave = true;
}

Note::Note(const QJsonObject& obj)
{
	fObject = obj;
	fNeedsSave = false;
}


bool Note::IsValid() const
{
	return !ID().isEmpty();
}

QString Note::ID() const
{
	return fObject["key"].toString();
}

QString Note::Title() const
{
	QString text = Text().trimmed();
	if (text.isEmpty()) {
		return "<empty note>";
	} else {
		int p = text.indexOf('\n');
		if (p < -1) return text;
		return text.left(p);
	}
}

QString Note::Text() const
{
	return fObject["content"].toString();
}

int Note::SyncNumber() const
{
	if (fObject.contains("syncnum")) {
		return fObject.value("syncnum").toInt();
	} else {
		return -1;
	}
}

bool Note::NeedsSave() const
{
	return fNeedsSave;
}

bool Note::NeedsSync() const
{
	return IsLocalKey() || ModifiedDate() > SyncDate();
}

bool Note::IsLocalKey() const
{
	return ID().startsWith("local");
}

bool Note::IsDeleted() const
{
	if (fObject.contains("deleted")) {
		return fObject["deleted"].toInt() == 1;
	} else {
		return false;
	}
}

bool Note::IsFavourite() const
{
	return SystemTags().contains("pinned");
}

QDateTime Note::CreatedDate() const
{
	return FromTimestamp(fObject["createdate"].toString().toDouble());
}

QDateTime Note::ModifiedDate() const
{
	if (fObject.contains("modifydate")) {
		return FromTimestamp(fObject["modifydate"].toString().toDouble());
	} else {
		return CreatedDate();
	}
}

QDateTime Note::SyncDate() const
{
	return FromTimestamp(fObject["syncdate"].toDouble());
}

QStringList Note::Tags() const
{
	QJsonArray array = fObject["tags"].toArray();

	QStringList result;
	for (int i = 0; i < array.size(); ++i) {
		result.append(array[i].toString());
	}
	return result;
}

QStringList Note::SystemTags() const
{
	QJsonArray array = fObject["systemtags"].toArray();

	QStringList result;
	for (int i = 0; i < array.size(); ++i) {
		result.append(array[i].toString());
	}
	return result;
}

bool Note::Contains(const QRegExp& expr) const
{
	if (expr.pattern().startsWith("#")) {
		QString pattern = expr.pattern();
		pattern = pattern.right(pattern.length() - 1);
		return Tags().contains(pattern);
	} else {
		return expr.indexIn(Text()) != -1;
	}
}

void Note::SetText(const QString& text)
{
	if (text != Text()) {
		fObject["content"] = text;
		fObject["modifydate"] = QString::number(ToTimestamp(QDateTime::currentDateTime()), 'f', 6);
		fNeedsSave = true;
	}
}

void Note::SetTags(const QStringList& tags)
{
	if (tags != Tags()) {
		fObject["tags"] = QJsonArray::fromStringList(tags);
		fObject["modifydate"] = QString::number(ToTimestamp(QDateTime::currentDateTime()), 'f', 6);
		fNeedsSave = true;
	}
}

void Note::SetFavourite(bool isFavourite)
{
	if (isFavourite == IsFavourite()) return;

	QStringList tags = SystemTags();
	if (isFavourite) {
		tags.append("pinned");
	} else {
		tags.removeAll("pinned");
	}
	fObject["systemtags"] = QJsonArray::fromStringList(tags);
	fObject["modifydate"] = QString::number(ToTimestamp(QDateTime::currentDateTime()), 'f', 6);
	fNeedsSave = true;
}

void Note::MarkDeleted(bool isDeleted)
{
	if (isDeleted == IsDeleted()) return;

	fObject["deleted"] = isDeleted ? 1 : 0;
	fObject["modifydate"] = QString::number(ToTimestamp(QDateTime::currentDateTime()), 'f', 6);
	fNeedsSave = true;
}

QString Note::ToJson() const
{
	QJsonDocument doc(fObject);
	return doc.toJson();
}

void Note::Sync(SimpleNote* server)
{
	if (server == 0) return;

	bool needsUpdate = NeedsSync();
	if (IsLocalKey()) {
		Remove();
		fObject.remove("key");
	}
	QJsonObject result;
	if (needsUpdate) {
		result = server->UpdateNote(fObject);
	} else {
		result = server->GetNote(ID());
	}
	if (!result.isEmpty()) {
		UpdateObject(fObject, result);
		fObject["syncdate"] = ToTimestamp(QDateTime::currentDateTime());
	}

	fNeedsSave = true;

	Save();
}

void Note::Save()
{
	if (!fNeedsSave) return;

	fObject["savedate"] = QString::number(ToTimestamp(QDateTime::currentDateTime()), 'f', 6);

	//TODO move to a common location
	QDir path(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
	if (!path.exists()) {
		path.mkpath(path.absolutePath());
	}
	QFile file(path.absoluteFilePath(ID() + ".json"));

	if (file.open(QIODevice::WriteOnly)) {
		QTextStream str(&file);
		str.setCodec("UTF-8");
		str << ToJson();
		file.close();
	}
	fNeedsSave = false;
}

void Note::Remove()
{
	QDir path(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
	QFile file(path.absoluteFilePath(ID() + ".json"));
	file.remove();
}

Note Note::FromFile(const QString& filename)
{
	QFile file(filename);

	if (!file.open(QIODevice::ReadOnly)) return Note(QJsonObject());

	QTextStream str(&file);
	str.setCodec("UTF-8");
	QString json = str.readAll();

	QJsonParseError error;
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8(), &error);
	if (error.error != QJsonParseError::NoError) {
		return Note(QJsonObject());
	}

	if (!document.isObject()) {
		return Note(QJsonObject());
	}

	return Note(document.object());
}


