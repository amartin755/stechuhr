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
#include <QKeyEvent>
#include "maindialog.h"

MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent), CLOCK_IN (tr("Clock in")), CLOCK_OUT (tr("Clock out")), BREAK (tr("Start break")), BREAK_END (tr("Finish break"))
{
    ui.setupUi (this);
    ui.workingTime->display ("--:--");
    this->setWindowIcon (QIcon(":/icon_main"));
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
        updateList (":/icon_clock_in", CLOCK_IN, m_wtClock.clockIn ());

        ui.btnClockInOut->setText (CLOCK_OUT);
        ui.btnClockInOut->setIcon (QIcon(":/icon_clock_out"));
        }
    else
    {
        m_updateTimer->stop ();
        if (m_wtClock.takesBreak ())
            breakStartStop();
        updateList (":/icon_clock_out", CLOCK_OUT, m_wtClock.clockOut());

        ui.btnClockInOut->setText (CLOCK_IN);
        ui.btnClockInOut->setIcon (QIcon(":/icon_clock_in"));
    }
    updateTime ();
}

void MainDialog::breakStartStop ()
{
    if (m_wtClock.hasClockedIn())
    {
        if (!m_wtClock.takesBreak ())
        {
            updateList (":/icon_break_start", BREAK, m_wtClock.startBreak());
            ui.workingTime->setSegmentStyle (QLCDNumber::Outline);

            ui.btnBreak->setText (BREAK_END);
            ui.btnBreak->setIcon (QIcon(":/icon_break_end"));
        }
        else
        {
            updateList (":/icon_break_end", BREAK_END, m_wtClock.finishBreak());
            ui.workingTime->setSegmentStyle (QLCDNumber::Flat);

            ui.btnBreak->setText (BREAK);
            ui.btnBreak->setIcon (QIcon(":/icon_break_start"));
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

void MainDialog::keyPressEvent(QKeyEvent *e)
{
    if(e->key() != Qt::Key_Escape)
        QDialog::keyPressEvent(e);
}
