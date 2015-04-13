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

#include "bookmark.h"

#include <QDir>
#include <QCryptographicHash>
#include <QByteArray>

Bookmark::Bookmark() : BookmarkItem() {
}

Bookmark* Bookmark::read(QObject* parent, QDomElement bookmark) {
	Bookmark* res = new Bookmark();
	res->setUrl(QUrl("about:blank"));
	res->setTitle("Empty page");
	res->setIcon(QIcon(":/icons/new.png"));

	QDomElement e = bookmark.firstChildElement();
	while (!e.isNull()) {
		if (e.nodeName() == "url") {
			res->setUrl(QUrl(e.text()));
			res->setIcon(iconFor(e.text()));
		}
		if (e.nodeName() == "title") res->setTitle(e.text());
		e = e.nextSiblingElement();
	}

	res->action()->setParent(parent);
	res->gotoAction()->setParent(parent);
	return res;
}

QDomElement Bookmark::toXml(QDomDocument* doc) {
	QDomElement res       = doc->createElement("bookmark");

	QDomElement urlElem   = doc->createElement("url");
	QDomText urlText      = doc->createTextNode(m_url.toString());

	QDomElement titleElem = doc->createElement("title");
	QDomText titleText    = doc->createTextNode(m_title);

	urlElem.appendChild(urlText);
	titleElem.appendChild(titleText);

	res.appendChild(urlElem);
	res.appendChild(titleElem);

	//Save the used icon if it down not exist
	QFile file(QDir::homePath()+"/.lightweight2/bookmarkicons/"+hashOf(m_url.toString())+".png");
	if (!file.exists())
		m_icon.pixmap(16,16).save(file.fileName());

	return res;
}

void Bookmark::setUrl(QUrl url) {
	m_url = url;
	m_menuAction->setData(QVariant(url));
	m_gotoAction->setData(QVariant(url));
}

QIcon Bookmark::iconFor(QString url) {
	return QIcon(QDir::homePath()+"/.lightweight2/bookmarkicons/"+hashOf(url)+".png");
}

QString Bookmark::hashOf(QString url) {
	QByteArray arr = QCryptographicHash::hash(url.toUtf8(), QCryptographicHash::Md5);
	return QString::fromUtf8(arr.toHex());
}
