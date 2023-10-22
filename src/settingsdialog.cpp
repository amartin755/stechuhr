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
#include <QSettings>

#include "settingsdialog.h"

SettingsDialog::SettingsDialog (QWidget *parent)
    : QDialog(parent)
{
    m_gui.setupUi (this);

    QSettings s;
    s.beginGroup ("logbook");
    m_gui.groupLogbook->setChecked(s.value ("enabled", true).toBool());
qWarning() << s.fileName();
    //m_gui.textEditPath.setPlainText
}
