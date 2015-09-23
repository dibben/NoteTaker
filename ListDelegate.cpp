/*!
	\file  ListDelegate.cpp
	\brief The ListDelegate implementation
	\author Author
	\date \$Date: Tue Sep 22 2015 $
	\version \$Id: ListDelegate.cpp $
*/

#include "ListDelegate.h"

#include <QPainter>
#include <QDateTime>
#include <QTextLayout>


ListDelegate::ListDelegate(QObject *parent) :
							QAbstractItemDelegate(parent)
{

}


bool ListDelegate::IsSelected(const QStyleOptionViewItem& option) const
{
	return (option.state & QStyle::State_Selected);
}


void ListDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
	QRect r = option.rect;

	if (IsSelected(option)) {
		DrawHighlightedItem(painter, r);
	} else {
		DrawStandardItem(painter, r);
	}

	painter->setPen(ForgroundPen(option));

	QIcon     icon  = QIcon(qvariant_cast<QPixmap>(index.data(Qt::DecorationRole)));
	QString   title = index.data(Qt::DisplayRole).toString();
	QString   text  = index.data(Qt::UserRole + 1).toString();
	QDateTime date  = index.data(Qt::UserRole + 2).toDateTime();

	r = option.rect.adjusted(5, 10, -10, -10);
	int imageSpace = DrawImage(painter, option, r, icon);

	int dateWidth = option.fontMetrics.width("31/12/2015");

	r = option.rect.adjusted(imageSpace, 5, -10-dateWidth, -20 );
	DrawTitle(painter, option, r, title);

	r = option.rect.adjusted(r.right()-10-dateWidth, 5, -10, -20);
	DrawDate(painter, option, r, date);

	r = option.rect.adjusted(imageSpace, 23, -10, 0);
	DrawBodyText(painter, option, r, text);
}


QPen ListDelegate::ForgroundPen(const QStyleOptionViewItem& option) const
{
	if (IsSelected(option)) {
		return QPen(Qt::white, 1, Qt::SolidLine);
	} else {
		return QPen(QPalette().color(QPalette::Text), 1, Qt::SolidLine);
	}
}


void ListDelegate::DrawHighlightedItem(QPainter* painter, const QRect& r) const
{
	QPen linePen(QColor::fromRgb(0,90,131), 1, Qt::SolidLine);
	QBrush brush = CreateHiglightGradient(r);

	DrawItem(painter, r, linePen, brush);
}


void ListDelegate::DrawStandardItem(QPainter* painter, const QRect& r) const
{
	QPen linePen( QPalette().color(QPalette::Mid), 1, Qt::SolidLine);
	QBrush brush( QPalette().color(QPalette::Base));

	DrawItem(painter, r, linePen, brush);
}


void ListDelegate::DrawItem(QPainter* painter, const QRect& r, const QPen& linePen, QBrush brush) const
{
	painter->setBrush(brush);
	painter->drawRect(r);

	painter->setPen(linePen);
	DrawItemOutline(painter, r);
}


QLinearGradient ListDelegate::CreateHiglightGradient(const QRect& r) const
{
	QLinearGradient gradientSelected(r.left(),r.top(),r.left(),r.height()+r.top());

	gradientSelected.setColorAt(0.0, QColor::fromRgb(119,213,247));
	gradientSelected.setColorAt(0.9, QColor::fromRgb(27,134,183));
	gradientSelected.setColorAt(1.0, QColor::fromRgb(0,120,174));

	return gradientSelected;
}

void ListDelegate::DrawItemOutline(QPainter* painter, const QRect& r) const
{
	painter->drawLine(r.topLeft(),r.topRight());
	painter->drawLine(r.topRight(),r.bottomRight());
	painter->drawLine(r.bottomLeft(),r.bottomRight());
	painter->drawLine(r.topLeft(),r.bottomLeft());
}

void ListDelegate::DrawTitle(QPainter* painter, const QStyleOptionViewItem& option, const QRect& r, const QString& title) const
{
	QFont font = option.font;
	font.setWeight(QFont::Bold);
	QFontMetrics metrics(font);

	QString titleText = metrics.elidedText(title, Qt::ElideRight, r.width());

	painter->setFont(font);
	painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignLeft, titleText);

}

void ListDelegate::DrawDate(QPainter* painter, const QStyleOptionViewItem& option, const QRect& r, const QDateTime& date) const
{
	painter->save();
	painter->setPen(Qt::cyan);

	QFont dateFont = option.font;
	QString dateString = FormatDate(date);
	painter->setFont(dateFont);
	painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignRight, dateString);
	painter->restore();
}

void ListDelegate::DrawBodyText(QPainter* painter, const QStyleOptionViewItem& option, const QRect& r, const QString& text) const
{
	QFont descriptionFont = option.font;
	painter->setFont(descriptionFont);

	QFontMetrics descriptionMetrics(descriptionFont);

	QTextLayout layout(text, descriptionFont);
	qreal height = 0;
	layout.beginLayout();
	for (int i = 0; i < 2;i++) {
		QTextLine line = layout.createLine();
		if (!line.isValid()) break;
		line.setLineWidth(r.width());
		height += descriptionMetrics.leading();
		line.setPosition(QPointF(0, height));
		height += descriptionMetrics.height();
	}
	layout.endLayout();

	layout.draw(painter, QPoint(r.left(),r.top()));
}


int ListDelegate::DrawImage(QPainter* painter, const QStyleOptionViewItem& option, const QRect& r, const QIcon& ic) const
{
	int imageSpace = 10;
	if (!ic.isNull()) {
		//ICON
		ic.paint(painter, r, Qt::AlignVCenter|Qt::AlignLeft);
		imageSpace = 25.;
	}
	return imageSpace;
}


QSize ListDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	Q_UNUSED(option);
	Q_UNUSED(index);
	return QSize(200, 60);
}



QString ListDelegate::FormatDate(const QDateTime& datetime) const
{
	QString dateString;

	QDate date = datetime.date();
	QDate currentDate = QDate::currentDate();

	if (date == QDate::currentDate()) {
		QTime time = datetime.time();
		dateString = time.toString(Qt::DefaultLocaleShortDate);
	} else if (date.year() == currentDate.year()) {
		if (date.weekNumber() == currentDate.weekNumber()) {
			dateString = date.toString("dddd");
		} else {
			dateString = date.toString("MMM dd");
		}
	} else {
		dateString = date.toString(Qt::SystemLocaleShortDate);
	}

	return dateString;
}
