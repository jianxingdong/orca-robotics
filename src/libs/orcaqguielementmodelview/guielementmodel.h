/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAQGUI_GUI_ELEMENT_MODEL_H
#define ORCA_ORCAQGUI_GUI_ELEMENT_MODEL_H

#include <QAbstractTableModel>
#include <QStringList>
#include <QColor>

#include <hydroqgui/iguielementfactory.h>
#include <hydroqgui/platformfocusmanager.h>
#include <hydroqgui/platformfocusmanager.h>
#include <hydroqgui/coordinateframemanager.h>
#include <hydroqgui/platformfocusmanager.h>
#include <hydroqgui/guielementset.h>
#include <hydroqgui/istringtocolormap.h>

namespace orcaqgemv
{

class GuiElementView;

//!
//! @brief Stores the set of GuiElements
//!
class GuiElementModel : public QAbstractTableModel,
                        public hydroqgui::PlatformFocusChangeReceiver
{
    Q_OBJECT

public:

    // AlexB: TODO: What is this gear for?
    enum Roles {
        InterfaceIdRole = Qt::UserRole + 1,
        ContextMenuRole,
        FocusRole
    };

    explicit GuiElementModel( const std::vector<hydroqgui::IGuiElementFactory*> &factories,
                              hydroqguielementutil::IHumanManager               &humanManager,
                              hydroqguielementutil::MouseEventManager           &mouseEventManager,
                              hydroqguielementutil::ShortcutKeyManager          &shortcutKeyManager,
                              hydroqgui::CoordinateFrameManager                 &coordinateFrameManager,
                              hydroqgui::PlatformFocusManager                   &platformFocusManager,
                              hydroqgui::GuiElementSet                          &guiElementSet,
                              hydroqgui::IStringToColorMap                      &platformColorMap,
                              QSplitter                                         *spaceBottomRight = 0,
                              QObject                                           *parent = 0);

    //
    // STANDARD MODEL API
    //
    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual bool removeRows( int row, int count, const QModelIndex & parent = QModelIndex() );

    //
    // CUSTOM API
    //
    void setView( GuiElementView* view );
    void updateGuiElements();
    void executeGuiElement( int guiElementIndex, int actionIndex );
    
    // sets the coordinate frame to the guielement's platform
    void setCoordinateFramePlatform( int guiElementIndex );
    void setOriginPlatform( int guiElementIndex );
    
    void setGuiElementColor( int guiElementIndex, const QColor &color );

    // Obtains indices of selected adapters from the view
    void selectedAdaptersInView( std::vector<int> &indices );

    // Creates the element from the factories.
    // Returns the created element, or NULL is none was created
    hydroqguielementutil::GuiElement* createGuiElement( const QString &elementType,
                                                         const QString &elementDescription,
                                                         const QString &platformName,
                                                         const QString &uniqueId );

    // Adds an element which has been created externally.
    void addGuiElement( hydroqguielementutil::GuiElement *element );
    
    void removeAndDeleteGuiElement( hydroqguielementutil::GuiElement *guiElement );

    // Inherited from PlatformFocusChangeReceiver
    void platformFocusChanged( const QString &newPlatformName );

    QColor platformColor( const QString &platformName )
        { return platformColorMap_.getColor(platformName); }

    bool isElementRemovable( int guiElementIndex );

    // const access to entire gui element set
    const hydroqgui::GuiElementSet &guiElementSet() const { return guiElementSet_; }


signals:
    void newPlatform( const QString& );
    void platformNeedsRemoval( const QString& );

public slots:
    void removeAllGuiElements();
    void createGuiElementFromSelection( const QList<QStringList> & );

private:
    
    // returns true if supported by a factory otherwise false
    bool instantiateFromFactories( hydroqguielementutil::GuiElement*         &element,
                                   const hydroqguielementutil::GuiElementInfo &guiElementInfo );

    hydroqgui::GuiElementSet &guiElementSet_;

    const QList<hydroqguielementutil::GuiElement*> &elements() const { return guiElementSet_.elements(); }
    
    const std::vector<hydroqgui::IGuiElementFactory*> factories_;
    QStringList headers_;
    hydroqguielementutil::IHumanManager      &humanManager_;
    hydroqguielementutil::MouseEventManager  &mouseEventManager_;
    hydroqguielementutil::ShortcutKeyManager &shortcutKeyManager_;
    hydroqgui::CoordinateFrameManager &coordinateFrameManager_;
    hydroqgui::PlatformFocusManager &platformFocusManager_;
    
    bool doesPlatformExist( const QString &platformName );
    bool doesElementExist( const QString& uniqueId );
    
    hydroqgui::IStringToColorMap &platformColorMap_;
    
    GuiElementView* view_;
    
    QString lookupTypeFromFactories( QStringList &ids );
};


}
#endif
