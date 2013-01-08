/**
 * VaL::bOK <valbok@gmail.com>
 * Created on: <10-Jun-2009 11:00:54 VaL>
 *
 * COPYRIGHT NOTICE: Copyright (C) 2009 VaL::bOK
 * SOFTWARE LICENSE: GNU General Public License v2.0
 */

/**
 * Handler for user commands.
 * Input commands are supported only. Like to fetch user data from widget in run time.
 */

#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#include <QWidget>
#include <QString>

namespace OpenForm
{

class CommandHandler
{

public:
    CommandHandler() {}
    ~CommandHandler() {}
    QString parse( QWidget *topLevel, QString command );
};

} // namespace OpenForm

#endif // COMMANDHANDLER_H
