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

#ifndef BOOKMARKBUTTON_H
#define BOOKMARKBUTTON_H

#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include "bookmarkitembutton.h"
#include "src/bookmark.h"

class BookmarkButton : public BookmarkItemButton {
	Q_OBJECT

	public:
		explicit BookmarkButton(Bookmark* m, bool managed, QWidget *parent = 0);
		~BookmarkButton();

	private slots:
		void deleteMe();
		void cutMe();
		void showEditButton(bool hideIt);
		void toggleEditUrl(bool editing);
		void finishEditUrl();

	signals:
		void toBeDeleted(BookmarkButton* bb);
		void toBeCut(Bookmark* bm);

	protected:
		QWidget* itemWidget() { return m_titleLabel; }
		void setItemWidgetText(QString text) { m_titleLabel->setText(text); }

	private:
		QLabel* m_titleLabel;
		QLineEdit* m_tfEditUrl;
		QToolButton* m_bEditUrl;

		QString text_startEditUrl;
		QString text_abortEditUrl;
};

#endif // BOOKMARKBUTTON_H
