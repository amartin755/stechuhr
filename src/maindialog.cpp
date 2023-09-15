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
#include "maindialog.h"

MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent), m_timeclockRunning (false), m_break (false), m_displayHours (0), m_displayMinutes (0)
{
    ui.setupUi (this);
    ui.workingTime->display ("--:--");
    m_updateTimer  = new QTimer (this);
    m_displayTimer = new QTimer (this);
    connect (m_updateTimer, &QTimer::timeout, this, &MainDialog::calcTime);
    connect (m_displayTimer, &QTimer::timeout, this, &MainDialog::refreshWorkingTime);
    connect (ui.btnClockInOut, &QPushButton::clicked, this, &MainDialog::clockInOut);
    connect (ui.btnBreak, &QPushButton::clicked, this, &MainDialog::breakStartStop);
}

void MainDialog::clockInOut ()
{
    if (!m_timeclockRunning)
    {
        ui.btnClockInOut->setText (tr("Clock out"));
        m_events.clear ();
        m_displayTimer->start (1000);
        m_updateTimer->start (1000 * 60);
        m_events.append (QPair<EventType, QDateTime>(CLOCK_IN, QDateTime::currentDateTime ()));
    }
    else
    {
        ui.btnClockInOut->setText (tr("Clock in"));
        m_updateTimer->stop ();
        m_displayTimer->stop ();
        if (m_break)
            breakStartStop ();

        m_events.append (QPair<EventType, QDateTime>(CLOCK_OUT, QDateTime::currentDateTime ()));
    }
    m_timeclockRunning = !m_timeclockRunning;
    calcTime ();
    refreshWorkingTime ();
    updateList ();
}

void MainDialog::breakStartStop ()
{
    if (m_timeclockRunning)
    {
        if (!m_break)
        {
            ui.btnBreak->setText (tr("Stop break"));
            m_events.append (QPair<EventType, QDateTime>(BREAK_START, QDateTime::currentDateTime ()));
        }
        else
        {
            ui.btnBreak->setText (tr("Start break"));
            m_events.append (QPair<EventType, QDateTime>(BREAK_STOP, QDateTime::currentDateTime ()));
        }
        m_break = !m_break;
        updateList ();
    }
}

void MainDialog::calcTime ()
{
    Q_ASSERT (m_events.count());

    QDateTime currTime = m_events.last().first == BREAK_START ? m_events.last().second : QDateTime::currentDateTime();
    qint64 elapsedSecs = m_events.first().second.secsTo (currTime) - getBreakDuration ();

    Q_ASSERT (elapsedSecs >= 0);

    m_displayHours    = elapsedSecs / 3600;
    m_displayMinutes  = (unsigned)(elapsedSecs - m_displayHours * 3600)/60;
    m_displayMinutes += elapsedSecs % 60 >= 30 ? 1 : 0;
}

qint64 MainDialog::getBreakDuration ()
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

void MainDialog::refreshWorkingTime ()
{
    static bool toggle = false;
    QString text = QString ("%1:%2").arg(m_displayHours, 2, 10, QChar('0')).arg(m_displayMinutes, 2, 10, QChar('0'));
    if (!m_break)
    {
        if (toggle)
            text[2] = ' ';
    }
    else
    {
        if (toggle)
            text.clear();
    }
    ui.workingTime->display (text);
    toggle = !toggle;
}

void MainDialog::updateList ()
{
    bool multipleDays = m_events.first().second.date() != m_events.last().second.date();

    ui.treeWidget->clear ();
    for (const QPair<EventType, QDateTime>& item : m_events)
    {
        QTreeWidgetItem* i;
        i = new QTreeWidgetItem (ui.treeWidget);
        i->setText (0, multipleDays ? QLocale::system().toString(item.second, QLocale::ShortFormat) : item.second.toString ("hh:mm:ss"));
        switch (item.first)
        {
        case CLOCK_IN:
            i->setText (1, tr("Clocked in"));
            break;
        case CLOCK_OUT:
            i->setText (1, tr("Clocked out"));
            break;
        case BREAK_START:
            i->setText (1, tr("Break"));
            break;
        case BREAK_STOP:
            i->setText (1, tr("Break end"));
            break;
        }
    }
    ui.treeWidget->resizeColumnToContents (0);
}
