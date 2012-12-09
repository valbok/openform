/**
 * @file commandhandler.cpp
 *
 * Implementation of CommandHandler class
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

/**
 * Handler for user commands.
 * Input commands are supported only. Like to fetch user data from widget in run time.
 */

#include "inputcommandparser.h"
#include "commandhandler.h"

using namespace OpenForm;

/**
 * Parses commands in \a command for widget \a topLevel
 *
 * @return Parsed command
 */
QString CommandHandler::parse( QWidget *topLevel, QString command )
{
    QString result;
    InputCommandParser inputParser( topLevel );
    result = inputParser.parse( command );

    return result;
}
