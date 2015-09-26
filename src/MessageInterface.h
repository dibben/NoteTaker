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
