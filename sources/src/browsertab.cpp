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

#include "browsertab.h"

#include <QDebug>

#include <QFileInfo>
#include <QFileDialog>
#include <QWebFrame>
#include <QWebElement>
#include <QPrinter>
#include <QPrintDialog>
#include <QBuffer>

#include "dialogs/askdownload.h"

BrowserTab::BrowserTab(QAction* bookmarks, QObject *parent) : QObject(parent) {
	acEditUrlText1 = tr("Hide URL-bar");
	acEditUrlText2 = tr("Show URL-bar");

	acSourceText1 = tr("Show sources");
	acSourceText2 = tr("Hide sources");

	//Create tab-button menu
	menu = new QMenu();
	acShow = new QAction(tr("Show"), this);
	acBookmark = new QAction(tr("Create bookmark"), this);
	acBookmark->setVisible(false);
	acBookmark->setIcon(QIcon(":/icons/bookmark.png"));
	acEditUrl = new QAction(acEditUrlText1, this);
	acEditUrl->setIcon(QIcon(":/icons/url.png"));
	acSource = new QAction(acSourceText1, this);
	acPrint = new QAction(tr("Print page"), this);
	acPrint->setIcon(QIcon(":/icons/print.png"));
	acSearch = new QAction(tr("Search the page"), this);
	acSearch->setIcon(QIcon(":/icons/pagesearch.png"));
	acClose = new QAction(tr("Close"), this);
	acClose->setIcon(QIcon(":/icons/close.png"));

	menu->addAction(acShow);
	menu->addAction(acEditUrl);
	menu->addAction(acSource);
	menu->addSeparator();
	menu->addAction(acBookmark);
	menu->addAction(bookmarks);
	menu->addSeparator();
	menu->addAction(acPrint);
	menu->addAction(acSearch);
	menu->addSeparator();
	menu->addAction(acClose);

	//Create Tab-button
	tabButton = new QToolButton();
	tabButton->setMenu(menu);
	tabButton->setDefaultAction(acShow);
	tabButton->setText(tr("Empty page"));
	tabButton->setIcon(QIcon(":/icons/new.png"));
	tabButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

	//Create WebView layout
	content = new QWidget();
	urlBar = new QLineEdit(content);
	splitter = new QSplitter(Qt::Vertical, content);
	view = new WebView(content);
	inspector = new QWebInspector(content);
	inspector->setPage(view->page());
	searchBar = new SearchBar(content);
	searchBar->hide();

	layout = new QVBoxLayout(content);
	layout->setContentsMargins(0,0,0,0);
	content->setLayout(layout);

	layout->addWidget(urlBar);
	layout->addWidget(splitter);
	splitter->addWidget(view);
	splitter->addWidget(inspector);
	layout->addWidget(searchBar);

	inspector->setVisible(false);

	view->page()->setForwardUnsupportedContent(true);

	loading = false;
	isActive = false;
	ssl = false;
	sslOk = true;

	//connect the signals with the slots
	connect(view, SIGNAL(urlChanged(QUrl)), this, SLOT(showUrl(QUrl)));
	connect(view, SIGNAL(titleChanged(QString)), this, SLOT(setTooltip()));
	connect(view, SIGNAL(iconChanged()), this, SLOT(setIcon()));

	connect(view->page(), SIGNAL(linkHovered(QString,QString,QString)), this, SLOT(updateHoverUrl(QString)));
	connect(view->page()->networkAccessManager(), SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(setSSLError(QNetworkReply*)));
	connect(view->page(), SIGNAL(windowCloseRequested()), this, SLOT(closeTab()));
	connect(view->page(), SIGNAL(downloadRequested(QNetworkRequest)), this, SLOT(startDownload(QNetworkRequest)));
	connect(view->page(), SIGNAL(unsupportedContent(QNetworkReply*)), this, SLOT(startDownload(QNetworkReply*)));
	connect(view->page(), SIGNAL(contentsChanged()), this, SLOT(setNewContent()));
	connect(view->page(), SIGNAL(printRequested(QWebFrame*)), this, SLOT(printFrame(QWebFrame*)));

	connect(view->page()->action(QWebPage::InspectElement), SIGNAL(triggered()), this, SLOT(showSource()));

	connect(urlBar, SIGNAL(returnPressed()), this, SLOT(loadUrl()));

	//Signals from tab-button menu
	connect(acShow, SIGNAL(triggered()), this, SLOT(showTab()));
	connect(acBookmark, SIGNAL(triggered()), this, SLOT(newBookmark()));
	connect(acEditUrl, SIGNAL(triggered()), this, SLOT(showUrlBar()));
	connect(acSource, SIGNAL(triggered()), this, SLOT(toggleSource()));
	connect(acPrint, SIGNAL(triggered()), this, SLOT(printFrame()));
	connect(acSearch, SIGNAL(triggered()), this, SLOT(showSearchBar()));
	connect(acClose, SIGNAL(triggered()), this, SLOT(closeTab()));

	connect(view, SIGNAL(newWindow(QWebView**)), this, SIGNAL(newWindow(QWebView**)));

	connect(view, SIGNAL(loadStarted()), this, SLOT(startLoading()));
	connect(view, SIGNAL(loadFinished(bool)), this, SLOT(finishLoading()));

	connect(searchBar, SIGNAL(searchFor(QString,QWebPage::FindFlags,bool*)), this, SLOT(search(QString,QWebPage::FindFlags,bool*)));

	connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(openBoomark(QAction*)));
}

