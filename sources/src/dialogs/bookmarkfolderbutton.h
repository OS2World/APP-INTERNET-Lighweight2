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

#ifndef BOOKMARKFOLDERBUTTON_H
#define BOOKMARKFOLDERBUTTON_H

#include <QToolButton>
#include "bookmarkitembutton.h"
#include "src/bookmarkfolder.h"

class BookmarkFolderButton : public BookmarkItemButton {
	Q_OBJECT

	public:
		explicit BookmarkFolderButton(BookmarkFolder* bf, bool managed, QWidget *parent = 0);
		~BookmarkFolderButton();

	private slots:
		void deleteMe();
		void cutMe();
		void jumpIn();
		void finishRenameing();

	signals:
		void toBeDeleted(BookmarkFolderButton* bmfb);
		void toBeCut(BookmarkFolder* bmf);
		void jumpedInto(BookmarkFolder* b);

	protected:
		QWidget* itemWidget() { return m_jumpButton; }
		void setItemWidgetText(QString text) { m_jumpButton->setText(text); }

	private:
		QToolButton* m_jumpButton;
};

#endif // BOOKMARKFOLDERBUTTON_H
