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


#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

class Settings
{
public:
    explicit Settings ();
    void setGuiState (const QByteArray& mainWindow, const QByteArray& splitter);
    void getGuiState (QByteArray& mainWindow, QByteArray& splitter) const;
    QString getDbPath () const;
    bool getDbEnabled () const;
    void setDbPath (const QString& path);
    void setDbEnabled (bool enabled);

public:

private:
    static const QString KEY_GROUP_GUI;
    static const QString KEY_GUI_MAIN_DLG;
    static const QString KEY_GUI_SPLITTER;
    static const QString KEY_GROUP_LOGBOOK;
    static const QString KEY_LOGBOOK_ENABLED;
    static const QString KEY_LOGBOOK_DB;

    QSettings m_settings;
};

#endif