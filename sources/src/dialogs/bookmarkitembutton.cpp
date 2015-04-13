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

#include "bookmarkitembutton.h"
#include "ui_bookmarkitembutton.h"

BookmarkItemButton::BookmarkItemButton(BookmarkItem* item, bool managed, QWidget *parent) : QWidget(parent), ui(new Ui::BookmarkItemButton) {
	ui->setupUi(this);

	text_startRename = tr("Rename");
	text_abortRename = tr("Abort rename");

	m_item = item;

	if (!managed) {
		ui->frame->layout()->removeItem(ui->layoutButton);
		ui->bDelete->hide();
		ui->bCut->hide();
	} else {
		connect(ui->bDelete, SIGNAL(clicked()), this, SLOT(deleteMe()));
		connect(ui->bCut, SIGNAL(clicked()), this, SLOT(cutMe()));
	}

	ui->tfRename->setText(m_item->title());
	ui->tfRename->setVisible(false);

	ui->bRename->setVisible(managed);
	ui->bRename->setToolTip(text_startRename);

	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	connect(ui->tfRename, SIGNAL(returnPressed()), this, SLOT(finishRenameing()));
	connect(ui->bRename, SIGNAL(clicked(bool)), this, SLOT(toggleRename(bool)));

#if defined(Q_WS_WIN)
	ui->frame->setFrameShape(QFrame::Panel);
#endif
}

BookmarkItemButton::~BookmarkItemButton() {
	delete ui;
}

void BookmarkItemButton::toggleRename(bool renameing) {
	ui->tfRename->setVisible(renameing);
	itemWidget()->setVisible(!renameing);
	ui->bRename->setChecked(renameing);

	if (!renameing) {
		ui->tfRename->setText(m_item->title());
		ui->bRename->setToolTip(text_startRename);
	} else {
		ui->tfRename->setFocus(Qt::OtherFocusReason);
		ui->bRename->setToolTip(text_abortRename);
	}
}

void BookmarkItemButton::finishRenameing() {
	m_item->setTitle(ui->tfRename->text());
	setItemWidgetText(m_item->title());
	ui->tfRename->setVisible(false);
	itemWidget()->setVisible(true);
	ui->bRename->setChecked(false);
	ui->bRename->setToolTip(text_startRename);
}

QHBoxLayout* BookmarkItemButton::mainLayout() {
	return (QHBoxLayout*) ui->frame->layout();
}

QLabel* BookmarkItemButton::iconLabel() {
	return ui->lIcon;
}

QString BookmarkItemButton::tfRenameContent() {
	return ui->tfRename->text();
}
