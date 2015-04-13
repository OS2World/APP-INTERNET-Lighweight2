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

#ifndef BOOKMARKITEM_H
#define BOOKMARKITEM_H

#include <QString>
#include <QIcon>
#include <QAction>
#include <QDomElement>

class BookmarkFolder;

class BookmarkItem {
	friend class Bookmark;
	friend class BookmarkFolder;

	public:
		BookmarkItem();
		~BookmarkItem();

		//Setter
		virtual void setTitle(QString title);
		void setIcon(QIcon icon);

		//Getter
		QString title()          { return m_title; }
		QIcon icon()             { return m_icon; }
		BookmarkFolder* parent() { return m_parent; }
		QAction* action()        { return m_menuAction; }
		QAction* gotoAction()    { return m_gotoAction; }

	protected:
		void setParent(BookmarkFolder* parent) { m_parent = parent; } //Breaks the Invariants. So only call privately
		QString m_title;
		QIcon m_icon;
		//We need the invariant:
		//I(item): item->parent() => parent->subfolders().contains(item)
		BookmarkFolder* m_parent;
		QAction* m_menuAction;
		QAction* m_gotoAction;
};

#endif // BOOKMARKITEM_H
