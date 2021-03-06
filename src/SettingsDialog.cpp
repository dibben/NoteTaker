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

#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"
#include "SpellingDictionary.h"
#include "SnippetTableModel.h"

#include <QSettings>
#include <QButtonGroup>
#include <QIntValidator>

SettingsDialog::SettingsDialog(QSharedPointer<SnippetCollection> snippets, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SettingsDialog)
{
	fSnippets = snippets;

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


	QList<SpellingDictionary> dictionaries = SpellingDictionary::SystemDictionaries();

	foreach (SpellingDictionary dictionary, dictionaries) {
		ui->fDictionaryCombo->addItem(dictionary.Title(), dictionary.Language());
	}

	fSnippetModel = new SnippetTableModel(snippets, ui->fSnippetTable);
	ui->fSnippetTable->setModel(fSnippetModel);
	connect(ui->fSnippetTable->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
			this, SLOT(OnSnippetChanged(QModelIndex,QModelIndex)));
	connect(ui->fSnippetEdit, SIGNAL(textChanged()), this, SLOT(SnippetTextChanged()));

	connect(ui->fAddSnippetButton, SIGNAL(clicked(bool)), this, SLOT(OnAddSnippet()));
	connect(ui->fRemoveSnippetButton, SIGNAL(clicked(bool)), this, SLOT(OnRemoveSnippet()));
	connect(ui->fRestoreSnippetButton, SIGNAL(clicked(bool)), this, SLOT(OnRestoreBuiltIn()));

	ui->fSnippetTable->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->fSnippetTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->fSnippetTable->horizontalHeader()->setStretchLastSection(true);

	OnSnippetChanged(QModelIndex(), QModelIndex());
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

QString SettingsDialog::CurrentLanguage()
{
	QSettings settings;
	return settings.value("spelling_language", QLocale::system().name()).toString();
}

bool SettingsDialog::CheckSpelling()
{
	QSettings settings;
	return settings.value("check_spelling", true).toBool();
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

	ui->fCheckSpellingBox->setChecked(settings.value("check_spelling", true).toBool());
	QString language = settings.value("spelling_language", QLocale::system().name()).toString();
	int pos = ui->fDictionaryCombo->findData(language);
	if (pos == -1) {
		pos = ui->fDictionaryCombo->findData(QLocale::system().name());
	}
	if (pos != -1) {
		ui->fDictionaryCombo->setCurrentIndex(pos);
	}

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
	settings.setValue("spelling_language", ui->fDictionaryCombo->currentData());
	settings.setValue("check_spelling", ui->fCheckSpellingBox->isChecked());

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

void SettingsDialog::OnAddSnippet()
{
	QModelIndex index = fSnippetModel->CreateSnippet();

	ui->fSnippetTable->setCurrentIndex(index);
	ui->fSnippetTable->scrollTo(index);

	ui->fSnippetTable->edit(index);
}

void SettingsDialog::OnRemoveSnippet()
{
	QModelIndex modelIndex = ui->fSnippetTable->selectionModel()->currentIndex();

	if (!modelIndex.isValid()) return;

	fSnippetModel->RemoveSnippet(modelIndex);
}


void SettingsDialog::OnSnippetChanged(const QModelIndex& current, const QModelIndex& previous)
{
	Q_UNUSED(previous);

	ui->fRemoveSnippetButton->setEnabled(current.isValid());
	ui->fSnippetEdit->setEnabled(current.isValid());

	if (current.isValid()) {
		Snippet snippet = fSnippets->GetSnippet(current.row());
		ui->fSnippetEdit->setReadOnly(snippet.IsBuiltIn());
		ui->fSnippetEdit->setPlainText(snippet.Contents());
	} else {
		ui->fSnippetEdit->clear();
	}

}

void SettingsDialog::SnippetTextChanged()
{
	QModelIndex modelIndex = ui->fSnippetTable->selectionModel()->currentIndex();
	if (!modelIndex.isValid())  return;

	Snippet snippet = fSnippets->GetSnippet(modelIndex.row());
	if (snippet.IsBuiltIn()) return;

	snippet.SetContents(ui->fSnippetEdit->toPlainText());

	fSnippets->Update(modelIndex.row(), snippet);
}

void SettingsDialog::OnRestoreBuiltIn()
{
	SnippetCollection builtin;
	builtin.RestoreBuiltIn();

	for (int eachSnipppet = 0; eachSnipppet < builtin.Count(); ++eachSnipppet) {
		Snippet s = builtin.GetSnippet(eachSnipppet);
		if (!fSnippets->Contains(s)) {
			fSnippetModel->InsertSnippet(s);
		}
	}
}
