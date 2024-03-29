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

  sc_prim_channel.h -- Abstract base class of all primitive channel classes.

  Original Author: Martin Janssen, Synopsys, Inc., 2001-05-21

 CHANGE LOG AT THE END OF THE FILE
 *****************************************************************************/

#ifndef SC_PRIM_CHANNEL_H
#define SC_PRIM_CHANNEL_H

#include "sysc/kernel/sc_object.h"
#include "sysc/kernel/sc_wait.h"
#include "sysc/kernel/sc_wait_cthread.h"

// 02/22/2015 GL: to include the definition of CHNL_MTX_TYPE_
#include "sysc/kernel/sc_process.h" 

namespace sc_core {


/**************************************************************************//**
 *  \struct chnl_scoped_lock
 *
 *  \brief The chnl_scoped_lock class to lock (and automatically release) a 
 *         mutex.
 *****************************************************************************/
// 02/22/2015 GL.

struct chnl_scoped_lock {
    /**
     *  \brief A reference to the channel lock.
     */ 
    CHNL_MTX_TYPE_& m_ref;

    /**
     *  \brief The constructor automatically acquires the channel lock.
     */
    explicit chnl_scoped_lock( CHNL_MTX_TYPE_& mtx ): m_ref( mtx )
    { 
        sc_process_b* cp = sc_get_current_process_b();
        if (cp) // not the root thread
            cp->lock_and_push( &m_ref ); 
    }

