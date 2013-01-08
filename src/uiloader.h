/**
 * VaL::bOK <valbok@gmail.com>
 * Created on: <10-Jun-2009 11:00:54 VaL>
 *
 * COPYRIGHT NOTICE: Copyright (C) 2009 VaL::bOK
 * SOFTWARE LICENSE: GNU General Public License v2.0
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

/**
 * This class contains methods to parse UI from xml
 */
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
