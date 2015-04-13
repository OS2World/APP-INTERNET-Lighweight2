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

#include "download.h"
#include "ui_download.h"

#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QDesktopServices>

QNetworkAccessManager* Download::manager = 0;

Download::Download(QUrl url, QString path, QWidget *parent) : QWidget(parent), ui(new Ui::Download) {
	ui->setupUi(this);

	checkSumTry = 0;
	checkSum = "NoCheck";
	sumDl = 0;

	ui->lFile->setText(QFileInfo(url.path()).fileName());
	ui->lUrl->setText(url.toString().left(url.toString().size()-ui->lFile->text().size()));
	ui->bOpen->hide();

	init(manager->get(QNetworkRequest(url)), path);

#if defined(Q_WS_WIN)
	ui->frame->setFrameShape(QFrame::Panel);
	ui->lEta->setFont(QFont("Courier New", 10));
#endif
}

Download::~Download() {
	disconnect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finishDownload(QNetworkReply*)));
	disconnect(download, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateProgress(qint64,qint64)));

	delete ui;
	delete hash;
	ui = 0;
	hash = 0;

	if (download->isOpen())
		download->close();
	if (saveFile.isOpen())
		saveFile.close();

	if (sumDl && sumDl->isOpen())
		sumDl->close();

	delete download;
	delete time;
	if (dotSHA1) delete dotSHA1;
	if (SHA1SUMS) delete SHA1SUMS;

	download = 0;
	time = 0;
}

void Download::updateProgress(qint64 progress, qint64 total) {
	int percent = 10000*progress/total;

	//Save current progress to file
	while (download->bytesAvailable() > 0) {
		QByteArray arr = download->read(1048576); //Read max 1MiB from download
		saveFile.write(arr);
		hash->addData(arr);
		saveFile.flush();
	}

	ui->pbProgress->setValue(percent);

	//Calculate ETA
	//Get the estimates download resttime
	int timeUsed = time->elapsed();
	int restTime = (progress>0)?((((float) timeUsed)/progress)*total-timeUsed)/1000:0; //Gets the resttime in secs

	//Show the rest of the Bytes
	qint64 rest = total-progress;

	//But only MiB or higher
	rest /= 1024;
	int kiBytes = rest%1024;
	rest /= 1024;
	int miBytes = rest%1024;
	rest /= 1024;
	int giBytes = rest%1024;
	rest /= 1024;
	int tiBytes = rest%1024;
	rest /= 1024;
	//EiB should be large enough for the next years
	int eiBytes = rest;

	QString restBytes = "";
	if (eiBytes > 0)      restBytes = "%1EiB %2TiB %3GiB %4MiB %5KiB";
	else if (tiBytes > 0) restBytes =       "%2TiB %3GiB %4MiB %5KiB";
	else if (giBytes > 0) restBytes =             "%3GiB %4MiB %5KiB";
	else if (miBytes > 0) restBytes =                   "%4MiB %5KiB";
	else                  restBytes =                         "%5KiB";

	if (restBytes.indexOf("%1") != -1) restBytes = restBytes.arg(QString::number(eiBytes).rightJustified(4, ' '));
	if (restBytes.indexOf("%2") != -1) restBytes = restBytes.arg(QString::number(tiBytes).rightJustified(4, ' '));
	if (restBytes.indexOf("%3") != -1) restBytes = restBytes.arg(QString::number(giBytes).rightJustified(4, ' '));
	if (restBytes.indexOf("%4") != -1) restBytes = restBytes.arg(QString::number(miBytes).rightJustified(4, ' '));
	if (restBytes.indexOf("%5") != -1) restBytes = restBytes.arg(QString::number(kiBytes).rightJustified(4, ' '));

	ui->lEta->setText(restBytes+"; "+QString::number(restTime/60)+":"+QString::number(restTime%60).rightJustified(2, '0'));
}

void Download::finishDownload(QNetworkReply *reply) {
	if (reply == sumDl) setCheckSum(); //Download of checksum-file is finished
	if (download != reply) return;

	//Our Download is finished
	if (saveFile.isOpen())
		saveFile.close();

	if (ui != 0) {
		//Only if download is not deleted
		ui->lEta->hide();
		ui->pbProgress->setFormat(tr("Finished"));
	}

	emit(downloadFinished());

	ui->bStop->setIcon(QIcon(":/icons/close.png"));
	ui->bStop->setToolTip(tr("Remove download from list"));
	finished = true;

	if (hash) {
		qDebug() << "Calculatedhash: "+QString::fromUtf8(hash->result().toHex());
		check();
	}

	ui->bOpen->show();
}

