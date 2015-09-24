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


#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

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

		SettingsDialog(QWidget *parent = 0);
		~SettingsDialog();


static	QFont	CurrentFont();
static	int		CurrentTabSize();

public slots:

virtual	int		exec();
virtual	void	accept();

signals:

		void	FontUpdated(const QFont& font, int tabsize);

private slots:

		void	OnFontSelection();

private:

		Ui::SettingsDialog* ui;
		QButtonGroup*		fProxyGroup;

};

#endif // TSETTINGSDIALOG_H
