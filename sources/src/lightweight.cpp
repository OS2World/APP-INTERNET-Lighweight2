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

#include "lightweight.h"
#include "ui_lightweight.h"

#include <QFile>
#include <QDir>
#include <QShortcut>
#include <QCryptographicHash>
#include <QSettings>

#include <QDebug>

#include "cookiejar.h"

#if defined(Q_OS_WIN)
//Only needed for Win
#include "os/qtwin.h"
#endif

//#define OTHER_OS //Uncomment it, if you want to compile it for non supportet OS that have problems with QSettings

Lightweight::Lightweight(QWidget *parent) : QWidget(parent), ui(new Ui::Lightweight) {
	ui->setupUi(this);

	//Create layout for tab-contents
	tabs = new QStackedLayout();
	tabList = new QList<BrowserTab*>();
	ui->verticalLayout->addLayout(tabs);
	ui->verticalLayout->setSpacing(0);

	currentTab = 0;
	fullscreen = false;

	//Create downloadmanagerQNetworkCookieJar>
	downloads = new DownloadList(this);
	tabs->addWidget(downloads);
	ui->bDownloads->hide();

	//Load bookmarks from xml file
	QFile f(QDir::homePath()+"/.lightweight2/bookmarks.xml");
	QDomDocument d;
	d.setContent(&f);
	bookmarks = BookmarkFolder::read(this, d.documentElement(), true);

	//Load bookmarks from old fileformat into the root folder
	QFile fi(QDir::homePath()+"/.lightweight2/bookmarks");
	if (fi.exists()) {
		fi.open(QFile::ReadOnly);
		QString line = QString::fromUtf8(fi.readLine().data()).replace("\n", "");
		while (line != "END") {
			QString title = line;
			QString url = QString::fromUtf8(fi.readLine().data()).replace("\n", "");

			Bookmark* b = new Bookmark();
			b->setTitle(title);
			b->setUrl(QUrl(url));
			b->setIcon(Bookmark::iconFor(url));
			bookmarks->addBookmark(b);

			line = QString::fromUtf8(fi.readLine().data()).replace("\n", "");
		}
		fi.close();

		//Save the bookmarks and then delete the old file
		saveBookmarks();
		fi.remove();
	}

	bmm = new BookmarkManager(bookmarks,this);
	connect(bmm, SIGNAL(bookmarkRequest(BookmarkFolder*)), this, SLOT(createBookmark(BookmarkFolder*)));

	plugMan = new PluginManager(this);
	cookieMan = new CookieManager(this);

	ui->bNewTab->setMenu(bookmarks->action()->menu());
	bookmarks->emptyAction()->setShortcut(QKeySequence(tr("Ctrl+T")));
	ui->bNewTab->setDefaultAction(bookmarks->emptyAction());

	//Signals for the bookmarkmenu
	connect(bookmarks->action()->menu(), SIGNAL(triggered(QAction*)), this, SLOT(openBookmark(QAction*)));
	connect(bookmarks->action()->menu(), SIGNAL(triggered(QAction*)), bookmarks->action()->menu(), SLOT(hide()));

	//Control-buttons
	connect(ui->bBack, SIGNAL(clicked()), this, SLOT(back()));
	connect(ui->bForward, SIGNAL(clicked()), this, SLOT(forward()));
	connect(ui->bStop, SIGNAL(clicked()), this, SLOT(stop()));
	connect(ui->bReload, SIGNAL(clicked()), this, SLOT(reload()));

	//Search-Textfield
	connect(ui->tfSearch, SIGNAL(returnPressed()), this, SLOT(search()));

	//Downloads
	connect(ui->bDownloads, SIGNAL(clicked()), this, SLOT(showDownloads()));
	connect(downloads, SIGNAL(hasDownloads(bool)), this, SLOT(setDLButtonVisible(bool)));
	connect(downloads, SIGNAL(downloadRunning(bool)), this, SLOT(setDownloadIcon(bool)));

	//Create shortcuts
	new QShortcut(QKeySequence(tr("F11")), this, SLOT(switchFullscreen())); //Fullscreen
	new QShortcut(QKeySequence(tr("Ctrl+Tab")), this, SLOT(nextTab()));    //Navigate tabs
	new QShortcut(QKeySequence(tr("Ctrl+Right")), this, SLOT(nextTab()));
	new QShortcut(QKeySequence(tr("Ctrl+Left")), this, SLOT(prevTab()));
	new QShortcut(QKeySequence(tr("Ctrl+W")), this, SLOT(closeCurrentTab())); //Close current tab
	new QShortcut(QKeySequence(tr("Ctrl+F")), this, SLOT(showSearchBar())); //Searchbar
	new QShortcut(QKeySequence(tr("Ctrl+E")), this, SLOT(showUrlBar())); //Show/Hide urlbar
	new QShortcut(QKeySequence(tr("Ctrl+B")), this, SLOT(openBMM())); //BookmarkManager
	new QShortcut(QKeySequence(tr("Ctrl+M")), this, SLOT(switchLWButtonVisibility())); //show hide Lightweight2 button

	//Default search engine
	ui->cbSearchEngine->addItem(QIcon(":/icons/search/google.png"), QString(), QVariant(tr("http://www.google.com/search?q=%{query}&ie=UTF-8&oe=UTF-8")));
	ui->cbSearchEngine->addItem(QIcon(":/icons/search/wikipedia.png"), QString(), QVariant(tr("http://en.wikipedia.org/wiki/%{query}")));

	//Get user search engines
	QDir dir(QDir::homePath()+"/.lightweight2/searchengines");
	QStringList subDirs = dir.entryList(QDir::Dirs, QDir::Name);
	for (int i = 0; i < subDirs.size(); i++) {
		QString subDirStr = (QString) subDirs.value(i);
		QFile file(QDir::homePath()+"/.lightweight2/searchengines/"+subDirStr+"/info");
		if (!file.exists()) continue;
		file.open(QFile::ReadOnly);
		QString url = file.readLine().replace("\n", "");
		ui->cbSearchEngine->addItem(QIcon(QDir::homePath()+QString("/.lightweight2/searchengines/%1/icon.png").arg(subDirStr)), QString(), QVariant(url));
		file.close();
	}

	//Load settings
#ifndef OTHER_OS
	//Set the last used search engine
	QSettings s;
	ui->cbSearchEngine->setCurrentIndex(s.value("searchengine", "0").toInt());

	//restore window-pos ans size
	this->move(s.value("windowpos", QVariant(QPoint(0,0))).toPoint());
	this->resize(s.value("windowsize", QVariant(QSize(640,480))).toSize());

	//Set maximized state
	if (s.value("maximized", QVariant(false)).toBool())
		this->setWindowState(Qt::WindowMaximized);

	ui->bLW->setVisible(s.value("lightweight2buttonvisible", "true").toBool());

#else
	QFile settings(QDir::homePath()+"/.lightweight2/settings");
	if (settings.exists()) {
		settings.open(QFile::ReadOnly);
		QString line;
		while (settings.bytesAvailable()) {
			line = QString::fromUtf8(settings.readLine());
			line.chop(1);

			if (line.startsWith("searchengine"))
				ui->cbSearchEngine->setCurrentIndex(line.remove(0,13).toInt());
			if (line.startsWith("windowpos")) {
				QStringList data = line.split("=").value(1).split(",");
				move(data.value(0).toInt(), data.value(1).toInt());
			}
			if (line.startsWith("windowsize")) {
				QStringList data = line.split("=").value(1).split(",");
				resize(data.value(0).toInt(), data.value(1).toInt());
			}
			if (line.startsWith("maximized")) {
				if (line.remove(0,10) == "1") setWindowState(Qt::WindowMaximized);
			} if (line.startsWith("lightweight2buttonvisible")) {
				QString val = line.remove(0,26);
				ui->bLW->setVisible(val == "1");
			}
		}
	}
#endif

	//Create Lightweight2 menu
	lwMenu = new QMenu(this);
	acPrivate = new QAction(tr("Private browsing"), this);
	acPrivate->setCheckable(true);
	acBookmarks = new QAction(tr("Bookmarks"), this);
	acBookmarks->setIcon(QIcon(":/icons/bookmark.png"));
	acCookies = new QAction(tr("Cookie domains"), this);
	acPlugins = new QAction(tr("Plugin hosts"), this);
	acQuit = new QAction(tr("Quit"), this);
	acQuit->setIcon(QIcon(":/icons/quit.png"));

	connect(acPrivate, SIGNAL(toggled(bool)), this, SLOT(setPrivateBrowsing(bool)));
	connect(acBookmarks, SIGNAL(triggered()), this, SLOT(openBMM()));
	connect(acPlugins, SIGNAL(triggered()), this, SLOT(openPlugMan()));
	connect(acCookies, SIGNAL(triggered()), this, SLOT(openCookieMan()));
	connect(acQuit, SIGNAL(triggered()), this, SLOT(close()));

	lwMenu->addAction(acPrivate);
	lwMenu->addSeparator();
	lwMenu->addAction(acBookmarks);
	lwMenu->addAction(acPlugins);
	lwMenu->addAction(acCookies);
	lwMenu->addSeparator();
	lwMenu->addAction(acQuit);

	ui->bLW->setMenu(lwMenu);

#if defined(Q_OS_WIN)
	//Use on Win Vista or 7 the glass background, if aviable
	if (QtWin::isCompositionEnabled()) {
		QtWin::extendFrameIntoClientArea(this);
		QtWin::extendFrameIntoClientArea(bmm);
		QtWin::extendFrameIntoClientArea(plugMan);
		QtWin::extendFrameIntoClientArea(cookieMan);
	}
#endif

	//Create default tab
	newTab("about:blank", QIcon(":/icons/new.png"));
}