BrowserTab::~BrowserTab() {
	delete acShow;
	delete acEditUrl;
	delete acSearch;
	delete acBookmark;

	content->deleteLater();
	tabButton->deleteLater();
	menu->deleteLater();
	acClose->deleteLater();
}

bool BrowserTab::isEmptyPage() {
	return !loading && (view->url().toString() == "about:blank");
}

void BrowserTab::gainFocus() {
	if (urlBar->isVisibleTo(content))
		urlBar->setFocus(Qt::OtherFocusReason);
	else
		view->setFocus(Qt::OtherFocusReason);
}

void BrowserTab::setActive(bool active) {
	isActive = active;
	if (isActive) setTabButtonStyle();
}

void BrowserTab::back() {
	view->back();
	setSSL(view->url().scheme() == "https");
	setTabButtonStyle();
}

void BrowserTab::forward() {
	view->forward();
	setSSL(view->url().scheme() == "https");
	setTabButtonStyle();
}

void BrowserTab::stop() {
	view->stop();
}

void BrowserTab::reload() {
	view->reload();
}

void BrowserTab::showTab() {
	emit(selected(this));
}

void BrowserTab::goTo(QString url, QIcon icon, bool hideUrlBar) {
	bool clearHistory = !view->history()->canGoBack() && isEmptyPage();

	loading = true;
	view->load(QUrl(url));
	if (!icon.isNull()) tabButton->setIcon(icon);
	else tabButton->setIcon(QWebSettings::iconForUrl(QUrl(url)));

	makeUrlBarVisible((url == "about:blank") && !hideUrlBar);

	acBookmark->setVisible(url != "about:blank");

	if (clearHistory) view->history()->clear();
}

void BrowserTab::showSearchBar() {
	searchBar->show();
	searchBar->gainFocus();
}

void BrowserTab::setTooltip() {
	QString title = view->title();
	if (title == "") title = tr("Empty page");

	//Code based on the demo code on
	// http://www.archivum.info/qt-interest@trolltech.com/2010-03/01101/Re-(Qt-interest)-QWebView-howto-embed-a-QPixmap-into-HTML-code-(Solved).html
	QPixmap pixmap = QPixmap::grabWidget(view);
	QByteArray byteArray;
	QBuffer buffer(&byteArray); // use buffer to store pixmap into byteArray
	buffer.open(QIODevice::WriteOnly);
	pixmap.save(&buffer, "PNG");
	QString html = "<img src=\"data:image/png;base64," + byteArray.toBase64() + "\"  width=\"%1\"; height=\"%2\";\">";
	html = html.arg(view->width()/5);
	html = html.arg(view->height()/5);
	tabButton->setToolTip(title+"<br>"+html);

	//Short title to 103 shown chars
	if (title.size() > 103)
		title = title.left(50)+"..."+title.right(50);
	tabButton->setText(title);
}

void BrowserTab::setIcon() {
	tabButton->setIcon(view->icon());
}

void BrowserTab::newBookmark() {
	emit(bookmark(view->title(), view->url().toString()));
}

void BrowserTab::startDownload(const QNetworkRequest &reply) {
	QString path = QFileDialog::getExistingDirectory(content, tr("Choose directory"), QDir::homePath()+"/Downloads");
	qDebug() << "DL Request: " << path;
	if (path != "")
		emit(downloadStarted(reply.url(), path));
}

void BrowserTab::startDownload(QNetworkReply* reply) {
	QUrl url = reply->url();

	//Only download content with valid content-header
	if (!reply->header(QNetworkRequest::ContentTypeHeader).isValid()) return;

	//QString fileName = QFileInfo(url.path()).fileName();
	//if (fileName == "safari.jsp") return;
	//if (fileName == "st") return;

	//First stop downloading, we create a new download later.
	if (reply->isOpen())
		reply->close();

	QString path = AskDownload::instance->askForDownload(url);
	qDebug() << "DL Reply: " << path;

	if (path != "") {
		qDebug() << "Start download of " << url;
		emit(downloadStarted(url, path));
	}

	//reply->deleteLater();
}

