/**
 * @file inputcommandparser.cpp
 *
 * Implementation of InputCommandParser class
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
 * This class contains methods to fetch user input data.
 *
 * @example {edit.text} It means from widget with name 'edit' fetch content of property 'text'.
 */

#include "inputcommandparser.h"
#include "uiloader.h"
#include <QTextStream>

using namespace OpenForm;

/**
 * Parses all commands in \a command for widget \a this->Widget
 *
 * @return Parsed string
 */
QString InputCommandParser::parse( const QString &input )
{
    if ( input.isEmpty() || !this->Widget )
    {
        return input;
    }

    QString result;
    QString commandItem;
    bool fetchCommand = false;
    bool escape = false;

    for ( int i = 0; i < input.length(); ++i )
    {
        // Skip processing if users want to use CommandElements::OPENED char for their needs
        if ( input[i] == CommandElements::ESCAPED && input[i + 1] == CommandElements::OPENED && !fetchCommand )
        {
            escape = true;
            continue;
        }

        if ( input[i] == CommandElements::OPENED )
        {
            if ( !fetchCommand && !escape )
            {
                fetchCommand = true;
                continue;
            }

            escape = false;
        }

        // Find out one command that needs to be parsed like {edit.text}
        if ( input[i] == CommandElements::CLOSED )
        {
            if ( fetchCommand )
            {
                fetchCommand = false;
                result += this->parseCommand( commandItem );
                commandItem = QString();
                continue;
            }

            escape = false;
        }

        if ( fetchCommand )
        {
            commandItem += input[i];
            continue;
        }

        result += input[i];
    }

    if ( fetchCommand )
    {
        result += CommandElements::OPENED + commandItem;
        fprintf( stderr, "Could not find '%c' in command '%s'\n", CommandElements::CLOSED.toLatin1(), input.toLocal8Bit().constData() );
    }

    // '<' and '>' are reserved chars to parse xml text, so it is impossible to use it in trigger commands.
    // Instead of these symbols users can use '[[' and ']]'.
    result = result.replace( "[[", "<" );
    result = result.replace( "]]", ">" );

    return result;
}

/**
 * Produces bad result if error
 */
inline QString InputCommandParser::badResult( const QString &command )
{
    return CommandElements::OPENED + command + CommandElements::CLOSED;
}

/**
 * Handles one command only, without text and other commands
 */
QString InputCommandParser::parseCommand( const QString &command )
{
    QString badResult = InputCommandParser::badResult( command );
    if ( !this->Widget )
    {
        return badResult;
    }

    QString item;
    // List of parsed items like widget name, property name etc
    QStringList commandItemList;

    for ( int i = 0; i < command.length(); ++i )
    {
        if ( command[i] == CommandElements::SEPARATOR )
        {
            commandItemList.append( item );
            item = QString();
            continue;
        }

        item += command[i];
    }

    // Add last item
    if ( !item.isEmpty() )
    {
        commandItemList.append( item );
    }

    QString widgetName = commandItemList.size() >= 1 ? commandItemList.at( 0 ) : QString();
    QString propertyName = commandItemList.size() >= 2 ? commandItemList.at( 1 ) : QString();

    if ( widgetName.isEmpty() )
    {
        fprintf( stderr, "Widget name is not provided in command '%s'\n", command.toLocal8Bit().constData() );
        return badResult;
    }

    if ( propertyName.isEmpty() )
    {
        fprintf( stderr, "Property name is not provided in command '%s'\n", command.toLocal8Bit().constData() );
        return badResult;
    }

    UiLoader loader;

    QWidget *widget = loader.widgetByName( this->Widget, widgetName );

    if ( !widget )
    {
        fprintf( stderr, "Could not find widget by name '%s'\n", widgetName.toLocal8Bit().constData() );
        return badResult;
    }

    DomWidget *ui_widget = loader.createDom( widget, 0 );

    if ( !ui_widget )
    {
        fprintf( stderr, "Could not create dom by widget\n" );
        return badResult;
    }

    // Parse properties
    PropertyElementParser elementParser( ui_widget, &command, &widgetName, &propertyName, &commandItemList );
    QString result = elementParser.parse();

    delete ui_widget;

    return result;
}

