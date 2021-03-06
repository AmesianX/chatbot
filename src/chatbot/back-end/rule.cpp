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

#include "back-end/rule.h"

#include <QtAlgorithms>
#include <QIcon>
#include <assert.h>

#define LVK_BE_RULE_VERSION     4


Lvk::BE::Rule::Rule()
    : m_name(""), m_input(), m_output(), m_parentItem(0), m_type(OrdinaryRule),
      m_enabled(false), m_status(Unsaved), m_checkState(Qt::Unchecked), m_id(0), m_nextCatId(0)
{
}

//--------------------------------------------------------------------------------------------------

Lvk::BE::Rule::Rule(const QString &name)
    : m_name(name), m_input(), m_output(), m_parentItem(0), m_type(OrdinaryRule),
      m_enabled(false), m_status(Unsaved), m_checkState(Qt::Unchecked), m_id(0), m_nextCatId(0)
{
}

//--------------------------------------------------------------------------------------------------

Lvk::BE::Rule::Rule(const QString &name, Type type)
    : m_name(name), m_input(), m_output(), m_parentItem(0), m_type(type),
      m_enabled(false), m_status(Unsaved), m_checkState(Qt::Unchecked), m_id(0), m_nextCatId(0)
{
}

//--------------------------------------------------------------------------------------------------

Lvk::BE::Rule::Rule(const QString &name, const QStringList &input, const QStringList &ouput)
    : m_name(name), m_input(input), m_output(ouput), m_parentItem(0), m_type(OrdinaryRule),
      m_enabled(false), m_status(Unsaved), m_checkState(Qt::Unchecked), m_id(0), m_nextCatId(0)
{
}

//--------------------------------------------------------------------------------------------------

Lvk::BE::Rule::Rule(const QString &name, Type type, const QStringList &input,
                    const QStringList &ouput)
    : m_name(name), m_input(input), m_output(ouput), m_parentItem(0), m_type(type),
      m_enabled(false), m_status(Unsaved), m_checkState(Qt::Unchecked), m_id(0), m_nextCatId(0)
{
}

//--------------------------------------------------------------------------------------------------

Lvk::BE::Rule::Rule(const Rule &other, bool deepCopy /*= false*/)
    : m_name(other.m_name), m_input(other.m_input), m_output(other.m_output),
      m_target(other.m_target), m_parentItem(0), m_type(other.m_type), m_enabled(other.m_enabled),
      m_status(Unsaved), m_checkState(Qt::Unchecked), m_id(0), m_nextCatId(0)
{
    if (deepCopy) {
        foreach (const Rule *rule, other.m_childItems) {
            appendChild(new Rule(*rule, true));
        }
    }
}

//--------------------------------------------------------------------------------------------------

Lvk::BE::Rule::~Rule()
{
    qDeleteAll(m_childItems);
}

//--------------------------------------------------------------------------------------------------

//Lvk::BE::Rule & Lvk::BE::Rule::operator=(const Rule& other)
//{
//    m_name = other.m_name;
//    m_target = other.m_target
//    m_input = other.m_input;
//    m_output = other.m_output;
//    m_nextCategory = other.m_nextCategory;
//    m_parentItem = 0;
//    m_type = other.m_type;
//    m_enabled = other.m_enabled;
//    m_status = Unsaved;
//    m_id = other.m_id;
//
//    return *this;
//}

//--------------------------------------------------------------------------------------------------

bool Lvk::BE::Rule::operator==(const Lvk::BE::Rule &other) const
{
    return m_type == other.m_type &&
           m_name == other.m_name &&
           m_target == other.m_target &&
           m_nextCatId == other.m_nextCatId &&
           m_input == other.m_input &&
           m_output == other.m_output &&
           m_id == other.m_id;
}

//--------------------------------------------------------------------------------------------------

bool Lvk::BE::Rule::operator!=(const Lvk::BE::Rule &other) const
{
    return !operator==(other);
}

//--------------------------------------------------------------------------------------------------

Lvk::BE::Rule * Lvk::BE::Rule::parent()
{
    return m_parentItem;
}

//--------------------------------------------------------------------------------------------------

const Lvk::BE::Rule * Lvk::BE::Rule::parent() const
{
    return m_parentItem;
}

//--------------------------------------------------------------------------------------------------

QList<Lvk::BE::Rule *> & Lvk::BE::Rule::children()
{
    return m_childItems;
}

//--------------------------------------------------------------------------------------------------

const QList<Lvk::BE::Rule *> & Lvk::BE::Rule::children() const
{
    return m_childItems;
}

//--------------------------------------------------------------------------------------------------

