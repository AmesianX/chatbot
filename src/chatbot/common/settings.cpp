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

#include "common/version.h"
#include "common/settings.h"
#include "common/settingskeys.h"

#ifdef Q_WS_MAC
#  define SETTINGS_FILENAME   "./chatbot.app/Contents/MacOS/chatbot.conf"
#else
#  define SETTINGS_FILENAME   "./chatbot.conf"
#endif

#ifdef DA_CONTEST
#  define DEFAULT_LANG  "es_AR"
#else
#  define DEFAULT_LANG  "en_EN"
#endif

//--------------------------------------------------------------------------------------------------
// Settings
//--------------------------------------------------------------------------------------------------

Lvk::Cmn::Settings::Settings(QObject *parent)
    : QSettings(SETTINGS_FILENAME, QSettings::IniFormat, parent)
{
}

//--------------------------------------------------------------------------------------------------

Lvk::Cmn::Settings::Settings(UserScope, QObject *parent)
    : QSettings(ORGANIZATION_NAME, APP_NAME, parent)
{
}

//--------------------------------------------------------------------------------------------------

QVariant Lvk::Cmn::Settings::value(const QString &key, const QVariant &defaultValue_) const
{
    QVariant defaultValue = defaultValue_;

    if (defaultValue.isNull()) {
        // Add here system-wide default values for known keys
        if (key == SETTING_LOGS_PATH) {
            defaultValue = QString("./logs");
        } else if (key == SETTING_DATA_PATH) {
            defaultValue = QString("./data");
        } else if (key == SETTING_LANG_PATH) {
            defaultValue = QString("./lang");
        } else if (key == SETTING_CLUE_PATH) {
            defaultValue = QString("./clue");
        } else if (key == SETTING_CLUE_CHARS_FILE) {
            defaultValue = QString("characters.txt");
        } else if (key == SETTING_APP_LANGUAGE) {
            defaultValue = QString(DEFAULT_LANG);
        } else if (key == SETTING_APP_SEND_STATS) {
            defaultValue = true;
        }
    }

    return QSettings::value(key, defaultValue);
}
