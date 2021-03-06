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

#ifndef LVK_CMN_CONVERSATIONREADER_H
#define LVK_CMN_CONVERSATIONREADER_H

#include "common/conversation.h"

class QIODevice;
class QString;

namespace Lvk
{

/// \addtogroup Lvk
/// @{

namespace Cmn
{

/// \ingroup Lvk
/// \addtogroup Cmn
/// @{

/**
 * \brief The ConversationReader class provides a format independent interface for reading
 *        chat conversations from files or other devices.
 *
 * To write a conversation see ConversationWriter class.
 */

class ConversationReader
{
public:

    /**
     * Constructs an empty ConversationReader object.
     */
    ConversationReader();

    /**
     * Constructs a ConversationReader object using the given device.
     * After construction class owns the pointer.
     */
    ConversationReader(QIODevice * device);

    /**
     * Constructs a ConversationReader object that will write to a file with the given name.
     */
    ConversationReader(const QString & filename);

    /**
     * Destructs the ConversationReader object and frees the device if was given any.
     */
    ~ConversationReader();

    /**
     * Reads a chat conversation from the device. The given pointer must be initialized.
     * Returns true on success; otherwise, returns false.
     */
    bool read(Conversation *conv);

    /**
     * Reads the next conversation entry from the device. The given pointer must be initialized.
     * Returns true on success; otherwise, returns false.
     */
    bool read(Conversation::Entry *entry);

    /**
     * Returns true if the current read position is at the end of the device
     * (i.e. there is no more data available for reading on the device); otherwise returns false.
     */
    bool atEnd();

private:
    ConversationReader(const ConversationReader&);
    ConversationReader& operator=(const ConversationReader&);

    QIODevice *m_device;
};

/// @}

} // namespace Cmn

/// @}

} // namespace Lvk


#endif // LVK_CMN_CONVERSATIONREADER_H