bool Lvk::BE::Rule::appendChild(Rule *item)
{
    // TODO review this! we should not append an item that already has a parent
    //assert(item->m_parentItem == 0 || item->m_parentItem == this);
    assert(!m_childItems.contains(item));

    item->m_parentItem = this;

    m_childItems.append(item);

    m_status = Unsaved;

    return true;
}

//--------------------------------------------------------------------------------------------------

bool Lvk::BE::Rule::insertChildren(int position, int count)
{
    if (position < 0 || position > m_childItems.size()) {
        return false;
    }

    for (int i = 0; i < count; ++i) {
        Rule *rule = new Rule();
        rule->m_parentItem = this;
        m_childItems.insert(position, rule);
    }

    m_status = Unsaved;

    return true;
}

//--------------------------------------------------------------------------------------------------

bool Lvk::BE::Rule::removeChildren(int position, int count)
{
    if (position < 0 || position + count > m_childItems.size()) {
        return false;
    }

    for (int i = 0; i < count; ++i) {
        delete m_childItems.takeAt(position);
    }

    m_status = Unsaved;

    return true;
}

//--------------------------------------------------------------------------------------------------

void Lvk::BE::Rule::removeAllChild()
{
    removeChildren(0, childCount());
}

//---------------------------------------------------------------------------------------------------

bool Lvk::BE::Rule::moveChildren(int position, int count, Lvk::BE::Rule *newParent)
{
    if (position < 0 || position + count > m_childItems.size()) {
        return false;
    }

    for (int i = 0; i < count; ++i) {
        newParent->appendChild(m_childItems.takeAt(position));
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

bool Lvk::BE::Rule::moveAllChildren(Lvk::BE::Rule *newParent)
{
    return moveChildren(0, childCount(), newParent);
}

//--------------------------------------------------------------------------------------------------

Lvk::BE::Rule * Lvk::BE::Rule::child(int row)
{
    return m_childItems[row];
}

//--------------------------------------------------------------------------------------------------

const Lvk::BE::Rule * Lvk::BE::Rule::child(int row) const
{
    return m_childItems[row];
}

//--------------------------------------------------------------------------------------------------

int Lvk::BE::Rule::childCount() const
{
    return m_childItems.count();
}

//--------------------------------------------------------------------------------------------------

Lvk::BE::Rule * Lvk::BE::Rule::nextSibling()
{
    if (m_parentItem != 0) {
        // TODO consider not using indexOf to improve performance. Instead store the sibling number
        //      as a class atribute
        int i = m_parentItem->m_childItems.indexOf(const_cast<Rule *>(this));

        if (i + 1 <  m_parentItem->m_childItems.size()) {
            return m_parentItem->m_childItems[i + 1];
        }
    }
    return 0;
}

//--------------------------------------------------------------------------------------------------

const Lvk::BE::Rule * Lvk::BE::Rule::nextSibling() const
{
    if (m_parentItem) {
        // TODO consider not using indexOf to improve performance. Instead store the sibling number
        //      as a class atribute
        int i = m_parentItem->m_childItems.indexOf(const_cast<Rule *>(this));

        if (i + 1 <  m_parentItem->m_childItems.size()) {
            return m_parentItem->m_childItems[i + 1];
        }
    }
    return 0;
}

//--------------------------------------------------------------------------------------------------

void Lvk::BE::Rule::clear()
{
    m_name.clear();
    m_input.clear();
    m_output.clear();
    m_target.clear();
    m_childItems.clear();
    m_status = Unsaved;
    m_checkState = Qt::Unchecked;
    m_nextCatId = 0;
}

//--------------------------------------------------------------------------------------------------

void Lvk::BE::Rule::setType(Rule::Type type)
{
    if (m_type != type) {
        m_type = type;
        m_status = Unsaved;
    }
}

//--------------------------------------------------------------------------------------------------

Lvk::BE::Rule::Type Lvk::BE::Rule::type() const
{
    return m_type;
}

//--------------------------------------------------------------------------------------------------

Lvk::BE::TargetList &Lvk::BE::Rule::target()
{
    m_status = Unsaved;

    return m_target;
}

//--------------------------------------------------------------------------------------------------

const Lvk::BE::TargetList &Lvk::BE::Rule::target() const
{
    return m_target;
}

//--------------------------------------------------------------------------------------------------

void Lvk::BE::Rule::setTarget(const TargetList &target)
{
    m_status = Unsaved;

    m_target = target;
}

//--------------------------------------------------------------------------------------------------

QStringList &Lvk::BE::Rule::input()
{
    m_status = Unsaved;

    return m_input;
}

//--------------------------------------------------------------------------------------------------

const QStringList &Lvk::BE::Rule::input() const
{
    return m_input;
}

//--------------------------------------------------------------------------------------------------

void Lvk::BE::Rule::setInput(const QStringList &input)
{
    m_status = Unsaved;

    m_input = input;
}

//--------------------------------------------------------------------------------------------------

QStringList &Lvk::BE::Rule::output()
{
    m_status = Unsaved;

    return m_output;
}

//--------------------------------------------------------------------------------------------------

const QStringList &Lvk::BE::Rule::output() const
{
    return m_output;
}

//--------------------------------------------------------------------------------------------------

void Lvk::BE::Rule::setOutput(const QStringList &output)
{
    m_status = Unsaved;

    m_output = output;
}

//--------------------------------------------------------------------------------------------------

quint64 Lvk::BE::Rule::nextCategory() const
{
    return m_nextCatId;
}

//--------------------------------------------------------------------------------------------------

void Lvk::BE::Rule::setNextCategory(quint64 catId)
{
    if (m_nextCatId != catId) {
        m_nextCatId = catId;

        m_status = Unsaved;
    }
}

//--------------------------------------------------------------------------------------------------

const QString &Lvk::BE::Rule::name() const
{
    return m_name;
}

//--------------------------------------------------------------------------------------------------

void Lvk::BE::Rule::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;

        m_status = Unsaved;
    }
}

