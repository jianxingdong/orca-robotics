/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_SEGWAY_RMP_PLAYERCLIENT_DRIVER_CONFIG_H
#define ORCA2_SEGWAY_RMP_PLAYERCLIENT_DRIVER_CONFIG_H

#include <string>
#include <orcaice/context.h>

namespace segwayrmp
{

//
// Convenience structure to hold all the config parameters.
// All units are S.I.
//
class PlayerClientDriverConfig 
{
public:

    PlayerClientDriverConfig();

    const char *host;
    int port;

    // returns 0 if everything looks good.
    int checkSanity( std::string &warnings, std::string &errors );
};

std::ostream &operator<<( std::ostream & s, const PlayerClientDriverConfig & c );

void readFromProperties( const orcaice::Context & context, PlayerClientDriverConfig & c );

} // namespace

#endif
