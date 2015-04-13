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

#include "bookmarkbutton.h"
#include "ui_bookmarkitembutton.h"

#include <QDebug>
#include <QWebSettings>

BookmarkButton::BookmarkButton(Bookmark* m, bool managed, QWidget *parent) : BookmarkItemButton(m, managed, parent) {
	text_startEditUrl = tr("Edit URL");
	text_abortEditUrl = tr("Abort editing URL");

	m_titleLabel = new QLabel(this);
	m_titleLabel->setText(m->title());

	m_tfEditUrl = new QLineEdit(this);
	m_tfEditUrl->setText(m->url().toString());
	m_tfEditUrl->hide();

	m_bEditUrl = new QToolButton(this);
	m_bEditUrl->setIcon(QIcon(":/icons/url.png"));
	m_bEditUrl->setCheckable(true);
	m_bEditUrl->setAutoRaise(true);
	m_bEditUrl->setVisible(managed);
	m_bEditUrl->setToolTip(text_startEditUrl);

	iconLabel()->setPixmap(m->icon().pixmap(16,16));
	mainLayout()->insertWidget(1, m_titleLabel);
	mainLayout()->insertWidget(2, m_tfEditUrl);
	mainLayout()->insertWidget(4, m_bEditUrl);

	connect(ui->bRename, SIGNAL(toggled(bool)), this, SLOT(showEditButton(bool)));
	connect(m_bEditUrl, SIGNAL(clicked(bool)), this, SLOT(toggleEditUrl(bool)));
	connect(m_tfEditUrl, SIGNAL(returnPressed()), this, SLOT(finishEditUrl()));
}

BookmarkButton::~BookmarkButton() {
}

void BookmarkButton::deleteMe() {
	emit(toBeDeleted(this));
}

void BookmarkButton::cutMe() {
	emit(toBeCut((Bookmark*) item()));
}

void BookmarkButton::showEditButton(bool hideIt) {
	m_bEditUrl->setVisible(!hideIt);
}

void BookmarkButton::toggleEditUrl(bool editing) {
	m_tfEditUrl->setVisible(editing);
	ui->bRename->setVisible(!editing);
	itemWidget()->setVisible(!editing);
	m_tfEditUrl->setText(((Bookmark*) item())->url().toString());

	if (editing) {
		m_tfEditUrl->setFocus(Qt::OtherFocusReason);
		m_bEditUrl->setToolTip(text_abortEditUrl);
	} else {
		m_bEditUrl->setToolTip(text_startEditUrl);
	}
}

void BookmarkButton::finishEditUrl() {
	m_tfEditUrl->setVisible(false);
	ui->bRename->setVisible(true);
	itemWidget()->setVisible(true);
	m_bEditUrl->setChecked(false);
	m_bEditUrl->setToolTip(text_startEditUrl);

	Bookmark* m = (Bookmark*) item();
	m->setUrl(QUrl(m_tfEditUrl->text()));
	QIcon newIcon = QWebSettings::iconForUrl(m->url());
	if (!newIcon.isNull()) m->setIcon(newIcon);

	iconLabel()->setPixmap(m->icon().pixmap(16,16));
}
