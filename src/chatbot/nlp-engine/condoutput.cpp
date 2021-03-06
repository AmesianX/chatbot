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

#include "nlp-engine/condoutput.h"
#include "nlp-engine/varstack.h"
#include "nlp-engine/predicate.h"
#include "nlp-engine/parser.h"

//--------------------------------------------------------------------------------------------------
// CondOutput
//--------------------------------------------------------------------------------------------------

Lvk::Nlp::CondOutput::CondOutput()
{
}

//--------------------------------------------------------------------------------------------------

void Lvk::Nlp::CondOutput::append(const QString &output, Nlp::Predicate *pred)
{
    m_outputs.append(output.trimmed());
    m_predicates.append(QSharedPointer<Nlp::Predicate>(pred));
}

//--------------------------------------------------------------------------------------------------

bool Lvk::Nlp::CondOutput::eval(const Nlp::VarStack &varStack, QString &output) const
{
    for (int i = 0; i < m_predicates.size(); ++i) {
        if (m_predicates[i]->eval(varStack)) {
            output = m_outputs[i];
            return true;
        }
    }
    return false;
}

//--------------------------------------------------------------------------------------------------

Lvk::Nlp::CondOutput Lvk::Nlp::CondOutput::fromRawString(const QString &s)
{
    CondOutput co;

    Nlp::Parser parser;

    int i = 0;
    int offset = 0;
    Nlp::Predicate *pred = 0;
    QString body;

    while (true) {
        i = parser.parseIf(s, &pred, &body, offset);
        if (i != -1) {
            co.append(body, pred);
            offset =  i + body.size() + 1;
        } else {
            break;
        }
    }

    if (co.m_predicates.size() > 0) {
        i = parser.parseElse(s, &body, offset);
        if (i != -1) {
            co.append(body, new Nlp::True());
        }
    } else {
        co.append(s, new Nlp::True());
    }

    return co;
}
