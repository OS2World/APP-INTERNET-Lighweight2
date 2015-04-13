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

#include "pluginfactory.h"

#include <QFile>
#include <QDir>
#include <QWidget>
#include <QSettings>
#include <QDebug>

#include "enableplugin.h"

PluginFactory* PluginFactory::factory = 0;

PluginFactory::PluginFactory(QObject *parent) : QWebPluginFactory(parent) {

	QFile hosts(QDir::homePath()+"/.lightweight2/whitelistPluginHosts");
	if (hosts.exists()) {
		hosts.open(QFile::ReadOnly);
		QString line;
		while (hosts.bytesAvailable()) {
			line = QString::fromUtf8(hosts.readLine());
			if (line.endsWith("\n")) line.chop(1);
			m_knownHosts.append(line);
		}
		hosts.close();
	}
	m_knownHosts.removeDuplicates();

	foreach (QString host, m_knownHosts)
		qDebug() << "Allowed plugins for: " << host;

	QSettings s;
	allowAll = s.value("allowpluginsforall", "true").toBool();

	m_allowedHostSession.append(m_knownHosts);
	m_allowedHosts.append(m_knownHosts);
}

void PluginFactory::allowHost(QString host, bool permamently) {
	if (!m_knownHosts.contains(host)) m_knownHosts.append(host);
	if (!m_allowedHostSession.contains(host)) m_allowedHostSession.append(host);

	if (permamently) if (!m_allowedHosts.contains(host)) m_allowedHosts.append(host);
}

void PluginFactory::disallowHost(QString host, bool onlyPermamently) {
	if (!m_knownHosts.contains(host)) m_knownHosts.append(host);
	if (!onlyPermamently) m_allowedHostSession.removeAll(host);
	m_allowedHosts.removeAll(host);
}

QObject* PluginFactory::create(const QString &mimeType, const QUrl &url, const QStringList &argumentNames, const QStringList &argumentValues) const {
	Q_UNUSED(mimeType);
	Q_UNUSED(argumentNames);
	Q_UNUSED(argumentValues);

	QString host = url.host();

	if (allowAll || m_allowedHostSession.contains(host))
		return 0;

	qDebug() << "Not allowed for plugin: " << host;

	return new EnablePlugin(url.host(), factory, 0);
}

QList<QWebPluginFactory::Plugin> PluginFactory::plugins() const {
	QList<QWebPluginFactory::Plugin> list;
	return list;
}

QStringList PluginFactory::knownHosts() {
	return m_knownHosts;
}

bool PluginFactory::isAllowed(QString host) {
	return m_allowedHostSession.contains(host);
}

bool PluginFactory::isAllowedPermamently(QString host) {
	return m_allowedHosts.contains(host);
}

void PluginFactory::saveAllowedHosts() {
	QFile hosts(QDir::homePath()+"/.lightweight2/whitelistPluginHosts");
	hosts.open(QFile::WriteOnly);
	foreach (QString host, m_allowedHosts) {
		hosts.write(host.toUtf8()+"\n");
	}
	hosts.flush();
	hosts.close();

	QSettings s;
	s.setValue("allowpluginsforall", QVariant(allowAll));

	qDebug() << "Plugin-hosts saved.";
}
