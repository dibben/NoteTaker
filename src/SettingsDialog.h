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


#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include "SnippetCollection.h"

#include <QDialog>
#include <QSharedPointer>

namespace Ui {
class SettingsDialog;
}

class SnippetTableModel;
class QButtonGroup;

class SettingsDialog : public QDialog
{
	Q_OBJECT
public:

enum EProxyType {
	kNoProxy,
	kSystemProxy,
	kCustomProxy
};

		SettingsDialog(QSharedPointer<SnippetCollection> snippets, QWidget *parent = 0);
		~SettingsDialog();


static	QFont	CurrentFont();
static	int		CurrentTabSize();
static	QString	CurrentLanguage();
static	bool	CheckSpelling();

public slots:

virtual	int		exec();
virtual	void	accept();

signals:

		void	FontUpdated(const QFont& font, int tabsize);

private slots:

		void	OnFontSelection();
		void	OnAddSnippet();
		void	OnRemoveSnippet();
		void	OnSnippetChanged(const QModelIndex& current, const QModelIndex& previous);
		void	SnippetTextChanged();
		void	OnRestoreBuiltIn();

private:

		Ui::SettingsDialog*		ui;
		QButtonGroup*						fProxyGroup;
		SnippetTableModel*					fSnippetModel;
		QSharedPointer<SnippetCollection>	fSnippets;

};

#endif // TSETTINGSDIALOG_H
