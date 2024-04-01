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


#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QList>
#include <QPair>

#include "eventtype.h"

class Database
{
public:
    Database (const QString& dbPath);
    ~Database ();
    bool isOpen () const {return m_isOpen;};
    bool storeWorkday (QList<QPair<EventType, QDateTime>> events, const QString& comment);

#ifndef NDEBUG
    void createTestData ();
#endif

private:
    bool create ();
    bool checkSchema ();
    QString m_lastError;
    bool m_isOpen;

};

#endif