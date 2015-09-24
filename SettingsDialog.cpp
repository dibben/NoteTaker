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

#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"

#include <QSettings>
#include <QButtonGroup>
#include <QIntValidator>

SettingsDialog::SettingsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);
	fProxyGroup = new QButtonGroup(this);
	fProxyGroup->setExclusive(true);
	fProxyGroup->addButton(ui->fNoProxyButton, kNoProxy);
	fProxyGroup->addButton(ui->fSystemProxyButton, kSystemProxy);
	fProxyGroup->addButton(ui->fSpecifiedProxyButton, kCustomProxy);

	ui->fProxyPortEdit->setValidator(new QIntValidator(1, 65535, this));

	connect(ui->fFontComboBox, SIGNAL(activated(int)), this, SLOT(OnFontSelection()));
	connect(ui->fSizeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(OnFontSelection()));
	connect(ui->fTabSpinBox, SIGNAL(valueChanged(int)), this, SLOT(OnFontSelection()));
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}

QFont SettingsDialog::CurrentFont()
{
	QSettings settings;

	QString fontFamily = settings.value("font_family", QFont().defaultFamily()).toString();
	int fontSize = settings.value("font_size", QFont().pointSize()).toInt();
	if (fontFamily.isEmpty() || fontSize <= 0) {
		return QFont();
	} else {
		return QFont(fontFamily, fontSize);
	}
}

int SettingsDialog::CurrentTabSize()
{
	QSettings settings;
	return settings.value("tab_size", 8).toInt();
}

int SettingsDialog::exec()
{
	QSettings settings;

	//simplenote
	ui->fSimpleNoteCheck->setChecked(settings.value("simpleware_sync", false).toBool());
	ui->fUserIdEdit->setText(settings.value("simpleware_id").toString());
	ui->fPasswordEdit->setText(settings.value("simpleware_password").toString());

	//editor
	QFont font = CurrentFont();
	ui->fFontComboBox->setCurrentFont(font);
	ui->fSizeSpinBox->setValue(font.pointSize());
	ui->fTabSpinBox->setValue(settings.value("tab_size", 8).toInt());

	//proxy
	int proxyType = settings.value("proxy_type", kSystemProxy).toInt();
	fProxyGroup->button(proxyType)->setChecked(true);
	ui->fProxyHostEdit->setText(settings.value("proxy_host").toString());
	ui->fProxyUserEdit->setText(settings.value("proxy_user").toString());
	ui->fProxyPasswordEdit->setText(settings.value("proxy_password").toString());
	ui->fProxyPortEdit->setText(QString::number(settings.value("proxy_port", 80).toInt()));

	return QDialog::exec();
}

void SettingsDialog::accept()
{
	QSettings settings;

	//simplenote
	settings.setValue("simpleware_sync", ui->fSimpleNoteCheck->isChecked());
	settings.setValue("simpleware_id", ui->fUserIdEdit->text());
	settings.setValue("simpleware_password", ui->fPasswordEdit->text());

	//editor
	QFont font = ui->fFontComboBox->currentFont();
	font.setPointSize(ui->fSizeSpinBox->value());

	settings.setValue("font_family", font.family());
	settings.setValue("font_size", font.pointSize());
	settings.setValue("tab_size", ui->fTabSpinBox->value());

	//proxy
	settings.setValue("proxy_type", fProxyGroup->checkedId());
	settings.setValue("proxy_host", ui->fProxyHostEdit->text());
	settings.setValue("proxy_user", ui->fProxyUserEdit->text());
	settings.setValue("proxy_password", ui->fProxyPasswordEdit->text());
	settings.setValue("proxy_port", ui->fProxyPortEdit->text().toInt());

	QDialog::accept();
}

void SettingsDialog::OnFontSelection()
{
	QFont font = ui->fFontComboBox->currentFont();
	font.setPointSize(ui->fSizeSpinBox->value());

	emit FontUpdated(font, ui->fTabSpinBox->value());
}
