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

#include "SnippetCollection.h"

#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

/*!
	\brief

*/

SnippetCollection::SnippetCollection()
{
}

int SnippetCollection::Count() const
{
	return fSnippets.count();
}

bool SnippetCollection::Contains(const QString& trigger) const
{
	return fSnippets.contains(trigger);
}

bool SnippetCollection::Contains(const Snippet& snippet) const
{
	QList<Snippet> list = Find(snippet.Trigger());
	foreach (const Snippet& s, list) {
		if (s == snippet) return true;
	}
	return false;
}

QList<Snippet> SnippetCollection::Find(const QString& trigger) const
{
	return fSnippets.values(trigger);
}

Snippet SnippetCollection::GetSnippet(int index) const
{
	return (fSnippets.begin() + index).value();
}

int SnippetCollection::Insert(const Snippet& snippet)
{
	QMultiMap<QString, Snippet>::iterator it = fSnippets.insert(snippet.Trigger(), snippet);
	return std::distance(fSnippets.begin(), it);
}

void SnippetCollection::Remove(const Snippet& snippet)
{
	fSnippets.remove(snippet.Trigger(), snippet);
}

void SnippetCollection::Remove(int index)
{
	Remove(GetSnippet(index));
}

void SnippetCollection::Update(int index, const Snippet& snippet)
{
	Snippet current = GetSnippet(index);
	if (snippet != current) {
		Remove(index);
		Insert(snippet);
	}
}

void SnippetCollection::Clear()
{
	fSnippets.clear();
}

void SnippetCollection::Load(const QString& filename)
{
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly)) return;

	QTextStream str(&file);
	str.setCodec("UTF-8");

	QJsonDocument doc = QJsonDocument::fromJson(str.readAll().toUtf8());
	QJsonArray array = doc.array();
	for (int eachSnippet = 0; eachSnippet < array.count(); ++eachSnippet) {
		Insert(Snippet(array[eachSnippet].toObject()));
	}
}

void SnippetCollection::Save(const QString& filename)
{
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly)) return;

	QJsonArray array;
	QMapIterator<QString, Snippet> it(fSnippets);
	while (it.hasNext()) {
		it.next();
		array.append(it.value().AsJson());
	}
	QJsonDocument doc(array);

	QTextStream str(&file);
	str.setCodec("UTF-8");
	str << doc.toJson();
	file.close();
}

void SnippetCollection::RestoreBuiltIn()
{
	SnippetCollection builtin;
	builtin.Load(":/builtin-snippets.json");

	for (int eachSnipppet = 0; eachSnipppet < builtin.Count(); ++eachSnipppet) {
		Snippet s = builtin.GetSnippet(eachSnipppet);
		if (!Contains(s)) {
			Insert(s);
		}
	}
}

