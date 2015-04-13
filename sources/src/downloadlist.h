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

#ifndef DOWNLOADLIST_H
#define DOWNLOADLIST_H

#include <QScrollArea>
#include <QUrl>

#include "download.h"

namespace Ui {
	class DownloadList;
}

class DownloadList : public QScrollArea {
	Q_OBJECT

	public:
		explicit DownloadList(QWidget *parent = 0);
		~DownloadList();

	signals:
		void hasDownloads(bool downloads);
		void downloadRunning(bool running);

	public slots:
		void addDownload(QUrl url, QString path);

	private slots:
		void stopDownload(Download* d);
		void finishDownload();

	private:
		Ui::DownloadList *ui;
		int running;
};

#endif // DOWNLOADLIST_H
