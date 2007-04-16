/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */


#include <iostream>
#include <QFileDialog>

#include <orcaqgui/ihumanmanager.h>
#include <orcaqgui/guiicons.h>
#include <orcaqgui2dfactory/wptolerancesdialog.h>

#include "pathfollower2delement.h"

using namespace std;
using namespace orca;
using namespace orcaqgui;

namespace orcaqgui2d {

////////////////////////////////////////////////////////////////////////////////

void
PathUpdateConsumer::setData( const orca::PathFollower2dData& newPath, const ::Ice::Current& )
{
    pathPipe_.set( newPath );
}

void
PathUpdateConsumer::setWaypointIndex( int index, const ::Ice::Current& )
{
    indexPipe_.set( index );
}

void PathUpdateConsumer::setActivationTime( const orca::Time& absoluteTime, double relativeTime, const ::Ice::Current& )
{
    cout << "PathFollower2d: got a new activation time. Not used, we rely on getData calls." << endl;
}

void PathUpdateConsumer::setEnabledState( bool enabledState, const ::Ice::Current& )
{
    cout << "PathFollower2d: enable state changed. Not used, we rely on getData calls." << endl;
}

////////////////////////////////////////////////////////////////////////////////

PathfollowerButtons::PathfollowerButtons( QObject *parent, orcaqgui::IHumanManager *humanManager, string proxyString)
    : humanManager_(humanManager)
{
    QPixmap openIcon(fileopen_xpm);
    QPixmap savePathIcon(filesave_path_xpm);
    QPixmap saveAsPathIcon(filesaveas_path_xpm);
    QPixmap goIcon(go_xpm);
    QPixmap stopIcon(stop_xpm);

    QPixmap waypointsIcon(waypoints_xpm);

    QPixmap sendIcon(send_xpm);
    QPixmap cancelIcon(cancel_xpm);

//     QAction* fileOpenPath = new QAction(openIcon, QString(proxyString.c_str()) + "\n" + "Open PathFollower Path File", this );
//     connect(fileOpenPath, SIGNAL(activated()), parent, SLOT(loadPathFromFile()));
    QAction* fileSavePathAs = new QAction(saveAsPathIcon, QString(proxyString.c_str()) + "\n" + "Save PathFollower Path As", this );
    connect(fileSavePathAs, SIGNAL(activated()), parent, SLOT(savePathAs()));
    QAction* fileSavePath = new QAction(savePathIcon, QString(proxyString.c_str()) + "\n" + "Save PathFollower Path", this );
    connect(fileSavePath, SIGNAL(activated()), parent, SLOT(savePath()));

    hiWaypoints_ = new QAction(waypointsIcon, QString(proxyString.c_str()) + "\n" + "&Pathfollower waypoints mode", this);
    hiWaypoints_->setCheckable(true);
    connect( hiWaypoints_,SIGNAL(activated()), parent, SLOT(waypointModeSelected()) );

    QAction* hiSend = new QAction(sendIcon,QString(proxyString.c_str()) + "\n" + "&Send PathFollower Path", this);
    connect( hiSend,SIGNAL(activated()), parent, SLOT(send()) );
    QAction* hiCancel = new QAction(cancelIcon, QString(proxyString.c_str()) + "\n" + "&Discard PathFollower Path", this);
    connect( hiCancel,SIGNAL(activated()), parent ,SLOT(cancel()) );

    QAction* hiGo = new QAction(goIcon, QString(proxyString.c_str()) + "\n" + "&PathFollower Go All Robots", this);
    connect( hiGo, SIGNAL(activated()), parent, SLOT(go()) ); 
    QAction* hiStop = new QAction(stopIcon, QString(proxyString.c_str()) + "\n" + "&PathFollower Stop All Robots", this);
    connect( hiStop, SIGNAL(activated()), parent, SLOT(stop()) ); 

//     humanManager->fileMenu()->addAction(fileOpenPath);
    humanManager->fileMenu()->addAction(fileSavePathAs);
    humanManager->fileMenu()->addAction(fileSavePath);

//     humanManager->toolBar()->addAction(fileOpenPath);
//     humanManager->toolBar()->addAction(fileSavePathAs);
//     humanManager->toolBar()->addAction(fileSavePath);

    humanManager->toolBar()->addAction( hiWaypoints_ );
    humanManager->toolBar()->addAction( hiSend );
    humanManager->toolBar()->addAction( hiCancel );
//     humanManager->toolBar()->addAction( hiStop );
//     humanManager->toolBar()->addAction( hiGo );
    
    humanManager->subscribeToKey( hiStop, QKeySequence(Qt::Key_Escape), this );
    humanManager->subscribeToKey( hiGo, QKeySequence(Qt::Key_F12), this );

    QAction *wpDialogAction = new QAction( QString(proxyString.c_str()) + "\n" + "&PathFollower Waypoint settings", this );
    connect( wpDialogAction, SIGNAL(activated()), parent, SLOT(waypointSettingsDialog()) );
    humanManager->optionsMenu()->addAction( wpDialogAction );

    QAction *sep = humanManager->toolBar()->addSeparator();
    sep->setParent( this );
}

PathfollowerButtons::~PathfollowerButtons() 
{
    humanManager_->unsubscribeFromKey( QKeySequence(Qt::Key_Escape), this ); 
    humanManager_->unsubscribeFromKey( QKeySequence(Qt::Key_F12), this ); 
}

void 
PathfollowerButtons::setWpButton( bool onOff )
{
    hiWaypoints_->setChecked( onOff ); 
}
/////////////////////////////////////////////////////

PathFollower2dElement::PathFollower2dElement( const orcaice::Context & context,
                                              const std::string &proxyString,
                                              orcaqgui::IHumanManager *humanManager)
    : doneInitialSetup_(false),
      proxyString_(proxyString),
      context_(context),
      humanManager_(humanManager),
      firstTime_(true),
      displayWaypoints_(true),
      displayPastWaypoints_(false),
      currentTransparency_(false),
      isRemoteInterfaceSick_(false),
      pathHI_( this,
               proxyString,
               humanManager,
               painter_,
               readWaypointSettings( context_.properties(), context_.tag() ),
               readActivateImmediately( context_.properties(), context_.tag() ),
               readDumpPath( context_.properties(), context_.tag() ) )
{
    cout<<"TRACE(pathfollower2delement.cpp): Instantiating w/ proxyString '" << proxyString << "'" << endl;
    
    painter_.initialize( displayWaypoints_, displayPastWaypoints_, currentTransparency_);
    
    pathUpdateConsumer_ = new PathUpdateConsumer;

    timer_ = new orcaice::Timer;
    activationTimer_ = new orcaice::Timer;
    activationTimer_->restart();
}

PathFollower2dElement::~PathFollower2dElement()
{
}

void
PathFollower2dElement::update()
{
    // if initial setup is not done yet (no connection established)
    // then try to connect every 5 seconds
    if ( !doneInitialSetup_ )
    {
        if (firstTime_) {
            doInitialSetup();
            timer_->restart();
            firstTime_=false;
        }
        if (timer_->elapsedSec()>5.0) {
            doInitialSetup();
            timer_->restart();
        }
    }
    
    if ( pathUpdateConsumer_->pathPipe_.isNewData() )
    {
        orca::PathFollower2dData newPath;
        pathUpdateConsumer_->pathPipe_.get( newPath );
        painter_.setData( newPath );
    }
    if ( pathUpdateConsumer_->indexPipe_.isNewData() )
    {
        int index;
        pathUpdateConsumer_->indexPipe_.get( index );
        painter_.setWpIndex( index );
    }
    
    // get the activation time
    bool isEnabled;
    int ret = isFollowerEnabled(isEnabled);
    if (ret!=0) return;
    
    if ( (activationTimer_->elapsedSec()>0.5) && isEnabled) 
    {
        try
        {
            double secondsSinceActivation;
            if (pathFollower2dPrx_->getRelativeActivationTime( secondsSinceActivation )) 
            {
                painter_.setRelativeStartTime( secondsSinceActivation );
            }
        }
        catch ( const orca::OrcaException &e )
        {
            stringstream ss;
            ss << e.what;
            humanManager_->showStatusMsg( Error, ss.str().c_str() );
        }
        catch ( const Ice::Exception &e )
        {
            stringstream ss;
            ss << "While trying to get activation time: " << endl << e;
            humanManager_->showStatusMsg( Error, ss.str().c_str() );
        }
        activationTimer_->restart();
    }
}

int 
PathFollower2dElement::isFollowerEnabled( bool &isEnabled )
{
    try
    {
        isEnabled = pathFollower2dPrx_->enabled();
        isRemoteInterfaceSick_ = false;
        return 0;
    }
    catch ( ... )
    {
        humanManager_->showStatusMsg( Error, "Problem when trying to get enabled state" );
        return -1;
    }
}

void 
PathFollower2dElement::setTransparency( bool useTransparency ) 
{ 
    cout << "TRACE(pathfollower2delement.cpp): setTransparency: " << useTransparency << endl;
    painter_.setTransparency( useTransparency ); 
    pathHI_.setTransparency( useTransparency );
    currentTransparency_ = useTransparency;
}

void
PathFollower2dElement::doInitialSetup()
{
    humanManager_->showStatusMsg(Information, "PathFollowerElement is trying to connect");
    
    // Here's what IceStormElement usually does for you if the GuiElement inherits from IceStormElement (see comments in .h file for more information)
    try 
    {
        orcaice::connectToInterfaceWithString( context_, pathFollower2dPrx_, proxyString_ );
        Ice::ObjectPtr pathFollowerObj = pathUpdateConsumer_;
        
        // subscribe
        callbackPrx_ = orcaice::createConsumerInterface<orca::PathFollower2dConsumerPrx>( context_, pathFollowerObj );
        pathFollower2dPrx_->subscribe(callbackPrx_);
    }
    catch ( ... )
    {
        humanManager_->showStatusMsg(Warning, "Problem connecting to pathfollower interface. Will try again later.");
        return;
    }
    humanManager_->showStatusMsg(Information, "Connected to pathfollower interface successfully.");
    
    doneInitialSetup_ = true;
    
    try
    {        
        // get initial path and set pipe
        PathFollower2dData data = pathFollower2dPrx_->getData();
        pathUpdateConsumer_->pathPipe_.set( data );
                
        // get initial waypoint in focus and set pipe
        int wpIndex = pathFollower2dPrx_->getWaypointIndex();
        pathUpdateConsumer_->indexPipe_.set( wpIndex );
    }
    catch ( ... )
    {
        humanManager_->showStatusMsg(Warning, "PathFollower2d: Problem getting initial data.");
    }
    
}

QStringList
PathFollower2dElement::contextMenu()
{
    QStringList s;
    if (displayWaypoints_) {
        s << "Switch all waypoints OFF";
    } else {
        s << "Switch all waypoints ON";
    }
    if (displayPastWaypoints_) {
        s << "Switch past waypoints OFF";
    } else {
        s << "Switch past waypoints ON";
    }
    if (currentTransparency_) {
        s << "Switch transparency OFF";
    } else {
        s << "Switch transparency ON";
    }
    bool isEnabled;
    int ret = isFollowerEnabled(isEnabled);
    if (ret!=0) {
        isRemoteInterfaceSick_=true;
        s << "";
    } else {
        if (isEnabled) {
            s << "Disable interface";
        } else {
            s << "Enable interface";
        }
    }
    
    s << "Save path as..."
      << "Save path";
    return s;
}

void 
PathFollower2dElement::execute( int action )
{
    cout<<"TRACE(pathfollower2delement.cpp): execute: " << action << endl;
    if ( action == 0 )
    {
        displayWaypoints_ = !displayWaypoints_;
        painter_.toggleDisplayWaypoints();    
    }
    else if ( action == 1 )
    {
        displayPastWaypoints_ = !displayPastWaypoints_;
        painter_.togglePastWaypoints();
    }
    else if ( action == 2 )
    {
        setTransparency(!currentTransparency_);
    }
    else if ( action == 3 )
    {
        if (isRemoteInterfaceSick_) return;
        pathFollower2dPrx_->setEnabled( !pathFollower2dPrx_->enabled() );
        bool isEnabled;
        isFollowerEnabled( isEnabled );
        QString str;
        if (isEnabled) {
            str = "Pathfollower reports it is ENABLED now.";
        } else {
            str = "Pathfollower reports it is DISABLED now.";
        }
        humanManager_->showStatusMsg(Information,str);
    }
    else if ( action == 4 )
    {
        pathHI_.savePathAs();
    }
    else if ( action == 5 )
    {
        pathHI_.savePath();
    }
    else
    {
        assert( false && "PathFollower2dElement::execute(): bad action" );
    }
}

void 
PathFollower2dElement::go()
{
    cout<<"TRACE(PathFollower2dElement): go()" << endl;
    humanManager_->showStatusMsg(Information,"Received GO signal");
    try
    {
        pathFollower2dPrx_->activateNow();
    }
    catch ( const Ice::Exception &e )
    {
        stringstream ss;
        ss << "While trying to activate pathfollower: " << endl << e;
        humanManager_->showStatusMsg(Error,QString(ss.str().c_str()));
    }
}

void 
PathFollower2dElement::stop()
{
    cout<<"TRACE(PathFollower2dElement): stop()" << endl;
    humanManager_->showStatusMsg(Information,"Received STOP signal");
    PathFollower2dData dummyPath;
    const bool activateNow = true;
    try
    {
        pathFollower2dPrx_->setData( dummyPath, activateNow );
    }
    catch ( const Ice::Exception &e )
    {
        stringstream ss;
        ss << "While trying to set (empty) pathfollower data: " << endl << e;
        humanManager_->showBoxMsg(Error,QString(ss.str().c_str()));
    }
}

void 
PathFollower2dElement::sendPath( const PathFollowerInput &pathInput, bool activateImmediately )
{
    cout<<"TRACE(PathFollower2dElement): sendPath()" << endl;
    
    try
    {
        // it's possible that we were desubscribed before, let's resubscribe to make sure
        pathFollower2dPrx_->subscribe(callbackPrx_);

        orca::PathFollower2dData data;
        bool isOk = pathInput.getPath( data );
        if (isOk) {
            pathFollower2dPrx_->setData( data, activateImmediately );
        } else {
            humanManager_->showStatusMsg( Warning, "No path to send!" );
            return;
        }
        if (!activateImmediately) 
            humanManager_->showStatusMsg( Information, "Path needs to be activated by pressing the Go button." );
    }
    catch ( const orca::OrcaException &e )
    {
        stringstream ss;
        ss << e.what;
        humanManager_->showBoxMsg( Error, ss.str().c_str() );
    }
    catch ( const Ice::Exception &e )
    {
        stringstream ss;
        ss << "While trying to set pathfollowing data: " << endl << e;
        humanManager_->showStatusMsg( Error, ss.str().c_str() );
    }
    
}

void 
PathFollower2dElement::paint( QPainter *p, int z )
{   
    painter_.paint( p, z );
    pathHI_.paint( p );
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

PathFollowerHI::PathFollowerHI( PathFollower2dElement *pfElement,
                                string proxyString,
                                orcaqgui::IHumanManager *humanManager, 
                                PathPainter &painter,
                                WaypointSettings wpSettings,
                                bool activateImmediately,
                                QString dumpPath )
    : pfElement_(pfElement),
      proxyString_( proxyString ),
      humanManager_(humanManager),
      painter_(painter),
      pathFileName_("/tmp"),
      pathFileSet_(false),
      wpSettings_(wpSettings),
      activateImmediately_(activateImmediately),
      pathInput_(NULL),
      gotMode_(false),
      dumpPath_(dumpPath),
      numPathDumps_(0),
      lastSavedPathFile_("")
{
    buttons_ = new PathfollowerButtons( this, humanManager, proxyString );
}

PathFollowerHI::~PathFollowerHI()
{
    if ( pathInput_ ) delete pathInput_;
    if ( buttons_ ) delete buttons_;
}

void 
PathFollowerHI::setFocus( bool inFocus )
{
    if (inFocus) {
        if (buttons_==0) {
            buttons_ = new PathfollowerButtons( this, humanManager_, proxyString_ );
        }
    } else {
        delete buttons_;
        buttons_=0;
    }
}

void 
PathFollowerHI::paint( QPainter *p )
{
    if ( pathInput_ ) 
    {
        pathInput_->paint(p);
    }
}

void PathFollowerHI::waypointSettingsDialog()
{
    QDialog *myDialog = new QDialog;
    Ui::WpTolerancesDialog ui;
    ui.setupUi(myDialog);
    
    int index = ui.spacingPropertyCombo->findText( QString(wpSettings_.spacingProperty.c_str()) );
    ui.spacingPropertyCombo->setCurrentIndex( index );
    ui.spacingValueSpin->setValue( wpSettings_.spacingValue );
    ui.distanceSpin->setValue( wpSettings_.distanceTolerance );
    ui.headingSpin->setValue( wpSettings_.headingTolerance );
    ui.maxSpeedSpin->setValue( wpSettings_.maxApproachSpeed );
    ui.maxTurnrateSpin->setValue( wpSettings_.maxApproachTurnrate );
    
    int ret = myDialog->exec();
    if (ret==QDialog::Rejected) return;
    
    wpSettings_.spacingProperty = ui.spacingPropertyCombo->currentText().toStdString();
    wpSettings_.spacingValue = ui.spacingValueSpin->value();
    wpSettings_.distanceTolerance = ui.distanceSpin->value(); 
    wpSettings_.headingTolerance = ui.headingSpin->value();
    wpSettings_.maxApproachSpeed = ui.maxSpeedSpin->value();
    wpSettings_.maxApproachTurnrate = ui.maxTurnrateSpin->value();

    if (pathInput_!=NULL)
        pathInput_->updateWpSettings( &wpSettings_ );
}

void 
PathFollowerHI::waypointModeSelected()
{
    if ( gotMode_ ) return;
    gotMode_ = humanManager_->requestMode( pfElement_ );
    
    if ( !gotMode_ )
    {
        humanManager_->showBoxMsg( Warning, "Couldn't take over the mode for PathFollower waypoints!" );
        return;
    }

    pathInput_ = new PathFollowerInput( this, &wpSettings_, humanManager_, lastSavedPathFile_ );
    pathInput_->setTransparency( useTransparency_ );
    buttons_->setWpButton( true );    
}

void
PathFollowerHI::setTransparency( bool useTransparency )
{ 
    useTransparency_ = useTransparency;
    if (pathInput_) 
        pathInput_->setTransparency( useTransparency ); 
}

void 
PathFollowerHI::send()
{
    cout<<"TRACE(PathFollowerHI): send()" << endl;
    
    if (pathInput_==NULL) {
        humanManager_->showBoxMsg( Warning, "Not in path input mode!" );
        return;
    }
        
    // save path to file automatically
    char buffer [5];
    sprintf(buffer,"%05d",numPathDumps_++);
    QString filename = dumpPath_ + "/pathdump" + QString(buffer) + ".txt";
    pathInput_->savePath( filename );
    lastSavedPathFile_ = filename;
    
    pfElement_->sendPath( *pathInput_, activateImmediately_ );
    
    cancel();
}

void 
PathFollowerHI::cancel()
{
    cout<<"TRACE(PathFollowerHI): cancel()" << endl;
    if ( gotMode_ )
    {
        humanManager_->relinquishMode( pfElement_ );
        lostMode();
    }
}

void 
PathFollowerHI::go()
{
    cout<<"TRACE(PathFollowerHI): go()" << endl;
    pfElement_->go();
}

void 
PathFollowerHI::stop()
{
    cout<<"TRACE(PathFollowerHI): stop()" << endl;
    pfElement_->stop();
}

void 
PathFollowerHI::allGo()
{
    cout<<"TRACE(PathFollowerHI): allGo()" << endl;
    const QList<GuiElement*> elements = humanManager_->guiElementModel().elements();
    for ( int i=0; i < elements.size(); i++ )
    {
        PathFollower2dElement *typedElem = dynamic_cast<PathFollower2dElement *>(elements[i]);
        if ( typedElem != NULL )
        {
            typedElem->go();
        }
    }
}
void 
PathFollowerHI::allStop()
{
    cout<<"TRACE(PathFollowerHI): allStop()" << endl;
    const QList<GuiElement*> elements = humanManager_->guiElementModel().elements();
    for ( int i=0; i < elements.size(); i++ )
    {
        PathFollower2dElement *typedElem = dynamic_cast<PathFollower2dElement *>(elements[i]);
        if ( typedElem != NULL )
        {
            typedElem->stop();
        }
    }
}

void
PathFollowerHI::lostMode()
{
    assert( pathInput_ != NULL );
    delete pathInput_;
    pathInput_ = NULL;
    buttons_->setWpButton( false );
    gotMode_ = false;
}

void 
PathFollowerHI::savePathAs()
{
    QString fileName = QFileDialog::getSaveFileName(
            0,
            "Choose a filename to save under",
            pathFileName_,
            "*.txt");
    
    if (!fileName.isEmpty())
    {
        painter_.savePath( fileName, humanManager_ );
        pathFileName_ = fileName;
        pathFileSet_ = true;
    }
}

void 
PathFollowerHI::savePath()
{
    if (!pathFileSet_)
    {   
        savePathAs();
    }
    else
    {
        painter_.savePath( pathFileName_, humanManager_ );
    }
}



}
