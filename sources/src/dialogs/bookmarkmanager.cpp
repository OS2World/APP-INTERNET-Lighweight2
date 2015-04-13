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

#include "bookmarkmanager.h"
#include "ui_bookmarkmanager.h"

#include <QWebSettings>
#include <QMessageBox>

BookmarkManager::BookmarkManager(BookmarkFolder* root, QWidget *parent) : QDialog(parent), ui(new Ui::BookmarkManager) {
	ui->setupUi(this);

	m_manageMode = true;
	m_rootFolder = root;

	clipboardBookmark = 0;
	clipboardBookmarkFolder = 0;
	clipboardButton = 0;

	m_stack.append(m_rootFolder);

	ui->bBack->setEnabled(false);

	//Create Add folder widget
	m_frAddFolder = new QFrame();
	m_tfFolderName = new QLineEdit();
	m_bAddFolder = new QPushButton(tr("Add folder"));

	m_frAddFolder->setLayout(new QHBoxLayout());
	m_frAddFolder->layout()->addWidget(m_tfFolderName);
	m_frAddFolder->layout()->addWidget(m_bAddFolder);

	//Create add booomark button
	m_bAddBookmark = new QPushButton(tr("Add bookmark here"));
	m_bAddBookmark->setStyleSheet("text-align:left");

	m_bCreateBookmark = new QPushButton(tr("Create bookmark from current page"));
	m_bCreateBookmark->setStyleSheet("text-align:left");
	m_bCreateBookmark->setAutoDefault(false);

	connect(ui->bClose, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ui->bBack, SIGNAL(clicked()), this, SLOT(goBack()));

	connect(m_bAddFolder, SIGNAL(clicked()), this, SLOT(createFolder()));
	connect(m_tfFolderName, SIGNAL(returnPressed()), this, SLOT(createFolder()));
	connect(m_bAddBookmark, SIGNAL(clicked()), this, SLOT(accept()));
	connect(m_bCreateBookmark, SIGNAL(clicked()), this, SLOT(createBookmark()));

	connect(ui->bUnCut, SIGNAL(clicked()), this, SLOT(clearClipboard()));
	connect(ui->bPaste, SIGNAL(clicked()), this, SLOT(pasteClipboard()));

	m_bAddFolder->setAutoDefault(false);
	m_bAddBookmark->setAutoDefault(false);
	m_bCreateBookmark->setAutoDefault(false);
	showContent();
}

BookmarkManager::~BookmarkManager() {
	//1. remove old content
	while (!m_currentContents.isEmpty()) {
		QWidget* w = m_currentContents.value(0);
		ui->items->removeWidget(w);
		m_currentContents.removeAll(w);
	}

	while (!m_bookmarkFolderButtons.isEmpty()) {
		BookmarkFolderButton* bfb = m_bookmarkFolderButtons.value(0);
		m_bookmarkFolderButtons.removeAll(bfb);
		delete bfb;
	}
	while (!m_bookmarkButtons.isEmpty()) {
		BookmarkButton* b = m_bookmarkButtons.value(0);
		m_bookmarkButtons.removeAll(b);
		delete b;
	}

	delete ui;
	delete m_frAddFolder;
	delete m_bAddBookmark;
}

void BookmarkManager::addBookmark(QString title, QString url) {
	bmUrl = url;
	bmTitle = title;
	m_manageMode = false;

	m_stack.clear();
	m_stack.append(m_rootFolder);

	showContent();

	exec();
}

void BookmarkManager::manageBookmarks(bool showAddBookmark) {
	m_stack.clear();
	m_stack.append(m_rootFolder);

	m_tfFolderName->clear();

	m_manageMode = true;
	m_showAddBookmark = showAddBookmark;

	showContent();

	exec();
}

void BookmarkManager::accept() {
	Bookmark* b = new Bookmark();
	b->setTitle(bmTitle);
	b->setUrl(QUrl(bmUrl));
	b->setIcon(QWebSettings::iconForUrl(b->url()));

	m_stack.last()->addBookmark(b);

	m_rootFolder->updateVisibility();
	QDialog::accept();
}

void BookmarkManager::reject() {
	m_rootFolder->updateVisibility();
	QDialog::reject();
}

void BookmarkManager::goInto(BookmarkFolder *f) {
	m_stack.append(f);
	showContent();
	ui->bBack->setEnabled(true);
}

void BookmarkManager::deleteBookmark(BookmarkButton *bmb) {
	m_stack.last()->removeBookmark((Bookmark*) bmb->item());
	m_bookmarkButtons.removeAll(bmb);
	deleteBookmarkItem(bmb);
}

void BookmarkManager::deleteBookmarkFolder(BookmarkFolderButton *bmfb) {
	m_stack.last()->removeFolder((BookmarkFolder*) bmfb->item());
	m_bookmarkFolderButtons.removeAll(bmfb);
	deleteBookmarkItem(bmfb);
}

void BookmarkManager::deleteBookmarkItem(BookmarkItemButton *item) {
	m_currentContents.removeAll(item);
	ui->items->removeWidget(item);
	delete item->item();
	item->deleteLater();
}

void BookmarkManager::goBack() {
	if (m_stack.size() < 2) return;
	m_stack.removeLast();
	showContent();

	if (m_stack.size() < 2) ui->bBack->setEnabled(false);
}

