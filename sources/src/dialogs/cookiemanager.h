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

#ifndef COOKIEMANAGER_H
#define COOKIEMANAGER_H

#include <QDialog>
#include <QCheckBox>

#include "src/cookiejar.h"

namespace Ui {
	class CookieManager;
}

class CookieManager : public QDialog {
	Q_OBJECT

	public:
		explicit CookieManager(QWidget *parent = 0);
		~CookieManager();

	public slots:
		void showManager();
		void reject();

		void setSaveCookiePolicy(bool saveAll);

	private:
		void initGUI();
		QList<QObject*> widgets;
		Ui::CookieManager *ui;
};

class CookieHelper : public QObject {
	Q_OBJECT

	public:
		explicit CookieHelper(QObject* parent=0) : QObject(parent) {}
		void setDomain(QString h) { domain = h; }
		void setSaveCB(QCheckBox* save) {cbSave = save; }

	public slots:
		void setSave() {
			if (cbSave->isChecked()) {
				CookieJar::jar->addToWhiteList(domain);
			} else {
				CookieJar::jar->removeFromWhiteList(domain);
			}
		}

	private:
		QString domain;
		QCheckBox* cbSave;
};
#endif // COOKIEMANAGER_H
