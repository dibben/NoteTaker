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

#include "SpellChecker.h"
#include "SpellingDictionary.h"

#include <hunspell/hunspell.hxx>

#include <QTextCodec>
#include <QStringList>
#include <QDir>
#include <QStandardPaths>
#include <QTextStream>

/*!
	\brief
*/

SpellChecker::SpellChecker()
{
	fTextCodec = 0;
}

SpellChecker::~SpellChecker()
{

}

bool SpellChecker::IsCorrect(const QString &word) const
{
	if (fChecker.isNull()) return true;

	return fChecker->spell( FromUnicode(word) ) != 0;
}

QStringList SpellChecker::Suggestions(const QString& word) const
{
	if (fChecker.isNull()) return QStringList();

	QStringList result;

	char** suggestedWords;
	int count = fChecker->suggest(&suggestedWords, FromUnicode(word));
	for (int i = 0; i < count; ++i) {
		result << ToUnicode(suggestedWords[i]);
	}
	fChecker->free_list(&suggestedWords, count);

	return result;
}

void SpellChecker::AddToUserDictionary(const QString& text)
{
	QString word = text.trimmed();
	if (word.isEmpty()) return;

	fChecker->add(FromUnicode(word).constData());

	QFile file(UserDictionary());
	if (!file.open(QIODevice::Append)) return;

	QTextStream str(&file);
	str << word << "\n";
	file.close();
}



QByteArray SpellChecker::FromUnicode(const QString& str) const
{
	if (fTextCodec == 0) return str.toUtf8();

	return fTextCodec->fromUnicode(str);
}

QString SpellChecker::ToUnicode(const char* str) const
{
	if (fTextCodec == 0) return QString::fromUtf8(str);
	return fTextCodec->toUnicode(str);
}




void SpellChecker::LoadDictionary(const QString& language)
{
	LoadDictionary(SpellingDictionary::DictionaryForLanguage(language));
}

void SpellChecker::LoadDictionary(const SpellingDictionary& dictionary)
{
	QString dicFilePath = dictionary.FilePath();
	QString affixFilePath(dicFilePath);
	affixFilePath.replace(".dic", ".aff");

	fChecker.reset(new Hunspell(affixFilePath.toLocal8Bit(), dicFilePath.toLocal8Bit()));
	fTextCodec = QTextCodec::codecForName(fChecker->get_dic_encoding());
	if (!fTextCodec) {
		fTextCodec = QTextCodec::codecForName("UTF-8");
	}

	LoadUserDictionary();
}

void SpellChecker::LoadUserDictionary()
{
	QFile file(UserDictionary());
	if (!file.open(QIODevice::ReadOnly)) return;

	QTextStream str(&file);
	while (!str.atEnd()) {
		QString word = str.readLine();
		if (!word.isEmpty()) {
			fChecker->add(FromUnicode(word).constData());
		}
	}
}


QString SpellChecker::UserDictionary() const
{
	QDir path(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
	if (!path.exists()) {
		path.mkpath(path.absolutePath());
	}
	return (path.absoluteFilePath("user.dic"));
}
