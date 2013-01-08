/**
 * VaL::bOK <valbok@gmail.com>
 * Created on: <10-Jun-2009 11:00:54 VaL>
 *
 * COPYRIGHT NOTICE: Copyright (C) 2009 VaL::bOK
 * SOFTWARE LICENSE: GNU General Public License v2.0
 */

#include "inputcommandparser.h"
#include "commandhandler.h"

using namespace OpenForm;

/**
 * Parses commands in \a command for widget \a topLevel
 */
QString CommandHandler::parse( QWidget *topLevel, QString command )
{
    QString result;
    InputCommandParser inputParser( topLevel );
    result = inputParser.parse( command );

    return result;
}
