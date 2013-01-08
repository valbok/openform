/**
 * VaL::bOK <valbok@gmail.com>
 * Created on: <10-Jun-2009 11:00:54 VaL>
 *
 * COPYRIGHT NOTICE: Copyright (C) 2009 VaL::bOK
 * SOFTWARE LICENSE: GNU General Public License v2.0
 */

#include "windowhandler.h"
#include "uiloader.h"
#include "preprocessor.h"
#include <QBuffer>

using namespace OpenForm;

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