Lightweight::~Lightweight() {
	delete ui;
	while (tabList->size() > 0) {
		BrowserTab* tab = tabList->first();
		tabList->removeAll(tab);
		delete tab;
	}
	delete tabList;
}

void Lightweight::openUrl(QString url) {
	qDebug() << url;
	if ((currentTab != 0) && currentTab->isEmptyPage())
		currentTab->goTo(url, QIcon(), true);
	else newTab(url, QIcon(), true);
}

void Lightweight::closeEvent(QCloseEvent *e) {
	//Save bookmarks to file
	saveBookmarks();

	//Save settings
#ifndef OTHER_OS
	QSettings s;
	s.setValue("searchengine", ui->cbSearchEngine->currentIndex());

	s.setValue("windowpos", this->pos());
	s.setValue("windowsize", this->size());

	s.setValue("maximized", this->windowState() == Qt::WindowMaximized);
	s.setValue("lightweight2buttonvisible", ui->bLW->isVisible());
#else
	QFile settings(QDir::homePath()+"/.lightweight2/settings");
	settings.open(QFile::WriteOnly);
	settings.write(QString("searchengine=%1\n").arg(ui->cbSearchEngine->currentIndex()).toUtf8());
	settings.write(QString("windowpos=%1,%2\n").arg(x()).arg(y()).toUtf8());
	settings.write(QString("windowsize=%1,%2\n").arg(width()).arg(height()).toUtf8());
	settings.write(QString("maximized=%1\n").arg(windowState() == Qt::WindowMaximized).toUtf8());
	settings.write(QString("lightweight2buttonvisible=%1\n").arg(ui->bLW->isVisible()).toUtf8());

	settings.flush();
	settings.close();
#endif

	for (int i = 0; i < tabList->size(); i++) {
		tabList->value(i)->stop();
	}

	CookieJar::jar->saveCookies();
	PluginFactory::factory->saveAllowedHosts();

	//Goodbye
	QWidget::closeEvent(e);
}

