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

#ifndef BROWSERTAB_H
#define BROWSERTAB_H

#include <QObject>
#include <QToolButton>
#include <QMenu>
#include <QVBoxLayout>
#include <QSplitter>
#include <QLineEdit>
#include <QLabel>
#include <QWebView>
#include <QWebHistory>
#include <QWebInspector>
#include <QProgressBar>

#include <QNetworkReply>

#include "webview.h"
#include "searchbar.h"

class BrowserTab : public QObject {
	Q_OBJECT

	public:
		explicit BrowserTab(QAction* bookmarks, QObject *parent = 0);
		~BrowserTab();
		bool isEmptyPage();
		WebView* getView() { return view; }
		QString currentUrl() { return view->url().toString(); }
		QString currentTitle() { return view->title(); }
		void gainFocus();

		//Used to enable/diable control buttons
		bool canGoBack()    { return view->history()->canGoBack(); }
		bool canGoForward() { return view->history()->canGoForward(); }
		bool isLoading()    { return loading; }

		void clearHistory() { view->page()->history()->clear(); }
		void setActive(bool active);

		QWidget* content;
		QToolButton* tabButton;

	signals:
		void selected(BrowserTab* tab);
		void closed(BrowserTab* tab);

		void newTab(QString url);
		void bookmark(QString title, QString url);
		void newWindow(QWebView** view);

		void downloadStarted(QUrl url, QString path);

		void loadingStateChanged(bool loading);

	public slots:
		void back();
		void forward();
		void stop();
		void reload();

		void showTab();

		void goTo(QString url, QIcon icon, bool hideUrlBar=true);

		void showSearchBar();

		void showUrlBar(); //Shows the urlBar

	private slots:
		//Signal handler
		void loadUrl(); //Load the url from urlBar
		void setTooltip(); //Sets the title
		void setIcon(); //Sets the icon
		void showUrl(QUrl url); //Sets the urlBar's test to the given url
		void updateHoverUrl(QString url); //Updates the currently hoverd url
		void setSSLError(QNetworkReply* reply); //Sets an SSLError

		void search(QString text, QWebPage::FindFlags flags, bool* found); //Used for searching

		void finishLoading() { loading = false; emit(loadingStateChanged(false)); setTabButtonStyle(); setTooltip(); }
		void startLoading()  { sslOk = true; loading = true; emit(loadingStateChanged(true));}

		void setNewContent();

		void printFrame(QWebFrame* frame=0);
		void toggleSource();
		void showSource();

		//signal emitter
		void closeTab();
		void openNewTab();
		void newBookmark();

		void startDownload(const QNetworkRequest& reply);
		void startDownload(QNetworkReply* reply);

		void makeUrlBarVisible(bool visible);

		void openBoomark(QAction* action);

	private:
		void setSSL(bool sslUsed);
		void setTabButtonStyle();

		//Webview items
		QVBoxLayout* layout;
		QLineEdit* urlBar;

		QVBoxLayout* viewLayout;
		QSplitter* splitter;
		WebView* view;
		QWebInspector* inspector;
		SearchBar* searchBar;

		//Menu items
		QMenu* menu;
		QAction* acShow;
		QAction* acEditUrl;
		QAction* acSource;
		QAction* acBookmark;
		QAction* acPrint;
		QAction* acSearch;
		QAction* acClose;

		//other stuff
		QString curHoverUrl;
		QString acEditUrlText1;
		QString acEditUrlText2;
		QString acSourceText1;
		QString acSourceText2;

		//SSL-Settings
		bool ssl;
		bool sslOk;

		bool loading;

		//Is this tab active?
		bool isActive;
};

#endif // BROWSERTAB_H
