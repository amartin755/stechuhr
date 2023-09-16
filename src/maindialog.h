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

#include "ui_mainDialog.h"
#include "stechuhr.h"

class MainDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MainDialog(QWidget *parent = nullptr);

private slots:
    void updateTime ();
    void clockInOut ();
    void breakStartStop ();

private:
    void updateList (const QString& iconPath, const QString& caption, const QDateTime& time);
    Stechuhr m_wtClock;

    Ui::Dialog ui;
    QTimer* m_updateTimer;
};

#endif
