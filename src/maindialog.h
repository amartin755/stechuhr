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


#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QTimer>
#include <QDateTime>
#include <QList>
#include <QPair>
#include <QApplication>

#include "ui_mainDialog.h"
#include "stechuhr.h"

class MainDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MainDialog(QApplication* theApp, QWidget *parent = nullptr);

private slots:
    void updateTime ();
    void clockInOut ();
    void breakStartStop ();
    void saveState ();

    void onClockedIn (const QDateTime&);
    void onClockedOut (const QDateTime&);
    void onBreakStarted (const QDateTime&);
    void onBreakFinished (const QDateTime&);
    void showAbout ();
    void undo ();

private:
    void updateList (const QString& iconPath, const QString& caption, const QDateTime& time);
    void updateWidgetStyles ();
    void setupTimer ();
    void keyPressEvent (QKeyEvent *e);
    void closeEvent (QCloseEvent *event);
    void readSettings ();
    Stechuhr m_wtClock;

    Ui::Dialog ui;
    QTimer* m_updateTimer;
    const QString CLOCK_IN;
    const QString CLOCK_OUT;
    const QString BREAK;
    const QString BREAK_END;
    const QString ICON_CLOCK_IN;
    const QString ICON_CLOCK_OUT;
    const QString ICON_BREAK;
    const QString ICON_BREAK_END;
};

#endif
