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

#ifndef BOOKMARKFOLDER_H
#define BOOKMARKFOLDER_H

#include <QList>
#include <QAction>
#include <QDomElement>

#include "bookmarkitem.h"
#include "bookmark.h"

class BookmarkFolder : public BookmarkItem {
	public:
		BookmarkFolder();
		~BookmarkFolder();

		//Read and write the folder
		static BookmarkFolder* read(QObject* parent, QDomElement folder, bool isRootFolder=false);
		QDomElement toXml(QDomDocument* doc);

		//Override from bookmarkItem
		virtual void setTitle(QString title);

		BookmarkFolder* getFolder(QString name);

		QAction* allAction() { return m_allAction; }
		QAction* emptyAction() { return m_emptyAction; }

		QList<BookmarkFolder*> subfolders() { return m_subfolders; }
		QList<Bookmark*> bookmarks() { return m_bookmarks; }

		int size();

		void addFolder(BookmarkFolder* folder, bool sort=false);
		void addBookmark(Bookmark* bookmark);
		void removeFolder(BookmarkFolder* folder);
		void removeBookmark(Bookmark* bookmark);

		void updateVisibility();
		void addAllAction();

		bool hasSubfolder(QString name);

	private:
		bool checkK(BookmarkFolder* folder); //Checks the parent()-chain of this to contain folder
		//returns false if parent()-chain contains folder, true otherwise
		//Or returns K(this, folder)

		void addSeparator();
		void addEmptyAction();
		void rearrangeEntries();

		//Must fulfill everytime:
		//folder => (J(folder): Forall i in (folder->subfolders() \/ folder->bookmarks() : i.parent() == folder)
		QList<BookmarkFolder*> m_subfolders;
		QList<Bookmark*> m_bookmarks;

		//We also need:
		//K(folder, chk) := (folder->parent() != 0) => [(folder->parent() != chk) /\ K(folder->parent(), chk) ]
		//K(folder, folder)
		QAction* m_allAction;
		QAction* m_emptyAction;
};

#endif // BOOKMARKFOLDER_H
