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

#include "webview.h"

#include <QAction>
#include <QMenu>
#include <QDesktopServices>
#include <QDebug>
#include "pluginfactory.h"
#include "dialogs/passworddialog.h"

QNetworkAccessManager* WebView::manager = 0;

WebView::WebView(QWidget *parent) : QWebView(parent) {
	curHoverUrl = QString();

	hoverUrlLeft = true;

	m_progress = new QProgressBar(parent);
	m_progress->setFixedSize(240,16);
	m_progress->move(0,y()+height()-16);
	m_progress->setFormat("");
	m_progress->hide();

	m_hoverUrl = new QLabel(parent);
	m_hoverUrl->setStyleSheet("background:rgb(224,224,224); color:black; font-size:14px");
	m_hoverUrl->setFixedHeight(16);
	m_hoverUrl->move(0,y()+height()-m_hoverUrl->height());
	m_hoverUrl->hide();

	//Translate page-actions and give them icons from Lightweight2
	page()->action(QWebPage::Stop)->setIcon(QIcon(":/icons/stop.png"));
	page()->action(QWebPage::Back)->setText(tr("Go back"));
	page()->action(QWebPage::Back)->setIcon(QIcon(":/icons/back.png"));

	page()->action(QWebPage::Reload)->setText(tr("Reload"));
	page()->action(QWebPage::Reload)->setIcon(QIcon(":/icons/reload.png"));

	page()->action(QWebPage::OpenLink)->setText(tr("Open link"));
	page()->action(QWebPage::OpenLinkInNewWindow)->setText(tr("Open link in new tab"));
	page()->action(QWebPage::OpenLinkInNewWindow)->setIcon(QIcon(":/icons/new.png"));

	page()->action(QWebPage::DownloadLinkToDisk)->setText(tr("Download link"));
	page()->action(QWebPage::DownloadLinkToDisk)->setIcon(QIcon(":/icons/download_run.png"));
	page()->action(QWebPage::CopyLinkToClipboard)->setText(tr("Copy link"));

	page()->action(QWebPage::InspectElement)->setText("Show sources");

	page()->setNetworkAccessManager(manager);
	page()->setPluginFactory(PluginFactory::factory);

	connect(page(), SIGNAL(linkHovered(QString,QString,QString)), this, SLOT(updateCurHoverUrl(QString)));
	connect(page()->networkAccessManager(), SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)), this, SLOT(getAuthentication(QNetworkReply*, QAuthenticator*)));

	//Show loading progress
	connect(this, SIGNAL(loadStarted()), m_progress, SLOT(show()));
	connect(this, SIGNAL(loadProgress(int)), m_progress, SLOT(setValue(int)));
	connect(this, SIGNAL(loadFinished(bool)), m_progress, SLOT(hide()));
	connect(this, SIGNAL(loadFinished(bool)), this, SLOT(update()));
}

QWebView* WebView::createWindow(QWebPage::WebWindowType type) {
	Q_UNUSED(type);

	QWebView* view;
	emit(newWindow(&view));
	return view;
}

void WebView::mousePressEvent(QMouseEvent *e) {
	if (e->button() == Qt::MidButton) {
		if (curHoverUrl.isNull()) return;
		QWebView* view = createWindow(QWebPage::WebBrowserWindow);
		view->load(QUrl(curHoverUrl));
	}
	else {
		if ((e->button() == Qt::LeftButton) && curHoverUrl.startsWith("mailto:"))
			QDesktopServices::openUrl(QUrl(curHoverUrl));
		else QWebView::mousePressEvent(e);
	}
}

void WebView::mouseMoveEvent(QMouseEvent* e) {
	if ((e->x() < width()/2) && (e->y() > height()-20)) {
		hoverUrlLeft = false;
	} else {
		hoverUrlLeft = true;
	}
	QWebView::mouseMoveEvent(e);
}

void WebView::resizeEvent(QResizeEvent* e) {
	m_progress->move(0, y()+height()-16);
	if (hoverUrlLeft) {
		m_hoverUrl->move(0, y()+height()-m_hoverUrl->height());
	} else {
		m_hoverUrl->move(width()-m_hoverUrl->width(), y()+height()-m_hoverUrl->height());
	}

	QWebView::resizeEvent(e);
}

void WebView::wheelEvent(QWheelEvent* e) {
	m_hoverUrl->hide();
	QWebView::wheelEvent(e);
}

void WebView::updateCurHoverUrl(QString url) {
	curHoverUrl = url;
	if (curHoverUrl != "") {
		//Trim hoverd url to 103 chars
		QString urlText = curHoverUrl;
		if (urlText.size() > 103) urlText = urlText.left(50)+"..."+urlText.right(50);
		m_hoverUrl->setText(urlText);
		m_hoverUrl->adjustSize();

		if (hoverUrlLeft) {
			m_hoverUrl->move(0, y()+height()-m_hoverUrl->height());
		} else {
			m_hoverUrl->move(width()-m_hoverUrl->width(), y()+height()-m_hoverUrl->height());
		}

		m_hoverUrl->show();
	} else {
		m_hoverUrl->hide();
		update();
	}
}

void WebView::getAuthentication(QNetworkReply* repl, QAuthenticator *auth) {
	Q_UNUSED(repl);

	PasswordDialog d(auth, 0);
	d.exec();
}
