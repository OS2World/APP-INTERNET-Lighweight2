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

#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <QWidget>
#include <QTime>
#include <QFile>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QCryptographicHash>

namespace Ui {
	class Download;
}

class Download : public QWidget {
	Q_OBJECT

	public:
		explicit Download(QUrl url, QString path, QWidget *parent = 0);
		~Download();
		bool isRunning() { return !finished; }

		static QNetworkAccessManager* manager;

	signals:
		void stoppedDownload(Download* d);
		void downloadFinished();

	private slots:
		void updateProgress(qint64 progess, qint64 total);
		void finishDownload(QNetworkReply* reply);

		void stopDownload();

		void setCheckSum();

		void openFile();

	private:
		void getCheckSum();
		void check();

		void init(QNetworkReply* reply, QString path);
		Ui::Download *ui;
		QNetworkReply* download;
		QCryptographicHash* hash;

		QNetworkReply* sumDl;
		QNetworkReply* dotSHA1;
		QNetworkReply* SHA1SUMS;

		int checkSumTry;

		QString checkSum;
		QString orgFileName;
		bool finished;
		QTime* time;
		QFile saveFile;
};

#endif // DOWNLOAD_H
