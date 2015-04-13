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

#ifndef SEARCHBAR_H
#define SEARCHBAR_H

#include <QWidget>
#include <QWebPage>

namespace Ui {
	class SearchBar;
}

class SearchBar : public QWidget {
	Q_OBJECT

	public:
		explicit SearchBar(QWidget *parent = 0);
		~SearchBar();

		void gainFocus();

	signals:
		void searchFor(QString text, QWebPage::FindFlags flags, bool* found);

	private slots:
		void searchLast();
		void searchNext();
		void reset();

	private:
		Ui::SearchBar *ui;
};

#endif // SEARCHBAR_H
