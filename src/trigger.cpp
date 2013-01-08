/**
 * VaL::bOK <valbok@gmail.com>
 * Created on: <10-Jun-2009 11:00:54 VaL>
 *
 * COPYRIGHT NOTICE: Copyright (C) 2009 VaL::bOK
 * SOFTWARE LICENSE: GNU General Public License v2.0
 */

#include "trigger.h"
#include "commandhandler.h"

using namespace OpenForm;

Trigger::Trigger( const QString &command, const QString &action, const QObject *object, const WindowHandler *windowHandler, const QString &sig ):
 Command( command ), WinHandler( windowHandler )
{
    if ( !object )
    {
        return;
    }

    this->setAction( action, object->objectName(), sig );

    // Handle provided signal
    QByteArray sigMacro = sig.toUtf8();
    sigMacro.prepend( "2" );
    QObject::connect( object, sigMacro, this, SLOT( handle() ) );

    // Update UI by new one
    QObject::connect( this, SIGNAL( uiChanged( const QString & ) ), this->WinHandler, SLOT( setUiFrom( const QString & ) ) );

    // When data should be returned to user
    if ( this->Action == Actions::RETURN )
    {
        QObject::connect( this, SIGNAL( dataReturned( const QString & ) ), this->WinHandler, SLOT( returnData( const QString & ) ) );
    }
}

void Trigger::setAction( const QString &action, const QString &objectName, const QString &sig )
{
    this->Action = ( action == Actions::EXECUTE || action == Actions::RETURN ) ? action : ( action.isEmpty() ? Actions::EXECUTE : QString() );
    if ( this->Action.isEmpty() )
    {
        fprintf( stderr, "%s::%s: Unsupported action '%s'\n", objectName.toLocal8Bit().constData(), sig.toLocal8Bit().constData(), action.toLocal8Bit().constData() );
        this->Action = Actions::EXECUTE;
    }
}

bool Trigger::handle()
{
    if ( this->Command.isEmpty() || !this->WinHandler )
    {
        return false;
    }

    CommandHandler parser;

    this->ParsedCommand = parser.parse( this->WinHandler->getWidget(), this->Command );

    // Check if it needs to execute the command
    if ( this->Action == Actions::EXECUTE )
    {
        // Previous returned data should be cleared
        this->ReturnedData = QString();

        this->Process.setReadChannelMode( QProcess::SeparateChannels );
        this->Process.setReadChannel( QProcess::StandardOutput );

        // Read data when it is ready
        connect( &this->Process, SIGNAL( readyReadStandardOutput() ), this, SLOT( readFromStdout() ) );
        // When reading is finished return data to update UI
        connect( &this->Process, SIGNAL( finished( int, QProcess::ExitStatus ) ), this, SLOT( finished( int, QProcess::ExitStatus ) ) );
        // Handle errors
        connect( &this->Process, SIGNAL( error( QProcess::ProcessError ) ), this, SLOT( error( QProcess::ProcessError ) ) );

        this->Process.start( this->ParsedCommand );
    }
    else if ( this->Action == Actions::RETURN ) // or just return parsed command to user
    {
        emit this->dataReturned( this->ParsedCommand );
    }

    return true;
}

/**
 * Reads data from stdout when data is ready to be read
 */
void Trigger::readFromStdout()
{
    QProcess *process = (QProcess*) this->sender();
    QByteArray result = process->readAllStandardOutput();

    this->ReturnedData += result.data();
}

/**
 * Update UI by returned data
 */
void Trigger::finished( int exitCode, QProcess::ExitStatus exitStatus )
{
    if ( exitStatus == QProcess::CrashExit )
    {
        fprintf( stderr, "Process has been crashed: '%s'\n", this->ParsedCommand.toLocal8Bit().constData() );
        return;
    }

    if ( exitCode != 0 )
    {
        fprintf( stderr, "Could not execute (exitcode: %i): '%s'\n", exitCode, this->ParsedCommand.toLocal8Bit().constData() );
        return;
    }

    if ( this->ReturnedData.isEmpty() )
    {
        fprintf( stderr, "No data returned: '%s'\n", this->ParsedCommand.toLocal8Bit().constData() );
        return;
    }

    emit this->uiChanged( this->ReturnedData );
}

/**
 * Handles errors
 */
void Trigger::error( QProcess::ProcessError error )
{
    QString errorStr;
    switch ( error )
    {
        case QProcess::FailedToStart:
        {
            errorStr = "Failed to start";
        } break;

        case QProcess::Crashed:
        {
            errorStr = "Crashed";
        } break;

        case QProcess::Timedout:
        {
            errorStr = "Timeout";
        } break;

        case QProcess::WriteError:
        {
            errorStr = "Write error";
        } break;

        case QProcess::ReadError:
        {
            errorStr = "Read error";
        } break;

        case QProcess::UnknownError:
        default:
        {
            errorStr = "Unknown error";
        } break;
    }

    fprintf( stderr, "Could not execute (%s): '%s'\n", errorStr.toLocal8Bit().constData(), this->ParsedCommand.toLocal8Bit().constData() );
}
