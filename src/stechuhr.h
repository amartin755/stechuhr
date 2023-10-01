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

#include <QObject>
#include <QDateTime>
#include <QList>
#include <QPair>

class Stechuhr : public QObject
{
    Q_OBJECT

public:
    explicit Stechuhr (QObject *parent = nullptr);

    bool hasClockedIn () const;
    bool takesBreak () const;
    bool exceedsDay () const;
    void clockIn ();
    void clockOut (const QDateTime* time = nullptr);
    void startBreak ();
    void finishBreak (const QDateTime* time = nullptr);
    void getWorkingTime (unsigned &hours, unsigned &minutes) const;

    void saveState ();
    bool isSavedSessionAvailable(QDateTime& savedAt) const;
    void loadSession ();
    void removeSession ();

signals:
    void clockedIn (const QDateTime&);
    void clockedOut (const QDateTime&);
    void breakStarted (const QDateTime&);
    void breakFinished (const QDateTime&);

private:
    enum EventType { CLOCK_IN, CLOCK_OUT, BREAK_START, BREAK_STOP };
    void handleEvent (EventType type, const QDateTime* time = nullptr, bool emitSignal = true);
    qint64 getBreakDuration () const;
    void saveSession () const;

    QList<QPair<EventType, QDateTime>> m_events;

    const QString KEY_GROUP_SESSION = "last-session";
    const QString KEY_EVENTS = "events";
    const QString KEY_EVENTS_EVENT = "event";
    const QString KEY_EVENTS_TIME = "time";
    const QString KEY_TERM = "terminated";
};

#endif