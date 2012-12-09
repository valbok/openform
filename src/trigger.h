/**
 * @file trigger.h
 *
 * Definition of Trigger class
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

#ifndef TRIGGERHANDLER_H
#define TRIGGERHANDLER_H

#include <QtCore/QObject>
#include <QProcess>

#include "windowhandler.h"

namespace OpenForm
{

/**
 * List of supported actions
 */
namespace Actions
{
    /**
     * When this action is used needs to execute command, fetch returned data and send to parsing for new GUI
     */
    const QString EXECUTE = "execute";

    /**
     * When this action is used needs just to parse userinput data and print it to stdout
     */
    const QString RETURN = "return";

}; // namespace Actions

class Trigger: public QObject
{
    Q_OBJECT

private:

    /**
     * Command that should be executed
     */
    const QString Command;

    /**
     * Parsed command
     */
    QString ParsedCommand;

    /**
     * Window handler where UI is stored
     */
    const WindowHandler *WinHandler;

    /**
     * Current action
     *
     * @sa namespace Actions
     */
    QString Action;

    /**
     * Result data of Command executing
     */
    QString ReturnedData;

    QProcess Process;
    void setAction( const QString &action, const QString &objectName, const QString &sig );

private slots:

    void readFromStdout();
    void finished( int exitCode, QProcess::ExitStatus exitStatus );
    void error( QProcess::ProcessError error );

public:
    Trigger( const QString &command, const QString &action, const QObject *object, const WindowHandler *windowHandler, const QString &sig );
    ~Trigger() {}

    QString getCommand() const { return this->Command; }

public slots:
    bool handle();

signals:

    /**
     * Emits when UI should be updated by \a data
     */
    void uiChanged( const QString &data );

    /**
     * Emits when just needs to return \a data to user
     */
    void dataReturned( const QString &data );

};

} // namespace OpenForm

#endif // TRIGGERHANDLER_H
