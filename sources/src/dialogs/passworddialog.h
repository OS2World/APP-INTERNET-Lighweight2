// Copyright 2011 Thomas Kamps anubis1@linux-ecke.de
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef PASSWORDDIALOG_H
#define PASSWORDDIALOG_H

#include <QDialog>
#include <QAuthenticator>

namespace Ui {
    class PasswordDialog;
}

class PasswordDialog : public QDialog {
	Q_OBJECT

	public:
		explicit PasswordDialog(QAuthenticator* a, QWidget *parent = 0);
		~PasswordDialog();

	public slots:
		void reject();
		void accept();

	private slots:
		void setPasswordMode(bool shown);

	private:
		Ui::PasswordDialog *ui;
		QAuthenticator* auth;
};

#endif // PASSWORDDIALOG_H
