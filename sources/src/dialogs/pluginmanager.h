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

#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QDialog>
#include <QCheckBox>
#include "src/pluginfactory.h"

namespace Ui {
	class PluginManager;
}

class PluginManager : public QDialog {
	Q_OBJECT

	public:
		explicit PluginManager(QWidget *parent = 0);
		~PluginManager();

	public slots:
		void showManager();
		void reject();
		void setAllowPolicy(bool allowAll);

	private:
		void initGUI();

		QList<QObject*> widgets;

		Ui::PluginManager *ui;
		PluginFactory* factory;

};

class PluginHelper : public QObject {
	Q_OBJECT

	public:
		explicit PluginHelper(QObject* parent=0) : QObject(parent) {}
		void setHost(QString h) { host = h; }
		void setAllowCB(QCheckBox* allow) {cbAllow = allow; }
		void setPermCB(QCheckBox* perm) {cbPerm = perm; }

	public slots:
		void setAllow() {
			if (cbAllow->isChecked()) {
				PluginFactory::factory->allowHost(host, cbPerm->isChecked());
				cbPerm->setEnabled(true);
			} else {
				PluginFactory::factory->disallowHost(host, false);
				cbPerm->setChecked(false);
				cbPerm->setEnabled(false);
			}
		}

		void setAllowPerm() {
			if (cbPerm->isChecked()) {
				PluginFactory::factory->allowHost(host, true);
			} else {
				PluginFactory::factory->disallowHost(host, true);
			}
		}

	private:
		QString host;
		QCheckBox* cbAllow;
		QCheckBox* cbPerm;
};

#endif // PLUGINMANAGER_H
