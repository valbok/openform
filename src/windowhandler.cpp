/**
 * @file windowhandler.cpp
 *
 * Implementation of WindowHandler class
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

/**
 * This class contains methods to control UI
 */

#include "windowhandler.h"
#include "uiloader.h"
#include "preprocessor.h"
#include <QBuffer>

using namespace OpenForm;

/**
 * Constructor
 */
WindowHandler::WindowHandler( QFile &file ): Widget( 0 )
{
    this->parseUiFrom( file );
}

/**
 * Parses UI from file
 */
void WindowHandler::parseUiFrom( QFile &file )
{
    PreProcessor preProcessor;

    UiLoader loader( this );
    this->setWidget( loader.load( preProcessor.process( &file ) ) );
}

/**
 * Parses UI from text
 */
void WindowHandler::parseUiFrom( const QString &data )
{
    PreProcessor preProcessor;

    UiLoader loader( this );
    this->setWidget( loader.load( preProcessor.process( data ) ) );
}

/**
 * Returns input data to stdout
 */
void WindowHandler::returnData( const QString &data ) const
{
    fprintf( stdout, "%s\n", data.toLocal8Bit().constData() );
    emit this->Widget->close();
}

/**
 * Updates current UI by new from text
 */
void WindowHandler::setUiFrom( const QString &data )
{
    PreProcessor preProcessor;

    // Every time when we try to update existing Ui, need to clear include list
    // to prevent errors about files are already included
    PreProcessor::clearIncludeList();

    UiLoader loader( this );

    QWidget *widget = loader.load( preProcessor.process( data ) );
    if ( !widget )
    {
        return;
    }

    this->setWidget( widget );
    this->show();
}

/**
 * Deletes widget
 */
void WindowHandler::deleteWidget( QWidget *widget )
{
    delete widget;
}

/**
 * Sets new widget
 */
void WindowHandler::setWidget( QWidget *widget )
{
    if ( !widget || this->Widget == widget )
    {
        return;
    }

    // Delete old widget
    WindowHandler::deleteWidget( this->Widget );
    this->Widget = widget;
}

/**
 * Checks if UI exists
 */
bool WindowHandler::hasUi() const
{
    return this->Widget ? true : false;
}

/**
 * Shows UI
 */
void WindowHandler::show() const
{
    if ( !this->hasUi() )
    {
        return;
    }

    this->Widget->show();
}