    /**
     *  \brief The destructor automatically releases the channel lock.
     */
    ~chnl_scoped_lock()
    {
        sc_process_b* cp = sc_get_current_process_b();
        if (cp) // not the root thread
            cp->pop_and_unlock( &m_ref ); 
    }
};

/**************************************************************************//**
 *  \class sc_prim_channel
 *
 *  \brief Abstract base class of all primitive channel classes.
 *****************************************************************************/

class sc_prim_channel
: public sc_object
{
    friend class sc_prim_channel_registry;

public:
    enum { list_end = 0xdb };
public:
    virtual const char* kind() const
        { return "sc_prim_channel"; }

    inline bool update_requested() 
    {
        // 02/25/2015 GL: add a lock to protect concurrent communication
        chnl_scoped_lock lock( m_mutex );

        return m_update_next_p != (sc_prim_channel*)list_end;
        // 02/25/2015 GL: return releases the lock
    }

    // request the update method to be executed during the update phase
    inline void request_update();

    // request the update method to be executed during the update phase
    // from a process external to the simulator.
    void async_request_update();

protected:

    // constructors
    sc_prim_channel();
    explicit sc_prim_channel( const char* );

    // destructor
    virtual ~sc_prim_channel();

    /**
     *  \brief The update method (does nothing by default).
     *
     *  This function is not supported by the out-of-order simulation in the
     *  current release.
     */
    // 09/21/2015 GL.
    virtual void update();

    // called by construction_done (does nothing by default)
    virtual void before_end_of_elaboration();

    // called by elaboration_done (does nothing by default)
    virtual void end_of_elaboration();

    // called by start_simulation (does nothing by default)
    virtual void start_of_simulation();

    // called by simulation_done (does nothing by default)
    virtual void end_of_simulation();

protected:

    // to avoid calling sc_get_curr_simcontext()

    // static sensitivity for SC_THREADs and SC_CTHREADs

    /**
     *  \brief A new parameter segment ID is added for the out-of-order 
     *         simulation.
     */ 
    // 08/19/2015 GL: modified for the OoO simulation
    void wait( int seg_id = -1)
        { sc_core::wait( seg_id, simcontext() ); }


    // dynamic sensitivity for SC_THREADs and SC_CTHREADs

    /**
     *  \brief A new parameter segment ID is added for the out-of-order 
     *         simulation.
     */ 
    // 08/19/2015 GL: modified for the OoO simulation
    void wait( const sc_event& e, int seg_id = -1)
        { sc_core::wait( e, seg_id, simcontext() ); }

    /**
     *  \brief A new parameter segment ID is added for the out-of-order 
     *         simulation.
     */ 
    // 08/19/2015 GL: modified for the OoO simulation
    void wait( const sc_event_or_list& el, int seg_id = -1 )
        { sc_core::wait( el, seg_id, simcontext() ); }

    /**
     *  \brief A new parameter segment ID is added for the out-of-order 
     *         simulation.
     */ 
    // 08/19/2015 GL: modified for the OoO simulation
    void wait( const sc_event_and_list& el, int seg_id = -1 )
        { sc_core::wait( el, seg_id, simcontext() ); }

    /**
     *  \brief A new parameter segment ID is added for the out-of-order 
     *         simulation.
     */ 
    // 08/19/2015 GL: modified for the OoO simulation
    void wait( const sc_time& t, int seg_id = -1 )
        { sc_core::wait( t, seg_id, simcontext() ); }

    /**
     *  \brief A new parameter segment ID is added for the out-of-order 
     *         simulation.
     */ 
    // 08/19/2015 GL: modified for the OoO simulation
    void wait( double v, sc_time_unit tu, int seg_id = -1 )
        { sc_core::wait( sc_time( v, tu, simcontext() ), seg_id, 
                         simcontext() ); }

    /**
     *  \brief A new parameter segment ID is added for the out-of-order 
     *         simulation.
     */ 
    // 08/19/2015 GL: modified for the OoO simulation
    void wait( const sc_time& t, const sc_event& e, int seg_id = -1 )
        { sc_core::wait( t, e, seg_id, simcontext() ); }

    /**
     *  \brief A new parameter segment ID is added for the out-of-order 
     *         simulation.
     */ 
    // 08/19/2015 GL: modified for the OoO simulation
    void wait( double v, sc_time_unit tu, const sc_event& e, int seg_id = -1 )
        { sc_core::wait( sc_time( v, tu, simcontext() ), e, seg_id, 
                         simcontext() ); }

    /**
     *  \brief A new parameter segment ID is added for the out-of-order 
     *         simulation.
     */ 
    // 08/19/2015 GL: modified for the OoO simulation
    void wait( const sc_time& t, const sc_event_or_list& el, int seg_id = -1 )
        { sc_core::wait( t, el, seg_id, simcontext() ); }

    /**
     *  \brief A new parameter segment ID is added for the out-of-order 
     *         simulation.
     */ 
    // 08/19/2015 GL: modified for the OoO simulation
    void wait( double v, sc_time_unit tu, const sc_event_or_list& el, 
               int seg_id = -1 )
        { sc_core::wait( sc_time( v, tu, simcontext() ), el, seg_id, 
                         simcontext() ); }

    /**
     *  \brief A new parameter segment ID is added for the out-of-order 
     *         simulation.
     */ 
    // 08/19/2015 GL: modified for the OoO simulation
    void wait( const sc_time& t, const sc_event_and_list& el, int seg_id = -1 )
        { sc_core::wait( t, el, seg_id, simcontext() ); }

    /**
     *  \brief A new parameter segment ID is added for the out-of-order 
     *         simulation.
     */ 
    // 08/19/2015 GL: modified for the OoO simulation
    void wait( double v, sc_time_unit tu, const sc_event_and_list& el, 
               int seg_id = -1 )
        { sc_core::wait( sc_time( v, tu, simcontext() ), el, seg_id, 
                         simcontext() ); }

    /**
     *  \brief A new parameter segment ID is added for the out-of-order 
     *         simulation.
     */ 
    // 08/19/2015 GL: modified for the OoO simulation
    void wait( int n, int seg_id = -1 )
        { sc_core::wait( n, seg_id, simcontext() ); }


    // static sensitivity for SC_METHODs

    /**
     *  \brief A new parameter segment ID is added for the out-of-order 
     *         simulation.
     */ 
    // 08/19/2015 GL: modified for the OoO simulation
    void next_trigger( )
        { sc_core::next_trigger( simcontext() ); }


    // dynamic sensitivity for SC_METHODs

    /**
     *  \brief A new parameter segment ID is added for the out-of-order 
     *         simulation.
     */ 
    // 08/19/2015 GL: modified for the OoO simulation
    void next_trigger( const sc_event& e )
        { sc_core::next_trigger( e, simcontext() ); }

    /**
     *  \brief A new parameter segment ID is added for the out-of-order 
     *         simulation.
     */ 
    // 08/19/2015 GL: modified for the OoO simulation
    void next_trigger( const sc_event_or_list& el )
        { sc_core::next_trigger( el, simcontext() ); }

    /**
     *  \brief A new parameter segment ID is added for the out-of-order 
     *         simulation.
     */ 
    // 08/19/2015 GL: modified for the OoO simulation
    void next_trigger( const sc_event_and_list& el )
        { sc_core::next_trigger( el, simcontext() ); }

    /**
     *  \brief A new parameter segment ID is added for the out-of-order 
     *         simulation.
     */ 
    // 08/19/2015 GL: modified for the OoO simulation
    void next_trigger( const sc_time& t )
        { sc_core::next_trigger( t, simcontext() ); }

    /**
     *  \brief A new parameter segment ID is added for the out-of-order 
     *         simulation.
     */ 
    // 08/19/2015 GL: modified for the OoO simulation
    void next_trigger( double v, sc_time_unit tu )
        {sc_core::next_trigger( sc_time( v, tu, simcontext() ), 
                                simcontext() );}

    /**
     *  \brief A new parameter segment ID is added for the out-of-order 
     *         simulation.
     */ 
    // 08/19/2015 GL: modified for the OoO simulation
    void next_trigger( const sc_time& t, const sc_event& e )
        { sc_core::next_trigger( t, e, simcontext() ); }

    /**
     *  \brief A new parameter segment ID is added for the out-of-order 
     *         simulation.
     */ 
    // 08/19/2015 GL: modified for the OoO simulation
    void next_trigger( double v, sc_time_unit tu, const sc_event& e )
        { sc_core::next_trigger( 
            sc_time( v, tu, simcontext() ), e, simcontext() ); }

    /**
     *  \brief A new parameter segment ID is added for the out-of-order 
     *         simulation.
     */ 
    // 08/19/2015 GL: modified for the OoO simulation
    void next_trigger( const sc_time& t, const sc_event_or_list& el )
        { sc_core::next_trigger( t, el, simcontext() ); }

    /**
     *  \brief A new parameter segment ID is added for the out-of-order 
     *         simulation.
     */ 
    // 08/19/2015 GL: modified for the OoO simulation
    void next_trigger( double v, sc_time_unit tu, const sc_event_or_list& el )
        { sc_core::next_trigger( 
            sc_time( v, tu, simcontext() ), el, simcontext() ); }

    /**
     *  \brief A new parameter segment ID is added for the out-of-order 
     *         simulation.
     */ 
    // 08/19/2015 GL: modified for the OoO simulation
    void next_trigger( const sc_time& t, const sc_event_and_list& el )
        { sc_core::next_trigger( t, el, simcontext() ); }

    /**
     *  \brief A new parameter segment ID is added for the out-of-order 
     *         simulation.
     */ 
    // 08/19/2015 GL: modified for the OoO simulation
    void next_trigger( double v, sc_time_unit tu, const sc_event_and_list& el )
        { sc_core::next_trigger( 
            sc_time( v, tu, simcontext() ), el, simcontext() ); }


    // for SC_METHODs and SC_THREADs and SC_CTHREADs

    bool timed_out()
        { return sc_core::timed_out( simcontext() ); }


#if 0 // @@@@####
    // delta count maintenance
    sc_dt::uint64 delta_count()
        { return simcontext()->m_delta_count; }
#endif

private:

    // called during the update phase of a delta cycle (if requested)
    void perform_update();

    // called when construction is done
    void construction_done();

    // called when elaboration is done
    void elaboration_done();

    // called before simulation starts
    void start_simulation();

    // called after simulation ends
    void simulation_done();

    // disabled
    sc_prim_channel( const sc_prim_channel& );
    sc_prim_channel& operator = ( const sc_prim_channel& );

private:

    sc_prim_channel_registry* m_registry;          // Update list manager.
    sc_prim_channel*          m_update_next_p;     // Next entry in update list.

protected:

    /**
     *  \brief A mutex to protect concurrent communication.
     */
    // 02/25/2015 GL.
    mutable CHNL_MTX_TYPE_ m_mutex;
};


/**************************************************************************//**
 *  \class sc_prim_channel_registry
 *
 *  \brief Registry for all primitive channels.
 *
 *  FOR INTERNAL USE ONLY!
 *****************************************************************************/

class sc_prim_channel_registry
{
    friend class sc_simcontext;

public:

