
#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
	Q_OBJECT
public:
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

};

#endif // TSETTINGSDIALOG_H