/**
 * Fetches value of property \a this->PropertyName
 */
QString PropertyElementParser::parse()
{
    QString result = InputCommandParser::badResult( *this->Command );
    QList< DomProperty* > list = this->TopDomWidget->elementProperty();
    bool foundProperty = false;

    for ( int i = 0; i < list.size(); ++i )
    {
        DomProperty *p = list.at( i );
        if ( p->attributeName() == this->PropertyName )
        {
            foundProperty = true;
            switch ( p->kind() )
            {
                case DomProperty::Bool:
                {
                    result = p->elementBool();
                } break;

                case DomProperty::Color:
                {
                    result = this->parseColor( p );
                } break;

                case DomProperty::Cstring:
                {
                    result = p->elementCstring();
                } break;

                case DomProperty::Cursor:
                {
                    result = QString::number( p->elementCursor() );
                } break;

                case DomProperty::CursorShape:
                {
                    result = p->elementCursorShape();
                } break;

                case DomProperty::Enum:
                {
                    result = p->elementEnum();
                } break;

                case DomProperty::Font:
                {
                    result = this->parseFont( p );
                } break;

                case DomProperty::IconSet:
                {
                    result = this->parseIconSet( p );
                } break;

                case DomProperty::Pixmap:
                {
                    result = p->elementPixmap()->text();
                } break;

                case DomProperty::Point:
                {
                    result = this->parsePoint( p );
                } break;

                case DomProperty::Rect:
                {
                    result = this->parseRect( p );
                } break;

                case DomProperty::Set:
                {
                    result = p->elementSet();
                } break;

                case DomProperty::Locale:
                {
                    result = p->elementLocale()->text();
                } break;

                case DomProperty::SizePolicy:
                {
                    result = this->parseSizePolicy( p );
                } break;

                case DomProperty::Size:
                {
                    result = this->parseSize( p );
                } break;

                case DomProperty::String:
                {
                    result = p->elementString()->text();
                } break;

                case DomProperty::StringList:
                {
                    result = this->parseStringList( p );
                } break;

                case DomProperty::Number:
                {
                    result = QString::number( p->elementNumber() );
                } break;

                case DomProperty::Float:
                {
                    result = QString::number( p->elementFloat() );
                } break;

                case DomProperty::Double:
                {
                    result = QString::number( p->elementDouble() );
                } break;

                case DomProperty::Date:
                {
                    result = this->parseDate( p );
                } break;

                case DomProperty::Time:
                {
                    result = this->parseTime( p );
                } break;

                case DomProperty::DateTime:
                {
                    result = this->parseDateTime( p );
                } break;

                case DomProperty::PointF:
                {
                    result = this->parsePointF( p );
                } break;

                case DomProperty::RectF:
                {
                    result = this->parseRectF( p );
                } break;

                case DomProperty::SizeF:
                {
                    result = this->parseSizeF( p );
                } break;

                case DomProperty::LongLong:
                {
                    result = QString::number( p->elementLongLong() );
                } break;

                case DomProperty::Char:
                {
                    result = QString::number( p->elementChar()->elementUnicode() );
                } break;

                case DomProperty::Url:
                {
                    result = p->elementUrl()->elementString()->text();
                } break;

                case DomProperty::UInt:
                {
                    result = QString::number( p->elementUInt() );
                } break;

                case DomProperty::ULongLong:
                {
                     result = QString::number( p->elementULongLong() );
                } break;

                case DomProperty::Palette:
                case DomProperty::Brush:
                case DomProperty::Unknown:
                default:
                {
                    fprintf( stderr, "Type of property is not supported\n" );
                } break;
            }
            break;
        }
    }

    if ( !foundProperty )
    {
        fprintf( stderr, "Widget '%s' does not have property '%s'\n", this->WidgetName->toLocal8Bit().constData(), this->PropertyName->toLocal8Bit().constData() );
    }

    return result;
}

/**
 * Fetches value of Color property
 */
