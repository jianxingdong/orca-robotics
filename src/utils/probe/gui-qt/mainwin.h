/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef QTDRIVER_MAIN_WINDOW_H
#define QTDRIVER_MAIN_WINDOW_H

#include <QMainWindow>

#include <orcaprobe/ibrowser.h>

class QListView;
class QTableView;
class QTreeView;
class QLabel;
class QMovie;
class QTimer;

namespace orcaqcm
{
class OcmModel;
}

namespace probe
{

class ProbeView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( orcaprobe::IBrowser *browser,
                orcaqcm::OcmModel        *model,
                const std::vector<std::string> & supportedInterfaces,
                QWidget *parent = 0, Qt::WFlags flags = 0 );

private slots:
    void reload();
    void top();
   
    void quit();
    void aboutOrca();
    void aboutApp();

private:

    virtual void keyPressEvent ( QKeyEvent * event );
    virtual void customEvent( QEvent* e );

    void setupMenuBar();

    orcaprobe::IBrowser *browser_;
    orcaqcm::OcmModel        *model_;
    std::vector<std::string>    supportedInterfaces_;
    
    ProbeView                *regView_;
    ProbeView                *platfView_;
    ProbeView                *compView_;
    ProbeView                *ifaceView_;
    ProbeView                *operView_;
    
    QLabel *pathLabel_;
    QMovie *movie_;
};

} // namespace

#endif