void Download::stopDownload() {
	disconnect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finishDownload(QNetworkReply*)));
	emit(stoppedDownload(this));
}

void Download::setCheckSum() {
	if (!sumDl) {
		checkSum = "NoCheck";
		return;
	}

	if (!sumDl->error()) {

		QByteArray arr;
		QString end;
		switch(checkSumTry) {
		case 0:
			checkSum = QString::fromUtf8(sumDl->read(40));
			if (!QRegExp("[0123456789abcdef]+").exactMatch(checkSum)) {
				checkSum = "NoCheck";
				break;
			}
			qDebug() << "Downloadedhash: "+checkSum;
			sumDl->close();
			if (finished) check();
			break;
		case 1:
			//Search for the right line
			end = orgFileName+"\n";
			arr = sumDl->readLine();
			while (arr.size() > 0) {
				QString line = QString::fromUtf8(arr);
				if (line.endsWith(end)) {
					checkSum = line.left(40);
					qDebug() << "Downloadedhash: "+checkSum;
					break;
				}
				arr = sumDl->readLine();
			}
			sumDl->close();
			break;
		default:
			checkSum = "NoCheck";
		}
	}
	if ((checkSum == "NoCheck") && (checkSumTry < 2)) {
		checkSumTry++;
		getCheckSum();
	}

	if ((checkSum != "NoCheck") && finished) check();
}

void Download::openFile() {
	if (!finished) return;
#ifdef Q_OS_OS2
	QDesktopServices::openUrl(QUrl("file:///"+QFileInfo(saveFile.fileName()).absoluteFilePath(), QUrl::TolerantMode));
#else
	QDesktopServices::openUrl(QUrl(saveFile.fileName()));
#endif
}

void Download::getCheckSum() {

	switch (checkSumTry) {
	case 0:
		//Sha1-sum at url.sha1
		dotSHA1 = manager->get(QNetworkRequest(QUrl(ui->lUrl->text()+orgFileName+".sha1")));
		sumDl = dotSHA1;
		break;
	case 1:
		//Sha1-sum in in urlPath/SHA1SUMS
		SHA1SUMS = manager->get(QNetworkRequest(QUrl(ui->lUrl->text()+"SHA1SUMS")));
		sumDl = SHA1SUMS;
		break;
	default:
		checkSum = "NoCheck";
		sumDl = 0;
	}
	if (sumDl) qDebug() << sumDl->url();
}

void Download::check() {
	if (checkSum == "NoCheck") return;
	if (checkSum == QString::fromUtf8(hash->result().toHex())) {
		ui->pbProgress->setFormat(ui->pbProgress->format()+tr(" (Is ok.)"));
	} else {
		ui->pbProgress->setFormat(ui->pbProgress->format()+ tr(" (Is NOT ok.)"));
	}
}

void Download::init(QNetworkReply *reply, QString path) {
	hash = new QCryptographicHash(QCryptographicHash::Sha1);
	time = new QTime();
	finished = false;

#if defined(Q_OS_WIN)
	if (!path.endsWith("\\")) path += "/";
#else
	if (!path.endsWith("/")) path += "/";
#endif

	orgFileName = ui->lFile->text();
	QFileInfo fi(path+ui->lFile->text());

	//Prevent overwriting files by creating a new name
	if (fi.exists()) {
		int i = 1;
		QString baseName = fi.completeBaseName();
		QString suffix = "."+fi.suffix();

		//A small workaround for tar.* files:
		if (baseName.endsWith(".tar")) {
			suffix = ".tar"+suffix;
			baseName = baseName.left(baseName.size()-4);
		}

		baseName += "_%1";

		do {
			fi.setFile(path+baseName.arg(i)+suffix);
			ui->lFile->setText(baseName.arg(i)+suffix);
			i++;
		} while (fi.exists());
		saveFile.setFileName(fi.filePath());
	} else {
		saveFile.setFileName(fi.filePath());
	}

	saveFile.open(QFile::WriteOnly);

	download = reply;
	time->start();

	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finishDownload(QNetworkReply*)));
	connect(download, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateProgress(qint64,qint64)));

	connect(ui->bStop, SIGNAL(clicked()), this, SLOT(stopDownload()));

	connect(ui->bOpen, SIGNAL(clicked()), this, SLOT(openFile()));

	//Start download of Sha1sum
	dotSHA1 = 0;
	SHA1SUMS = 0;
	getCheckSum();
}
