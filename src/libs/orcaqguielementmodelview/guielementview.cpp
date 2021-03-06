/*
 * Orca-Robotics Project: Components for robotics
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <QStringList>
#include <QMouseEvent>
#include <QMenu>
#include <QColorDialog>
#include <QHeaderView>

#include "guielementview.h"
#include "guielementmodel.h"   // need to access custom API


using namespace std;

namespace orcaqgemv {

GuiElementView::GuiElementView( QWidget *parent )
    : QTableView(parent)
{
    setSelectionBehavior(QAbstractItemView::SelectRows);

    horizontalHeader()->setStretchLastSection( true );
}

void
GuiElementView::contextMenuEvent( QContextMenuEvent* e )
{
    QMenu menu(this);

    QStringList actions = currentIndex().data( GuiElementModel::ContextMenuRole ).toStringList();

    for ( int i=0; i<actions.size(); ++i ) {
        //QString dummyAction = "action"+QString::number(i);
        //menu.addAction( actions[i], this, dummyAction.toAscii() );
        QAction* a = menu.addAction( actions[i] );
        switch ( i )
        {
        case 0 :
            QObject::connect( a,SIGNAL(triggered()), this,SLOT(action0()) );
            break;
        case 1 :
            QObject::connect( a,SIGNAL(triggered()), this,SLOT(action1()) );
            break;
        case 2 :
            QObject::connect( a,SIGNAL(triggered()), this,SLOT(action2()) );
            break;
        case 3 :
            QObject::connect( a,SIGNAL(triggered()), this,SLOT(action3()) );
            break;
        case 4 :
            QObject::connect( a,SIGNAL(triggered()), this,SLOT(action4()) );
            break;
        case 5 :
            QObject::connect( a,SIGNAL(triggered()), this,SLOT(action5()) );
            break;
        case 6 :
            QObject::connect( a,SIGNAL(triggered()), this,SLOT(action6()) );
            break;
        case 7 :
            QObject::connect( a,SIGNAL(triggered()), this,SLOT(action7()) );
            break;
        case 8 :
            QObject::connect( a,SIGNAL(triggered()), this,SLOT(action8()) );
            break;
        case 9 :
            QObject::connect( a,SIGNAL(triggered()), this,SLOT(action9()) );
            break;
        }
    }

    menu.addSeparator();
    menu.addAction( "Set As Coordinate Frame", this, SLOT( setCooFrameToCurrentGuiElement() ) );
    menu.addAction( "Set As Origin", this, SLOT( setOriginToCurrentGuiElement() ) );
    menu.addAction( "Set Colour", this, SLOT( setColourOfCurrentGuiElement() ) );

    // check whether element is removable
    GuiElementModel* guiElementModel = dynamic_cast<GuiElementModel*>( model() );
    if ( guiElementModel->isElementRemovable( currentIndex().row() ) ) {
        menu.addAction( "Remove", this, SLOT( removeCurrentGuiElement() ) );
    }

    menu.exec( e->globalPos() );
}

void
GuiElementView::selectedAdaptersInView(int numElements, vector<int>& indeces)
{
    // make a shortlist of the adapters currently displayed
    vector<int> shortList;
    for (int i=0; i<numElements; i++)
    {
        if ( !isRowHidden(i) ) shortList.push_back(i);
    }

    // get all selected adapters
    QModelIndexList list = selectedIndexes();
    QVector<int> selectedRows;
    for (int i=0; i<list.size(); i++)
    {
        selectedRows.push_back( list[i].row() );
    }

    // check if any of the selected adapters match any of shortlisted ones
    for( unsigned int i=0; i<shortList.size(); i++ )
    {
        if ( selectedRows.contains(shortList[i]) )
        {
            indeces.push_back(shortList[i]);
        }
    }

    // if indeces is empty, send signal to *all* displayed items
    if (indeces.size()==0) {
        indeces = shortList;
    }
}

void
GuiElementView::showAllElements( int numElements )
{
    for (int i=0; i<numElements; i++) {
        showRow( i );
    }
}

void
GuiElementView::hideElements(int numElements, const vector<int>& indeces)
{
    showAllElements(numElements);

    for (unsigned int i=0; i<indeces.size(); i++) {
        hideRow( indeces[i] );
    }
}

void
GuiElementView::action( int index )
{
    // debug
    cout<<"GuiElementView::action: action #"<<index<<endl;

    GuiElementModel* guiElementModel = dynamic_cast<GuiElementModel*>( model() );
    guiElementModel->executeGuiElement( currentIndex().row(), index );
}

void
GuiElementView::setCooFrameToCurrentGuiElement()
{
    GuiElementModel* guiElementModel = dynamic_cast<GuiElementModel*>( model() );
    guiElementModel->setCoordinateFramePlatform( currentIndex().row() );
}

void
GuiElementView::setOriginToCurrentGuiElement()
{
    GuiElementModel* guiElementModel = dynamic_cast<GuiElementModel*>( model() );
    guiElementModel->setOriginPlatform( currentIndex().row() );
}

void
GuiElementView::setColourOfCurrentGuiElement()
{
    QColor color = QColorDialog::getColor();
    if ( color.isValid() )
    {
        GuiElementModel* guiElementModel = dynamic_cast<GuiElementModel*>( model() );
        guiElementModel->setGuiElementColor( currentIndex().row(), color );
    }
}

void
GuiElementView::removeCurrentGuiElement()
{
    model()->removeRows( currentIndex().row(), 1, QModelIndex() );
}

}

