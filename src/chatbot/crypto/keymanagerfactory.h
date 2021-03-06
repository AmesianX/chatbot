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

#ifndef LVK_CRYPTO_KEYMANAGERFACTORY_H
#define LVK_CRYPTO_KEYMANAGERFACTORY_H

#include "crypto/keymanager.h"

namespace Lvk
{

/// \addtogroup Lvk
/// @{

namespace Crypto
{

/// \ingroup Lvk
/// \addtogroup Crypto
/// @{

/**
 * \brief The KeyManagerFactory class provides a factory of Key Managers
 */

class KeyManagerFactory
{
public:

    /**
     * Creates an instance of the default key manager.
     */
    KeyManager *create();
};

/// @}

} // namespace Crypto

/// @}

} // namespace Lvk


#endif // LVK_CRYPTO_KEYMANAGERFACTORY_H

