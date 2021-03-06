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

#include "common/conversation.h"
#include "common/globalstrings.h"

#include <QStringList>
#include <QRegExp>

//--------------------------------------------------------------------------------------------------
// Conversation::Entry
//--------------------------------------------------------------------------------------------------

#define DATE_TIME_LOG_FORMAT    "dd-MM-yyyy hh:mm:ss"

Lvk::Cmn::Conversation::Entry::Entry()
    : match(false), ruleId(0)
{
}

//--------------------------------------------------------------------------------------------------

Lvk::Cmn::Conversation::Entry::Entry(const QDateTime &dateTime, const QString &from,
                                    const QString &to, const QString &msg, const QString &response,
                                    bool match, quint64 ruleId /*= 0*/)
    : dateTime(dateTime), from(from), to(to), msg(msg), response(response), match(match),
      ruleId(ruleId)
{
}

//--------------------------------------------------------------------------------------------------

bool Lvk::Cmn::Conversation::Entry::isNull() const
{
    return dateTime.isNull()
            && from.isNull()
            && to.isNull()
            && msg.isNull()
            && response.isNull()
            && !match
            && !ruleId;
}

//--------------------------------------------------------------------------------------------------

void Lvk::Cmn::Conversation::Entry::clear()
{
    dateTime = QDateTime();
    from.clear();
    to.clear();
    msg.clear();
    response.clear();
    match = false;
    ruleId = 0;
}

//--------------------------------------------------------------------------------------------------

bool Lvk::Cmn::Conversation::Entry::operator==(const Lvk::Cmn::Conversation::Entry &other) const
{
    QString strDate      = dateTime.toString(STR_GLOBAL_DATE_TIME_FORMAT);
    QString strOtherDate = other.dateTime.toString(STR_GLOBAL_DATE_TIME_FORMAT);

    return  strDate     == strOtherDate
            && from     == other.from
            && to       == other.to
            && msg      == other.msg
            && response == other.response
            && match    == other.match
            && ruleId   == other.ruleId;
}

bool Lvk::Cmn::Conversation::Entry::operator!=(const Lvk::Cmn::Conversation::Entry &other) const
{
    return !this->operator==(other);
}

//--------------------------------------------------------------------------------------------------
// Conversation
//--------------------------------------------------------------------------------------------------

Lvk::Cmn::Conversation::Conversation()
{
}

//--------------------------------------------------------------------------------------------------

void Lvk::Cmn::Conversation::setEntries(const QList<Lvk::Cmn::Conversation::Entry> &entries)
{
    m_entries = entries;
}

//--------------------------------------------------------------------------------------------------

QList<Lvk::Cmn::Conversation::Entry> & Lvk::Cmn::Conversation::entries()
{
    return m_entries;
}

//--------------------------------------------------------------------------------------------------

const QList<Lvk::Cmn::Conversation::Entry> & Lvk::Cmn::Conversation::entries() const
{
    return m_entries;
}

//--------------------------------------------------------------------------------------------------

void Lvk::Cmn::Conversation::append(const Lvk::Cmn::Conversation::Entry &entry)
{
    m_entries.append(entry);
}

//--------------------------------------------------------------------------------------------------

void Lvk::Cmn::Conversation::clear()
{
    m_entries.clear();
}

//--------------------------------------------------------------------------------------------------

bool Lvk::Cmn::Conversation::operator==(const Lvk::Cmn::Conversation &other) const
{
    return m_entries == other.m_entries;
}


//--------------------------------------------------------------------------------------------------

bool Lvk::Cmn::Conversation::isEmpty() const
{
    return m_entries.isEmpty();
}

//--------------------------------------------------------------------------------------------------

int Lvk::Cmn::Conversation::size() const
{
    return m_entries.size();
}

//--------------------------------------------------------------------------------------------------

bool Lvk::Cmn::Conversation::operator!=(const Lvk::Cmn::Conversation &other) const
{
    return !this->operator==(other);
}

