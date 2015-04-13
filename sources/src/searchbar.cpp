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

#include "searchbar.h"
#include "ui_searchbar.h"

SearchBar::SearchBar(QWidget *parent) : QWidget(parent), ui(new Ui::SearchBar) {
	ui->setupUi(this);

	setMaximumHeight(32);
	ui->tfSearch->setFixedHeight(28);

	connect(ui->bLast, SIGNAL(clicked()), this, SLOT(searchLast()));
	connect(ui->bNext, SIGNAL(clicked()), this, SLOT(searchNext()));
	connect(ui->tfSearch, SIGNAL(returnPressed()), this, SLOT(searchNext()));
	connect(ui->bClose, SIGNAL(clicked()), this, SLOT(reset()));
}

SearchBar::~SearchBar() {
	delete ui;
}

void SearchBar::gainFocus() {
	ui->tfSearch->setFocus(Qt::OtherFocusReason);
}

void SearchBar::searchLast() {
	QWebPage::FindFlags flags = QWebPage::FindBackward;
	if (ui->cbCS->isChecked()) flags |= QWebPage::FindCaseSensitively;
	bool found = true;
	emit(searchFor(ui->tfSearch->text(), flags, &found));

	if (found) {
		ui->tfSearch->setStyleSheet("background:rgb(192,255,192); font:black;");
	} else {
		ui->tfSearch->setStyleSheet("background:rgb(255,192,192); font:black;");
	}
}

void SearchBar::searchNext() {
	QWebPage::FindFlags flags = 0;
	if (ui->cbCS->isChecked()) flags |= QWebPage::FindCaseSensitively;
	bool found = true;
	emit(searchFor(ui->tfSearch->text(), flags, &found));

	if (found) {
		ui->tfSearch->setStyleSheet("background:rgb(192,255,192); font:black;");
	} else {
		ui->tfSearch->setStyleSheet("background:rgb(255,192,192); font:black;");
	}
}

void SearchBar::reset() {
	ui->tfSearch->setText("");
	ui->tfSearch->setStyleSheet("");
	ui->cbCS->setChecked(false);
	hide();
}
