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

#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWebView>
#include <QMouseEvent>
#include <QAuthenticator>
#include <QProgressBar>
#include <QWheelEvent>
#include <QLabel>

class WebView : public QWebView {
	Q_OBJECT

	public:
		explicit WebView(QWidget *parent = 0);
		static QNetworkAccessManager* manager;

	protected:
		QWebView* createWindow(QWebPage::WebWindowType type);
		void mousePressEvent(QMouseEvent* e);
		void mouseMoveEvent(QMouseEvent* e);
		void resizeEvent(QResizeEvent* e);
		void wheelEvent(QWheelEvent* e);

	signals:
		void newWindow(QWebView** view);

	private slots:
		void updateCurHoverUrl(QString url);
		void getAuthentication(QNetworkReply* repl, QAuthenticator* auth);

	private:
		QString curHoverUrl;
		QProgressBar* m_progress;
		QLabel* m_hoverUrl;
		bool hoverUrlLeft;
};

#endif // WEBVIEW_H
