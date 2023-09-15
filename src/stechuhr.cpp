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
#include "stechuhr.h"

Stechuhr::Stechuhr ()
{

}

bool Stechuhr::hasClockedIn () const
{
    return !m_events.isEmpty() && m_events.first().first == CLOCK_IN && m_events.last().first != CLOCK_OUT;
}

bool Stechuhr::takesBreak () const
{
    return !m_events.isEmpty() && m_events.last().first == BREAK_START;
}

bool Stechuhr::exceedsDay () const
{
    return !m_events.isEmpty() && (m_events.first().second.date() != m_events.last().second.date());
}

const QDateTime& Stechuhr::clockIn ()
{
    m_events.clear ();
    m_events.append (QPair<EventType, QDateTime>(CLOCK_IN, QDateTime::currentDateTime ()));
    return getLastEventTime();
}

const QDateTime& Stechuhr::clockOut ()
{
    Q_ASSERT (hasClockedIn());
    if (takesBreak ())
        finishBreak ();
    m_events.append (QPair<EventType, QDateTime>(CLOCK_OUT, QDateTime::currentDateTime ()));
    return getLastEventTime();
}

const QDateTime& Stechuhr::startBreak ()
{
    Q_ASSERT (hasClockedIn());
    Q_ASSERT (!takesBreak());
    m_events.append (QPair<EventType, QDateTime>(BREAK_START, QDateTime::currentDateTime ()));
    return getLastEventTime();
}

const QDateTime& Stechuhr::finishBreak ()
{
    Q_ASSERT (takesBreak());
    m_events.append (QPair<EventType, QDateTime>(BREAK_STOP, QDateTime::currentDateTime ()));
    return getLastEventTime();
}

void Stechuhr::getWorkingTime (unsigned &hours, unsigned &minutes) const
{
    hours = 0;
    minutes = 0;
    if (hasClockedIn ())
    {
        QDateTime currTime = takesBreak () ? m_events.last().second : QDateTime::currentDateTime();
        qint64 elapsedSecs = m_events.first().second.secsTo (currTime) - getBreakDuration ();

        Q_ASSERT (elapsedSecs >= 0);

        hours    = elapsedSecs / 3600;
        minutes  = (unsigned)(elapsedSecs - hours * 3600)/60;
        minutes += elapsedSecs % 60 >= 30 ? 1 : 0;
    }
}

const QDateTime& Stechuhr::getLastEventTime () const
{
    return m_events.last().second;
}

qint64 Stechuhr::getBreakDuration () const
{
    qint64 duration = 0;
    if (m_events.count())
    {
        QDateTime begin, end;
        for (const QPair<EventType, QDateTime>& item : m_events)
        {
            if (item.first == BREAK_START)
            {
                begin = item.second;
            }
            else if (item.first == BREAK_STOP)
            {
                end = item.second;
                duration += begin.secsTo (end);
            }
        }
    }
    return duration;
}