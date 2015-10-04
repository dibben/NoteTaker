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

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "SettingsDialog.h"
#include "ListDelegate.h"
#include "SpellChecker.h"
#include "SpellingDictionary.h"

#include <QAction>
#include <QKeySequence>
#include <QIcon>
#include <QDir>
#include <QStandardPaths>
#include <QTextCursor>
#include <QTimer>
#include <QSettings>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QProgressDialog>
#include <QFileDialog>
#include <QNetworkProxy>
#include <QNetworkProxyFactory>
#include <QStyledItemDelegate>
#include <QPainter>


typedef QSharedPointer<Note>	NotePtr;

class TagListDelegate : public QStyledItemDelegate
{
public:
	TagListDelegate(QObject* parent = 0);
	void	paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
};

TagListDelegate::TagListDelegate(QObject* parent) :
					QStyledItemDelegate(parent)
{

}

void TagListDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QStyledItemDelegate::paint(painter, option, index);

	QString count = index.data(Qt::UserRole).toString();

	QFont font = option.font;
	QRect r = option.rect;
	painter->setFont(font);
	painter->setPen(Qt::gray);
	r = option.rect.adjusted(0, 0, -10, 0);
	painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignRight, count);
}


MainWindow::MainWindow(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	ui->fEditor->setAcceptRichText(false);

	fSimpleNote = 0;

	ui->fNoteList->setItemDelegate(new ListDelegate(ui->fNoteList));
	ui->fTagsComboBox->view()->setItemDelegate(new TagListDelegate(ui->fTagsComboBox));


	QList<int> sizes;
	sizes << 200 << 400;

	ui->fSplitter->setSizes(sizes);
	fTimer = new QTimer(this);
	fTimer->setInterval(5000);
	fTimer->start();
	connect(fTimer, SIGNAL(timeout()), this, SLOT(OnTimeout()));


	QAction* quitAction = new QAction(tr("Quit"), this);
	quitAction->setShortcut(QKeySequence("Ctrl+Q"));
	connect(quitAction, SIGNAL(triggered(bool)), this, SLOT(close()));
	addAction(quitAction);


	QAction* addNoteAction = new QAction(tr("New Note"), this);
	addNoteAction->setIcon(QIcon(":/images/images/add2.png"));
	addNoteAction->setShortcut(QKeySequence("Ctrl+N"));
	connect(addNoteAction, SIGNAL(triggered(bool)), this, SLOT(OnAdd()));
	ui->fNoteList->addAction(addNoteAction);

	ui->fAddButton->setDefaultAction(addNoteAction);


	QAction* deleteCurrent = new QAction(tr("Remove"), ui->fNoteList);
	deleteCurrent->setIcon(QIcon(":/images/images/delete2.png"));
	connect(deleteCurrent, SIGNAL(triggered(bool)), this, SLOT(RemoveCurrent()));
	ui->fNoteList->addAction(deleteCurrent);
	ui->fNoteList->setContextMenuPolicy(Qt::ActionsContextMenu);


	QAction* syncCurrentAction = new QAction("Sync Current", this);
	syncCurrentAction->setShortcut(QKeySequence("Ctrl+S"));
	connect(syncCurrentAction, SIGNAL(triggered(bool)), this, SLOT(SaveCurrent()));
	addAction(syncCurrentAction);

	QAction* syncAction = new QAction("Full Sync", this);
	syncAction->setShortcut(QKeySequence("Ctrl+Shift+S"));
	connect(syncAction, SIGNAL(triggered(bool)), this, SLOT(FullSync()));
	addAction(syncAction);

	QAction* tagsAction = new QAction("Edit Tags", this);
	tagsAction->setShortcut(QKeySequence("Ctrl+G"));
	connect(tagsAction, SIGNAL(triggered(bool)), this, SLOT(EditTags()));
	addAction(tagsAction);

	QAction* settingsAction = new QAction("Settings...", this);
	connect(settingsAction, SIGNAL(triggered(bool)), this, SLOT(OnSettings()));
	addAction(settingsAction);

	ui->fSettingsButton->addAction(syncCurrentAction);
	ui->fSettingsButton->addAction(syncAction);
	ui->fSettingsButton->addAction(settingsAction);
	ui->fSettingsButton->setPopupMode(QToolButton::InstantPopup);


	connect(ui->fFavouriteButton, SIGNAL(clicked(bool)), this, SLOT(OnFavourite()));


	connect(ui->fEditor, SIGNAL(textChanged()), this, SLOT(OnTextChanged()));
	connect(ui->fSearchEdit, SIGNAL(textChanged(QString)), this, SLOT(OnSearchChanged()));
	connect(ui->fTagsEdit, SIGNAL(textChanged(QString)), this, SLOT(OnTagsChanged()));
	connect(ui->fExportButton, SIGNAL(clicked(bool)), this, SLOT(OnExport()));
	connect(ui->fTagsComboBox, SIGNAL(activated(int)), this, SLOT(OnTagSelected()));
	connect(ui->fEditor, SIGNAL(OpenNote(QString)), this, SLOT(OpenNote(QString)));

	fCurrent = -1;

	fNotes.SetMessageReceiver(this);

	fSnippets.reset(new SnippetCollection);
	ui->fEditor->SetSnippets(fSnippets);

	fNotesModel = new NoteListModel(&fNotes, this);
	fFilterModel = new NoteSortFilterModel(this);
	fFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
	fFilterModel->setSortCaseSensitivity(Qt::CaseInsensitive);
	fFilterModel->setSourceModel(fNotesModel);

	ui->fNoteList->setModel(fFilterModel);

	connect(ui->fNoteList->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(OnSelection(QModelIndex)));

	LoadNotes();
	UpdateNoteList();

	ui->fNoteList->setCurrentIndex(fFilterModel->index(0, 0));
	ui->fNoteList->setFocus();

	RestoreSettings();
	SetEditorFont(SettingsDialog::CurrentFont(), SettingsDialog::CurrentTabSize());
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::OnSearchChanged()
{
	QString searchText = ui->fSearchEdit->text();
	fFilterModel->setFilterRegExp(searchText);
	ui->fEditor->SetSearchText(searchText);
}

void MainWindow::OnTagSelected()
{
	fFilterModel->SetFilterTag(CurrentTag());
}

void MainWindow::SaveCurrent()
{
	if (fCurrent < 0) return;

	QString currentText = ui->fEditor->toPlainText();

	if (fNotes.UpdateNote(fCurrent, currentText)) {
		SetMessage("Note Saved");

		SimpleNote* sync = GetSynchroniser();
		if (sync != 0) {
			fNotes.GetNote(fCurrent)->Sync(sync);
			SetMessage("Note Synchronised");
		}
	} else {
		SetMessage(QString());
	}
}

void MainWindow::SaveAll()
{
	SimpleNote* sync = GetSynchroniser();
	fNotes.SaveNotes();
	fNotes.SyncLocalNotes(sync);
}



void MainWindow::closeEvent(QCloseEvent* ev)
{
	SaveCurrent();
	SaveAll();
	SaveSettings();
	QDialog::closeEvent(ev);
}


void MainWindow::keyPressEvent(QKeyEvent* ev)
{
	if (ev->key() == Qt::Key_Escape) {
		if (ui->fTagsEdit->hasFocus()) {
			ui->fEditor->setFocus();
		} else {
			ui->fNoteList->setFocus();
		}
	} else if (ev->key() == Qt::Key_Return && !ui->fEditor->hasFocus() ) {
		OnEnter();
	} else if (ev->matches(QKeySequence::Find)) {
		ui->fSearchEdit->setFocus();
	} else {
		QDialog::keyPressEvent(ev);
	}
}

void MainWindow::SaveSettings()
{
	QSettings settings;
	settings.setValue("geometry", saveGeometry());
	settings.setValue("splitter", ui->fSplitter->saveState());
	settings.setValue("selected_tag", CurrentTag());
}

void MainWindow::RestoreSettings()
{
	QSettings settings;
	restoreGeometry(settings.value("geometry").toByteArray());
	ui->fSplitter->restoreState(settings.value("splitter").toByteArray());

	SetDictionary();
	SetProxy();
	LoadSnippets();
}

void MainWindow::OnEnter()
{
	if (ui->fNoteList->currentIndex().isValid()) {
		ui->fEditor->setFocus();
		ui->fEditor->moveCursor(QTextCursor::End);
	} else if (!ui->fSearchEdit->text().isEmpty()) {
		OnAdd(ui->fSearchEdit->text());
		ui->fEditor->setFocus();
		ui->fEditor->moveCursor(QTextCursor::End);
	}
}

void MainWindow::OnTimeout()
{
	SaveCurrent();
}

void MainWindow::EditTags()
{
	ui->fTagsEdit->setFocus();
}


void MainWindow::UpdateNoteList()
{

	fNotesModel->ResetModel();
	fFilterModel->sort(0);
	SetEditorEnabled(ui->fNoteList->model()->rowCount() > 0);
}


void MainWindow::LoadSnippets()
{
	fSnippets->Clear();
	QString filename = UserSnippetFile();
	if (QFileInfo::exists(filename)) {
		fSnippets->Load(UserSnippetFile());
	} else {
		fSnippets->RestoreBuiltIn();
	}
	ui->fEditor->SnippetsUpdated();
}

void MainWindow::SaveSnippets() const
{
	fSnippets->Save(UserSnippetFile());
}

QString MainWindow::UserSnippetFile() const
{
	QDir path(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
	if (!path.exists()) {
		path.mkpath(path.absolutePath());
	}
	return path.absoluteFilePath("snippets.json");
}


void MainWindow::OnSelection(const QModelIndex& index)
{
	SaveCurrent();

	if (!index.isValid()) {
		ClearCurrentNote();
	} else {

		//TODO remove int -. use key
		fCurrent = index.data(Qt::UserRole).toInt();
		SetCurrentNote(*fNotes.GetNote(fCurrent));
	}

	UpdateFavouriteIcon();
}

void MainWindow::SetCurrentNote(const Note& note)
{
	ui->fEditor->setText(note.Text());
	ui->fTagsEdit->setText(note.Tags().join(" "));
	ui->fFavouriteButton->setChecked(note.IsFavourite());

	ui->fEditor->SetNoteTitles(fNotes.NoteTitles());
}

void MainWindow::ClearCurrentNote()
{
	fCurrent = -1;
	ui->fFavouriteButton->setChecked(false);
	ui->fEditor->clear();
	ui->fTagsEdit->clear();
}



void MainWindow::OnTextChanged()
{
	if (fCurrent < 0) return;

	NotePtr  note = fNotes.GetNote(fCurrent);
	QString currentText = ui->fEditor->toPlainText();
	note->SetText(currentText);

	fNotesModel->NoteChanged(fCurrent);

	if (note->NeedsSave()) {
		SetMessage(tr("Note modified"));

		fTimer->stop();
		fTimer->start();
	}
}

void MainWindow::OnTagsChanged()
{
	//TODO Factor out function to get current note
	if (fCurrent < 0) return;

	NotePtr  note = fNotes.GetNote(fCurrent);

	QStringList currentTags = ui->fTagsEdit->text().split(" ", QString::SkipEmptyParts);
	note->SetTags(currentTags);
	if (note->NeedsSave()) {
		UpdateTagSelector();
		SetMessage(tr("Note modified"));
	}	
}



void MainWindow::OnAdd(const QString& text)
{
	fNotesModel->AddNote(text, CurrentTag());

	int rows = fNotesModel->rowCount();
	QModelIndex lastIndex = fNotesModel->index(rows - 1);
	QModelIndex index = fFilterModel->mapFromSource(lastIndex);

	OnSelection(index);
	ui->fNoteList->setCurrentIndex(index);
}



void MainWindow::OnSettings()
{
	SaveSnippets();

	SettingsDialog settings(fSnippets);

	connect(&settings, SIGNAL(FontUpdated(QFont, int)), this, SLOT(SetEditorFont(QFont, int)));

	if (settings.exec() == SettingsDialog::Accepted)  {
		if (fSimpleNote) {
			delete fSimpleNote;
			fSimpleNote = 0;
		}
		SetProxy();
		SetDictionary();
		SaveSnippets();
		ui->fEditor->SnippetsUpdated();
	} else {
		LoadSnippets();
	}

	SetEditorFont(settings.CurrentFont(), settings.CurrentTabSize());
}

void MainWindow::OnFavourite()
{
	if (fCurrent < 0) return;

	NotePtr note = fNotes.GetNote(fCurrent);

	note->SetFavourite(ui->fFavouriteButton->isChecked());
	UpdateFavouriteIcon();
	fFilterModel->invalidate();
}

void MainWindow::UpdateFavouriteIcon()
{
	if (ui->fFavouriteButton->isChecked()) {
		ui->fFavouriteButton->setIcon(QIcon(":/images/images/star_yellow.png"));
	} else {
		ui->fFavouriteButton->setIcon(QIcon(":/images/images/star_grey.png"));
	}

}

void MainWindow::SetEditorEnabled(bool enable)
{
	ui->fEditor->setEnabled(enable);
	ui->fTagsEdit->setEnabled(enable);
	ui->fTagsLabel->setEnabled(enable);
	ui->fFavouriteButton->setEnabled(enable);
	ui->fExportButton->setEnabled(enable);
}


void MainWindow::LoadNotes()
{
	fNotes.LoadNotes();
	UpdateTagSelector();
	SetMessage(tr("Notes Loaded"));
}

void MainWindow::SetMessage(const QString& message)
{
	ui->fMessageLabel->setText(message);
}


void MainWindow::OnExport()
{
	QModelIndex current = ui->fNoteList->currentIndex();
	if (!current.isValid()) return;

	QSettings settings;
	QString lastDir = settings.value("export_dir", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString();

	QString filename = QFileDialog::getSaveFileName(this, tr("Save Note As Text"), lastDir, tr("Text Files(*.txt)"));
	if (!filename.isEmpty()) {
		int index = current.data(Qt::UserRole).toInt();
		fNotes.GetNote(index)->Export(filename);
	}
}

void MainWindow::RemoveCurrent()
{
	QModelIndex current = ui->fNoteList->currentIndex();
	if (!current.isValid()) return;
	int index = current.data(Qt::UserRole).toInt();

	fNotes.DeleteNote(index);

	fCurrent = -1;
	//TODO use signal from note instead
	fFilterModel->invalidate();

	if (current.row() >= ui->fNoteList->model()->rowCount()) {
		current = ui->fNoteList->model()->index(current.row() - 1, 0);
	} else {
		current = ui->fNoteList->model()->index(current.row(), 0);
	}
	OnSelection(current);
	ui->fNoteList->setCurrentIndex(current);
}

QString MainWindow::CurrentTag() const
{
	if (ui->fTagsComboBox->currentIndex() == 0) return QString();

	return ui->fTagsComboBox->currentText();
}


void MainWindow::UpdateTagSelector()
{
	QString currentTag = CurrentTag();

	ui->fTagsComboBox->clear();
	ui->fTagsComboBox->addItem(tr("All Notes"));

	QHash<QString, int> tagList = fNotes.AllTags();
	QStringList tagNames = tagList.keys();
	tagNames.sort();
	for (int eachTag = 0; eachTag < tagNames.size(); ++eachTag) {
		QString tag = tagNames[eachTag];
		ui->fTagsComboBox->addItem(tag, tagList[tag]);
	}

	if (currentTag.isEmpty()) {
		ui->fTagsComboBox->setCurrentIndex(0);
	} else {
		int pos = ui->fTagsComboBox->findText(currentTag);
		if (pos < 0) pos = 0;
		ui->fTagsComboBox->setCurrentIndex(pos);
	}
}

SimpleNote* MainWindow::GetSynchroniser()
{
	if (fSimpleNote == 0) {
		QSettings settings;

		bool useSync = settings.value("simpleware_sync", false).toBool();
		QString id = settings.value("simpleware_id").toString();
		QString password = settings.value("simpleware_password").toString();
		if (useSync) {
			fSimpleNote = new SimpleNote(id, password);
		}

		if (fSimpleNote) {
			fSimpleNote->Token();
			if (fSimpleNote->HasError()) {
				QMessageBox::warning(this, tr("Cannot logon to Simplenote"), tr("Bad user id / password."));
				delete fSimpleNote;
				fSimpleNote = 0;
			}
		}
	}
	if (fSimpleNote) {
		fSimpleNote->ClearError();
	}

	return fSimpleNote;
}



void MainWindow::FullSync()
{
	SimpleNote* synchroniser = GetSynchroniser();
	if (synchroniser == 0) return;

	QString id = ui->fNoteList->currentIndex().data(NoteListModel::kID).toString();

	QProgressDialog progress(this);
	fNotes.FullSync(synchroniser, &progress);

	UpdateNoteList();

	int rows = ui->fNoteList->model()->rowCount();
	for (int eachRow = 0; eachRow < rows; ++eachRow) {
		QModelIndex index = ui->fNoteList->model()->index(eachRow, 0);
		if (index.data(NoteListModel::kID).toString() == id) {
			OnSelection(index);
			ui->fNoteList->setCurrentIndex(index);
			break;
		}
	}

	UpdateTagSelector();
}



void MainWindow::SetEditorFont(const QFont& font, int tabSize)
{
	ui->fEditor->SetFont(font, tabSize);
}

int MainWindow::FindRow(int index) const
{
	int row = -1;
	int numDisplayed = ui->fNoteList->model()->rowCount();
	for (int eachNote = 0; eachNote < numDisplayed; ++eachNote) {
		//TODO cleanup the mess
		int id = ui->fNoteList->model()->index(eachNote, 0).data(Qt::UserRole).toInt();
		if (id == index) {
			row = eachNote;
			break;
		}
	}

	return row;
}

void MainWindow::OpenNote(const QString& title)
{
	int index = fNotes.FindIndex(title);
	if (index == -1) return;

	int row = FindRow(index);

	if (row == -1) {
		ui->fTagsComboBox->setCurrentIndex(0);
		row = FindRow(index);
	}
	if (row == -1) {
		ui->fSearchEdit->clear();
		row = FindRow(index);
	}
	if (row == -1) return;


	QModelIndex current = ui->fNoteList->model()->index(row, 0);
	OnSelection(current);
	ui->fNoteList->setCurrentIndex(current);

}

void MainWindow::SetDictionary()
{
	ui->fEditor->SetSpellingLanguage(SettingsDialog::CurrentLanguage());
	ui->fEditor->SetSpellingCheckEnabled(SettingsDialog::CheckSpelling());
}

void MainWindow::SetProxy()
{
	QSettings settings;
	int proxyType = settings.value("proxy_type", SettingsDialog::kSystemProxy).toInt();
	if (proxyType == SettingsDialog::kNoProxy) {
		QNetworkProxy proxy(QNetworkProxy::NoProxy);
		QNetworkProxy::setApplicationProxy(proxy);
	} else if (proxyType == SettingsDialog::kSystemProxy) {
		QNetworkProxyFactory::setUseSystemConfiguration(true);
	} else {
		QString host = settings.value("proxy_host").toString();
		QString user = settings.value("proxy_user").toString();
		QString password = settings.value("proxy_password").toString();
		int port = settings.value("proxy_port", 80).toInt();

		QNetworkProxy proxy(QNetworkProxy::HttpProxy);
		proxy.setHostName(host);
		proxy.setPort(port);
		if (!user.isEmpty()) {
			proxy.setUser(user);
			proxy.setPassword(password);
		}
		QNetworkProxy::setApplicationProxy(proxy);
	}
}


