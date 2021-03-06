/*
 * Copyright (C) 2012 Andres Pagliano, Gabriel Miretti, Gonzalo Buteler,
 * Nestor Bustamante, Pablo Perez de Angelis
 *
 * This file is part of LVK Chatbot.
 *
 * LVK Chatbot is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LVK Chatbot is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LVK Chatbot.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "da-clue/scriptmanager.h"
#include "da-clue/scriptparser.h"
#include "common/settings.h"
#include "common/settingskeys.h"
#include "common/globalstrings.h"

#include <QStringList>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QtDebug>
#include <QObject>

//--------------------------------------------------------------------------------------------------
// Helpers
//--------------------------------------------------------------------------------------------------

namespace
{

inline bool equalFilename(QString f1, QString f2)
{
#ifdef WIN32
    return QFileInfo(f1).fileName().compare(QFileInfo(f2).fileName(), Qt::CaseInsensitive) == 0;
#else
    return QFileInfo(f1).fileName().compare(QFileInfo(f2).fileName(), Qt::CaseSensitive) == 0;
#endif
}

} // namespace


//--------------------------------------------------------------------------------------------------
// ScriptManager
//--------------------------------------------------------------------------------------------------

Lvk::Clue::ScriptManager::ScriptManager()
    : m_error(Clue::NoError), m_format(Clue::XmlPlain)
{
    initPaths();
}

//--------------------------------------------------------------------------------------------------

void Lvk::Clue::ScriptManager::initPaths()
{
    qDebug() << "ScriptManager: Initializing paths...";

    m_clueBasePath = Cmn::Settings().value(SETTING_CLUE_PATH).toString();
    if (!m_clueBasePath.endsWith("/")) {
        m_clueBasePath.append("/");
    }

    m_charsPath = m_clueBasePath + Cmn::Settings().value(SETTING_CLUE_CHARS_FILE).toString();
}

//--------------------------------------------------------------------------------------------------

QList<Lvk::Clue::Character> Lvk::Clue::ScriptManager::characters()
{
    QList<Clue::Character> chars;

    qDebug() << "ScriptManager: Opening file" << m_charsPath;

    QFile f(m_charsPath);

    if (f.open(QFile::ReadOnly | QFile::Text)) {
        QStringList lines = QString::fromUtf8(f.readAll()).split("\n");

        foreach (QString line, lines) {
            line = line.trimmed();

            if (line.isEmpty()) {
                continue;
            }

            qDebug() << "ScriptManager: Parsing character" << line;

            // If detective
            if (line.endsWith("*")) {
                line.truncate(line.size() - 1);
                chars.append(Clue::Character(line, true));
            } else {
                chars.append(Clue::Character(line));
            }
        }
    } else {
        qCritical() << "ScriptManager: Could not open" << m_charsPath;
    }

    return chars;
}

//--------------------------------------------------------------------------------------------------

const QString & Lvk::Clue::ScriptManager::currentCharacter() const
{
    return m_curChar;
}

//--------------------------------------------------------------------------------------------------

void Lvk::Clue::ScriptManager::setCurrentCharacter(const QString &name)
{
    m_curChar = name.trimmed();
}

//--------------------------------------------------------------------------------------------------

void Lvk::Clue::ScriptManager::setScriptFormat(Clue::ScriptFormat format)
{
    m_format = format;
}

//--------------------------------------------------------------------------------------------------

bool Lvk::Clue::ScriptManager::loadScripts()
{
    if (m_curChar.isEmpty()) {
        return false;
    }
    return loadScriptsForCharacter(m_curChar);
}

//--------------------------------------------------------------------------------------------------

bool Lvk::Clue::ScriptManager::loadScriptsForCharacter(const Clue::Character &c)
{
    return loadScriptsForCharacter(c.name);
}

//--------------------------------------------------------------------------------------------------

bool Lvk::Clue::ScriptManager::loadScriptsForCharacter(const QString &name)
{
    qDebug() << "ScriptManager: Loading scripts for character" << name;

    QDir dir(m_clueBasePath);
    QStringList nameFilters;
    nameFilters.append("*." SCRIPT_FILE_EXT);
    QStringList files = dir.entryList(nameFilters, QDir::Files, QDir::Name);

    m_scripts.clear();
    resetError();

    foreach (const QString &file, files) {
        if (!loadFile(m_clueBasePath + file, name)) {
            if (m_error == Clue::CharacterMismatchError) {
                resetError();
            } else {
                break;
            }
        }
    }

    return m_error == Clue::NoError;
}


//--------------------------------------------------------------------------------------------------

bool Lvk::Clue::ScriptManager::loadFile(const QString &filename, const QString &name)
{
    Clue::ScriptParser parser;
    Clue::Script script;

    resetError();

    if (parser.parse(filename, script, m_format)) {
        if (QString::compare(script.character, name, Qt::CaseInsensitive) == 0) {
            int i = 0;

            for (; i < m_scripts.size() && !equalFilename(m_scripts[i].filename, filename); ++i);

            if (i < m_scripts.size()) {
                m_scripts[i] = script;
            } else {
                m_scripts.append(script);
            }
        } else {
            setError(Clue::CharacterMismatchError, filename, script.character);
        }
    } else {
        setParsingError(parser);
    }

    return m_error == Clue::NoError;
}

//--------------------------------------------------------------------------------------------------

const Lvk::Clue::ScriptList & Lvk::Clue::ScriptManager::scripts()
{
    return m_scripts;
}

//--------------------------------------------------------------------------------------------------

bool Lvk::Clue::ScriptManager::import(const QString &filename)
{
    qDebug() << "ScriptManager: Importing file" << filename;

    if (!QFile::exists(filename)) {
        setError(Clue::FileNotFoundError, filename);
        return false;
    }

    QString impFile = m_clueBasePath + QFileInfo(filename).fileName();

    qDebug() << "ScriptManager: copying to" << impFile;

    if (QFile::exists(impFile) && !QFile::remove(impFile)) {
        setError(Clue::CannotOverwriteError, impFile);
        return false;
    }

    if (!QFile::copy(filename, impFile)) {
        setError(Clue::CannotCopyError, filename);
        return false;
    }

    if (!loadFile(impFile, m_curChar)) {
        QFile::remove(impFile);
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

bool Lvk::Clue::ScriptManager::remove(const QString &filename)
{
    qDebug() << "ScriptManager: Removing file" << filename;

    QString rmFile = m_clueBasePath + QFileInfo(filename).fileName();

    if (!QFile::exists(rmFile)) {
        setError(Clue::FileNotFoundError, rmFile);
        return false;
    }

    if (!QFile::remove(rmFile)) {
        setError(Clue::CannotRemoveError, rmFile);
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

void Lvk::Clue::ScriptManager::clear()
{
    m_curChar.clear();
    m_scripts.clear();
    m_errMsg.clear();
    m_error = Clue::NoError;
}

//--------------------------------------------------------------------------------------------------

Lvk::Clue::ScriptError Lvk::Clue::ScriptManager::error(QString *errMsg)
{
    if (errMsg) {
        *errMsg = m_errMsg;
    }

    return m_error;
}

//--------------------------------------------------------------------------------------------------

void Lvk::Clue::ScriptManager::setError(Clue::ScriptError err, const QString &filename,
                                        const QString &extra)
{
    m_error = err;

    switch (err) {
    case Clue::NoError:
        m_errMsg = "";
        break;

    case Clue::FileNotFoundError:
        m_errMsg = QObject::tr("File not found '%1'").arg(filename);
        break;

    case Clue::ReadError:
        m_errMsg = QObject::tr("Cannot read file '%1'").arg(filename);
        break;

    case Clue::InvalidFormatError:
        m_errMsg = QObject::tr("Invalid format in file '%1'").arg(filename);
        break;

    case Clue::CharacterMismatchError:
        m_errMsg = QObject::tr("Current character '%1' does not match character '%2' in file '%3'")
                .arg(m_curChar, extra, filename);
        break;

    case Clue::CannotCopyError:
        m_errMsg = QObject::tr("Cannot copy file '%1'").arg(filename);
        break;

    case Clue::CannotOverwriteError:
        m_errMsg = QObject::tr("Cannot overwrite file '%1'").arg(filename);
        break;

    case Clue::CannotRemoveError:
        m_errMsg = QObject::tr("Cannot remove file '%1'").arg(filename);
        break;

    case Clue::UnknownError:
    default:
        m_errMsg = QObject::tr("Unknown error while parsing file '%1'").arg(filename);
        break;
    }

    if (!m_errMsg.isEmpty()) { 
        (err == Clue::CharacterMismatchError ? qDebug() : qCritical())
                << "ScriptManager: " << m_errMsg;
    }
}

//--------------------------------------------------------------------------------------------------

void Lvk::Clue::ScriptManager::setParsingError(const Clue::ScriptParser &parser)
{
    m_error = parser.error(&m_errMsg);

    if (!m_errMsg.isEmpty()) {
        qCritical() << "ScriptManager: " << m_errMsg;
    }
}

//--------------------------------------------------------------------------------------------------

void Lvk::Clue::ScriptManager::resetError()
{
    m_error = Clue::NoError;
    m_errMsg.clear();
}

