/**
 * VaL::bOK <valbok@gmail.com>
 * Created on: <10-Jun-2009 11:00:54 VaL>
 *
 * COPYRIGHT NOTICE: Copyright (C) 2009 VaL::bOK
 * SOFTWARE LICENSE: GNU General Public License v2.0
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

/**
 * This class contains methods to handle pre processors directives like comments or include etc
 */
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
