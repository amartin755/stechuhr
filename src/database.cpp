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

#include <QtGlobal>
#include <QFileInfo>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "database.h"

static QString schema_v1 = R"(
CREATE TABLE IF NOT EXISTS "projects" (
    "id"	INTEGER NOT NULL,
    "deleted"	INTEGER,
    "name"	TEXT,
    "description"	TEXT,
    "color"	INTEGER,
    PRIMARY KEY("id" AUTOINCREMENT)
);
CREATE TABLE IF NOT EXISTS "schema" (
    "magic"	INTEGER,
    "version"	INTEGER
);
CREATE TABLE IF NOT EXISTS "workdays" (
    "id"	INTEGER NOT NULL,
    "comment"	TEXT,
    PRIMARY KEY("id" AUTOINCREMENT)
);
CREATE TABLE IF NOT EXISTS "events" (
    "workday"	INTEGER NOT NULL,
    "event"	INTEGER NOT NULL,
    "timestamp"	INTEGER NOT NULL,
    "project"	INTEGER,
    FOREIGN KEY("workday") REFERENCES "workdays"("id") ON DELETE CASCADE,
    FOREIGN KEY("project") REFERENCES "projects"("id")
);
INSERT INTO "schema" VALUES (42,1);
)";

Database::Database (const QString& dbPath) : m_isOpen (false)
{
    // if the parent directory doesn't exist, create it
    QFileInfo dbFile (dbPath);
    if (!dbFile.dir().exists())
    {
        dbFile.dir().mkpath (dbFile.dir().absolutePath());
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName (dbPath);

    if (db.open ())
    {
        bool schemaOk = checkSchema ();
        if (!schemaOk)
        {
            // create tables and check again
            create ();
            schemaOk = checkSchema ();
        }

        m_isOpen = schemaOk;
    }
    else
    {
        m_lastError = db.lastError().text();
    }
}

Database::~Database ()
{
    QString conn;
    {
        QSqlDatabase db = QSqlDatabase::database ();
        conn = db.connectionName ();
        db.close ();
    }
    QSqlDatabase::removeDatabase (conn);
}

bool Database::create ()
{
    QSqlQuery query;
    QStringList statements = schema_v1.split (";");
    for (const auto& sql : statements)
    {
        if (sql.length() > 2)
        {
            if (!query.exec (sql))
            {
                m_lastError = query.lastError().text();
                qCritical() << m_lastError;

                return false;
            }
        }
    }

    return true;
}

bool Database::checkSchema ()
{
    QSqlQuery query ("SELECT version FROM schema WHERE magic = 42");

    if (query.exec () && query.first ())
    {
        if (query.value (0).toInt () == 1)
            return true;
        
        m_lastError = "Unknown database schema!";
    }
    else
    {
        m_lastError = query.lastError().text();
    }

    qCritical() << m_lastError;
    return false;
}

bool Database::storeWorkday (QList<QPair<EventType, QDateTime>> events, const QString& comment)
{
    Q_ASSERT (events.size() >= 2);

    QSqlDatabase db = QSqlDatabase::database ();
    QSqlQuery query;

    if (db.transaction())
    {
        if (!query.prepare ("INSERT INTO workdays (comment) VALUES (:comment)"))
            goto query_error_out;

        query.bindValue (":comment", comment);
        if (!query.exec ())
            goto query_error_out;

        int id = query.lastInsertId().toInt();
        if (!query.prepare ("INSERT INTO events (workday, event, timestamp) "
                            "VALUES (:workday, :event, :timestamp)"))
            goto query_error_out;

        query.bindValue (":workday", id);
        for (const QPair<EventType, QDateTime>& item : events)
        {
            query.bindValue (":event", item.first);
            query.bindValue (":timestamp", item.second.toSecsSinceEpoch());
            if (!query.exec())
                goto query_error_out;
        }

        if (!db.commit ())
            goto db_error_out;

        return true;
    }

db_error_out:
    m_lastError = db.lastError().text();
    return false;

query_error_out:
    m_lastError = query.lastError().text();
    db.rollback ();
    return false;
}

#ifndef NDEBUG
#include <QRandomGenerator>
void Database::createTestData ()
{
    const int numEnties = 10000;
    QList<QPair<EventType, QDateTime>> events;
    QRandomGenerator rand;
    QTime time;
    QDate date = QDate::currentDate ();
    date = date.addDays (-1 * numEnties);

    for (int n = 0; n < numEnties; n++)
    {
        // clock in
        int h = rand.bounded (7, 11);;
        int m = rand.bounded (0, 60);
        time.setHMS (h, m, 0);
        events.append (QPair<EventType, QDateTime>(EventType::CLOCK_IN, QDateTime (date, time)));


        // clock out
        h = rand.bounded (6, 11);;
        m = rand.bounded (0, 60);
        time = time.addSecs (m*60 + h*60*60);
        events.append (QPair<EventType, QDateTime>(EventType::CLOCK_OUT, QDateTime (date, time)));

        if (!storeWorkday (events, QDateTime (date, time).toString()))
            break;

        events.clear ();  
        date = date.addDays (1);      
    }
}
#endif