BrowserTab* Lightweight::newTab(QString url, QIcon icon, bool hideUrlBar) {
	//Check if there is already an tab with that url or an empty page
	for (int i = 0; i < tabList->size(); i++) {
		if (tabList->value(i)->currentUrl() == url) {
			showTab(tabList->value(i));
			return tabList->value(i);
		}
		if (tabList->value(i)->isEmptyPage()) {
			tabList->value(i)->goTo(url, icon, hideUrlBar);
			showTab(tabList->value(i));
			return tabList->value(i);
		}
	}

	//Tab not found, so create a new one
	BrowserTab* tab = new BrowserTab(bookmarks->gotoAction(), this);
	tabList->append(tab);
	ui->tabcontainer->layout()->addWidget(tab->tabButton);
	tabs->addWidget(tab->content);
	tabs->setCurrentWidget(tab->content);

	if (!url.isNull())
		tab->goTo(url, icon, hideUrlBar);

	//Signals from the new created tab
	connect(tab, SIGNAL(selected(BrowserTab*)), this, SLOT(showTab(BrowserTab*)));
	connect(tab, SIGNAL(closed(BrowserTab*)), this, SLOT(closeTab(BrowserTab*)));
	connect(tab, SIGNAL(newTab(QString)), this, SLOT(newTab(QString)));
	connect(tab, SIGNAL(newWindow(QWebView**)), this, SLOT(newWindow(QWebView**)));
	connect(tab, SIGNAL(downloadStarted(QUrl, QString)), downloads, SLOT(addDownload(QUrl, QString)));
	connect(tab, SIGNAL(loadingStateChanged(bool)), this, SLOT(setButtonsForCurrentTab()));
	connect(tab, SIGNAL(bookmark(QString,QString)), bmm, SLOT(addBookmark(QString,QString)));

	showTab(tab);

	//Set focus on the urlbar, if we created an empty page
	if (tab->isEmptyPage())
		tab->gainFocus();

	return tab;
}

