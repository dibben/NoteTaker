/*!
	\file   NoteEditor.h
	\brief The NoteEditor class
	\author Author
	\date $Date: Wed Sep 23 2015 $
	\version $Id: NoteEditor.h $
*/

#ifndef NOTE_EDITOR_H
#define NOTE_EDITOR_H

#include <QTextEdit>

/*!
	\class  NoteEditor
	\author Author
	\date Wed Sep 23 2015

	\brief
*/

class NoteEditor : public QTextEdit
{
	Q_OBJECT
public:
	NoteEditor(QWidget *parent = 0);



public slots:

		void	OpenURL(const QUrl& url);

protected:


virtual void	mousePressEvent(QMouseEvent* e);

private:

		QString FindUrl(const QTextCursor& cursor) const;
		QString FindUrlAtPosition(const QString& text, int pos) const;

};

#endif
