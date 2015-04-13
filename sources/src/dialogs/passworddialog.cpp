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

#include "passworddialog.h"
#include "ui_passworddialog.h"

PasswordDialog::PasswordDialog(QAuthenticator* a, QWidget *parent) : QDialog(parent), ui(new Ui::PasswordDialog) {
	ui->setupUi(this);
	auth = a;

	connect(ui->rbShowPassw, SIGNAL(toggled(bool)), this, SLOT(setPasswordMode(bool)));
	connect(ui->tfPassw, SIGNAL(returnPressed()), this, SLOT(accept()));
}

PasswordDialog::~PasswordDialog() {
	delete ui;
}

void PasswordDialog::reject() {
	QDialog::reject();
}

void PasswordDialog::accept() {
	auth->setUser(ui->tfUser->text());
	auth->setPassword(ui->tfPassw->text());
	QDialog::accept();
}

void PasswordDialog::setPasswordMode(bool shown) {
	if (!shown) {
		ui->tfPassw->setEchoMode(QLineEdit::Password);
	} else {
		ui->tfPassw->setEchoMode(QLineEdit::Normal);
	}
}