QString PropertyElementParser::parseColor( const DomProperty *p ) const
{
    QString usage = USAGE +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "red"   + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "green" + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "blue"  + "\n";

    if ( this->CommandItemList->size() < 3 )
    {
        fprintf( stderr, NO_ELEMENT, "Color", usage.toLocal8Bit().constData() );
        return InputCommandParser::badResult( *this->Command );
    }

    QString result;
    QString element = this->CommandItemList->at( 2 ).toLower();
    DomColor *dom = p->elementColor();

    if ( element == "red" )
    {
        result = QString::number( dom->elementRed() );
    }
    else if ( element == "green" )
    {
        result = QString::number( dom->elementGreen() );
    }
    else if ( element == "blue" )
    {
        result = QString::number( dom->elementBlue() );
    }
    else
    {
        fprintf( stderr, UNSUPPORTED_ELEMENT, "Color", element.toLocal8Bit().constData(), usage.toLocal8Bit().constData() );
        result = InputCommandParser::badResult( *this->Command );
    }

    return result;
}

/**
 * Fetches value of Font property
 */
QString PropertyElementParser::parseFont( const DomProperty *p ) const
{
    QString usage = USAGE +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "family"        + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "pointsize"     + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "weight"        + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "italic"        + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "bold"          + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "underline"     + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "strikeout"     + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "antialiasing"  + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "stylestrategy" + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "kerning"       + "\n";

    if ( this->CommandItemList->size() < 3 )
    {
        fprintf( stderr, NO_ELEMENT, "Font", usage.toLocal8Bit().constData() );
        return InputCommandParser::badResult( *this->Command );
    }

    QString result;
    QString element = this->CommandItemList->at( 2 ).toLower();
    DomFont *dom = p->elementFont();

    if ( element == "family" )
    {
        result = dom->elementFamily();
    }
    else if ( element == "pointsize" )
    {
        result = QString::number( dom->elementPointSize() );
    }
    else if ( element == "weight" )
    {
        result = QString::number( dom->elementWeight() );
    }
    else if ( element == "italic" )
    {
        result = dom->elementItalic() ? "true" : "false";
    }
    else if ( element == "bold" )
    {
        result = dom->elementBold() ? "true" : "false";
    }
    else if ( element == "underline" )
    {
        result = dom->elementUnderline() ? "true" : "false";
    }
    else if ( element == "strikeout" )
    {
        result = dom->elementStrikeOut() ? "true" : "false";
    }
    else if ( element == "antialiasing" )
    {
        result = dom->elementAntialiasing() ? "true" : "false";
    }
    else if ( element == "stylestrategy" )
    {
        result = dom->elementStyleStrategy();
    }
    else if ( element == "kerning" )
    {
        result = dom->elementKerning() ? "true" : "false";
    }
    else
    {
        fprintf( stderr, UNSUPPORTED_ELEMENT, "Font", element.toLocal8Bit().constData(), usage.toLocal8Bit().constData() );
        result = InputCommandParser::badResult( *this->Command );
    }

    return result;
}

/**
 * Fetches value of IconSet property
 */
QString PropertyElementParser::parseIconSet( const DomProperty *p ) const
{
    QString usage = USAGE +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "normaloff"   + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "normalon"    + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "disabledoff" + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "disabledon"  + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "activeoff"   + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "activeon"    + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "selectedoff" + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "selectedon"  + "\n";

    if ( this->CommandItemList->size() < 3 )
    {
        fprintf( stderr, NO_ELEMENT, "IconSet", usage.toLocal8Bit().constData() );
        return InputCommandParser::badResult( *this->Command );
    }

    QString result;
    QString element = this->CommandItemList->at( 2 ).toLower();
    DomResourceIcon *dom = p->elementIconSet();

    if ( element == "normaloff" )
    {
        result = dom->elementNormalOff()->text();
    }
    else if ( element == "normalon" )
    {
        result = dom->elementNormalOn()->text();
    }
    else if ( element == "disabledoff" )
    {
        result = dom->elementDisabledOff()->text();
    }
    else if ( element == "disabledon" )
    {
        result = dom->elementDisabledOn()->text();
    }
    else if ( element == "activeoff" )
    {
        result = dom->elementActiveOff()->text();
    }
    else if ( element == "activeon" )
    {
        result = dom->elementActiveOn()->text();
    }
    else if ( element == "selectedoff" )
    {
        result = dom->elementSelectedOff()->text();
    }
    else if ( element == "selectedon" )
    {
        result = dom->elementSelectedOn()->text();
    }
    else
    {
        fprintf( stderr, UNSUPPORTED_ELEMENT, "IconSet", element.toLocal8Bit().constData(), usage.toLocal8Bit().constData() );
        result = InputCommandParser::badResult( *this->Command );
    }

    return result;
}

