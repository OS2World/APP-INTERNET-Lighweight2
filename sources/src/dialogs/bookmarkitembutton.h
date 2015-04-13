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

#ifndef BOOKMARKITEMBUTTON_H
#define BOOKMARKITEMBUTTON_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>

#include "src/bookmarkitem.h"

namespace Ui {
	class BookmarkItemButton;
}

class BookmarkItemButton : public QWidget {
	Q_OBJECT

	public:
		explicit BookmarkItemButton(BookmarkItem* item, bool managed, QWidget *parent = 0);
		~BookmarkItemButton();

		BookmarkItem* item() { return m_item; }

	protected slots:
		void toggleRename(bool renameing);
		virtual void finishRenameing();

	private slots:
		virtual void deleteMe() = 0;
		virtual void cutMe() = 0;

	protected:
		virtual QWidget* itemWidget() = 0;
		virtual void setItemWidgetText(QString text) = 0;
		QHBoxLayout* mainLayout();
		QLabel* iconLabel();
		QString tfRenameContent();
		BookmarkItem* m_item;

		Ui::BookmarkItemButton *ui;

	private:
		QString text_startRename;
		QString text_abortRename;
};

#endif // BOOKMARKITEMBUTTON_H