void Lightweight::newWindow(QWebView **view) {
	//Creates a new Tab and sets it to the given argument
	BrowserTab* t = newTab("about:blank", QIcon(), true);

	(*view) = t->getView();
}

//Control functions
void Lightweight::back() {
	if (currentTab == 0) return;
	currentTab->back();
}

void Lightweight::forward() {
	if (currentTab == 0) return;
	currentTab->forward();
}

void Lightweight::stop() {
	if (currentTab == 0) return;
	currentTab->stop();
}

void Lightweight::reload() {
	if (currentTab == 0) return;
	currentTab->reload();
}

void Lightweight::search() {
	//Create query
	QString query = ui->cbSearchEngine->itemData(ui->cbSearchEngine->currentIndex()).toString();
	query = query.replace("%{query}", ui->tfSearch->text());

	//Go to the query
	if ((currentTab != 0) && currentTab->isEmptyPage()) currentTab->goTo(query, ui->cbSearchEngine->itemIcon(ui->cbSearchEngine->currentIndex()));
	else newTab(query, ui->cbSearchEngine->itemIcon(ui->cbSearchEngine->currentIndex()), true);
}

void Lightweight::showTab(BrowserTab *tab) {
	if (tab == 0) return;
	if (currentTab) {
		currentTab->setActive(false);
		currentTab->tabButton->setAutoRaise(true);
	}
	tabs->setCurrentWidget(tab->content);
	currentTab = tab;

	currentTab->setActive(true);
	currentTab->tabButton->setAutoRaise(false);

	setButtonsForCurrentTab();
	currentTab->gainFocus();
	pushTab(tab);
}

void Lightweight::setButtonsForCurrentTab() {
	//Enabled/disbales the controlbuttons for the current tab
	if (currentTab == 0) {
		ui->bBack->setEnabled(false);
		ui->bForward->setEnabled(false);
		ui->bReload->hide();
		ui->bStop->show();
		ui->bStop->setEnabled(false);
	} else {
		ui->bStop->setEnabled(true);
		ui->bBack->setEnabled(currentTab->canGoBack());
		ui->bForward->setEnabled(currentTab->canGoForward());
		ui->bStop->setVisible(currentTab->isLoading());
		ui->bReload->setVisible(!currentTab->isLoading());
	}
}

void Lightweight::closeTab(BrowserTab *tab) {
	if (tabList->size() > 1) {
		tabs->removeWidget(tab->content);
		ui->tabcontainer->layout()->removeWidget(tab->tabButton);

		tabList->removeAll(tab);
		tabStack.removeAll(tab);

		if (!tabList->isEmpty() && (tab == currentTab)) {
			showTab(tabStack.last());
		}

		if (tabList->isEmpty()) {
			currentTab = 0;
		}

		delete tab;
	} else {
		currentTab->goTo("about:blank", QIcon(":/icons/new.png"));
		currentTab->clearHistory();
		setButtonsForCurrentTab();
	}
}

