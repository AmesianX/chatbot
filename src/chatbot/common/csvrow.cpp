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

#include "common/csvrow.h"

#include <QStringList>

#define QUOTE   "\""
#define COMMA   ","

//--------------------------------------------------------------------------------------------------
// Helpers
//--------------------------------------------------------------------------------------------------

namespace
{

//--------------------------------------------------------------------------------------------------

void escape(QString &s)
{
    if (s.contains(COMMA)) {
        if (s.contains(QUOTE)) {
            s.replace(QUOTE, QUOTE QUOTE);
        }
        s = QUOTE + s + QUOTE;
    }
}

//--------------------------------------------------------------------------------------------------

void unescape(QString &s)
{
    if (s.size() >= 2 && s.startsWith(QUOTE) && s.endsWith(QUOTE)) {
        s = s.mid(1, s.size() - 2);

        s.replace(QUOTE QUOTE, QUOTE);
    }
}

//--------------------------------------------------------------------------------------------------

bool hasStartQuote(QString &s)
{
    return s.startsWith(QUOTE);
}

//--------------------------------------------------------------------------------------------------

bool hasEndQuote(QString &s)
{
    return s.endsWith(QUOTE) && (!s.endsWith(QUOTE QUOTE) || s.size() == 2);
}

//--------------------------------------------------------------------------------------------------

bool isAllSpace(const QString &s)
{
    for (int i = 0; i < s.size(); ++i) {
        if (!s[i].isSpace()) {
            return false;
        }
    }
    return true;
}

} // namespace


//--------------------------------------------------------------------------------------------------
// CsvRow
//--------------------------------------------------------------------------------------------------

Lvk::Cmn::CsvRow::CsvRow()
{
}

//--------------------------------------------------------------------------------------------------

Lvk::Cmn::CsvRow::CsvRow(unsigned size)
{
    for (unsigned i = 0; i < size; ++i) {
        m_cells.append(QString());
    }
}

//--------------------------------------------------------------------------------------------------

Lvk::Cmn::CsvRow::CsvRow(const QStringList &cells)
{
    foreach (const QString &cell, cells) {
        append(cell);
    }
}

//--------------------------------------------------------------------------------------------------

Lvk::Cmn::CsvRow::CsvRow(const QString &str)
{
    parseString(str);
}

//--------------------------------------------------------------------------------------------------

void Lvk::Cmn::CsvRow::parseString(const QString &str)
{
    clear();

    if (isAllSpace(str)) {
        return;
    }

    QStringList cells = str.split(COMMA);

    for (int i = 0; i < cells.size(); ++i) {
        QString &cell = cells[i];

        // Check if we splitted up a scaped comma
        if (hasStartQuote(cell) && !hasEndQuote(cell)) {
            // Join cells until we find the end quote
            while (i+1 < cells.size() && !hasEndQuote(cell)) {
                cell += COMMA;
                cell += cells[++i];
            }
        }

        unescape(cell);
        append(cell);
    }
}

//--------------------------------------------------------------------------------------------------

QString Lvk::Cmn::CsvRow::toString() const
{
    QString str;

    for (int i = 0; i < m_cells.size(); ++i) {
        if (!str.isEmpty()) {
            str += COMMA;
        }
        QString cell = m_cells[i];

        escape(cell);
        str += cell;
    }

    return str;
}

//--------------------------------------------------------------------------------------------------

const QList<QString> & Lvk::Cmn::CsvRow::cells() const
{
    return m_cells;
}

//--------------------------------------------------------------------------------------------------

void Lvk::Cmn::CsvRow::append(const QString &cell)
{
    if (!cell.contains('\n')) {
        m_cells.append(cell);
    } else {
        m_cells.append(QString(cell).remove('\n'));
    }
}

//--------------------------------------------------------------------------------------------------

const QString & Lvk::Cmn::CsvRow::operator[](int n) const
{
    return m_cells[n];
}

//--------------------------------------------------------------------------------------------------

QString & Lvk::Cmn::CsvRow::operator[](int n)
{
    return m_cells[n];
}

//--------------------------------------------------------------------------------------------------

int Lvk::Cmn::CsvRow::size() const
{
    return m_cells.size();
}

//--------------------------------------------------------------------------------------------------

void Lvk::Cmn::CsvRow::clear()
{
    m_cells.clear();
}

//--------------------------------------------------------------------------------------------------

bool Lvk::Cmn::CsvRow::operator==(const Lvk::Cmn::CsvRow &other) const
{
    return m_cells == other.m_cells;
}

//--------------------------------------------------------------------------------------------------

bool Lvk::Cmn::CsvRow::operator!=(const Lvk::Cmn::CsvRow &other) const
{
    return !this->operator ==(other);
}
