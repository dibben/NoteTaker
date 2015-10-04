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

#include "CompleterModel.h"

#include "Snippet.h"
#include <QVariant>

#include <QMessageBox>

CompleterModel::CompleterModel(QSharedPointer<SnippetCollection> snippets, QObject* parent) :
	QAbstractListModel(parent)
{
	fSnippets = snippets;
}


int CompleterModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return fSnippets->Count() + fTitles.count();
}

QVariant CompleterModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid() || index.row() > rowCount()) return QVariant();


	int numSnippets = fSnippets->Count();
	bool isTitle = index.row() >= numSnippets;
	int titleIndex = index.row() - numSnippets;

	Snippet snippet;
	if (!isTitle) {
		snippet = fSnippets->GetSnippet(index.row());
	}

	switch (role) {
		case Qt::DisplayRole:
			if (isTitle) {
				return fTitles[titleIndex];
			} else {
				return QString("%1 %2").arg(snippet.Trigger(), -10).arg(snippet.Description());
			}

		case Qt::EditRole:
			if (isTitle) {
				return "[[" + fTitles[titleIndex];
			} else {
				return snippet.Trigger();
			}

		case Qt::ToolTipRole:
			if (isTitle) {
				return fTitles[titleIndex];
			} else {
				return snippet.InsertionText().toHtmlEscaped();
			}

		case Qt::UserRole:
			if (isTitle) {
				return QVariant(fTitles[titleIndex]);
			} else {
				return QVariant::fromValue(snippet);
			}

		case kCompletionText:
			if (isTitle) {
				return "[[" + fTitles[titleIndex] + "]] ";
			} else {
				return snippet.InsertionText();
			}
		case kCursorPosition:
			if (isTitle) {
				return fTitles[titleIndex].size() + 5;
			} else {
				return snippet.CursorPos();
			}

		break;
	}
	return QVariant();
}

Snippet CompleterModel::GetSnippet(const QModelIndex& index) const
{
	int numSnippets = fSnippets->Count();
	if (index.row() >= numSnippets) {
		return Snippet();
	} else {
		return fSnippets->GetSnippet(index.row());
	}
}

void CompleterModel::ResetModel()
{
	beginResetModel();
	endResetModel();
}


void CompleterModel::SetTitles(const QStringList& noteTitles)
{
	fTitles = noteTitles;
	ResetModel();
}


