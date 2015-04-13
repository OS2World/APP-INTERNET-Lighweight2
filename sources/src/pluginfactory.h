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

#ifndef PLUGINFACTORY_H
#define PLUGINFACTORY_H

#include <QWebPluginFactory>
#include <QUrl>

class PluginFactory : public QWebPluginFactory {
	Q_OBJECT

	public:
		explicit PluginFactory(QObject *parent = 0);

		QObject* create(const QString &mimeType, const QUrl &url, const QStringList &argumentNames, const QStringList &argumentValues) const;
		QList<Plugin> plugins() const;

		static PluginFactory* factory;

		void allowHost(QString host, bool permamently=true);
		void disallowHost(QString host, bool onlyPermamenty=true);

		QStringList knownHosts();

		bool isAllowed(QString host);
		bool isAllowedPermamently(QString host);

		void saveAllowedHosts();

		bool isAllowedForAll() { return allowAll; }
		void setAllowAll(bool allowAll) { this->allowAll = allowAll; }

	private:
		QStringList m_allowedHosts;       //Stores all hosts, that we save on the whitelist  Subset of m_allowedHostSession
		QStringList m_allowedHostSession; //Stores all hosts, that we allow plugins from     Subset of m_knownHosts
		QStringList m_knownHosts;         //Stores all hosts, that we know
		bool allowAll;
};

#endif // PLUGINFACTORY_H
