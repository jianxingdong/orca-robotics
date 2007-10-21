/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TRACERMON_TERM_NCURSES_INPUT_HANDLER_H
#define ORCA2_TRACERMON_TERM_NCURSES_INPUT_HANDLER_H

#include <hydroutil/safethread.h>
#include "../user.h"
#include "../network.h"

namespace tracermon
{

class InputHandler : public hydroutil::SafeThread
{
public:

    InputHandler( tracermon::Network& network, tracermon::User& user );
    virtual ~InputHandler();

    // from Thread
    virtual void walk();
    
private:

    tracermon::Network&    network_;
    tracermon::User&       user_;
};

} // namespace

#endif
