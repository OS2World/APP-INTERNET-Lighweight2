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

#include <QtGui/QApplication>
#include <QTranslator>
#include <QLocale>
#include <QWebSettings>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QDesktopServices>
#include <QDir>
#include <QDebug>
#include <QSettings>
#include <QLibraryInfo>

#include <QImageReader>

#include "lightweight.h"
#include "cookiejar.h"
#include "pluginfactory.h"
#include "dialogs/askdownload.h"

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);

	//Setup Appname and version
	a.setApplicationName("Lightweight2");
	a.setOrganizationName("Lightweight2");
	a.setApplicationVersion("1.9");

	//Create used directorys
	QDir(QDir::homePath()).mkpath(".lightweight2/bookmarkicons");
	QDir(QDir::homePath()).mkpath(".lightweight2/searchengines");
	QDir(QDir::homePath()).mkpath("Downloads");

	//Setup shared NetworkAccessManager for all actions
	Download::manager = new QNetworkAccessManager();
	WebView::manager = Download::manager;
	CookieJar::jar = new CookieJar();
	WebView::manager->setCookieJar(CookieJar::jar);

	PluginFactory::factory = new PluginFactory();

	//Setup disk cache
	QNetworkDiskCache* cache = new QNetworkDiskCache();
	cache->setCacheDirectory(QDesktopServices::storageLocation(QDesktopServices::CacheLocation));
	qDebug() << "Cache directory: " << cache->cacheDirectory();
	cache->setMaximumCacheSize(128*1024*1024); //128MiB Cache
	WebView::manager->setCache(cache);

	//Load translations
	QTranslator transqt; //Qt given translations
	transqt.load(QLibraryInfo::location(QLibraryInfo::TranslationsPath)+QString("/qt_%1").arg(QLocale::system().name()), QCoreApplication::applicationDirPath());
	a.installTranslator(&transqt);

	QTranslator trans;   //Translations from local compile
	trans.load(QString("ts/Lightweight2_%1").arg(QLocale::system().name()), QCoreApplication::applicationDirPath());
	a.installTranslator(&trans);

#if defined(Q_OS_UNIX)
	//Only for unix systems
	QTranslator transsys; //Tranlations for installation
	transsys.load(QString("ts/Lightweight2_%1").arg(QLocale::system().name()), PKGDATADIR);
	a.installTranslator(&transsys);
#endif

	//Change some web settings
	QWebSettings::globalSettings()->setIconDatabasePath(QDir::homePath() + "/.lightweight2");
	QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
	QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

	Lightweight w;

	//Set up ask download question
	AskDownload::instance = new AskDownload(&w);

	//Load command line arguments and load the given urls
	QStringList urls = a.arguments();
	for (int i = 1; i < urls.size(); i++) { //Skip first "url": its the app-path
		QString url = (QString) urls.value(i);
		QFileInfo info(url);
		if (info.exists()) w.openUrl(info.canonicalFilePath());
		else w.openUrl(url);
	}

	w.show();

	return a.exec();
}
