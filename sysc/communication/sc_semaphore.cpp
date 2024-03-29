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

  sc_semaphore.cpp -- The sc_semaphore primitive channel class.

  Original Author: Martin Janssen, Synopsys, Inc., 2001-05-21

  CHANGE LOG IS AT THE END OF THE FILE
 *****************************************************************************/

#include "sysc/communication/sc_communication_ids.h"
#include "sysc/communication/sc_semaphore.h"
#include "sysc/kernel/sc_simcontext.h"
#include "sysc/kernel/sc_wait.h"

namespace sc_core {

// ----------------------------------------------------------------------------
//  CLASS : sc_semaphore
//
//  The sc_semaphore primitive channel class.
// ----------------------------------------------------------------------------

// error reporting

void
sc_semaphore::report_error( const char* id, const char* add_msg ) const
{
    char msg[BUFSIZ];
    if( add_msg != 0 ) {
	std::sprintf( msg, "%s: semaphore '%s'", add_msg, name() );
    } else {
	std::sprintf( msg, "semaphore '%s'", name() );
    }
    SC_REPORT_ERROR( id, msg );
}


// constructors

sc_semaphore::sc_semaphore( int init_value_ )
: sc_object( sc_gen_unique_name( "semaphore" ) ),
  m_free( (std::string(SC_KERNEL_EVENT_PREFIX)+"_free_event").c_str() ),
  m_value( init_value_ )
{
    CHNL_MTX_INIT_( m_mutex ); // 02/10/2015 GL: initialize the channel lock
    if( m_value < 0 ) {
	report_error( SC_ID_INVALID_SEMAPHORE_VALUE_ );
    }
}

sc_semaphore::sc_semaphore( const char* name_, int init_value_ )
: sc_object( name_ ), 
  m_free( (std::string(SC_KERNEL_EVENT_PREFIX)+"_free_event").c_str() ),
  m_value( init_value_ )
{
    CHNL_MTX_INIT_( m_mutex ); // 02/10/2015 GL: initialize the channel lock
    if( m_value < 0 ) {
	report_error( SC_ID_INVALID_SEMAPHORE_VALUE_ );
    }
}


// interface methods

// lock (take) the semaphore, block if not available
// 08/19/2015 GL: modified for the OoO simulation

int
sc_semaphore::wait( int seg_id )
{
    // 02/24/2015 GL: acquire the channel lock
    chnl_scoped_lock lock( m_mutex );

    while( in_use() ) {
	sc_core::wait( m_free, seg_id, sc_get_curr_simcontext() ); 
    }
    -- m_value;
    return 0;
    // 02/24/2015 GL: return releases the lock
}


// lock (take) the semaphore, return -1 if not available

int
sc_semaphore::trywait()
{
    // 02/24/2015 GL: acquire the channel lock
    chnl_scoped_lock lock( m_mutex );

    if( in_use() ) {
	return -1;
    }
    -- m_value;
    return 0;
    // 02/24/2015 GL: return releases the lock
}


// unlock (give) the semaphore

int
sc_semaphore::post()
{
    // 02/16/2015 GL: acquire the channel lock
    //sc_get_current_process_b()->lock_and_push( &m_mutex );

    // 02/24/2015 GL: acquire the channel lock
    chnl_scoped_lock lock( m_mutex );

    ++m_value;
    m_free.notify();

    // 02/16/2015 GL: release the channel lock
    //sc_get_current_process_b()->pop_and_unlock( &m_mutex );

    return 0;
    // 02/24/2015 GL: return releases the lock
}

} // namespace sc_core

// $Log: sc_semaphore.cpp,v $
// Revision 1.5  2011/08/26 20:45:42  acg
//  Andy Goodrich: moved the modification log to the end of the file to
//  eliminate source line number skew when check-ins are done.
//
// Revision 1.4  2011/03/23 16:17:22  acg
//  Andy Goodrich: hide the sc_events that are kernel related.
//
// Revision 1.3  2011/02/18 20:23:45  acg
//  Andy Goodrich: Copyright update.
//
// Revision 1.2  2010/11/02 16:31:01  acg
//  Andy Goodrich: changed object derivation to use sc_object rather than
//  sc_prim_channel as the parent class.
//
// Revision 1.1.1.1  2006/12/15 20:20:04  acg
// SystemC 2.3
//
// Revision 1.6  2006/11/28 20:30:49  acg
//  Andy Goodrich: updated from 2.2 source. sc_event_queue constructors
//  collapsed into a single constructor with an optional argument to get
//  the sc_module_name stack done correctly. Class name prefixing added
//  to sc_semaphore calls to wait() to keep gcc 4.x happy.
//
// Revision 1.3  2006/01/13 18:47:42  acg
// Added $Log command so that CVS comments are reproduced in the source.
//

// Taf!