//--------------------------------------------------------------------------------------------------

bool Lvk::BE::Rule::isComplete() const
{
    switch (m_type) {
    case OrdinaryRule:
        return m_input.size() > 0 && m_output.size() > 0;
    case ContainerRule:
        return m_name.size() > 0;
    case EvasiveRule:
        return m_output.size() > 0;
    default:
        return false;
    }
}

//--------------------------------------------------------------------------------------------------

bool Lvk::BE::Rule::isEmpty() const
{
    return m_name.isEmpty() && m_output.isEmpty() && m_input.isEmpty() && m_target.isEmpty();
}

//--------------------------------------------------------------------------------------------------

QDataStream &Lvk::BE::operator<<(QDataStream &stream, const Rule &rule)
{
    stream << LVK_BE_RULE_VERSION;
    stream << rule.name();
    stream << rule.target();
    stream << rule.input();
    stream << rule.output();
    stream << rule.nextCategory();
    stream << static_cast<int>(rule.type());
    stream << rule.childCount();
    stream << rule.id();

    for (int i = 0; i < rule.children().size(); ++i) {
        stream << *(rule.children()[i]);
    }

    return stream;
}

//--------------------------------------------------------------------------------------------------

QDataStream &Lvk::BE::operator>>(QDataStream &stream, Rule &rule)
{
    int version = 0;
    QString name;
    TargetList target;
    QStringList input;
    QStringList output;
    int type;
    int childCount;
    quint64 id = 0;
    quint64 nextCatId = 0;

    stream >> version;
    stream >> name;

    if (version > 0) {
        stream >> target;
    }

    stream >> input;
    stream >> output;

    if (version > 3) {
        stream >> nextCatId;
    }

    stream >> type;
    stream >> childCount;
    stream >> id;

    rule.setTarget(target);
    rule.setName(name);
    rule.setInput(input);
    rule.setOutput(output);
    rule.setNextCategory(nextCatId);
    rule.setType(static_cast<Rule::Type>(type));
    rule.setId(id);

    for (int i = 0; i < childCount; ++i) {
        Rule *child = new Rule();
        stream >> *child;
        rule.appendChild(child);
    }

    return stream;
}

//--------------------------------------------------------------------------------------------------

bool Lvk::BE::Rule::enabled() const
{
    return m_enabled;
}

//--------------------------------------------------------------------------------------------------

void Lvk::BE::Rule::setEnabled(bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        m_status = Unsaved;
    }
}

//--------------------------------------------------------------------------------------------------

Lvk::BE::Rule::Status Lvk::BE::Rule::status() const
{
    return m_status;
}

//--------------------------------------------------------------------------------------------------

void Lvk::BE::Rule::setStatus(Status status)
{
    m_status = status;
}

//--------------------------------------------------------------------------------------------------

Qt::CheckState Lvk::BE::Rule::checkState() const
{
    return m_checkState;
}

//--------------------------------------------------------------------------------------------------

void Lvk::BE::Rule::setCheckState(Qt::CheckState state)
{
    m_checkState = state;
}

//--------------------------------------------------------------------------------------------------

quint64 Lvk::BE::Rule::id() const
{
    return m_id;
}

//--------------------------------------------------------------------------------------------------

void Lvk::BE::Rule::setId(quint64 id)
{
    m_id = id;
}
