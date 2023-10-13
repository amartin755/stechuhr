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
#include <QMessageBox>
#include <QtLogging>
#include <QSettings>
#include "maindialog.h"
#include "datetimedialog.h"

MainDialog::MainDialog(QApplication* theApp, QWidget *parent)
    : QDialog(parent), 
      CLOCK_IN (tr("Clock in")), CLOCK_OUT (tr("Clock out")), BREAK (tr("Start break")), BREAK_END (tr("Finish break")),
      ICON_CLOCK_IN (":/icon_clock_in"), ICON_CLOCK_OUT (":/icon_clock_out"), ICON_BREAK (":/icon_break_start"), ICON_BREAK_END (":/icon_break_end")
{
    setWindowFlags (Qt::Window);
    ui.setupUi (this);
    ui.workingTime->display ("--:--");
    this->setWindowIcon (QIcon(":/icon_main"));
    readSettings ();

    m_updateTimer  = new QTimer (this);

    connect (m_updateTimer, &QTimer::timeout, this, &MainDialog::updateTime);
    connect (ui.btnClockInOut, &QPushButton::clicked, this, &MainDialog::clockInOut);
    connect (ui.btnBreak, &QPushButton::clicked, this, &MainDialog::breakStartStop);
    connect (theApp, &QApplication::aboutToQuit, this, &MainDialog::saveState, Qt::DirectConnection);
    connect (&m_wtClock, &Stechuhr::clockedIn, this, &MainDialog::onClockedIn);
    connect (&m_wtClock, &Stechuhr::clockedOut, this, &MainDialog::onClockedOut);
    connect (&m_wtClock, &Stechuhr::breakStarted, this, &MainDialog::onBreakStarted);
    connect (&m_wtClock, &Stechuhr::breakFinished, this, &MainDialog::onBreakFinished);
    connect (ui.btnAbout, &QPushButton::clicked, this, &MainDialog::showAbout);
    connect (ui.btnUndo, &QPushButton::clicked, this, &MainDialog::undo);
    connect (ui.treeWidget, &QTreeWidget::itemDoubleClicked, this, &MainDialog::editEvent);

    // not implemented yes -> hide
    ui.btnReport->setVisible (false);
    ui.btnSettings->setVisible (false);

    /**
     * Handle unfinished sessions. There are two cases
     * 1. Termination time is known (savedAt valid): Session can either be continued or
     *    finished. savedAt is used as clock out time.
     * 2. Otherwhise the session can only be continued.
     * In both cases also removing and ignoring of the last session is possible. Let the user
     * decide which way to go.
     */
    QDateTime savedAt;
    if (m_wtClock.isSavedSessionAvailable (savedAt))
    {
        QMessageBox msgBox;
        QPushButton *btnClockOut = nullptr;

        msgBox.setIcon (QMessageBox::Question);
        msgBox.setText (tr("Application has been closed while clocked in."));
        QPushButton* btnContinue = msgBox.addButton (tr("Continue"), QMessageBox::AcceptRole);
        msgBox.setStandardButtons(QMessageBox::Discard);

        if (savedAt.isValid()) // finishing possible
        {
            msgBox.setInformativeText(tr("Do you want to continue, clock out or discard?"));
            btnClockOut = msgBox.addButton (
                CLOCK_OUT + QString(" (%1)").arg (QLocale::system().toString(savedAt, QLocale::ShortFormat)), QMessageBox::ApplyRole);
        }
        else
        {
            msgBox.setInformativeText(tr("Do you want to continue or discard?"));
        }
        if (msgBox.exec() == QMessageBox::Discard)
        {
            m_wtClock.removeSession ();
        }
        else
        {
            Q_ASSERT (msgBox.clickedButton() == btnContinue || msgBox.clickedButton() == btnClockOut);

            // Restore last session
            m_wtClock.loadSession ();
            if (msgBox.clickedButton() == btnClockOut)
            {
                m_wtClock.clockOut (&savedAt);
            }
        }
    }
}

void MainDialog::clockInOut ()
{
    if (!m_wtClock.hasClockedIn ())
    {
        ui.treeWidget->clear();
        m_wtClock.clockIn ();
    }
    else
    {
        m_wtClock.clockOut();
    }
}

