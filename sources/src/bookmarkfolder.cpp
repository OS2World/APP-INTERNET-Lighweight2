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

#include "bookmarkfolder.h"

#include <QApplication>
#include <QMenu>

BookmarkFolder::BookmarkFolder() : BookmarkItem() {
	m_menuAction->setMenu(new QMenu());
	m_gotoAction->setMenu(new QMenu());

	m_allAction = new QAction(0);
	m_menuAction->setVisible(false);
	m_gotoAction->setVisible(false);
	m_allAction->setVisible(false);
}

BookmarkFolder::~BookmarkFolder() {
	//Delete all subfolders
	while (!m_subfolders.isEmpty()) {
		BookmarkFolder* f = m_subfolders.value(0);
		m_subfolders.removeAll(f);
		delete f;
	}

	//Delete all bookmarks
	while (!m_bookmarks.isEmpty()) {
		Bookmark* b = m_bookmarks.value(0);
		m_bookmarks.removeAll(b);
		delete b;
	}

	QMenu* m = m_menuAction->menu();
	m_menuAction->setMenu(0);
	delete m;

	m = m_gotoAction->menu();
	m_gotoAction->setMenu(0);
	delete m;

	delete m_allAction;
	if (m_emptyAction) delete m_emptyAction;
}

BookmarkFolder* BookmarkFolder::read(QObject *parent, QDomElement folder, bool isRootFolder) {
	BookmarkFolder* res = new BookmarkFolder();

	if (isRootFolder) {
		res->addEmptyAction();
	}

	res->addAllAction();
	res->setTitle(folder.attribute("name", "Root"));

	if (isRootFolder) {
		res->gotoAction()->setText(QApplication::tr("Go to"));
	}

	QDomElement e = folder.firstChildElement();
	while (!e.isNull()) {
		if (e.nodeName() == "bookmarkfolder") res->addFolder(read(parent, e));
		if (e.nodeName() == "bookmark") res->addBookmark(Bookmark::read(parent, e));
		e = e.nextSiblingElement();
	}

	res->action()->setParent(parent);
	res->gotoAction()->setParent(parent);
	return res;
}

QDomElement BookmarkFolder::toXml(QDomDocument* doc) {
	QDomElement folder = doc->createElement("bookmarkfolder");
	folder.setAttribute("name", m_title);

	for (int i = 0; i < m_subfolders.size(); i++) {
		folder.appendChild(m_subfolders.value(i)->toXml(doc));
	}

	for (int i = 0; i < m_bookmarks.size(); i++) {
		folder.appendChild(m_bookmarks.value(i)->toXml(doc));
	}

	return folder;
}

void BookmarkFolder::setTitle(QString title) {
	BookmarkItem::setTitle(title);
	m_menuAction->setData(QVariant(title));
	m_gotoAction->setData(QVariant(title));
	m_allAction->setData(QVariant("all|"+m_title));
}

BookmarkFolder* BookmarkFolder::getFolder(QString name) {
	if (m_title == name) return this;

	for (int i = 0; i < m_subfolders.size(); i++) {
		BookmarkFolder* res = m_subfolders.value(i)->getFolder(name);
		if (res != 0) return res;
	}

	return 0;
}

void BookmarkFolder::addFolder(BookmarkFolder *folder, bool sort) {
	BookmarkFolder* p = folder->parent();
	//assume: I(folder) /\ J(p) /\ J(this) /\ K(this, this) /\ K(folder, folder)

	//Do only if K(this, folder) !!!
	if (!checkK(folder)) return;

	//Do only if this != folder
	if (folder == this) return;

	if (p) p->removeFolder(folder);
	//Now we have I(folder) /\ J(this) /\ J(p):
	//I(folder) /\ J(p) follows from p->removeFolder(folder)
	//this is untouched here

	m_subfolders.append(folder);
	if (sort) rearrangeEntries();
	else {
		m_menuAction->menu()->addAction(folder->action());
		m_gotoAction->menu()->addAction(folder->gotoAction());
	}
	folder->setParent(this);
	//Here we have J(P): p was untocuhed here
	//I(folder) follows from: this->subfolders().contains(folder) /\ (folder->parent() == this)
	//J(this) follows from: We added one element to subfolders and its parent is set to this

	updateVisibility();

	//K(this, this) follows from: We did not change the parent of this
	//K(folder, folder) follows from: We only made a change if K(this, folder) <=> parent()-chain of this does not contain folder and
	//                                this != folder

	//Here we have I(folder) /\ J(p) /\ J(this) /\ K(this, this) /\ K(folder, folder)
}

