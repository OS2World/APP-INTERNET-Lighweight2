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

#include "pluginmanager.h"
#include "ui_pluginmanager.h"

#include <QCheckBox>

PluginManager::PluginManager(QWidget *parent) : QDialog(parent), ui(new Ui::PluginManager) {
	ui->setupUi(this);
	factory = PluginFactory::factory;

	initGUI();

	connect(ui->bClose, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ui->cbAllowAll, SIGNAL(clicked(bool)), this, SLOT(setAllowPolicy(bool)));
}

PluginManager::~PluginManager() {
	delete ui;
}

void PluginManager::showManager() {
	ui->cbAllowAll->setChecked(factory->isAllowedForAll());

	initGUI();
	exec();
}

void PluginManager::reject() {
	factory->saveAllowedHosts();
	QDialog::reject();
}

void PluginManager::setAllowPolicy(bool allowAll) {
	factory->setAllowAll(allowAll);
}

void PluginManager::initGUI() {
	while (!widgets.isEmpty()) {
		delete widgets.first();
		widgets.removeFirst();
	}

	QStringList hosts = factory->knownHosts();
	int row = 1;
	foreach (QString host, hosts) {
		QLabel* lHost = new QLabel(host, this);
		QCheckBox* cbAllow = new QCheckBox("", this);
		QCheckBox* cbAllowPerm = new QCheckBox("", this);

		if (factory->isAllowed(host)) {
			cbAllow->setChecked(true);
			if (factory->isAllowedPermamently(host)) cbAllowPerm->setChecked(true);
		} else {
			cbAllowPerm->setEnabled(false);
		}

		PluginHelper* helper = new PluginHelper(this);
		helper->setHost(host);
		helper->setAllowCB(cbAllow);
		helper->setPermCB(cbAllowPerm);
		connect(cbAllow, SIGNAL(clicked()), helper, SLOT(setAllow()));
		connect(cbAllowPerm, SIGNAL(clicked()), helper, SLOT(setAllowPerm()));

		widgets.append(lHost);
		widgets.append(cbAllow);
		widgets.append(cbAllowPerm);
		widgets.append(helper);

		ui->items->addWidget(lHost, row, 0);
		ui->items->addWidget(cbAllow, row, 2, Qt::AlignCenter);
		ui->items->addWidget(cbAllowPerm, row, 4, Qt::AlignCenter);

		row++;
	}
}