void MainDialog::breakStartStop ()
{
    if (m_wtClock.hasClockedIn())
    {
        if (!m_wtClock.takesBreak ())
        {
            m_wtClock.startBreak ();
        }
        else
        {
            m_wtClock.finishBreak ();
        }
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

void MainDialog::updateList (int pos, const QDateTime& time)
{
    bool multipleDays = m_wtClock.exceedsDay ();
    QTreeWidgetItem* i = ui.treeWidget->topLevelItem (pos);
    i->setText (1, multipleDays ? QLocale::system().toString(time, QLocale::ShortFormat) : time.toString ("hh:mm:ss"));
    ui.treeWidget->resizeColumnToContents (0);
}

void MainDialog::setupTimer ()
{
    bool running = m_updateTimer->isActive();
    if (m_wtClock.hasClockedIn ())
    {
        if (!running)
            m_updateTimer->start (1000 * 60);
    }
    else
    {
        if (running)
            m_updateTimer->stop ();
    }
    if (running != m_updateTimer->isActive())
        qInfo() << "Update timer set to: " << m_updateTimer->isActive();
}

void MainDialog::keyPressEvent (QKeyEvent *e)
{
    if (e->key () != Qt::Key_Escape)
        QDialog::keyPressEvent (e);
}

void MainDialog::saveState ()
{
    m_wtClock.saveState ();
    qInfo() << "time to say goodby";
}

void MainDialog::onClockedIn (const QDateTime& time)
{
    updateList (ICON_CLOCK_IN, CLOCK_IN, time);
    updateWidgetStyles ();
    updateTime ();
    setupTimer ();
}

void MainDialog::onClockedOut (const QDateTime& time)
{
    updateList (ICON_CLOCK_OUT, CLOCK_OUT, time);
    updateWidgetStyles ();
    updateTime ();
    setupTimer ();
}

void MainDialog::onBreakStarted (const QDateTime& time)
{
    updateList (ICON_BREAK, BREAK, time);
    updateWidgetStyles ();
    updateTime ();
}

void MainDialog::onBreakFinished (const QDateTime& time)
{
    updateList (ICON_BREAK_END, BREAK_END, time);
    updateWidgetStyles ();
    updateTime ();
}

void MainDialog::updateWidgetStyles ()
{
    if (m_wtClock.hasClockedIn ())
    {
        ui.btnClockInOut->setText (CLOCK_OUT);
        ui.btnClockInOut->setIcon (QIcon(ICON_CLOCK_OUT));
    }
    else
    {
        ui.btnClockInOut->setText (CLOCK_IN);
        ui.btnClockInOut->setIcon (QIcon(ICON_CLOCK_IN));
    }
    if (m_wtClock.takesBreak ())
    {
        ui.btnBreak->setText (BREAK_END);
        ui.btnBreak->setIcon (QIcon(ICON_BREAK_END));
        ui.workingTime->setSegmentStyle (QLCDNumber::Outline);
    }
    else
    {
        ui.btnBreak->setText (BREAK);
        ui.btnBreak->setIcon (QIcon(ICON_BREAK));
        ui.workingTime->setSegmentStyle (QLCDNumber::Flat);
    }
}

void MainDialog::undo ()
{
    m_wtClock.undo ();
    int size = ui.treeWidget->topLevelItemCount();
    if (size > 0)
    {
        delete ui.treeWidget->takeTopLevelItem (size - 1);
        updateWidgetStyles ();
        updateTime ();
        setupTimer ();
        if (size == 1)
            ui.workingTime->display ("--:--");
    }
}

void MainDialog::showAbout()
{
    QMessageBox::about(this, tr("About Stechuhr"), tr(
    "<H2><b>Stechuhr</b></H2>"
    "Version %1 (%2)<br><br>"

    "A working time tracking utility.<br>"
    "<a href=https://github.com/amartin755/stechuhr>"
    "https://github.com/amartin755/stechuhr</a><br><br>"

    "Copyright (C) 2023 Andreas Martin<br><br>"

    "This program is covered by the GNU General Public License, version 3 (GPLv3),"
    "<a href=http://www.gnu.org/licenses>http://www.gnu.org/licenses</a><br>"
    "It uses the 3rd party components, covered by their respective license:<br><br>"

    "QT [v%3] (<a href=http://qt.io>http://qt.io</a>)<br>"
    "Breezy icons (<a href=https://github.com/KDE/breeze-icons>https://github.com/KDE/breeze-icons></a>)"

    "<br><br>Build:<br>%4-%5 (%6)"
    )
    .arg (APP_VERSION)
    .arg (BUILD_TIME)
    .arg (QT_VERSION_STR)
    .arg (GIT_BRANCH)
    .arg (GIT_COMMIT)
    .arg (BUILD_TYPE));
}

void MainDialog::closeEvent(QCloseEvent *event)
{
    QSettings s;
    s.beginGroup ("gui-state");
    s.setValue("main-dialog", saveGeometry());
    s.setValue("main-splitter", ui.splitter->saveState());
    s.endGroup ();

    QDialog::closeEvent(event);
}

void MainDialog::readSettings()
{
    QSettings s;
    s.beginGroup ("gui-state");
    restoreGeometry(s.value("main-dialog").toByteArray());
    ui.splitter->restoreState(s.value("main-splitter").toByteArray());
    s.endGroup ();
}

void MainDialog::editEvent(QTreeWidgetItem *item, int)
{
    int pos = ui.treeWidget->indexOfTopLevelItem (item);
    QDateTime time, min, max;
    if (m_wtClock.getTimeOfEvent (pos, time, min, max))
    {
        DateTimeInputDialog dlg (time, this);
        if (dlg.exec () == QDialog::Accepted)
        {
            time = dlg.dateTime ();

            if (time > max || time < min)
            {
                QMessageBox::critical (this, "", 
                    tr("Allowed range of entered date and time must be between<br>%1 and %2")
                    .arg (QLocale::system().toString(min, QLocale::ShortFormat))
                    .arg (QLocale::system().toString(max, QLocale::ShortFormat)));
            }
            else
            {
                m_wtClock.setTimeOfEvent (pos, time);
                updateList (pos, time);
                updateTime ();
            }
        }
    }
}