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

#include "askdownload.h"
#include "ui_askdownload.h"

#include <QFileInfo>
#include <QFileDialog>

AskDownload* AskDownload::instance = 0;

AskDownload::AskDownload(QWidget *parent) : QDialog(parent), ui(new Ui::AskDownload) {
	ui->setupUi(this);

	ui->bSaveTo->setFlat(false);
	ui->bAbort->setFlat(false);

	connect(ui->bSaveTo, SIGNAL(clicked()), this, SLOT(accept()));
	connect(ui->bAbort, SIGNAL(clicked()), this, SLOT(reject()));
}

AskDownload::~AskDownload() {
	delete ui;
}

QString AskDownload::askForDownload(QUrl url) {
	ui->lFile->setText(QFileInfo(url.path()).fileName());
	ui->lUrl->setText(url.toString().left(url.toString().size()-ui->lFile->text().size()));

	exec();

	if (result() == QDialog::Accepted) {
		return getDLPath();
	}
	return "";
}

QString AskDownload::getDLPath() {
	QString path = QFileDialog::getExistingDirectory(this, tr("Choose directory"), QDir::homePath()+"/Downloads");
	if (path.isNull() || path.isEmpty()) {
		reject();
		return "";
	}
	return path;
}
