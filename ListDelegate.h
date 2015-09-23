/*!
	\file   ListDelegate.h
	\brief The ListDelegate class
	\author Author
	\date $Date: Tue Sep 22 2015 $
	\version $Id: ListDelegate.h $
*/

#ifndef LIST_DELEGATE_H
#define LIST_DELEGATE_H

#include <QAbstractItemDelegate>

/*!
	\class  ListDelegate
	\author David Dibben
	\date Tue Sep 22 2015

	\brief A QListItemView delegate to draw the note list
*/

class ListDelegate : public QAbstractItemDelegate
{
	Q_OBJECT
public:

	ListDelegate(QObject *parent = 0);

	void	paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
	QSize	sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index ) const;

	bool	IsSelected(const QStyleOptionViewItem& option) const;


private:

	QString FormatDate(const QDateTime& datetime) const;

	void	DrawBodyText(QPainter* painter, const QStyleOptionViewItem& option, const QRect& r, const QString& text) const;
	void	DrawHighlightedItem(QPainter* painter, const QRect& r) const;
	void	DrawStandardItem(QPainter* painter, const QRect& r) const;
	void	DrawItemOutline(QPainter* painter, const QRect& r) const;
	void	DrawItem(QPainter* painter, const QRect& r, const QPen& linePen, QBrush brush) const;
	void	DrawTitle(QPainter* painter, const QStyleOptionViewItem& option, const QRect& r, const QString& title) const;
	void	DrawDate(QPainter* painter, const QStyleOptionViewItem& option, const QRect& r, const QDateTime& date) const;
	int		DrawImage(QPainter* painter, const QStyleOptionViewItem& option, const QRect& r, const QIcon& ic) const;

	QLinearGradient CreateHiglightGradient(const QRect& r) const;
	QPen			ForgroundPen(const QStyleOptionViewItem& option) const;

};

#endif
