/**
 * VaL::bOK <valbok@gmail.com>
 * Created on: <10-Jun-2009 11:00:54 VaL>
 *
 * COPYRIGHT NOTICE: Copyright (C) 2009 VaL::bOK
 * SOFTWARE LICENSE: GNU General Public License v2.0
 */

/**
 * Top level of Open Form tool.
 */

#include <QtGui/QApplication>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include "windowhandler.h"

#define OPENFORM_VERSION "0.0.1"
#define OPENFORM_VERSION_STR "Open Form version %s\n", OPENFORM_VERSION

using namespace OpenForm;

void showHelp( const char *appName )
{
    fprintf( stderr, OPENFORM_VERSION_STR );

    fprintf( stderr, "Usage: %s [options] <uifile>\n\n"
                     "  -h, -help                 display this help and exit\n"
                     "  -v, -version              display version\n"
                     "\n", appName );
}

int main( int argc, char *argv[] )
{
    QApplication a( argc, argv );

    const char *inputFile = 0;

    int arg = 1;
    while ( arg < argc )
    {
        QString opt = QString::fromLocal8Bit( argv[arg] );
        if ( opt == QLatin1String( "-h" ) || opt == QLatin1String( "-help" ) )
        {
            showHelp( argv[0] );
            return 0;
        }
        else if ( opt == QLatin1String( "-v" ) || opt == QLatin1String( "-version" ) )
        {
            fprintf( stderr, OPENFORM_VERSION_STR );
            return 0;
        }
        else if ( !inputFile )
        {
            inputFile = argv[arg];
        }
        else
        {
            showHelp( argv[0] );
            return 1;
        }

        ++arg;
    }

    QString fileName = QString::fromLocal8Bit( inputFile );
    QFile file;

    if ( fileName.isEmpty() )
    {
        file.open( stdin, QIODevice::ReadOnly );
    }
    else
    {
        file.setFileName( fileName );
        if ( !file.exists() )
        {
            fprintf( stderr, "The file '%s' does not exist\n", fileName.toLocal8Bit().constData() );
            return 1;
        }

        if ( !file.open( QFile::ReadOnly | QFile::Text ) )
        {
            fprintf( stderr, "The file '%s' could not be opened\n", fileName.toLocal8Bit().constData() );
            return 2;
        }

        QFileInfo fileInfo( file );
        // Included or needed to execute files will be searched in this path
        QDir::setCurrent( fileInfo.path() );
    }

    WindowHandler window( file );
    file.close();

    if ( !window.hasUi() )
    {
        fileName = !fileName.isEmpty() ? "file '" + fileName + "'" : "<stdin>";
        fprintf( stderr, "Could not load widget from %s\n", fileName.toLocal8Bit().constData() );
        return 4;
    }

    window.show();

    return a.exec();
}