    void insert( sc_prim_channel& );
    void remove( sc_prim_channel& );


    int size() const
        { return m_prim_channel_vec.size(); }

    inline void request_update( sc_prim_channel& );
    void async_request_update( sc_prim_channel& );

    bool pending_updates() const
    { 
        return m_update_list_p != (sc_prim_channel*)sc_prim_channel::list_end 
               || pending_async_updates();
    }   

    bool pending_async_updates() const;

private:

    // constructor
    explicit sc_prim_channel_registry( sc_simcontext& simc_ );

    // destructor
    ~sc_prim_channel_registry();

    // called during the update phase of a delta cycle
    void perform_update();

    // called when construction is done
    bool construction_done();

    // called when elaboration is done
    void elaboration_done();

    // called before simulation starts
    void start_simulation();

    // called after simulation ends
    void simulation_done();

    // disabled
    sc_prim_channel_registry();
    sc_prim_channel_registry( const sc_prim_channel_registry& );
    sc_prim_channel_registry& operator = ( const sc_prim_channel_registry& );

private:
    class async_update_list;   

    async_update_list*            m_async_update_list_p; // external updates.
    int                           m_construction_done;   // # of constructs.
    std::vector<sc_prim_channel*> m_prim_channel_vec;    // existing channels.
    sc_simcontext*                m_simc;                // simulator context.
    sc_prim_channel*              m_update_list_p;       // internal updates.

