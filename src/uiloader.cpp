/**
 * VaL::bOK <valbok@gmail.com>
 * Created on: <10-Jun-2009 11:00:54 VaL>
 *
 * COPYRIGHT NOTICE: Copyright (C) 2009 VaL::bOK
 * SOFTWARE LICENSE: GNU General Public License v2.0
 */

#include <QtGui/QtGui>
#include "uiloader.h"

using namespace OpenForm;

UiLoader::TriggerListMap UiLoader::TriggerList;

/**
 * Searches object by name
 */
QObject *UiLoader::objectByName( QWidget *topLevel, const QString &name )
{
    Q_ASSERT( topLevel );
    if ( topLevel->objectName() == name )
        return topLevel;

    return qFindChild< QObject* >( topLevel, name );
}

/**
 * Deletes all existing triggers
 */
void UiLoader::deleteTriggers()
{
    QList< TriggerMap > triggerItems = UiLoader::TriggerList.values();
    for ( int i = 0; i < triggerItems.size(); i++ )
    {
        QList< Trigger* > triggers = triggerItems[i].values();
        qDeleteAll( triggers );
    }

    UiLoader::TriggerList.clear();
}

/**
 * Deletes all trggers for object with \a objectName
 */
void UiLoader::deleteTriggers( const QString &objectName )
{
    if ( !UiLoader::TriggerList.contains( objectName ) )
    {
        return;
    }

    TriggerMap triggerItems = UiLoader::TriggerList[objectName];
    QList < Trigger* > triggers = triggerItems.values();
    qDeleteAll( triggers );
    UiLoader::TriggerList.remove( objectName );
}

/**
 * Deletes triggers by \a objectName and its \a signalName
 */
void UiLoader::deleteTriggers( const QString &objectName, const QString &signalName )
{
    if ( !UiLoader::TriggerList.contains( objectName ) )
    {
        return;
    }

    TriggerMap *triggerItems = &UiLoader::TriggerList[objectName];

    if ( !triggerItems->contains( signalName ) )
    {
        return;
    }

    delete (*triggerItems)[signalName];
    triggerItems->remove( signalName );
}


/**
 * Parses triggers and install it for \a widget
 */
void UiLoader::createTriggers( DomTriggers *ui_triggers, QWidget *widget )
{
    if ( !this->WinHandler )
    {
        return;
    }

    typedef QList< DomTrigger* > DomTriggerList;
    typedef QList< DomEvent* > DomEventList;

    Q_ASSERT( widget != 0 );

    if ( ui_triggers == 0 )
    {
        return;
    }

    const DomTriggerList triggers = ui_triggers->elementTrigger();
    if ( triggers.empty() )
    {
        return;
    }

    // Needs to figure out already defined triggers for current object
    QMap< QString, QString > triggerList;

    const DomTriggerList::const_iterator cend = triggers.constEnd();
    for ( DomTriggerList::const_iterator it = triggers.constBegin(); it != cend; ++it )
    {
        QString objectName = (*it)->attributeObject();

        if ( objectName.isEmpty() )
        {
            fprintf( stderr, "Attribute \"object\" is required for a trigger\n" );
            continue;
        }

        QObject *object = UiLoader::objectByName( widget, objectName );
        if ( !object )
        {
            fprintf( stderr, "Trigger could not find object by name '%s'\n", objectName.toLocal8Bit().constData() );
            continue;
        }

        const DomEventList events = (*it)->elementEvent();
        if ( events.empty() )
        {
            continue;
        }

        const DomEventList::const_iterator cEventEnd = events.constEnd();
        for ( DomEventList::const_iterator eventIt = events.constBegin(); eventIt != cEventEnd; ++eventIt )
        {
            QString command = (*eventIt)->text();
            QString sig = (*eventIt)->attributeSignal().simplified();
            if ( command.isEmpty() || sig.isEmpty() )
            {
                continue;
            }

            // Skip all whitespaces in signal
            // It needs to allow to use signal names without brackets
            QString signalName;
            for ( int i = 0; i < sig.length(); i++ )
            {
                if ( sig[i].isSpace() )
                {
                    continue;
                }

                signalName += sig[i];
            }

            // Add brackets if needed
            if ( signalName.right( 2 ) != "()" )
            {
                signalName += "()";
            }

            QString triggerKey = objectName + signalName;

            if ( triggerList.contains( triggerKey ) )
            {
                fprintf( stderr, "%s::%s: Trigger already exists: '%s'\n", objectName.toLocal8Bit().constData(),
                         signalName.toLocal8Bit().constData(), triggerList[triggerKey].toLocal8Bit().constData() );
                continue;
            }

            // Delete previous trigger for current object and signal
            UiLoader::deleteTriggers( objectName, signalName );

            UiLoader::TriggerList[objectName][signalName] = new Trigger( command, (*eventIt)->attributeAction(), object, this->WinHandler, signalName );

            triggerList[triggerKey] = command;
        }
    }
}