void BrowserTab::makeUrlBarVisible(bool visible) {
	urlBar->setVisible(visible);
	acEditUrl->setText(urlBar->isVisibleTo(content)?acEditUrlText1:acEditUrlText2);
}

void BrowserTab::openBoomark(QAction *action) {
	QUrl url = action->data().toUrl();
	if (url.isEmpty()) return;
	if (!url.isValid()) return;

	goTo(url.toString(), action->icon(), true);
}

void BrowserTab::closeTab() {
	emit(closed(this));
}

void BrowserTab::setSSL(bool sslUsed) {
	ssl = sslUsed;
	setTabButtonStyle();
}

void BrowserTab::setTabButtonStyle() {
	QString style;
	QPalette p;
	if (ssl || !sslOk) {
		style += "color:black;";
		if (!sslOk){
			p.setColor(QPalette::Button, QColor(255,160,160));
			style += "background:rgb(255,160,160);";
		} else {
			p.setColor(QPalette::Button, QColor(128,255,128));
			style += "background:rgb(160,255,160);";
		}
	}

	if (!isActive) {
		style += "font-weight:bold;";
	} else {
		style += "font-weight:normal;";
	}

	tabButton->setStyleSheet(style);
	tabButton->setPalette(p);
}

void BrowserTab::setSSLError(QNetworkReply* reply) {
	QUrl url = reply->url();
	QString urlStr = url.toString();
	qDebug() << "SSL Error from: " << url;

	//Check any frames of this page
	bool ourError = false;

	//Do a layer-search of the frames-tree
	QList<QWebFrame*> frames;
	frames.append(view->page()->mainFrame());
	while (!frames.isEmpty()) {
		QWebFrame* f = frames.first();
		frames.removeFirst();
		//Check its url
		if (f->url() == url) {
			ourError = true;
			break;
		}

		//Check if an elemen of the frame references to the given url
		//Use a layer search of document-tree
		QList<QWebElement> elements;
		elements.append(f->documentElement());
		while (!elements.isEmpty()) {
			QWebElement e = elements.first();
			elements.removeFirst();

			if (e.attribute("src" "") == urlStr) {
				ourError = true;
				break;
			}

			QWebElement child = e.firstChild();
			while (!child.isNull()) {
				elements.append(child);
				child = child.nextSibling();
			}
		}
		frames.append(f->childFrames());
	}

	if (ourError) {
		sslOk = false;
		setTabButtonStyle();
	}
}

void BrowserTab::showUrlBar() {
	makeUrlBarVisible(!urlBar->isVisible());
}

void BrowserTab::search(QString text, QWebPage::FindFlags flags, bool *found) {
	(*found) = view->page()->findText(text, flags);
}

void BrowserTab::setNewContent() {
	setTabButtonStyle();
	//setTooltip(); //To slow when typing
}

void BrowserTab::printFrame(QWebFrame* frame) {
	if (frame == 0) frame = view->page()->mainFrame();

	QPrinter p;
	QPrintDialog pDialog(&p, view);
	pDialog.exec();
	if (pDialog.result() != QDialog::Accepted) return;
	frame->print(&p);
}

void BrowserTab::toggleSource() {
	inspector->setVisible(!inspector->isVisibleTo(content));
	if (inspector->isVisibleTo(content)) {
		acSource->setText(acSourceText2);
	} else {
		acSource->setText(acSourceText1);
	}
}

void BrowserTab::showSource() {
	inspector->show();
	acSource->setText(acSourceText2);
}

void BrowserTab::showUrl(QUrl url) {
	QString urlStr = url.toString();
	if (!urlStr.startsWith("http")) return;
	setSSL(urlStr.startsWith("https"));
	if (urlStr == "about:blank") urlStr = "";
	urlBar->setText(urlStr);
}

void BrowserTab::loadUrl() {
	QString url = urlBar->text();

	if (!url.startsWith("http://") && (url != "about:blank")) url = "http://"+url;
	setSSL(url.startsWith("https"));

	QIcon icon = QWebSettings::iconForUrl(QUrl(url));
	if (url == "about:blank") icon = QIcon(":/icons/new.png");
	goTo(url, icon, (url != "about:blank"));
}

void BrowserTab::openNewTab() {
	emit(newTab(curHoverUrl));
}

void BrowserTab::updateHoverUrl(QString url) {
	if (url.isNull()) {
		return;
	}
	curHoverUrl = url;
}