void BookmarkFolder::addBookmark(Bookmark *bookmark) {
	BookmarkFolder* p = bookmark->parent();
	//Assume: I(bookmark) /\ J(p) /\ J(this) /\ K(this, this)

	if (p) p->removeBookmark(bookmark);
	//Now we have: I(bookmark) /\ J(p) /\ J(this)
	//I(bookmark) /\ J(p) follows from p->removeBookmark(bookmark)
	//this is untocuhed here

	m_bookmarks.append(bookmark);
	m_menuAction->menu()->addAction(bookmark->action());
	m_gotoAction->menu()->addAction(bookmark->gotoAction());
	bookmark->setParent(this);
	//p was untoched here => J(p)
	//I(bookmark) follows from: this->bookmarks().contains(bookamrk) /\ (bookmark.parent() == this)
	//J(this) follows from: We added one element to bookmarks and its parent was set to this

	updateVisibility();
	//K(this, this) follows from: We did not change parent of this

	//Here we have I(folder) /\ J(p) /\ J(this) /\ K(this, this)
}

void BookmarkFolder::removeBookmark(Bookmark *bookmark) {
	//assume: I(bookmark) /\ J(this) /\ K(this, this)

	m_bookmarks.removeAll(bookmark);
	m_menuAction->menu()->removeAction(bookmark->action());
	m_gotoAction->menu()->removeAction(bookmark->gotoAction());
	//J(this) still fulfills: We reduced the bookmarks

	bookmark->setParent(0);
	//I(bookmark) fulfills again: (bookmark->parent() == 0)

	//K(this, this) follows from: we did not change parent of this

	updateVisibility();
	//Now we have I(bookmark) /\ J(this) /\ K(this, this)
}

void BookmarkFolder::removeFolder(BookmarkFolder *folder) {
	//assume: I(folder) /\ J(this) /\ K(this, this) /\ K(folder, folder)

	m_subfolders.removeAll(folder);
	m_menuAction->menu()->removeAction(folder->action());
	m_gotoAction->menu()->removeAction(folder->gotoAction());
	//J(this) still fulfills: We recuded the subfolders

	folder->setParent(0);
	//I(folder) fulfills now again: (folder->parent() == 0)

	//K(folder, folder) follows from: folder->parent() == 0
	//K(this, this) follows from: we did not change parent of this

	updateVisibility();
	//Now I(folder) /\ J(this) /\ K(this, this) /\ K(folder, folder)
}

void BookmarkFolder::addSeparator() {
	m_menuAction->menu()->addSeparator();
}

void BookmarkFolder::addEmptyAction() {
	m_emptyAction = m_menuAction->menu()->addAction(QIcon(":/icons/new.png"), QApplication::tr("Empty page"));
	m_emptyAction->setData("about:blank");
}

void BookmarkFolder::addAllAction() {
	m_allAction->setText(QApplication::tr("Open all"));
	m_menuAction->menu()->addAction(m_allAction);
	m_menuAction->menu()->addSeparator();
	m_allAction->setData(QVariant(QString("all|")+m_title));
}

bool BookmarkFolder::hasSubfolder(QString name) {
	foreach (BookmarkFolder* f, m_subfolders)
		if (f->title() == name) return true;

	return false;
}

bool BookmarkFolder::checkK(BookmarkFolder* folder) {
	if (parent() == 0) return true;
	if (parent() == folder) return false;

	return parent()->checkK(folder);
}

int BookmarkFolder::size() {
	int res = m_bookmarks.size();
	foreach (BookmarkFolder* f, m_subfolders) {
		res += f->size();
	}

	return res;
}

void BookmarkFolder::updateVisibility() {
	int s = size();
	m_gotoAction->setVisible(s > 0);
	m_menuAction->setVisible(s > 0);
	m_allAction->setVisible(size() > 1);

	foreach (BookmarkFolder* f, m_subfolders) {
		f->updateVisibility();
	}
}

void BookmarkFolder::rearrangeEntries() {
	//Sorts the entries: first (if there the emptypage), second "open all" thrist: the fodlers, and at last the bookmarks
	m_menuAction->menu()->clear();
	m_gotoAction->menu()->clear();

	if (m_emptyAction) m_menuAction->menu()->addAction(m_emptyAction);
	m_menuAction->menu()->addAction(m_allAction);

	m_menuAction->menu()->addSeparator();

	for (int i = 0; i < m_subfolders.size(); i++) {
		m_menuAction->menu()->addAction(m_subfolders.value(i)->action());
		m_gotoAction->menu()->addAction(m_subfolders.value(i)->gotoAction());
	}
	for (int i = 0; i < m_bookmarks.size(); i++) {
		m_menuAction->menu()->addAction(m_bookmarks.value(i)->action());
		m_gotoAction->menu()->addAction(m_bookmarks.value(i)->gotoAction());
	}
}
