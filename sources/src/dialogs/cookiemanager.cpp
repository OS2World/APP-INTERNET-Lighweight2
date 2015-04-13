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

#include "cookiemanager.h"
#include "ui_cookiemanager.h"

CookieManager::CookieManager(QWidget *parent) : QDialog(parent), ui(new Ui::CookieManager) {
	ui->setupUi(this);

	connect(ui->bClose, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ui->cbSaveAll, SIGNAL(clicked(bool)), this, SLOT(setSaveCookiePolicy(bool)));

}

CookieManager::~CookieManager() {
	delete ui;
}

void CookieManager::showManager() {
	ui->cbSaveAll->setChecked(CookieJar::jar->savesAllCookies());
	initGUI();
	exec();
}

void CookieManager::reject() {
	CookieJar::jar->saveWhiteList();
	QDialog::reject();
}

void CookieManager::setSaveCookiePolicy(bool saveAll) {
	CookieJar::jar->setSaveAll(saveAll);
}

void CookieManager::initGUI() {
	//Delete all old widgets
	while (!widgets.isEmpty()) {
		delete widgets.first();
		widgets.removeFirst();
	}

	//Create new widgets
	int row = 1;
	QStringList domains = CookieJar::jar->knownDomains();
	foreach (QString domain, domains) {
		QLabel* lDomain = new QLabel(domain, this);
		QCheckBox* cbSave = new QCheckBox(this);
		cbSave->setChecked(CookieJar::jar->isOnWhiteList(domain));
		CookieHelper* helper = new CookieHelper(this);
		helper->setDomain(domain);
		helper->setSaveCB(cbSave);
		connect(cbSave, SIGNAL(clicked()), helper, SLOT(setSave()));

		widgets.append(lDomain);
		widgets.append(cbSave);
		widgets.append(helper);

		ui->items->addWidget(lDomain, row, 0);
		ui->items->addWidget(cbSave, row, 2,Qt::AlignCenter);
		row++;
	}
}
