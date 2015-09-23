/*!
	\file   MessageInterface.h
	\brief The MessageInterface class
	\author Author
	\date $Date: Wed Sep 23 2015 $
	\version $Id: MessageInterface.h $
*/

#ifndef MESSAGE_INTERFACE_H
#define MESSAGE_INTERFACE_H

class QString;

/*!
	\class  MessageInterface
	\author Author
	\date Wed Sep 23 2015

	\brief
*/

class MessageInterface
{
public:

virtual	~MessageInterface();

virtual	void	SetMessage(const QString& message) = 0;

};

#endif
