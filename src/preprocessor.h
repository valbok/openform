/**
 * @file preprocessor.h
 *
 * Definition of PreProcessor class
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

#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QMap>

namespace OpenForm
{

/**
 * Reserved words
 */
namespace Reserved
{
    const QChar COMMENT   = '#';
    const QChar NEWLINE   = '\n';
    const QString INCLUDE = QString( COMMENT ) + "include";
    const QChar QUOTE     = '"';
}

class PreProcessor
{
    /**
     * List of included files.
     *
     * @key   - Name of file that was included.
     * @value - From which file name/data text and line position it was included.
     */
    static QMap< QString, QString > IncludeList;

    /**
     * Text data that should be processed
     */
    QString Data;

    /**
     * File name where data is fetched from
     */
    QString FileName;

    QString processInclude( const QString &fileName, const int &linePos );
    QString findIncludeFileName( const QString &line, const int &linePos );
    QString process();

public:
    PreProcessor(): Data( QString() ), FileName( QString() ) { }
    ~PreProcessor() {}
    QString process( QFile *file );
    QString process( const QString &data );

    static void clearIncludeList();
};


} // namespace OpenForm

#endif // PREPROCESSOR_H
