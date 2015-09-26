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

#include "SpellingDictionary.h"

#include <QLocale>
#include <QDir>
#include <QFileInfo>
#include <QDirIterator>
#include <QStandardPaths>
#include <QApplication>

QHash<QString, SpellingDictionary> SpellingDictionary::fSystemDictionaries;

QStringList SpellingDictionary::DictionaryPaths()
{
	QStringList paths;

	// Debian
	paths << QStringLiteral("/usr/local/share/myspell/dicts")
		  << QStringLiteral("/usr/share/myspell/dicts");

	// Ubuntu
	paths << QStringLiteral("/usr/share/hunspell");

	// Fedora
	paths << QStringLiteral("/usr/local/share/myspell")
		  << QStringLiteral("/usr/share/myspell");

	//windows
	paths << QStandardPaths::standardLocations(QStandardPaths::DataLocation);
	paths << qApp->applicationDirPath();

	return paths;
}


QList<SpellingDictionary> SpellingDictionary::SystemDictionaries()
{
	LoadDictionaries();

	QList<SpellingDictionary> result = fSystemDictionaries.values();
	qSort(result);
	return result;
}


SpellingDictionary SpellingDictionary::DictionaryForLanguage(const QString& language)
{
	LoadDictionaries();
	return fSystemDictionaries.value(language);
}


void SpellingDictionary::LoadDictionaries()
{
	if (!fSystemDictionaries.isEmpty()) return;

	QStringList filters;
	filters << "*.dic";

	foreach (const QString &path, DictionaryPaths()) {
		QDirIterator it(path, filters, QDir::Files);
		while (it.hasNext()) {
			it.next();
			SpellingDictionary dic(it.filePath());
			fSystemDictionaries.insert(dic.Language(), dic);
		}
	}
}


SpellingDictionary::SpellingDictionary()
{
}

SpellingDictionary::SpellingDictionary(const QString& filePath)
{
	fLanguage = QFileInfo(filePath).completeBaseName();
	fFilePath = filePath;
}

SpellingDictionary::SpellingDictionary(const QString& language, const QString& filePath)
{
	fLanguage = language;
	fFilePath = filePath;
}

QString SpellingDictionary::Title() const
{
	QString country = CountryName();
	if (country.isEmpty()) {
		return LanguageName();
	} else {
		return QString("%1 / %2").arg(LanguageName()).arg(country);
	}
}

QString SpellingDictionary::Language() const
{
	return fLanguage;
}

QString SpellingDictionary::LanguageName() const
{
	QString result = QLocale(Language()).nativeLanguageName();
	if (result.isEmpty()) {
		result = Language();
	}
	return result;
}

QString SpellingDictionary::CountryName() const
{
	return QLocale(Language()).nativeCountryName();
}

QString SpellingDictionary::FilePath() const
{
	return fFilePath;
}

bool SpellingDictionary::operator==(const SpellingDictionary& other) const
{
	return fLanguage == other.fLanguage;
}

bool SpellingDictionary::operator!=(const SpellingDictionary& other) const
{
	return !(*this == other);
}

bool SpellingDictionary::operator<(const SpellingDictionary& other) const
{
	return QString::compare(Title(), other.Title(), Qt::CaseInsensitive) < 0;
}

bool SpellingDictionary::operator>(const SpellingDictionary& other) const
{
	return QString::compare(Title(), other.Title(), Qt::CaseInsensitive) > 0;
}