/**
 * Creates widget by \a ui
 *
 * @note added creating of triggers
 */
QWidget *UiLoader::create( DomUI *ui, QWidget *parentWidget )
{
    DomWidget *ui_widget = ui->elementWidget();
    if ( !ui_widget )
    {
        // If there is no widget tag in XML but UI exists,
        // so need to handle triggers if provided.
        if ( this->Updating && this->WinHandler && this->WinHandler->hasUi() )
        {
            this->createTriggers( ui->elementTriggers(), this->WinHandler->getWidget() );
        }

        return 0;
    }

    this->initialize( ui );

    QWidget *widget = QFormBuilder::create( ui_widget, parentWidget );

    if ( !widget )
    {
        return 0;
    }

    // If UI widget exists and need to update it, use current UI widget for creating triggers etc.
    // It is needed to allow to update connections, triggers etc without updating widgets.
    // Otherwise need to use recently parsed widget, it means new UI is being created.
    QWidget *newWidget = this->Updating && this->WinHandler && this->WinHandler->hasUi() ? this->WinHandler->getWidget() : widget;

    this->createConnections( ui->elementConnections(), newWidget );
    this->createTriggers( ui->elementTriggers(), newWidget );
    this->createResources( ui->elementResources() ); // maybe this should go first, before create()...
    this->applyTabStops( newWidget, ui->elementTabStops() );
    this->reset();

    return widget;
}

/**
 *  Returns new or existing widget by \a widgetName
 */
QWidget *UiLoader::createWidget( const QString &widgetName, QWidget *parentWidget, const QString &name )
{
    if ( !this->Updating || !this->WinHandler || !this->WinHandler->hasUi() )
    {
        return QFormBuilder::createWidget( widgetName, parentWidget, name );
    }

    QWidget *widget = UiLoader::widgetByName( this->WinHandler->getWidget(), name );
    if ( !widget )
    {
        fprintf( stderr, "Loaded user interface does not have object with name '%s'\n", name.toLocal8Bit().constData() );
    }

    return widget;
}

QAction *UiLoader::createAction( QObject *parent, const QString &name )
{
    if ( !this->Updating || !this->WinHandler || !this->WinHandler->hasUi() )
    {
        return QFormBuilder::createAction( parent, name );
    }

    QObject *object = UiLoader::objectByName( this->WinHandler->getWidget(), name );
    if ( !object )
    {
        fprintf( stderr, "Loaded user interface does not have object with name '%s'\n", name.toLocal8Bit().constData() );
    }

    return dynamic_cast< QAction* > ( object );
}

/**
 * Creates new DomTriggers
 */
DomTriggers *UiLoader::saveTriggers()
{
    return new DomTriggers;
}

/**
 * Saves DomUI
 */
void UiLoader::saveDom( DomUI *ui, QWidget *widget )
{
    QFormBuilder::saveDom( ui, widget );
    if ( DomTriggers *ui_triggers = saveTriggers() )
    {
        ui->setElementTriggers( ui_triggers );
    }
}

/**
 * @overloaded
 *
 * Parses and creates widget from QIODevice
 */
QWidget *UiLoader::load( QIODevice *dev, QWidget *parentWidget )
{
    QXmlStreamReader reader;
    reader.setDevice( dev );

    return this->load( reader, parentWidget );
}

/**
 * @overloaded
 *
 * Parses and creates widget from QString
 */
