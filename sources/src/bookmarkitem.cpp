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

#include "bookmarkitem.h"

BookmarkItem::BookmarkItem() {
	m_menuAction = new QAction(0);
	m_gotoAction = new QAction(0);

	m_parent = 0;
}

BookmarkItem::~BookmarkItem() {
	delete m_menuAction;
	delete m_gotoAction;
}

void BookmarkItem::setIcon(QIcon icon) {
	m_icon = icon;
	m_menuAction->setIcon(icon);
	m_gotoAction->setIcon(icon);
}

void BookmarkItem::setTitle(QString title) {
	m_title = title;
	m_menuAction->setText(title);
	m_gotoAction->setText(title);
}
