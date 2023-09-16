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
    : QDialog(parent)
{
    ui.setupUi (this);
    ui.workingTime->display ("--:--");
    m_updateTimer  = new QTimer (this);
    connect (m_updateTimer, &QTimer::timeout, this, &MainDialog::updateTime);
    connect (ui.btnClockInOut, &QPushButton::clicked, this, &MainDialog::clockInOut);
    connect (ui.btnBreak, &QPushButton::clicked, this, &MainDialog::breakStartStop);
}

void MainDialog::clockInOut ()
{
    if (!m_wtClock.hasClockedIn ())
    {
        ui.treeWidget->clear();
        m_updateTimer->start (1000 * 60);
        updateList (":/clock_in", tr("Clocked in"), m_wtClock.clockIn ());

        ui.btnClockInOut->setText (tr("Clock out"));
        ui.btnClockInOut->setIcon (QIcon(":/clock_out"));
        }
    else
    {
        m_updateTimer->stop ();
        if (m_wtClock.takesBreak ())
            breakStartStop();
        updateList (":/clock_out", tr("Clocked Out"), m_wtClock.clockOut());

        ui.btnClockInOut->setText (tr("Clock in"));
        ui.btnClockInOut->setIcon (QIcon(":/clock_in"));
    }
    updateTime ();
}

void MainDialog::breakStartStop ()
{
    if (m_wtClock.hasClockedIn())
    {
        if (!m_wtClock.takesBreak ())
        {
            updateList (":/break_start", tr("Break"), m_wtClock.startBreak());
            ui.workingTime->setSegmentStyle (QLCDNumber::Outline);

            ui.btnBreak->setText (tr("Stop break"));
            ui.btnBreak->setIcon (QIcon(":/break_end"));
        }
        else
        {
            updateList (":/break_end", tr("Break end"), m_wtClock.finishBreak());
            ui.workingTime->setSegmentStyle (QLCDNumber::Flat);

            ui.btnBreak->setText (tr("Start break"));
            ui.btnBreak->setIcon (QIcon(":/break_start"));
        }
        updateTime ();
    }
}

void MainDialog::updateTime ()
{
    unsigned displayHours = 0, displayMinutes = 0;
    m_wtClock.getWorkingTime (displayHours, displayMinutes);
 
    QString text = QString ("%1:%2").arg(displayHours, 2, 10, QChar('0')).arg(displayMinutes, 2, 10, QChar('0'));
    ui.workingTime->display (text);
}

void MainDialog::updateList (const QString& iconPath, const QString& caption, const QDateTime& time)
{
    bool multipleDays = m_wtClock.exceedsDay ();
    QTreeWidgetItem* i = new QTreeWidgetItem (ui.treeWidget);
    i->setText (1, multipleDays ? QLocale::system().toString(time, QLocale::ShortFormat) : time.toString ("hh:mm:ss"));
    i->setText (2, caption);
    i->setIcon (0, QIcon (iconPath));
    ui.treeWidget->resizeColumnToContents (0);
}
