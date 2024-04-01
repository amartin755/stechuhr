// SPDX-License-Identifier: GPL-3.0-only
/*
 * STECHUHR <https://github.com/amartin755/stechuhr>
 * Copyright (C) 2023 Andreas Martin (netnag@mailbox.org)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QStandardPaths>
#include "settings.h"


const QString Settings::KEY_GROUP_GUI = "gui-state/";
const QString Settings::KEY_GUI_MAIN_DLG = KEY_GROUP_GUI + "main-dialog";
const QString Settings::KEY_GUI_SPLITTER = KEY_GROUP_GUI + "main-splitter";
const QString Settings::KEY_GROUP_LOGBOOK = "logbook/";
const QString Settings::KEY_LOGBOOK_ENABLED = KEY_GROUP_LOGBOOK + "enabled";
const QString Settings::KEY_LOGBOOK_DB      = KEY_GROUP_LOGBOOK + "db";

Settings::Settings ()
{
}

void Settings::setGuiState (const QByteArray& mainWindow, const QByteArray& splitter)
{
    m_settings.setValue(KEY_GUI_MAIN_DLG, mainWindow);
    m_settings.setValue(KEY_GUI_SPLITTER, splitter);
}

void Settings::getGuiState (QByteArray& mainWindow, QByteArray& splitter) const
{
    mainWindow = m_settings.value (KEY_GUI_MAIN_DLG).toByteArray ();
    splitter   = m_settings.value (KEY_GUI_SPLITTER).toByteArray ();
}

QString Settings::getDbPath () const
{
    return m_settings.value (KEY_LOGBOOK_DB, 
        QStandardPaths::writableLocation (QStandardPaths::AppDataLocation) + "/logbook.sqlite").toString();
}

bool Settings::isDbEnabled () const
{
    return m_settings.value (KEY_LOGBOOK_ENABLED, true).toBool();
}

void Settings::setDbPath (const QString& path)
{
    m_settings.setValue (KEY_LOGBOOK_DB, path);
}

void Settings::setDbEnabled (bool enabled)
{
    m_settings.setValue (KEY_LOGBOOK_ENABLED, enabled);
}
