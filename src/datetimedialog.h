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


#ifndef DATETIMEDIALOG_H
#define DATETIMEDIALOG_H

#include <QDateTime>

#include "ui_dateTimeDialog.h"

class DateTimeInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DateTimeInputDialog(const QDateTime &dateTime, QWidget *parent = nullptr);
    QDateTime dateTime () const;

private:
    Ui::dateTimDialog m_gui;
};

#endif
