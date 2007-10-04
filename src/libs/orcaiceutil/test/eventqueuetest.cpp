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

#include <IceUtil/Time.h>

#include <orcaiceutil/eventqueue.h>
#include <orcaiceutil/thread.h>
#include <orcaiceutil/mathdefs.h>

using namespace std;

class TestEvent : public orcaiceutil::Event
{
public:
    TestEvent( double d ) :
        Event( 99 ),
        d_(d) {};

    double d_;
};
typedef IceUtil::Handle<TestEvent> TestEventPtr;

class TestEventQueueOptimizer : public orcaiceutil::EventQueueOptimizer
{
public:
    // this combine function adds the member variables of the two events
    virtual bool combine( orcaiceutil::EventPtr& existing, const orcaiceutil::EventPtr& extra ) 
    { 
        TestEventPtr eExisting = TestEventPtr::dynamicCast( existing );
        TestEventPtr eExtra = TestEventPtr::dynamicCast( extra );
        if ( !eExisting || !eExtra ) {
            return false; 
        }

        eExisting->d_ += eExtra->d_;
        return true;
    };
};

class ExtCaller : public orcaiceutil::Thread
{
public:
    ExtCaller( orcaiceutil::EventQueuePtr q ) :
        q_(q) {};

    // from thread
    virtual void run()
    {
        while ( !isStopping() )
        {
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
//             cout<<"thread ok"<<endl;
        }
    };

    void callMeBack()
    {
        orcaiceutil::EventPtr e = new TestEvent( 1109 );
        q_->add( e );
    };
private:
    orcaiceutil::EventQueuePtr q_;
};

int main(int argc, char * argv[])
{
    cout<<"testing size() with empty queue... ";
    orcaiceutil::EventQueuePtr q = new orcaiceutil::EventQueue;
    if ( q->size() != 0 ) {
        cout<<"failed"<<endl<<"expect size=0, size="<<q->size()<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;
    
    cout<<"testing size()... ";
    orcaiceutil::EventPtr e = new TestEvent( 1969.0 );
    q->add( e );
    q->add( e );
    if ( q->size() != 2 ) {
        cout<<"failed"<<endl<<"expect size=2, size="<<q->size()<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;

    cout<<"testing clear()... ";
    q->clear();
    if ( q->size() != 0 ) {
        cout<<"failed"<<endl<<"expect size=0, size="<<q->size()<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;

    cout<<"testing get() with non-empty queue ... ";
    q->add( e );
    q->add( e );
    orcaiceutil::EventPtr eout;
    // if something is wrong here, it will block forever
    q->get( eout );
    if ( eout->type() != 99 ) {
        cout<<"failed"<<endl<<"expect type=99, got="<<eout->type()<<endl;
        exit(EXIT_FAILURE);
    }
    TestEventPtr teout = TestEventPtr::dynamicCast( eout );
    if ( teout->d_ != 1969.0 ) {
        cout<<"failed"<<endl<<"expect value=1969, got="<<teout->d_<<endl;
        exit(EXIT_FAILURE);
    }
    if ( q->size() != 1 ) {
        cout<<"failed"<<endl<<"expect size=1, size="<<q->size()<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;

    cout<<"testing timedGet() with non-empty queue ... ";
    int ret = q->timedGet( eout, 50 );
    if ( !ret ) {
        cout<<"failed"<<endl<<"expect result=1, got="<<(int)ret<<endl;
        exit(EXIT_FAILURE);
    }
    if ( q->size() != 0 ) {
        cout<<"failed"<<endl<<"expect size=0, size="<<q->size()<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;

    cout<<"testing timeout result of timedGet() with empty queue ... ";
    ret = q->timedGet( eout, 50 );
    if ( ret ) {
        cout<<"failed"<<endl<<"expect result=0, got="<<(int)ret<<endl;
        exit(EXIT_FAILURE);
    }
    if ( q->size() != 0 ) {
        cout<<"failed"<<endl<<"expect size=0, size="<<q->size()<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;

    cout<<"testing optimizedAdd() with empty queue... ";
    e = new TestEvent( 1.0 );
    q->optimizedAdd( e );
    if ( q->size() != 1 ) {
        cout<<"failed"<<endl<<"expect size=1, size="<<q->size()<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;

    cout<<"testing optimizedAdd() with non-empty queue and no optimizer ... ";
    e = new TestEvent( 31.0 );
    q->optimizedAdd( e );
    if ( q->size() != 2 ) {
        cout<<"failed"<<endl<<"expect size=1, size="<<q->size()<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;

    cout<<"testing optimizedAdd() with non-empty queue ... ";
    q->get( eout );
    orcaiceutil::EventQueueOptimizerPtr opt = new TestEventQueueOptimizer;
    q->setOptimizer( opt );
    e = new TestEvent( 37.0 );
    q->optimizedAdd( e );
    if ( q->size() != 1 ) {
        cout<<"failed"<<endl<<"expect size=1, size="<<q->size()<<endl;
        exit(EXIT_FAILURE);
    }
    q->get( eout );
    teout = TestEventPtr::dynamicCast( eout );
    if ( !NEAR( teout->d_, 68.0, 1E-6 ) ) {
        cout<<"failed"<<endl<<"expect value=68.0, got="<<teout->d_<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;

    cout<<"testing add() and get() and add() ... ";
    q->clear();
    e = new TestEvent( 2006.0 );
    q->add( e );

    // if something is wrong here, it will block forever
    q->timedGet( eout, 10 );

    e = new TestEvent( 2007.0 );
    q->add( e );
    if ( q->size() != 1 ) {
        cout<<"failed"<<endl<<"expect size=1, size="<<q->size()<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;


    cout<<"testing external caller ... ";
    ExtCaller* extCaller = new ExtCaller( q );
    extCaller->start();
    extCaller->callMeBack();
    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    extCaller->callMeBack();
    if ( q->size() != 3 ) {
        cout<<"failed"<<endl<<"expect size=1, size="<<q->size()<<endl;
        exit(EXIT_FAILURE);
    }
    orcaiceutil::stopAndJoin( extCaller );
    cout<<"ok"<<endl;

    // TODO: test get and timedGet() with empty queue. requires another thread
    
    return EXIT_SUCCESS;
}