/**
 * Fetches value of Point property
 */
QString PropertyElementParser::parsePoint( const DomProperty *p ) const
{
    QString usage = USAGE +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "x" + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "y" + "\n";

    if ( this->CommandItemList->size() < 3 )
    {
        fprintf( stderr, NO_ELEMENT, "Point", usage.toLocal8Bit().constData() );
        return InputCommandParser::badResult( *this->Command );
    }

    QString result;
    QString element = this->CommandItemList->at( 2 ).toLower();
    DomPoint *dom = p->elementPoint();

    if ( element == "x" )
    {
        result = QString::number( dom->elementX() );
    }
    else if ( element == "y" )
    {
        result = QString::number( dom->elementY() );
    }
    else
    {
        fprintf( stderr, UNSUPPORTED_ELEMENT, "Point", element.toLocal8Bit().constData(), usage.toLocal8Bit().constData() );
        result = InputCommandParser::badResult( *this->Command );
    }

    return result;
}

/**
 * Fetches value of Rect property
 */
QString PropertyElementParser::parseRect( const DomProperty *p ) const
{
    QString usage = USAGE +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "x"      + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "y"      + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "width"  + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "height" + "\n";

    if ( this->CommandItemList->size() < 3 )
    {
        fprintf( stderr, NO_ELEMENT, "Rect", usage.toLocal8Bit().constData() );
        return InputCommandParser::badResult( *this->Command );
    }

    QString result;
    QString element = this->CommandItemList->at( 2 ).toLower();
    DomRect *dom = p->elementRect();

    if ( element == "x" )
    {
        result = QString::number( dom->elementX() );
    }
    else if ( element == "y" )
    {
        result = QString::number( dom->elementY() );
    }
    else if ( element == "width" )
    {
        result = QString::number( dom->elementWidth() );
    }
    else if ( element == "height" )
    {
        result = QString::number( dom->elementHeight() );
    }
    else
    {
        fprintf( stderr, UNSUPPORTED_ELEMENT, "Rect", element.toLocal8Bit().constData(), usage.toLocal8Bit().constData() );
        result = InputCommandParser::badResult( *this->Command );
    }

    return result;
}

/**
 * Fetches value of SizePolicy property
 */
QString PropertyElementParser::parseSizePolicy( const DomProperty *p ) const
{
    QString usage = USAGE +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "hsizetype"  + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "vsizetype"  + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "horstretch" + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "verstretch" + "\n";

    if ( this->CommandItemList->size() < 3 )
    {
        fprintf( stderr, NO_ELEMENT, "SizePolicy", usage.toLocal8Bit().constData() );
        return InputCommandParser::badResult( *this->Command );
    }

    QString result;
    QString element = this->CommandItemList->at( 2 ).toLower();
    DomSizePolicy *dom = p->elementSizePolicy();

    if ( element == "hsizetype" )
    {
        result = QString::number( dom->elementHSizeType() );
    }
    else if ( element == "vsizetype" )
    {
        result = QString::number( dom->elementVSizeType() );
    }
    else if ( element == "horstretch" )
    {
        result = QString::number( dom->elementHorStretch() );
    }
    else if ( element == "verstretch" )
    {
        result = QString::number( dom->elementVerStretch() );
    }
    else
    {
        fprintf( stderr, UNSUPPORTED_ELEMENT, "SizePolicy", element.toLocal8Bit().constData(), usage.toLocal8Bit().constData() );
        result = InputCommandParser::badResult( *this->Command );
    }

    return result;
}

/**
 * Fetches value of Size property
 */
