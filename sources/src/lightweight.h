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

#ifndef LIGHTWEIGHT_H
#define LIGHTWEIGHT_H

#include <QWidget>
#include <QStackedLayout>
#include <QList>
#include <QMenu>

#include "bookmarkfolder.h"
#include "browsertab.h"
#include "downloadlist.h"
#include "dialogs/bookmarkmanager.h"
#include "dialogs/pluginmanager.h"
#include "dialogs/cookiemanager.h"

namespace Ui {
	class Lightweight;
}

class Lightweight : public QWidget {
	Q_OBJECT

	public:
		explicit Lightweight(QWidget *parent = 0);
		~Lightweight();
		void openUrl(QString url);

	protected:
		void closeEvent(QCloseEvent* e);

	private slots:
		BrowserTab* newTab(QString url="about:blank", QIcon icon = QIcon(), bool hideUrlBar=false);
		void newWindow(QWebView** view);

		void back();
		void forward();
		void stop();
		void reload();

		void search();

		void showTab(BrowserTab* tab);
		void setButtonsForCurrentTab();
		void closeTab(BrowserTab* tab);
		void openBookmark(QAction* ac);

		void switchFullscreen();
		void nextTab();
		void prevTab();
		void closeCurrentTab();

		void showDownloads();

		void setDownloadIcon(bool running);

		void showSearchBar();
		void showUrlBar();
		void openBMM();

		void openPlugMan();
		void openCookieMan();

		void switchLWButtonVisibility();

		void createBookmark(BookmarkFolder* folder);

		void setDLButtonVisible(bool vis);

		void setPrivateBrowsing(bool privateBrowsing);

	private:
		void openAll(BookmarkFolder* folder);
		void saveBookmarks();
		void pushTab(BrowserTab* tab);

		Ui::Lightweight *ui;
		QStackedLayout* tabs;
		QList<BrowserTab*>* tabList;
		QList<BrowserTab*> tabStack;

		BookmarkFolder* bookmarks;
		BookmarkManager* bmm;

		PluginManager* plugMan;
		CookieManager* cookieMan;

		BrowserTab* currentTab;
		DownloadList* downloads;

		bool fullscreen;

		//Lightweight2 Button menu
		QMenu* lwMenu;
		QAction* acPrivate;
		QAction* acCookies;
		QAction* acBookmarks;
		QAction* acPlugins;
		QAction* acQuit;
};

#endif // LIGHTWEIGHT_H