void BookmarkManager::showContent() {
	//1. remove old content
	while (!m_currentContents.isEmpty()) {
		QWidget* w = m_currentContents.value(0);
		ui->items->removeWidget(w);
		m_currentContents.removeAll(w);
	}

	while (!m_bookmarkFolderButtons.isEmpty()) {
		BookmarkFolderButton* bfb = m_bookmarkFolderButtons.value(0);
		m_bookmarkFolderButtons.removeAll(bfb);
		delete bfb;
	}
	while (!m_bookmarkButtons.isEmpty()) {
		BookmarkButton* b = m_bookmarkButtons.value(0);
		m_bookmarkButtons.removeAll(b);
		delete b;
	}

	//Remove clipboard-button
	if (clipboardButton) {
		ui->clipboard->removeWidget(clipboardButton);
		delete clipboardButton;
		clipboardButton = 0;
	}
	ui->lClipboard->hide();
	ui->bUnCut->hide();
	ui->bPaste->hide();

	//Hide the real-name for the root folder
	if (m_stack.size() == 1) ui->lFolderName->setText(tr("Bookmarks"));
	else ui->lFolderName->setText(m_stack.last()->title());

	//2. create new contents
	BookmarkFolder* f = m_stack.last();
	QList<BookmarkFolder*> subfolders = f->subfolders();
	QList<Bookmark*> bookmarks = f->bookmarks();

	//First the folders
	for (int i = 0; i < subfolders.size(); i++) {
		BookmarkFolderButton* bfb = new BookmarkFolderButton(subfolders.value(i), m_manageMode);
		ui->items->addWidget(bfb);
		m_currentContents.append(bfb);
		m_bookmarkFolderButtons.append(bfb);
		if (bfb->item() == clipboardBookmarkFolder) bfb->setEnabled(false);
		connect(bfb, SIGNAL(jumpedInto(BookmarkFolder*)), this, SLOT(goInto(BookmarkFolder*)));
		connect(bfb, SIGNAL(toBeDeleted(BookmarkFolderButton*)), this, SLOT(deleteBookmarkFolder(BookmarkFolderButton*)));
		connect(bfb, SIGNAL(toBeCut(BookmarkFolder*)), this, SLOT(setClipboard(BookmarkFolder*)));
	}

	//Show option to add a folder
	ui->items->addWidget(m_frAddFolder);
	m_currentContents.append(m_frAddFolder);

	m_bAddBookmark->hide();
	m_bCreateBookmark->hide();
	if (!m_manageMode) {
		//Add a "Insert here" button here
		ui->items->addWidget(m_bAddBookmark);
		m_bAddBookmark->show();
		m_currentContents.append(m_bAddBookmark);
	} else if (m_showAddBookmark) {
		//Add "create bookmark" button here
		ui->items->addWidget(m_bCreateBookmark);
		m_bCreateBookmark->show();
		m_currentContents.append(m_bCreateBookmark);
	}

	//At last the bookmarks
	for (int i = 0; i < bookmarks.size(); i++) {
		BookmarkButton* b = new BookmarkButton(bookmarks.value(i), m_manageMode);
		ui->items->addWidget(b);
		m_currentContents.append(b);
		m_bookmarkButtons.append(b);
		if (b->item() == clipboardBookmark) b->setEnabled(false);
		connect(b, SIGNAL(toBeDeleted(BookmarkButton*)), this, SLOT(deleteBookmark(BookmarkButton*)));
		connect(b, SIGNAL(toBeCut(Bookmark*)), this, SLOT(setClipboard(Bookmark*)));
	}

	//Create clipboard-item
	if (clipboardBookmark) {
		clipboardButton = new BookmarkButton(clipboardBookmark, false, this);
	}
	if (clipboardBookmarkFolder) {
		clipboardButton = new BookmarkFolderButton(clipboardBookmarkFolder, false, this);
	}
	if (clipboardButton) {
		clipboardButton->setEnabled(false);
		ui->clipboard->insertWidget(1, clipboardButton);
		ui->lClipboard->show();
		ui->bUnCut->show();
		ui->bPaste->show();
	}

	m_bAddFolder->setFocus(Qt::OtherFocusReason);
}

void BookmarkManager::setClipboard(Bookmark *m) {
	clearClipboardInt();
	clipboardBookmark = m;
	showContent();
}

void BookmarkManager::setClipboard(BookmarkFolder *f) {
	clearClipboardInt();
	clipboardBookmarkFolder = f;
	showContent();
}

void BookmarkManager::clearClipboard() {
	clearClipboardInt();
	showContent();
}

void BookmarkManager::clearClipboardInt() {
	if (clipboardButton) {
		ui->clipboard->removeWidget(clipboardButton);
		ui->lClipboard->hide();
		ui->bUnCut->hide();
		ui->bPaste->hide();
		delete clipboardButton;
		clipboardButton = 0;
	}

	clipboardBookmark = 0;
	clipboardBookmarkFolder = 0;
}

void BookmarkManager::pasteClipboard() {
	if (clipboardBookmark) {
		m_stack.last()->addBookmark(clipboardBookmark);
	}
	if (clipboardBookmarkFolder) {
		m_stack.last()->addFolder(clipboardBookmarkFolder, true);
	}

	clearClipboard();
}

void BookmarkManager::createFolder() {
	QString name = m_tfFolderName->text();
	if (name == "") return;

	//Check if the name is already given
	if (m_stack.last()->hasSubfolder(name)) {
		QMessageBox::information(this, tr("Create folder"), tr("There is already a folder with that name"));
		return;
	}

	BookmarkFolder* bmf = new BookmarkFolder();
	bmf->setTitle(name);
	bmf->addAllAction();

	m_stack.last()->addFolder(bmf, true);

	m_tfFolderName->clear();

	showContent();
}

void BookmarkManager::createBookmark() {
	emit(bookmarkRequest(m_stack.last()));

	showContent();
}