    /**
     *  \brief A mutex to protect concurrent requests.
     */
    // 02/25/2015 GL.
    CHNL_MTX_TYPE_ m_mutex;
};


// IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII

// ----------------------------------------------------------------------------
//  CLASS : sc_prim_channel_registry
//
//  Registry for all primitive channels.
//  FOR INTERNAL USE ONLY!
// ----------------------------------------------------------------------------

inline
void
sc_prim_channel_registry::request_update( sc_prim_channel& prim_channel_ )
{
    // 02/25/2015 GL: add a lock to protect concurrent requests from different 
    //                primitive channels
    chnl_scoped_lock lock( m_mutex );

    prim_channel_.m_update_next_p = m_update_list_p;
    m_update_list_p = &prim_channel_;
    // 02/25/2015 GL: return releases the lock
}

// ----------------------------------------------------------------------------
//  CLASS : sc_prim_channel
//
//  Abstract base class of all primitive channel classes.
// ----------------------------------------------------------------------------

// request the update method (to be executed during the update phase)

inline
void
sc_prim_channel::request_update()
{
    // 02/25/2015 GL: add a lock to protect concurrent communication
    chnl_scoped_lock lock( m_mutex );

    if( ! m_update_next_p ) {
	m_registry->request_update( *this );
    }
    // 02/25/2015 GL: return releases the lock
}

// request the update method from external to the simulator (to be executed 
// during the update phase)

// 02/25/2015 GL: assume this method is MT-safe and protected by 
//                async_update_list
inline
void
sc_prim_channel::async_request_update() 
{
    m_registry->async_request_update(*this);
}


// called during the update phase of a delta cycle (if requested)

inline
void
sc_prim_channel::perform_update()
{
    update();
    m_update_next_p = 0;
}


} // namespace sc_core


/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date: Andy Goodrich, Forte,
                               Bishnupriya Bhattacharya, Cadence Design Systems,
                               25 August, 2003
  Description of Modification: phase callbacks
    
 *****************************************************************************/
//$Log: sc_prim_channel.h,v $
//Revision 1.10  2011/08/26 21:38:32  acg
// Philipp A. Hartmann: removed unused switch m_construction_done.
//
//Revision 1.9  2011/08/07 19:08:01  acg
// Andy Goodrich: moved logs to end of file so line number synching works
// better between versions.
//
//Revision 1.8  2011/05/09 04:07:37  acg
// Philipp A. Hartmann:
//   (1) Restore hierarchy in all phase callbacks.
//   (2) Ensure calls to before_end_of_elaboration.
//
//Revision 1.7  2011/05/05 17:44:01  acg
// Philip A. Hartmann: change in the name of pending_async_updates.
//
//Revision 1.6  2011/04/19 15:03:48  acg
// Philipp A. Hartmann: remove ASYNC_UPDATE preprocessor check from header.
//
//Revision 1.5  2011/04/19 02:36:26  acg
// Philipp A. Hartmann: new aysnc_update and mutex support.
//
//Revision 1.4  2011/04/05 20:48:09  acg
// Andy Goodrich: changes to make sure that event(), posedge() and negedge()
// only return true if the clock has not moved.
//
//Revision 1.3  2011/02/18 20:23:45  acg
// Andy Goodrich: Copyright update.
//
//Revision 1.2  2011/01/20 16:52:15  acg
// Andy Goodrich: changes for IEEE 1666 2011.
//
//Revision 1.1.1.1  2006/12/15 20:20:04  acg
//SystemC 2.3
//
//Revision 1.3  2006/05/08 17:52:47  acg
// Andy Goodrich:
//   (1) added David Long's forward declarations for friend functions,
//       methods, and operators to keep the Microsoft compiler happy.
//   (2) Added delta_count() method to sc_prim_channel for use by
//       sc_signal so that the friend declaration in sc_simcontext.h
//	   can be for a non-templated class (i.e., sc_prim_channel.)
//
//Revision 1.2  2006/01/03 23:18:26  acg
//Changed copyright to include 2006.
//
//Revision 1.1.1.1  2005/12/19 23:16:43  acg
//First check in of SystemC 2.1 into its own archive.
//
//Revision 1.10  2005/07/30 03:44:11  acg
//Changes from 2.1.
//
//Revision 1.9  2005/06/10 22:43:55  acg
//Added CVS change log annotation.

#endif

// Taf!
