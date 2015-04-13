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

#ifndef BOOKMARKMANAGER_H
#define BOOKMARKMANAGER_H

#include <QDialog>
#include <QList>
#include <QFrame>
#include <QLineEdit>
#include "src/bookmarkfolder.h"

#include "bookmarkbutton.h"
#include "bookmarkfolderbutton.h"
#include "bookmarkitembutton.h"

namespace Ui {
	class BookmarkManager;
}

class BookmarkManager : public QDialog {
	Q_OBJECT

	public:
		explicit BookmarkManager(BookmarkFolder* root, QWidget *parent = 0);
		~BookmarkManager();

	public slots:
		void addBookmark(QString title, QString url); //Called by tabbutton->Bookmark. Executes this in select folder mode
		                                              //If user has choosen a folder, a new bookmark will be created there
		                                              //If user aborts, nothing happens

		void manageBookmarks(bool showAddBookmark);  //Opens this in manage mode: The user can create folders, delete bookmarks and folders
		void accept();
		void reject();

	private slots:
		void goInto(BookmarkFolder* f);
		void deleteBookmark(BookmarkButton* bmb);
		void deleteBookmarkFolder(BookmarkFolderButton* bmfb);
		void deleteBookmarkItem(BookmarkItemButton* item);

		void goBack();
		void createFolder();
		void createBookmark();

		void setClipboard(Bookmark* m);
		void setClipboard(BookmarkFolder* f);
		void clearClipboard();

		void pasteClipboard();

	signals:
		void bookmarkRequest(BookmarkFolder* bmf);

	private:
		void showContent();
		void clearClipboardInt();

		Ui::BookmarkManager *ui;

		bool m_manageMode;
		bool m_showAddBookmark;

		QList<BookmarkFolder*> m_stack;
		QList<QWidget*> m_currentContents;
		QList<BookmarkButton*> m_bookmarkButtons;
		QList<BookmarkFolderButton*> m_bookmarkFolderButtons;

		BookmarkFolder* m_rootFolder;
		QString bmUrl;
		QString bmTitle;

		//Add Folder widget
		QFrame* m_frAddFolder;
		QLineEdit* m_tfFolderName;
		QPushButton* m_bAddFolder;

		QPushButton* m_bAddBookmark;
		QPushButton* m_bCreateBookmark;

		//Invariant:
		//B := (clipboardBookmark <=> !clipboardBookmarkFolder) || !(clipboardBookmark || clipboardBookmarkFolder)
		Bookmark* clipboardBookmark;
		BookmarkFolder* clipboardBookmarkFolder;
		BookmarkItemButton* clipboardButton;
};

#endif // BOOKMARKMANAGER_H
