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

#ifndef SPELL_CHECKER_H
#define SPELL_CHECKER_H


#include <QString>
#include <QScopedPointer>

class Hunspell;
class QTextCodec;
class SpellingDictionary;


/*!
	\class  SpellChecker
	\author Author
	\date Sat Sep 26 2015

	\brief
*/

class SpellChecker
{
public:

	SpellChecker();
	~SpellChecker();

	bool		IsCorrect(const QString& word) const;
	QStringList	Suggestions(const QString& word) const;

	void		AddToUserDictionary(const QString& text);

	void		LoadDictionary(const QString& language);
	void		LoadDictionary(const SpellingDictionary& dictionary);
	void		LoadUserDictionary();

	QString		UserDictionary() const;

private:

	QByteArray	FromUnicode(const QString& str) const;
	QString		ToUnicode(const char* str) const;

	SpellChecker(const SpellChecker&);
	void	operator=(const SpellChecker&);

	QScopedPointer<Hunspell>	fChecker;
	QTextCodec*					fTextCodec;

};

#endif
