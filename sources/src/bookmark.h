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

#ifndef BOOKMARK_H
#define BOOKMARK_H

#include <QString>
#include <QIcon>
#include <QUrl>
#include <QAction>
#include <QDomElement>

#include "bookmarkitem.h"

class Bookmark : public BookmarkItem {
	public:
		Bookmark();
		static Bookmark* read(QObject* parent, QDomElement bookmark);
		QDomElement toXml(QDomDocument* doc);

		void setUrl(QUrl url);
		QUrl url() { return m_url; }

		static QIcon iconFor(QString url);

	private:
		static QString hashOf(QString url);

		QUrl m_url;
};

#endif // BOOKMARK_H