QWidget *UiLoader::load( const QString &data, QWidget *parentWidget )
{
    QXmlStreamReader reader;
    reader.addData( data );

    return this->load( reader, parentWidget );
}

/**
 * Parses and creates widget from QXmlStreamReader
 *
 * Returns Widget - if new GUI should be installed
 *         0      - when just need to update current GUI
 */
QWidget *UiLoader::load( QXmlStreamReader &reader, QWidget *parentWidget )
{
    DomUI ui;
    bool initialized = false;
    this->Updating = false;

    const QString uiElement = QLatin1String( "ui" );
    const QString updateElement = QLatin1String( "update" );

    while ( !reader.atEnd() )
    {
        if ( reader.readNext() == QXmlStreamReader::StartElement )
        {
            // If first element is <update>
            if ( this->WinHandler && reader.name().compare( updateElement, Qt::CaseInsensitive ) == 0 )
            {
                // If there is no loaded UI
                if ( !this->WinHandler->hasUi() )
                {
                    reader.raiseError( QCoreApplication::translate( "QAbstractFormBuilder", "No loaded user interface to update" ) );
                    break;
                }

                ui.readUpdated( reader );
                initialized = true;
                this->Updating = true;
            }
            else if ( reader.name().compare( uiElement, Qt::CaseInsensitive ) == 0 )
            {
                ui.read( reader );
                initialized = true;
                UiLoader::deleteTriggers();
            }
            else
            {
                reader.raiseError( QCoreApplication::translate( "QAbstractFormBuilder", "Unexpected element <%1>" ).arg( reader.name().toString() ) );
            }
        }
    }

    if ( reader.hasError() )
    {
        qWarning( "%s", qPrintable( QCoreApplication::translate( "QAbstractFormBuilder", "An error has occurred while reading the ui file at line %1, column %2: %3" )
                                    .arg( reader.lineNumber() ).arg( reader.columnNumber() )
                                    .arg( reader.errorString() ) ) );
        return 0;
    }

    if ( !initialized )
    {
        qWarning( "%s", qPrintable( QCoreApplication::translate( "QAbstractFormBuilder", "Invalid ui file: The root element <ui> is missing." ) ) );
        return 0;
    }

    QWidget *widget = this->create( &ui, parentWidget );

    // Need to return widget when new GUI is provided only
    return !this->Updating ? widget : 0;
}

/**
 * Computes properties.
 *
 * @note inherites due to have to skip uneeded warning about unsupported flags
 */
QList<DomProperty*> QAbstractFormBuilder::computeProperties(QObject *obj)
{
    QList<DomProperty*> lst;

    const QMetaObject *meta = obj->metaObject();

    QHash<QByteArray, bool> properties;
    const int propertyCount = meta->propertyCount();
    for(int i=0; i < propertyCount; ++i)
        properties.insert(meta->property(i).name(), true);

    const QList<QByteArray> propertyNames = properties.keys();

    const int propertyNamesCount = propertyNames.size();
    for(int i=0; i<propertyNamesCount ; ++i) {
        const QString pname = QString::fromUtf8(propertyNames.at(i));
        const QMetaProperty prop = meta->property(meta->indexOfProperty(pname.toUtf8()));

        if (!prop.isWritable() || !checkProperty(obj, QLatin1String(prop.name())))
            continue;

        const QVariant v = prop.read(obj);

        DomProperty *dom_prop = 0;
        if (v.type() == QVariant::Int) {
            dom_prop = new DomProperty();

            if (prop.isEnumType()) {
                QString scope = QString::fromUtf8(prop.enumerator().scope());
                if (scope.size())
                    scope += QString::fromUtf8("::");
                const QString e = QString::fromUtf8(prop.enumerator().valueToKey(v.toInt()));
                if (e.size())
                    dom_prop->setElementEnum(scope + e);
            } else
                dom_prop->setElementNumber(v.toInt());
            dom_prop->setAttributeName(pname);
        } else {
            dom_prop = createProperty(obj, pname, v);
        }

        if (!dom_prop || dom_prop->kind() == DomProperty::Unknown)
            delete dom_prop;
        else
            lst.append(dom_prop);
    }

    return lst;
}