void Lightweight::openBookmark(QAction *ac) {
	QUrl url = ac->data().toUrl();
	QString urlStr = url.toString();

	if (urlStr.startsWith("all|")) {
		//Try to open all bookmarks in given action
		QString fName = urlStr.right(urlStr.size()-4);
		if (fName == "") return;

		BookmarkFolder* bf = bookmarks->getFolder(fName);
		if (bf == 0) return;

		openAll(bf);
		return;
	}

	QIcon icon = Bookmark::iconFor(urlStr);
	if (urlStr == "about:blank") icon = QIcon(":/icons/new.png");
	bool hideUrlBar = (urlStr != "about:blank");

	newTab(urlStr, icon, hideUrlBar);
}

void Lightweight::switchFullscreen() {
	if (fullscreen) {
		this->setWindowState(Qt::WindowMaximized);
		fullscreen = false;
	} else {
		this->setWindowState(Qt::WindowFullScreen);
		fullscreen = true;
	}
}

void Lightweight::nextTab() {
	if (currentTab == 0) return;
	int i = (tabList->indexOf(currentTab)+1)%tabList->size();
	showTab(tabList->value(i));
}

void Lightweight::prevTab() {
	if (currentTab == 0) return;
	int i = (tabList->indexOf(currentTab)-1);
	if (i < 0) i += tabList->size();
	showTab(tabList->value(i));
}

void Lightweight::closeCurrentTab() {
	if (currentTab != 0) closeTab(currentTab);
}

void Lightweight::showDownloads() {
	tabs->setCurrentWidget(downloads);

	//Set current tab inactive and unset currentTab
	if (currentTab) {
		currentTab->setActive(false);
		currentTab->tabButton->setAutoRaise(true);
	}
	currentTab = 0;

	setButtonsForCurrentTab();
}

void Lightweight::setDownloadIcon(bool running) {
	if (running)
		ui->bDownloads->setIcon(QIcon(":/icons/download_run.png"));
	else
		ui->bDownloads->setIcon(QIcon(":/icons/download.png"));
}

void Lightweight::showSearchBar() {
	if (currentTab == 0) return;
	currentTab->showSearchBar();
}

void Lightweight::showUrlBar() {
	if (currentTab == 0) return;
	currentTab->showUrlBar();
	currentTab->gainFocus();
}

void Lightweight::openBMM() {
	bmm->manageBookmarks(currentTab && !currentTab->isEmptyPage());
}


void Lightweight::openPlugMan() {
	plugMan->showManager();
}

void Lightweight::openCookieMan() {
	cookieMan->showManager();
}

void Lightweight::switchLWButtonVisibility() {
	ui->bLW->setVisible(!ui->bLW->isVisible());
}

void Lightweight::createBookmark(BookmarkFolder *folder) {
	if (currentTab == 0) return;

	Bookmark* b = new Bookmark();
	b->setTitle(currentTab->currentTitle());
	b->setUrl(QUrl(currentTab->currentUrl()));
	b->setIcon(QWebSettings::iconForUrl(b->url()));

	folder->addBookmark(b);
}

void Lightweight::setDLButtonVisible(bool vis) {
	ui->bDownloads->setVisible(vis);
	if (!vis) {
		showTab(tabStack.last());
	}
}

void Lightweight::setPrivateBrowsing(bool privateBrowsing) {
	QWebSettings::globalSettings()->setAttribute(QWebSettings::PrivateBrowsingEnabled, privateBrowsing);
}

void Lightweight::openAll(BookmarkFolder *folder) {
	//First all bookmarks
	QList<Bookmark*> bms = folder->bookmarks();
	for (int i = 0; i < bms.size(); i++) {
		openBookmark(bms.value(i)->action());
	}

	//Now recursive all subfolders
	QList<BookmarkFolder*> bmfs = folder->subfolders();
	for (int i = 0; i < bmfs.size(); i++) {
		openAll(bmfs.value(i));
	}
}

void Lightweight::saveBookmarks() {
	QDomDocument d("Bookmarks");
	QFile file(QDir::homePath()+"/.lightweight2/bookmarks.xml");
	QTextStream stream(&file);
	file.open(QFile::WriteOnly);
	d.appendChild(bookmarks->toXml(&d));
	d.save(stream, 4);
}

void Lightweight::pushTab(BrowserTab *tab) {
	tabStack.removeAll(tab);
	tabStack.append(tab);
}
