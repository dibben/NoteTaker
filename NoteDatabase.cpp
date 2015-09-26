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

#include "NoteDatabase.h"
#include "MessageInterface.h"
#include "SimpleNote.h"

#include <QDir>
#include <QStandardPaths>
#include <QProgressDialog>
#include <QJsonArray>

/*!
	\brief

*/

namespace {
	typedef QSharedPointer<Note>	NotePtr;

	bool Compare(const NotePtr& n1, const NotePtr& n2)
	{
		if (n1->IsFavourite() == n2->IsFavourite()) {
			return (n1->CreatedDate() < n2->CreatedDate());
		} else {
			return n1->IsFavourite();
		}
	}

}

NoteDatabase::NoteDatabase()
{
	fMessageReceiver = 0;
}

int NoteDatabase::Size() const
{
	return fNotes.size();
}

NoteDatabase::NotePtr NoteDatabase::GetNote(int index) const
{
	if (index < 0 || index >= Size()) return NotePtr();
	return fNotes[index];
}

QHash<QString, int> NoteDatabase::AllTags() const
{
	QHash<QString, int> tagSet;

	int numNotes = fNotes.size();
	for (int eachNote = 0; eachNote < numNotes; ++eachNote) {
		QStringList tags = fNotes[eachNote]->Tags();
		foreach (QString tag, tags) {
			if (tagSet.contains(tag)) {
				tagSet[tag]++;
			} else {
				tagSet[tag] = 1;
			}
		}
	}

	return tagSet;
}

bool NoteDatabase::UpdateNote(int index, const QString& text)
{
	if (index < 0 || index >= Size()) return false;

	fNotes[index]->SetText(text);
	if (fNotes[index]->NeedsSave()) {
		fNotes[index]->Save();
		return true;
	} else {
		return false;
	}

}

void NoteDatabase::DeleteNote(int index)
{
	if (index < 0 || index >= Size()) return;
	fNotes[index]->MarkDeleted(true);
	fNotes[index]->Save();
}

void NoteDatabase::AddNote(const QString& text, const QString& tag)
{
	NotePtr note(new Note(text));
	if (!tag.isEmpty()) {
		note->SetTags(QStringList() << tag);
	}
	fNotes.append( note );
}

void NoteDatabase::AddNote(NotePtr note)
{
	if (note.isNull()) return;

	fNotes.append(note);
	note->Save();
}

void NoteDatabase::RemoveNotes(const QList<int>& indices)
{
	QList<int> deletedNotes = indices;
	std::sort(deletedNotes.begin(), deletedNotes.end());
	for (int i = deletedNotes.size() - 1; i >= 0; --i) {
		RemoveNote(deletedNotes[i]);
	}
}

void NoteDatabase::RemoveNote(int index)
{
	if (index < 0 || index >= Size()) return;
	fNotes[index]->Remove();
	fNotes.removeAt(index);
}

void NoteDatabase::LoadNotes()
{
	QDir path(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
	if (!path.exists()) {
		path.mkpath(path.absolutePath());
	}
	QStringList noteFiles = path.entryList(QStringList() << "*.json", QDir::Files);
	foreach (QString eachFile, noteFiles) {
		NotePtr note(new Note( Note::FromFile(path.absoluteFilePath(eachFile))));
		if (note->IsValid()) {
			fNotes.append(note);
		}
	}

	SortNotes();
}

void NoteDatabase::SaveNotes() const
{
	int numNotes = fNotes.size();
	for (int eachNote = 0; eachNote < numNotes; ++eachNote) {
		fNotes[eachNote]->Save();
	}
}

void NoteDatabase::SyncLocalNotes(SimpleNote* synchroniser, QProgressDialog* progress)
{
	if (synchroniser == 0) return;

	int numNotes = Size();
	for (int eachNote = 0; eachNote < numNotes; ++eachNote) {
		if (progress) progress->setValue(eachNote);
		if (GetNote(eachNote)->NeedsSync()) {
			SetMessage("Syncing local note");
			GetNote(eachNote)->Sync(synchroniser);
		}
	}
}



void NoteDatabase::SortNotes()
{
	std::sort(fNotes.begin(), fNotes.end(), Compare);
}

void NoteDatabase::FullSync(SimpleNote* synchroniser, QProgressDialog* progress)
{
	SetMessage("Doing full sync");

	int numNotes = Size();

	if (progress) {
		progress->setCancelButton(0);
		progress->setMaximum(numNotes);
		progress->setLabelText(QObject::tr("Syncing local notes"));
	}

	SyncLocalNotes(synchroniser, progress);

	if (synchroniser->HasError()) {
		SetMessage(synchroniser->ErrorMessage());
		return;
	}

	SetMessage("Getting note list");

	QJsonArray list = synchroniser->GetNoteList();
	if (list.isEmpty() || synchroniser->HasError()) {
		SetMessage(synchroniser->ErrorMessage());
		return;
	}

	if (progress) {
		progress->setMaximum(list.size());
		progress->setLabelText(QObject::tr("Syncing remote notes"));
	}

	QHash<QString, int> localKeys = NoteKeys();

	for (int eachNote = 0; eachNote < list.size(); ++eachNote) {
		if (progress) progress->setValue(eachNote);
		QJsonObject serverNote  = list[eachNote].toObject();
		QString key = serverNote["key"].toString();
		int syncNum = serverNote["syncnum"].toInt();
		if (localKeys.contains(key)) {
			NotePtr note = GetNote(localKeys[key]);
			if (syncNum > note->SyncNumber()) {
				note->Sync(synchroniser);
			}
			localKeys.remove(key);
		} else {
			AddNote(Note::Create(synchroniser, key));
		}
	}

	SetMessage("Removing local notes");
	RemoveNotes(localKeys.values());


	SortNotes();
}

QHash<QString, int> NoteDatabase::NoteKeys() const
{
	QHash<QString, int> localKeys;
	int numNotes = Size();
	for (int eachNote = 0; eachNote < numNotes; ++eachNote) {
		localKeys.insert(GetNote(eachNote)->ID(), eachNote);
	}

	return localKeys;
}

void NoteDatabase::SetMessageReceiver(MessageInterface* interface)
{
	fMessageReceiver = interface;
}

void NoteDatabase::SetMessage(const QString& message)
{
	if (fMessageReceiver) {
		fMessageReceiver->SetMessage(message);
	}
}


