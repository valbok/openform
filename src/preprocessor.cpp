/**
 * VaL::bOK <valbok@gmail.com>
 * Created on: <10-Jun-2009 11:00:54 VaL>
 *
 * COPYRIGHT NOTICE: Copyright (C) 2009 VaL::bOK
 * SOFTWARE LICENSE: GNU General Public License v2.0
 */

#include "preprocessor.h"
#include <QTextStream>
#include <QRegExp>
#include <QStringList>
#include <QDebug>

using namespace OpenForm;

QMap< QString, QString > PreProcessor::IncludeList;

/**
 * @overloaded
 */
QString PreProcessor::process( QFile *file )
{
    if ( !file || ( !file->isOpen() && !file->open( QFile::ReadOnly | QFile::Text ) ) )
    {
        return QString();
    }

    this->Data = file->readAll().data();
    this->FileName = file->fileName();

    return this->process();
}

/**
 * @overloaded
 */
QString PreProcessor::process( const QString &data )
{
    this->Data = data.toLatin1();

    return this->process();
}

/**
 * Processes derictivies like comments or includes
 */
QString PreProcessor::process()
{
    QString result;
    if ( this->Data.isEmpty() )
    {
        return result;
    }

    QString trimmedLine, tmpLine;
    QTextStream stream( this->Data );

    stream.setCodec( "UTF-8" );

    int linePos = 0;
    while ( !stream.atEnd() )
    {
        ++linePos;
        tmpLine = stream.readLine().trimmed();

        for ( int i = 0; i < tmpLine.length(); i++ )
        {
            if ( tmpLine[i] == Reserved::COMMENT )
            {
                // Check if we found include string
                if ( tmpLine.mid( i, Reserved::INCLUDE.length() ) == Reserved::INCLUDE  )
                {
                    // Fetch all characters from pos (where 'include' is found) to the end of the line
                    QString includeResult = this->processInclude( this->findIncludeFileName( tmpLine.mid( i ), linePos ), linePos );
                    if ( includeResult.isEmpty() )
                    {
                        break;
                    }

                    trimmedLine += includeResult;
                }

                break;
            }

            trimmedLine += tmpLine[i];
        }

        result += trimmedLine + Reserved::NEWLINE;
        trimmedLine = QString();
    }

    return result;
}

/**
 * Processes include derective.
 * Returns parsed data from file \a fileName or empty string if failed
 */
QString PreProcessor::processInclude( const QString &fileName, const int &linePos )
{
    QString result;

    if ( fileName.isEmpty() )
    {
        return result;
    }

    QFile file( fileName );
    if ( !file.exists() )
    {
        fprintf( stderr, "%s[%i]: The file '%s' does not exist\n", this->FileName.toLocal8Bit().constData(),
                 linePos, fileName.toLocal8Bit().constData() );
        return result;
    }

    if ( !file.open( QFile::ReadOnly | QFile::Text ) )
    {
        fprintf( stderr, "%s[%i]: The file '%s' could not be opened\n", this->FileName.toLocal8Bit().constData(),
                 linePos, fileName.toLocal8Bit().constData() );
        return result;
    }

    QString incStr = ( !this->FileName.isEmpty() ? this->FileName: this->Data ) + "[" + QString::number( linePos ) + "]";

    if ( PreProcessor::IncludeList.contains( fileName ) && PreProcessor::IncludeList[fileName] == incStr )
    {
        fprintf( stderr, "'%s' is already included in '%s'\n", fileName.toLocal8Bit().constData(), incStr.toLocal8Bit().constData() );
        return result;
    }

    PreProcessor::IncludeList[fileName] = incStr;
    PreProcessor preProcessor;
    result = preProcessor.process( &file ) + Reserved::NEWLINE;

    return result;
}

/**
 * Searches file name in \a line that should be included.
 * Returns file name or empty string if failed
 */
QString PreProcessor::findIncludeFileName( const QString &line, const int &linePos )
{
    bool startReading = false;
    QString includeFileName, ignoredBefore;
    // List of ignored elements
    QStringList ignoredList;
    int lineLen = line.length();

    for ( int i = Reserved::INCLUDE.length(); i < lineLen ; i++ )
    {
        // If '"' is found we need to start/stop reading the file name
        if ( line[i] == Reserved::QUOTE )
        {
            startReading = !startReading;
            // If we stop reading and this is not the end of the line
            // need to ignore unneeded elements
            if ( !startReading && ( i + 1 ) < lineLen )
            {
                ignoredList.append( line.mid( i + 1, lineLen - i ) );
                break;
            }
            continue;
        }
        else if ( !startReading && !line[i].isSpace() ) // if unexpected elements have been found before filename
        {
            ignoredBefore += line[i];
        }

        // Read file name
        if ( startReading )
        {
            includeFileName += line[i];
        }
    }

    // If file name reading is on, it means second '"' is missing
    if ( startReading )
    {
        fprintf( stderr, "%s[%i]: Missed closed '\"'\n", this->FileName.toLocal8Bit().constData(), linePos );
        includeFileName = QString();
    }

    if ( !ignoredBefore.isEmpty() )
    {
        ignoredList.prepend( ignoredBefore );
    }

    int ignoredSize = ignoredList.size();
    if ( ignoredSize )
    {
        if ( includeFileName.isEmpty() )
        {
            fprintf( stderr, "%s[%i]: Could not find a filename\n", this->FileName.toLocal8Bit().constData(), linePos );
            return includeFileName;
        }

        fprintf( stderr, "%s[%i]: Ignored elements:\n", this->FileName.toLocal8Bit().constData(), linePos );
        for ( int i = 0; i < ignoredSize; i++ )
        {
            fprintf( stderr, "%s\n", ignoredList[i].trimmed().toLocal8Bit().constData() );
        }
    }

    return includeFileName;
}

/**
 * Clears static include list
 */
void PreProcessor::clearIncludeList()
{
    PreProcessor::IncludeList.clear();
}
