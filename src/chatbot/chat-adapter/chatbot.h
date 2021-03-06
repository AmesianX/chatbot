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

#ifndef LVK_CA_CHATBOT_H
#define LVK_CA_CHATBOT_H

#include <QObject>

#include "chat-adapter/contactinfo.h"
#include "common/conversation.h"

class QString;

namespace Lvk
{

/// \addtogroup Lvk
/// @{

namespace CA
{

/// \ingroup Lvk
/// \addtogroup CA
/// @{

class ChatbotAI;


/**
 * \brief The Chatbot class provides the abstract interface for all chatbots.
 *
 * A chatbot connects to a chat server and waits for a conversation to start. If a message arrives
 * it replies but it does not start conversations by its own. To reply a message the chatbot
 * requires a ChatbotAI object to be set with setAI().
 *
 * \see VirtualUser, XmppChatbot, FbChatbot, GTalkChatbot
 */
class Chatbot : public QObject
{
    Q_OBJECT

public:

    /**
     * Destroys the chatbot and disconnects from the chat server if nescessary.
     */
    virtual ~Chatbot();

    /**
     * Connects to the default chat server using \a user and \a password.
     * Emits \a connected signal on success. Otherwise; emits \a connectionError.
     * If the connection ends prematurely, emits \a disconnected.
     */
    virtual void connectToServer(const QString &user, const QString &password) = 0;

    /**
     * Connects to the chat server at \a domain using \a user and \a password
     * Emits \a connected signal on success. Otherwise; emits \a connectionError.
     * If the connection ends prematurely, emits \a disconnected.
     */
    virtual void connectToServer(const QString &user, const QString &password,
                                 const QString &domain) = 0;

    /**
     * Disconnects from chat server.
     */
    virtual void disconnectFromServer() = 0;

    /**
     * Returns true if the chatbot is connected. Otherwise; returns false.
     */
    virtual bool isConnected() const = 0;

    /**
     * Returns the username used to connect to the server. Returns an empty string, if
     * connectToServer() was never called or it is disconnected.
     */
    virtual const QString &username() = 0;

    /**
     * Returns the domain used to connect to the server. Returns an empty string, if
     * connectToServer() was never called or it is disconnected.
     */
    virtual const QString &domain() = 0;

    /**
     * Sets the AI for the chatbot.
     */
    virtual void setAI(ChatbotAI *AI) = 0;

    /**
     * Returns the AI of the chatbot
     */
    virtual ChatbotAI *AI() = 0;

    /**
     * Returns the roster. i.e. the list of all chat contacts.
     */
    virtual ContactInfoList roster() const = 0;

    /**
     * Sets the black list roster \a blackList.
     * The black list roster is the list of contacts that the chatbot is forbidden to talk to.
     * By default, this list is empty.
     */
    virtual void setBlackListRoster(const ContactInfoList &blackList) = 0;

    /**
     * Returns the black list roster.
     * The black list roster is the list of contacts that the chatbot is forbidden to talk to.
     * By default, this list is empty.
     */
    virtual ContactInfoList blackListRoster() const = 0;

    /**
     * Sets the \a filename where the chat history must be stored or loaded
     */
    virtual void setHistoryFilename(const QString &filename) = 0;

    /**
     * Returns the filename where the chat history is stored
     */
    virtual QString historyFilename() const = 0;

    /**
     * Returns the chat history of the chatbot
     */
    virtual const Cmn::Conversation &chatHistory() const = 0;

    /**
     * Sets the chat history of the chatbot.
     */
    virtual void setChatHistory(const Cmn::Conversation &conv) = 0;

    /**
     * Clears the chat history.
     */
    virtual void clearHistory() = 0;

signals:

    /**
     * This signal is emitted after invoking connectToServer() if the connection was successful.
     */
    void connected();

    /**
     * This signal is emitted after invoking connectToServer() if the connection has ended.
     */
    void disconnected();

    /**
     * This signal es emitted after invoking connectToServer() if there was an error while trying
     * to connect to the chat server.
     */
    void error(int err);

    /**
     * This signal is emitted whenever the chatbot receives a chat message.
     */
    void newConversationEntry(const Cmn::Conversation::Entry &entry);
};

/// @}

} // namespace CA

/// @}

} // namespace Lvk

#endif // LVK_CA_CHATBOT_H
