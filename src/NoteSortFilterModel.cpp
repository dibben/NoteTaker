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

#include "NoteSortFilterModel.h"
#include "NoteListModel.h"

#include <QDateTime>

/*!
	\brief

*/

NoteSortFilterModel::NoteSortFilterModel(QObject *parent) :
	QSortFilterProxyModel(parent)
{
}

void NoteSortFilterModel::SetFilterTag(const QString& tagString)
{
	if (fFilterTag != tagString) {
		fFilterTag = tagString;
		invalidateFilter();
	}
}

bool NoteSortFilterModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
	bool leftFav = sourceModel()->data(left, NoteListModel::kIsFavourite).toBool();
	bool rightFav = sourceModel()->data(right, NoteListModel::kIsFavourite).toBool();

	if (leftFav != rightFav) {
		return leftFav;
	}

	QVariant leftData = sourceModel()->data(left, NoteListModel::kDate);
	QVariant rightData = sourceModel()->data(right, NoteListModel::kDate);

	return leftData.toDateTime() > rightData.toDateTime();
}

bool NoteSortFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
	if (!QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent)) return false;

	QModelIndex current = sourceModel()->index(sourceRow, 0);
	if (current.data(NoteListModel::kDeleted).toBool()) return false;

	if (!fFilterTag.isEmpty()) {
		QStringList tags = current.data(NoteListModel::kTags).toStringList();
		if (!tags.contains(fFilterTag)) {
			return false;
		}
	}

	return true;
}
