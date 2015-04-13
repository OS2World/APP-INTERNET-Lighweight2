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

#include "enableplugin.h"
#include "ui_enableplugin.h"

#include <QWebView>
#include <QTimer>

EnablePlugin::EnablePlugin(QString host, PluginFactory* fac, QWidget *parent) : QWidget(parent), ui(new Ui::EnablePlugin) {
	ui->setupUi(this);
	m_host = host;
	m_factory = fac;
	ui->bEnable->setText(tr("Enable plugins for\n%1").arg(host));
	ui->bEnable->setToolTip(ui->bEnable->text());

	connect(ui->bEnable, SIGNAL(clicked()), this, SLOT(enablePlugins()));

	if (!fac->knownHosts().contains(host)) fac->disallowHost(host);

	QTimer::singleShot(1000, this, SLOT(show()));
}

EnablePlugin::~EnablePlugin() {
	delete ui;
}

void EnablePlugin::enablePlugins() {
	m_factory->allowHost(m_host, ui->cbSave->isChecked());
	reload();
}

void EnablePlugin::reload() {
	QObject* w = parent();
	while (w) {
		if (w->inherits("QWebView")) {
			((QWebView*) w)->reload();
			break;
		}

		w = w->parent();
	}
}