QString PropertyElementParser::parseSize( const DomProperty *p ) const
{
    QString usage = USAGE +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "width"  + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "height" + "\n";

    if ( this->CommandItemList->size() < 3 )
    {
        fprintf( stderr, NO_ELEMENT, "Size", usage.toLocal8Bit().constData() );
        return InputCommandParser::badResult( *this->Command );
    }

    QString result;
    QString element = this->CommandItemList->at( 2 ).toLower();
    DomSize *dom = p->elementSize();

    if ( element == "width" )
    {
        result = QString::number( dom->elementWidth() );
    }
    else if ( element == "height" )
    {
        result = QString::number( dom->elementHeight() );
    }
    else
    {
        fprintf( stderr, UNSUPPORTED_ELEMENT, "Size", element.toLocal8Bit().constData(), usage.toLocal8Bit().constData() );
        result = InputCommandParser::badResult( *this->Command );
    }

    return result;
}

/**
 * Fetches value of StringList property
 */
QString PropertyElementParser::parseStringList( const DomProperty *p ) const
{
    QString result;
    DomStringList *dom = p->elementStringList();
    QStringList stringList = dom->elementString();
    int size = stringList.size();

    for ( int i = 0; i < size; ++i )
    {
        result += stringList[i];
        if ( ( i + 1 ) < size )
        {
            result += ",";
        }
    }

    return result;
}

/**
 * Fetches value of Date property
 */
QString PropertyElementParser::parseDate( const DomProperty *p ) const
{
    QString usage = USAGE +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "year"  + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "month" + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "day"   + "\n";

    if ( this->CommandItemList->size() < 3 )
    {
        fprintf( stderr, NO_ELEMENT, "Date", usage.toLocal8Bit().constData() );
        return InputCommandParser::badResult( *this->Command );
    }

    QString result;
    QString element = this->CommandItemList->at( 2 ).toLower();
    DomDate *dom = p->elementDate();

    if ( element == "year" )
    {
        result = QString::number( dom->elementYear() );
    }
    else if ( element == "month" )
    {
        result = QString::number( dom->elementMonth() );
    }
    else if ( element == "day" )
    {
        result = QString::number( dom->elementDay() );
    }
    else
    {
        fprintf( stderr, UNSUPPORTED_ELEMENT, "Date", element.toLocal8Bit().constData(), usage.toLocal8Bit().constData() );
        result = InputCommandParser::badResult( *this->Command );
    }

    return result;
}

/**
 * Fetches value of Time property
 */
QString PropertyElementParser::parseTime( const DomProperty *p ) const
{
    QString usage = USAGE +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "hour"   + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "minute" + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "second" + "\n";

    if ( this->CommandItemList->size() < 3 )
    {
        fprintf( stderr, NO_ELEMENT, "Time", usage.toLocal8Bit().constData() );
        return InputCommandParser::badResult( *this->Command );
    }

    QString result;
    QString element = this->CommandItemList->at( 2 ).toLower();
    DomTime *dom = p->elementTime();

    if ( element == "hour" )
    {
        result = QString::number( dom->elementHour() );
    }
    else if ( element == "minute" )
    {
        result = QString::number( dom->elementMinute() );
    }
    else if ( element == "second" )
    {
        result = QString::number( dom->elementSecond() );
    }
    else
    {
        fprintf( stderr, UNSUPPORTED_ELEMENT, "Time", element.toLocal8Bit().constData(), usage.toLocal8Bit().constData() );
        result = InputCommandParser::badResult( *this->Command );
    }

    return result;
}

/**
 * Fetches value of DateTime property
 */
