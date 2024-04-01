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

#include <QFileDialog>

#include "settingsdialog.h"


SettingsDialog::SettingsDialog (Settings& settings, QWidget *parent)
    : QDialog(parent), m_settings (settings)
{
    m_gui.setupUi (this);

    m_gui.groupLogbook->setChecked (m_settings.isDbEnabled ());
    m_gui.textEditPath->setText (m_settings.getDbPath ());

    connect (this, &QDialog::accepted, this, &SettingsDialog::onAccepted);
    connect (m_gui.btnSelectDbPath, &QPushButton::clicked, this, &SettingsDialog::selectDbFile);
}

void SettingsDialog::onAccepted ()
{
    m_settings.setDbPath (m_gui.textEditPath->text());
    m_settings.setDbEnabled (m_gui.groupLogbook->isChecked ());
}

void SettingsDialog::selectDbFile ()
{
    //FIXME DontConfirmOverwrite seems to be ignored on gnome
    QString f = QFileDialog::getSaveFileName (this, "", m_gui.textEditPath->text (), "", nullptr, QFileDialog::DontConfirmOverwrite);
}
