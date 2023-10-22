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
#include <QStandardPaths>
#include <QStringList>
#include <QFileDialog>

#include "settingsdialog.h"

SettingsDialog::SettingsDialog (QWidget *parent)
    : QDialog(parent)
{
    m_gui.setupUi (this);

    QSettings s;
    s.beginGroup ("logbook");
    m_gui.groupLogbook->setChecked (s.value ("enabled", true).toBool());

    QString dbPath = s.value ("db").toString();
    if (dbPath.isEmpty ())
    {
        getDefaultDbPath (dbPath);
        dbPath += "/logbook.sqlite";
    }
    m_gui.textEditPath->setText (dbPath);

    connect (this, &QDialog::accepted, this, &SettingsDialog::onAccepted);
    connect (m_gui.btnSelectDbPath, &QPushButton::clicked, this, &SettingsDialog::selectDbFile);
}

void SettingsDialog::getDefaultDbPath (QString& path) const
{
    path = QStandardPaths::writableLocation (QStandardPaths::AppDataLocation);
qWarning() << "defaultPath: " << path;
}

void SettingsDialog::onAccepted ()
{
    QSettings s;
    s.beginGroup ("logbook");
    s.setValue ("enabled", m_gui.groupLogbook->isEnabled ());
    s.setValue ("db", m_gui.textEditPath->text());
}

void SettingsDialog::selectDbFile ()
{
    //FIXME DontConfirmOverwrite seems to be ignored on gnome
    QString f = QFileDialog::getSaveFileName (this, "", m_gui.textEditPath->text (), "", nullptr, QFileDialog::DontConfirmOverwrite);
}
