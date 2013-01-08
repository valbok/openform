/**
 * Definition of UiLoader class
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

#ifndef UILOADER_H
#define UILOADER_H

#include "uilib/formbuilder.h"
#include "windowhandler.h"
#include "trigger.h"
#include "ui4_p.h"
#include "inputcommandparser.h"

namespace OpenForm
{

class UiLoader: public QFormBuilder
{
    /**
     * @key   - Event name like "clicked()"
     * @value - Trigger
     */
    typedef QMap< QString, Trigger* > TriggerMap;

    /**
     * @key   - Object name
     * @value - Events for this object
     */
    typedef QMap< QString, TriggerMap > TriggerListMap;

    /**
     * List of triggers
     */
    static TriggerListMap TriggerList;

    /**
     * UI handler
     */
    WindowHandler *WinHandler;

    /**
     * Updating or creating new UI
     */
    bool Updating;

public:
    UiLoader(): QFormBuilder(), WinHandler( 0 ), Updating( false ) {}
    UiLoader( WindowHandler *window ): QFormBuilder(), WinHandler( window ), Updating( false ) {}

    static QObject *objectByName( QWidget *topLevel, const QString &name );
    QWidget *load( QXmlStreamReader &reader, QWidget *parentWidget = 0 );
    QWidget *load( const QString &data, QWidget *parentWidget = 0 );
    QWidget *load( QIODevice *dev, QWidget *parentWidget = 0 );

    // Allow InputCommandParser to use some protected functions
    friend class InputCommandParser;

protected:
    virtual void createTriggers( DomTriggers *triggers, QWidget *widget );
    virtual QWidget *create( DomUI *ui, QWidget *parentWidget );
    virtual QWidget *createWidget( const QString &widgetName, QWidget *parentWidget, const QString &name );
    virtual QAction *createAction( QObject *parent, const QString &name );
    virtual QList< DomProperty* > computeProperties( QObject *obj );

    DomTriggers *saveTriggers();
    virtual void saveDom( DomUI *ui, QWidget *widget );
    static void deleteTriggers();
    static void deleteTriggers( const QString &objectName );
    static void deleteTriggers( const QString &objectName, const QString &signalName );
};

} // namespace OpenForm

#endif // UILOADER_H
