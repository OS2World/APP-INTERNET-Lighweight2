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

#include "bookmarkfolderbutton.h"

#include <QDebug>
#include <QMessageBox>

BookmarkFolderButton::BookmarkFolderButton(BookmarkFolder* bmf, bool managed, QWidget *parent) : BookmarkItemButton(bmf, managed, parent) {

	m_jumpButton = new QToolButton(this);
	m_jumpButton->setAutoRaise(true);
	m_jumpButton->setText(bmf->title());
	m_jumpButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	iconLabel()->setVisible(false);

	connect(m_jumpButton, SIGNAL(clicked()), this, SLOT(jumpIn()));

	mainLayout()->insertWidget(1, m_jumpButton);
}

BookmarkFolderButton::~BookmarkFolderButton() {
}

void BookmarkFolderButton::jumpIn() {
	emit(jumpedInto((BookmarkFolder*) item()));
}

void BookmarkFolderButton::finishRenameing() {
	if (((BookmarkFolder*) item())->hasSubfolder(tfRenameContent())) {
		QMessageBox::information(this, tr("Rename folder"), tr("There is already a folder with that name"));
		toggleRename(false);
		return;
	}

	BookmarkItemButton::finishRenameing();
}

void BookmarkFolderButton::deleteMe() {
	emit(toBeDeleted(this));
}

void BookmarkFolderButton::cutMe() {
	emit(toBeCut((BookmarkFolder*) item()));
}
