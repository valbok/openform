/**
 * VaL::bOK <valbok@gmail.com>
 * Created on: <10-Jun-2009 11:00:54 VaL>
 *
 * COPYRIGHT NOTICE: Copyright (C) 2009 VaL::bOK
 * SOFTWARE LICENSE: GNU General Public License v2.0
 */

#ifndef WINDOWHANDLER_H
#define WINDOWHANDLER_H

#include <QtGui/QMainWindow>
#include <QtCore/QFile>

namespace OpenForm
{

/**
 * This class contains methods to control UI
 */
class WindowHandler: public QObject
{
    Q_OBJECT

    /**
     * Ui widget
     */
    QWidget *Widget;

    static void deleteWidget( QWidget *widget );
    void setWidget( QWidget *widget );
public:
    WindowHandler( QFile &file );
    virtual ~WindowHandler() { WindowHandler::deleteWidget( this->Widget ); }
    bool hasUi() const;
    void show() const;
    void parseUiFrom( QFile &file );
    void parseUiFrom( const QString &data );
    QWidget *getWidget() const { return this->Widget; }

public slots:
    void setUiFrom( const QString &data );
    void returnData( const QString &data ) const;
};

} // namespace OpenForm

#endif // WINDOWHANDLER_H
