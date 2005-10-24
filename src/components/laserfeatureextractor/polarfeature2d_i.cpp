/*
 *  Orca-Base: Components for robotics.
 *
 *  Copyright (C) 2004
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "polarfeature2d_i.h"
#include <iostream>

using namespace std;
using namespace orca;

PolarFeature2dI::PolarFeature2dI( orcaiceutil::PtrBuffer<orca::PolarFeature2dSequenceDataPtr>   &polarFeatureDataBuffer ) 
    : polarFeatureDataBuffer_( polarFeatureDataBuffer )
{

}


// served out the data to the client (it was stored here by the driver at the last read)
orca::PolarFeature2dSequenceDataPtr 
PolarFeature2dI::getData(const Ice::Current& current) const
{
    std::cout << "Sending data back" << std::endl;

    // we don't need to pop the data here because we don't block on it.
    if ( polarFeatureDataBuffer_.isEmpty() )
    {
        //! @todo what should happen if there's no data?
        cout << "ERROR(laser_i.cpp): getData() called when no data had been generated!!" << endl;
        cout << "ERROR(laser_i.cpp): Don't know what to do!" << endl;
        exit(EXIT_FAILURE);
    }

    // create a null pointer. data will be cloned into it.
    orca::PolarFeature2dSequenceDataPtr data;
    polarFeatureDataBuffer_.get( data );

    return data;
}
