/*****************************************************************************

  The following code is derived, directly or indirectly, from the SystemC
  source code Copyright (c) 1996-2014 by all Contributors.
  All Rights reserved.

  The contents of this file are subject to the restrictions and limitations
  set forth in the SystemC Open Source License (the "License");
  You may not use this file except in compliance with such restrictions and
  limitations. You may obtain instructions on how to receive a copy of the
  License at http://www.accellera.org/. Software distributed by Contributors
  under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
  ANY KIND, either express or implied. See the License for the specific
  language governing rights and limitations under the License.

 *****************************************************************************/

/*****************************************************************************

  sc_event_queue.h -- Event Queue Facility Definitions

  Original Author: Ulli Holtmann, Synopsys, Inc.

  CHANGE LOG IS AT THE END OF THE FILE
 *****************************************************************************/

#ifndef SC_EVENT_QUEUE_H
#define SC_EVENT_QUEUE_H


/*
  Class sc_event_queue

  A queue that can contain any number of pending notifications.
  The queue has a similiar interface like an sc_event but has different
  semantics: it can carry any number of pending notification. The
  general rule is that _every_ call to notify() will cause a 
  corresponding trigger at the specified wall-clock time that can be
  observed (the only exception is when notifications are explicitly
  cancelled). 

  If multiple notifications are pending at the same wall-clock
  time, then the event queue will trigger in different delta cycles
  in order to ensure that sensitive processes can notice each
  trigger. The first trigger happens in the earliest delta cycle
  possible which is the same behavior as a normal timed event.
  
*/

#include "sysc/communication/sc_interface.h"
#include "sysc/kernel/sc_module.h"
#include "sysc/kernel/sc_event.h"
#include "sysc/communication/sc_port.h"

namespace sc_core {


/**************************************************************************//**
 *  \class sc_event_queue_if
 *
 *  \brief The sc_event_queue interface class.
 *****************************************************************************/

class sc_event_queue_if : public virtual sc_interface
{
public:
    virtual void notify (double when, sc_time_unit base) =0;
    virtual void notify (const sc_time& when) =0;
    virtual void cancel_all() =0;
};

/**************************************************************************//**
 *  \class sc_event_queue
 * 
 *  \brief A queue that can contain any number of pending delta, or timed
 *         events.
 *****************************************************************************/

class sc_event_queue: 
  public sc_event_queue_if,
  public sc_module
{
 public:

    SC_HAS_PROCESS( sc_event_queue );

    /**
     *  \brief This constructor function is not supported by the out-of-order 
     *         simulation in the current release.
     */
    // 08/20/2015 GL.
    sc_event_queue( sc_module_name name_ = sc_gen_unique_name("event_queue") );

    ~sc_event_queue();

    // API of sc_object
    inline virtual const char* kind() const { return "sc_event_queue"; }

    //
    // API of sc_event_queue_if
    //
    inline virtual void notify (double when, sc_time_unit base);
           virtual void notify (const sc_time& when);
           virtual void cancel_all();

    //
    // API for using the event queue in processes
    //

    // get the default event
    inline virtual const sc_event& default_event() const;

/*
    //
    // Possible extensions:
    //
    
    // Cancel an events at a specific time
    void cancel (const sc_time& when);
    void cancel (double when, sc_time_unit base);

    // How many events are pending altogether?
    unsigned pending() const;

    // How many events are pending at the specific time?
    unsigned pending(const sc_time& when) const;
    unsigned pending(double when, sc_time_unit base) const;
*/

 private:
    void fire_event();

 private:
    sc_ppq<sc_time*> m_ppq;
    sc_event m_e;
    sc_dt::uint64 m_change_stamp;
    unsigned m_pending_delta;
};

inline
void sc_event_queue::notify (double when, sc_time_unit base )
{
	notify( sc_time(when,base) );
}
    
inline
const sc_event& sc_event_queue::default_event() const
{ 
  return m_e; 
}


//
// Using event queue as a port
//
typedef sc_port<sc_event_queue_if,1,SC_ONE_OR_MORE_BOUND> sc_event_queue_port;

} // namespace sc_core

// $Log: sc_event_queue.h,v $
// Revision 1.5  2011/08/26 20:45:40  acg
//  Andy Goodrich: moved the modification log to the end of the file to
//  eliminate source line number skew when check-ins are done.
//
// Revision 1.4  2011/04/05 20:48:09  acg
//  Andy Goodrich: changes to make sure that event(), posedge() and negedge()
//  only return true if the clock has not moved.
//
// Revision 1.3  2011/02/18 20:23:45  acg
//  Andy Goodrich: Copyright update.
//
// Revision 1.2  2008/05/20 16:45:52  acg
//  Andy Goodrich: changed which unique name generator is used from the
//  global one to the one for sc_modules.
//
// Revision 1.1.1.1  2006/12/15 20:20:04  acg
// SystemC 2.3
//
// Revision 1.4  2006/11/28 20:30:48  acg
//  Andy Goodrich: updated from 2.2 source. sc_event_queue constructors
//  collapsed into a single constructor with an optional argument to get
//  the sc_module_name stack done correctly. Class name prefixing added
//  to sc_semaphore calls to wait() to keep gcc 4.x happy.
//
// Revision 1.3  2006/01/13 18:47:42  acg
// Added $Log command so that CVS comments are reproduced in the source.
//

#endif // SC_EVENT_QUEUE_H
