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


#ifndef STECHUHR_H
#define STECHUHR_H

#include <QDateTime>
#include <QList>
#include <QPair>

class Stechuhr
{
public:
    Stechuhr ();

    bool hasClockedIn () const;
    bool takesBreak () const;
    bool exceedsDay () const;
    const QDateTime& clockIn ();
    const QDateTime& clockOut ();
    const QDateTime& startBreak ();
    const QDateTime& finishBreak ();
    void getWorkingTime (unsigned &hours, unsigned &minutes) const;
    const QDateTime& getLastEventTime () const;

private:
    qint64 getBreakDuration () const;

    enum EventType { CLOCK_IN, CLOCK_OUT, BREAK_START, BREAK_STOP };
    QList<QPair<EventType, QDateTime>> m_events;

};

#endif