QString PropertyElementParser::parseDateTime( const DomProperty *p ) const
{
    QString usage = USAGE +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "hour"   + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "minute" + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "second" + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "year"   + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "month"  + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "day"    + "\n";

    if ( this->CommandItemList->size() < 3 )
    {
        fprintf( stderr, NO_ELEMENT, "DateTime", usage.toLocal8Bit().constData() );
        return InputCommandParser::badResult( *this->Command );
    }

    QString result;
    QString element = this->CommandItemList->at( 2 ).toLower();
    DomDateTime *dom = p->elementDateTime();

    if ( element == "hour" )
    {
        result = QString::number( dom->elementHour() );
    }
    else if ( element == "minute" )
    {
        result = QString::number( dom->elementMinute() );
    }
    else if ( element == "second" )
    {
        result = QString::number( dom->elementSecond() );
    }
    else if ( element == "year" )
    {
        result = QString::number( dom->elementYear() );
    }
    else if ( element == "month" )
    {
        result = QString::number( dom->elementMonth() );
    }
    else if ( element == "day" )
    {
        result = QString::number( dom->elementDay() );
    }
    else
    {
        fprintf( stderr, UNSUPPORTED_ELEMENT, "DateTime", element.toLocal8Bit().constData(), usage.toLocal8Bit().constData() );
        result = InputCommandParser::badResult( *this->Command );
    }

    return result;
}

/**
 * Fetches value of PointF property
 */
QString PropertyElementParser::parsePointF( const DomProperty *p ) const
{
    QString usage = USAGE +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "x" + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "y" + "\n";

    if ( this->CommandItemList->size() < 3 )
    {
        fprintf( stderr, NO_ELEMENT, "PointF", usage.toLocal8Bit().constData() );
        return InputCommandParser::badResult( *this->Command );
    }

    QString result;
    QString element = this->CommandItemList->at( 2 ).toLower();
    DomPointF *dom = p->elementPointF();

    if ( element == "x" )
    {
        result = QString::number( dom->elementX() );
    }
    else if ( element == "y" )
    {
        result = QString::number( dom->elementY() );
    }
    else
    {
        fprintf( stderr, UNSUPPORTED_ELEMENT, "PointF", element.toLocal8Bit().constData(), usage.toLocal8Bit().constData() );
        result = InputCommandParser::badResult( *this->Command );
    }

    return result;
}

/**
 * Fetches value of RectF property
 */
QString PropertyElementParser::parseRectF( const DomProperty *p ) const
{
    QString usage = USAGE +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "x, " +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "y, " +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "width, " +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "height.";

    if ( this->CommandItemList->size() < 3 )
    {
        fprintf( stderr, NO_ELEMENT, "RectF", usage.toLocal8Bit().constData() );
        return InputCommandParser::badResult( *this->Command );
    }

    QString result;
    QString element = this->CommandItemList->at( 2 ).toLower();
    DomRectF *dom = p->elementRectF();

    if ( element == "x" )
    {
        result = QString::number( dom->elementX() );
    }
    else if ( element == "y" )
    {
        result = QString::number( dom->elementY() );
    }
    else if ( element == "width" )
    {
        result = QString::number( dom->elementWidth() );
    }
    else if ( element == "height" )
    {
        result = QString::number( dom->elementHeight() );
    }
    else
    {
        fprintf( stderr, UNSUPPORTED_ELEMENT, "RectF", element.toLocal8Bit().constData(), usage.toLocal8Bit().constData() );
        result = InputCommandParser::badResult( *this->Command );
    }

    return result;
}

/**
 * Fetches value of SizeF property
 */
QString PropertyElementParser::parseSizeF( const DomProperty *p ) const
{
    QString usage = USAGE +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "width"  + "\n" +
        *this->WidgetName + CommandElements::SEPARATOR + *this->PropertyName + CommandElements::SEPARATOR + "height" + "\n";

    if ( this->CommandItemList->size() < 3 )
    {
        fprintf( stderr, NO_ELEMENT, "SizeF", usage.toLocal8Bit().constData() );
        return InputCommandParser::badResult( *this->Command );
    }

    QString result;
    QString element = this->CommandItemList->at( 2 ).toLower();
    DomSizeF *dom = p->elementSizeF();

    if ( element == "width" )
    {
        result = QString::number( dom->elementWidth() );
    }
    else if ( element == "height" )
    {
        result = QString::number( dom->elementHeight() );
    }
    else
    {
        fprintf( stderr, UNSUPPORTED_ELEMENT, "SizeF", element.toLocal8Bit().constData(), usage.toLocal8Bit().constData() );
        result = InputCommandParser::badResult( *this->Command );
    }

    return result;
}

