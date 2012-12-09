/**
 * @file inputcommandparser.h
 *
 * Definition of InputCommandParser class
 *
 * VaL::bOK <val@valbok.name>
 * Created on: <10-Jun-2009 11:00:54 VaL>
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

#ifndef INPUTCOMMANDPARSER_H
#define INPUTCOMMANDPARSER_H

#include <QWidget>

#include "ui4_p.h"

namespace OpenForm
{

/**
 * Reserved elements of commands
 */
namespace CommandElements
{
    const QChar OPENED    = '{';
    const QChar CLOSED    = '}';
    const QChar ESCAPED   = '\\';
    const QChar SEPARATOR = '.';
}; // namespace CommandElements

/**
 * Handler to fetch user input data
 */
class InputCommandParser
{
    QWidget *Widget;

    QString parseCommand( const QString &command );

public:
    InputCommandParser( QWidget *widget ): Widget( widget ) {}
    ~InputCommandParser() {}
    QString parse( const QString &input );

    static inline QString badResult( const QString &command );
};

#define USAGE      QString( "Use one of the following:\n" )
#define NO_ELEMENT          "%s element is not provided. %s\n"
#define UNSUPPORTED_ELEMENT "Unsupported %s element '%s'. %s\n"

/**
 * Handler for property elements
 */
class PropertyElementParser
{
    /**
     * Dom of widget
     */
    const DomWidget *TopDomWidget;

    /**
     * Command that should be handled
     */
    const QString *Command;

    /**
     * Name of widget
     */
    const QString *WidgetName;

    /**
     * Name of property
     */
    const QString *PropertyName;

    /**
     * Parsed Command for elements
     */
    const QStringList *CommandItemList;

    QString parseColor( const DomProperty *p ) const;
    QString parseFont( const DomProperty *p ) const;
    QString parseIconSet( const DomProperty *p ) const;
    QString parsePoint( const DomProperty *p ) const;
    QString parseRect( const DomProperty *p ) const;
    QString parseSizePolicy( const DomProperty *p ) const;
    QString parseSize( const DomProperty *p ) const;
    QString parseStringList( const DomProperty *p ) const;
    QString parseDate( const DomProperty *p ) const;
    QString parseTime( const DomProperty *p ) const;
    QString parseDateTime( const DomProperty *p ) const;
    QString parsePointF( const DomProperty *p ) const;
    QString parseRectF( const DomProperty *p ) const;
    QString parseSizeF( const DomProperty *p ) const;

public:
    PropertyElementParser( const DomWidget *ui_widget, const QString *command, const QString *widgetName, const QString *propertyName, const QStringList *commandItemList ):
        TopDomWidget( ui_widget ), Command( command ), WidgetName( widgetName ), PropertyName( propertyName ), CommandItemList( commandItemList ) {}
    ~PropertyElementParser() {}
    QString parse();
};

} // namespace OpenForm

#endif // INPUTCOMMANDPARSER_H
