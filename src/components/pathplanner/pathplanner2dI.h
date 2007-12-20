/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef I_H
#define I_H

#include <IceStorm/IceStorm.h>

#include <orca/pathplanner2d.h>
#include <hydroutil/store.h>

namespace pathplanner
{

class PathPlanner2dI : public orca::PathPlanner2d
{
public:
    PathPlanner2dI( 
        hydroutil::Store<orca::PathPlanner2dTask> &pathPlannerTaskProxy,
        hydroutil::Store<orca::PathPlanner2dData> &pathPlannerDataProxy,
        const orcaice::Context & context
    );

    // remote calls
    virtual ::Ice::Int setTask(const ::orca::PathPlanner2dTask&, const ::Ice::Current& = ::Ice::Current());

    virtual void subscribe(const ::orca::PathPlanner2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());
    virtual void unsubscribe(const ::orca::PathPlanner2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual ::orca::PathPlanner2dData getData(const ::Ice::Current& ) const;

    // local calls
    void localSetData( const ::orca::PathPlanner2dData& data );

private:

    hydroutil::Store<orca::PathPlanner2dTask>& pathPlannerTaskStore_;

    // the driver puts the latest computed path into here
    hydroutil::Store<orca::PathPlanner2dData>& pathPlannerDataStore_;

    // The topic to which we'll publish
    IceStorm::TopicPrx topicPrx_;
    
    // The interface to which we'll publish
    orca::PathPlanner2dConsumerPrx  consumerPrx_;

    orcaice::Context context_;

};

} // namespace

#endif
