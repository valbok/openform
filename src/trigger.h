/**
 * VaL::bOK <valbok@gmail.com>
 * Created on: <10-Jun-2009 11:00:54 VaL>
 *
 * COPYRIGHT NOTICE: Copyright (C) 2009 VaL::bOK
 * SOFTWARE LICENSE: GNU General Public License v2.0
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

/**
 * This class contains methods to handle events e.g. to execute a command when someone clicks on button
 */
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
