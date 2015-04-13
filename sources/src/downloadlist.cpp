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

#include "downloadlist.h"
#include "ui_downloadlist.h"

#include <QDebug>

DownloadList::DownloadList(QWidget *parent) : QScrollArea(parent), ui(new Ui::DownloadList) {
	ui->setupUi(this);
	running = 0;
}

DownloadList::~DownloadList() {
	delete ui;
}

void DownloadList::addDownload(QUrl url, QString path) {
	Download* d = new Download(url, path, this);
	ui->layout->insertWidget(0, d);
	emit(hasDownloads(true));
	emit(downloadRunning(true));

	running++;
	qDebug() << "Downloads running: " << running;

	connect(d, SIGNAL(stoppedDownload(Download*)), this, SLOT(stopDownload(Download*)));
	connect(d, SIGNAL(downloadFinished()), this, SLOT(finishDownload()));

}

void DownloadList::stopDownload(Download* d) {
	ui->layout->removeWidget(d);

	if (d->isRunning()) running--;
	qDebug() << "Downloads running: " << running;
	if (running == 0) emit(downloadRunning(false));

	if (ui->layout->count() == 1) emit(hasDownloads(false));

	d->deleteLater();
}

void DownloadList::finishDownload() {
	running--;
	qDebug() << "Downloads running: " << running;
	if (running == 0) emit(downloadRunning(false));
}
