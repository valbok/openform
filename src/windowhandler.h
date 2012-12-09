/**
 * @file windowhandler.h
 *
 * Definition of WindowHandler class
 *
 * VaL::bOK <val@valbok.name>
 * Created on: <11-Jun-2009 11:00:00 VaL>
 *
 * COPYRIGHT NOTICE: Copyright (C) 2009 VaL::bOK
 * SOFTWARE LICENSE: GNU General Public License v2.0
 * NOTICE: >
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of version 2.0  of the GNU General
 *   Public License as published by the Free Software Foundation.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of version 2.0 of the GNU General
 *   Public License along with this program; if not, write to the Free
 *   Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *   MA 02110-1301, USA.
 */

#ifndef WINDOWHANDLER_H
#define WINDOWHANDLER_H

#include <QtGui/QMainWindow>
#include <QtCore/QFile>

namespace OpenForm
{

class WindowHandler: public QObject
{
    Q_OBJECT

    /**
     * Ui widget
     */
    QWidget *Widget;

    static void deleteWidget( QWidget *widget );
    void setWidget( QWidget *widget );
public:
    WindowHandler( QFile &file );
    virtual ~WindowHandler() { WindowHandler::deleteWidget( this->Widget ); }
    bool hasUi() const;
    void show() const;
    void parseUiFrom( QFile &file );
    void parseUiFrom( const QString &data );
    QWidget *getWidget() const { return this->Widget; }

public slots:
    void setUiFrom( const QString &data );
    void returnData( const QString &data ) const;
};

} // namespace OpenForm

#endif // WINDOWHANDLER_H
