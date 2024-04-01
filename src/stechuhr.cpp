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
#include <QSettings>

#include "stechuhr.h"
#include "database.h"

Stechuhr::Stechuhr (Settings& settings, QObject *parent) : QObject (parent), m_settings (settings)
{
}

bool Stechuhr::hasClockedIn () const
{
    return !m_events.isEmpty() && m_events.first().first == EventType::CLOCK_IN && m_events.last().first != EventType::CLOCK_OUT;
}

bool Stechuhr::takesBreak () const
{
    return !m_events.isEmpty() && m_events.last().first == EventType::BREAK_START;
}

bool Stechuhr::exceedsDay () const
{
    return !m_events.isEmpty() && (m_events.first().second.date() != QDateTime::currentDateTime ().date());
}

void Stechuhr::clockIn ()
{
    m_events.clear ();
    handleEvent (EventType::CLOCK_IN);
    saveSession ();
}

void Stechuhr::clockOut (const QDateTime* time)
{
    Q_ASSERT (hasClockedIn());

    if (takesBreak ())
        finishBreak (time);
    handleEvent (EventType::CLOCK_OUT, time);
    saveSession ();

    if (m_settings.isDbEnabled ())
        writeToLogbook ();
}

void Stechuhr::startBreak ()
{
    Q_ASSERT (hasClockedIn());
    Q_ASSERT (!takesBreak());
    handleEvent (EventType::BREAK_START);
    saveSession ();
}

void Stechuhr::finishBreak (const QDateTime* time)
{
    Q_ASSERT (takesBreak());
    handleEvent (EventType::BREAK_STOP, time);
    saveSession ();
}

void Stechuhr::handleEvent (EventType type, const QDateTime* t, bool emitSignal)
{
    QDateTime time = t == nullptr ? QDateTime::currentDateTime () : *t;
    m_events.append (QPair<EventType, QDateTime>(type, time));
    if (emitSignal)
    {
        switch (type)
        {
            case EventType::CLOCK_IN:
                emit clockedIn (time);
                break;
            case EventType::CLOCK_OUT:
                emit clockedOut (time);
                break;
            case EventType::BREAK_START:
                emit breakStarted (time);
                break;
            case EventType::BREAK_STOP:
                emit breakFinished (time);
                break;
        }
    }
}

bool Stechuhr::getTimeOfEvent (int index, QDateTime& time, QDateTime& minAllowedValue, QDateTime& maxAllowedValue) const
{
    int size = m_events.size();
    Q_ASSERT (index < size);
    if (index < size)
    {
        time = m_events.at (index).second;
        maxAllowedValue = QDateTime::currentDateTime ();
        minAllowedValue.setSecsSinceEpoch (0);
        if (index > 0)
            minAllowedValue = m_events.at (index - 1).second;
        if (size > 1 && index < (size - 1))
            maxAllowedValue = m_events.at (index + 1).second;
        return true;
    }
    return false;
}

bool Stechuhr::setTimeOfEvent (int index, QDateTime& time)
{
    Q_ASSERT (index < m_events.size());
    if (index < m_events.size())
    {
        m_events[index].second = time;
        saveSession ();
        return true;
    }
    return false;
}

void Stechuhr::getWorkingTime (unsigned &hours, unsigned &minutes) const
{
    hours = 0;
    minutes = 0;
    if (m_events.size () == 0)
        return;

    bool clockedIn = hasClockedIn ();
    bool round = !clockedIn; // round when work is finished

    QDateTime currTime = takesBreak () || !clockedIn ? m_events.last().second : QDateTime::currentDateTime();
    qint64 elapsedSecs = m_events.first().second.secsTo (currTime) - getBreakDuration ();

    Q_ASSERT (elapsedSecs >= 0);

    if (round && elapsedSecs % 60 >= 30)
        elapsedSecs += 60 - elapsedSecs % 60; // round up to next minute

    hours    = elapsedSecs / 3600;
    minutes  = (unsigned)(elapsedSecs - hours * 3600)/60;
}

qint64 Stechuhr::getBreakDuration () const
{
    qint64 duration = 0;
    if (m_events.count())
    {
        QDateTime begin, end;
        for (const QPair<EventType, QDateTime>& item : m_events)
        {
            if (item.first == EventType::BREAK_START)
            {
                begin = item.second;
            }
            else if (item.first == EventType::BREAK_STOP)
            {
                end = item.second;
                duration += begin.secsTo (end);
            }
        }
    }
    return duration;
}

void Stechuhr::undo ()
{
    m_events.removeLast ();
    saveSession ();
}

void Stechuhr::saveState ()
{
    if (hasClockedIn ())
    {
        QSettings s;

        s.beginGroup (KEY_GROUP_SESSION);
        s.setValue (KEY_TERM, QDateTime::currentDateTime ());
        s.endGroup ();
    }
}

bool Stechuhr::isSavedSessionAvailable (QDateTime& savedAt) const
{
    QSettings s;
    if (s.status() != QSettings::NoError)
    {
        qWarning() << "Could not read stored session.";
        return false;
    }

    s.beginGroup (KEY_GROUP_SESSION);
    savedAt = s.value (KEY_TERM).toDateTime();
    int size = s.beginReadArray (KEY_EVENTS);
    if (size == 0)
        return false;

    s.setArrayIndex (0);
    if (s.value (KEY_EVENTS_EVENT) != EventType::CLOCK_IN)
        return false;

    s.setArrayIndex (size - 1);
    return s.value (KEY_EVENTS_EVENT) != EventType::CLOCK_OUT;
}

void Stechuhr::saveSession () const
{
    QSettings s;
    if (s.status() != QSettings::NoError)
    {
        qCritical() << "Could not store session.";
        return;
    }

    s.beginGroup (KEY_GROUP_SESSION);
    s.remove (KEY_TERM);
    s.beginWriteArray (KEY_EVENTS);
    s.remove ("");

    int n = 0;
    for (const QPair<EventType, QDateTime>& item : m_events)
    {
        s.setArrayIndex (n++);
        s.setValue (KEY_EVENTS_EVENT, item.first);
        s.setValue (KEY_EVENTS_TIME,  item.second);
    }
    s.endArray ();
    s.endGroup ();
}

bool Stechuhr::writeToLogbook () const
{
    Database db(m_settings.getDbPath());
db.createTestData ();
    db.storeWorkday (m_events, "");
    return true;
}

void Stechuhr::loadSession ()
{
    QSettings s;
    if (s.status() != QSettings::NoError)
    {
        qWarning() << "Could not read stored session.";
        return;
    }

    m_events.clear ();
    s.beginGroup (KEY_GROUP_SESSION);
    int size = s.beginReadArray (KEY_EVENTS);

    for (int n = 0; n < size; n++)
    {
        s.setArrayIndex (n);
        QDateTime time = s.value(KEY_EVENTS_TIME).toDateTime();
        handleEvent ((EventType)s.value(KEY_EVENTS_EVENT).toInt(), &time);
    }
    s.endArray ();
    s.endGroup ();
}

void Stechuhr::removeSession ()
{
    QSettings s;
    if (s.status() != QSettings::NoError)
    {
        qCritical() << "Could not clear stored session.";
        return;
    }
    s.beginGroup (KEY_GROUP_SESSION);
    s.remove (KEY_TERM);
    s.beginWriteArray (KEY_EVENTS);
    s.remove ("");
    s.endArray ();
    s.endGroup ();
    m_events.clear ();
 }

