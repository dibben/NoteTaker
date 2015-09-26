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

#ifndef SPELLING_DICTIONARY_H
#define SPELLING_DICTIONARY_H

#include <QString>
#include <QHash>

/*!
	\class  SpellingDictionary
	\author David Dibben
	\date Sat Sep 26 2015

	\brief
*/

class SpellingDictionary
{
public:
		SpellingDictionary();
		SpellingDictionary(const QString& filePath);
		SpellingDictionary(const QString &language, const QString &filePath);

		QString		Title() const;
		QString		Language() const;
		QString		LanguageName() const;
		QString		CountryName() const;

		QString		FilePath() const;

		bool		operator==(const SpellingDictionary& other) const;
		bool		operator!=(const SpellingDictionary& other) const;
		bool		operator<(const SpellingDictionary& other) const;
		bool		operator>(const SpellingDictionary& other) const;

static	SpellingDictionary			DictionaryForLanguage(const QString& language);
static	QList<SpellingDictionary>	SystemDictionaries();

private:

static	QStringList DictionaryPaths();
static	void		LoadDictionaries();

		QString		fLanguage;
		QString		fFilePath;
static	QHash<QString, SpellingDictionary>	fSystemDictionaries;
};


#endif
