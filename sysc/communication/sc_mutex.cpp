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

  sc_mutex.cpp -- The sc_mutex primitive channel class.

  Original Author: Martin Janssen, Synopsys, Inc., 2001-05-21

  CHANGE LOG IS AT THE END OF THE FILE
 *****************************************************************************/

#include "sysc/communication/sc_mutex.h"
#include "sysc/kernel/sc_simcontext.h"

namespace sc_core {

// ----------------------------------------------------------------------------
//  CLASS : sc_mutex
//
//  The sc_mutex primitive channel class.
// ----------------------------------------------------------------------------

// constructors

sc_mutex::sc_mutex()
: sc_object( sc_gen_unique_name( "mutex" ) ),
  m_owner( 0 ),
  m_free( (std::string(SC_KERNEL_EVENT_PREFIX)+"_free_event").c_str() )
{
    CHNL_MTX_INIT_( m_mutex ); // 02/10/2015 GL: initialize the channel lock
}

sc_mutex::sc_mutex( const char* name_ )
: sc_object( name_ ),
  m_owner( 0 ),
  m_free( (std::string(SC_KERNEL_EVENT_PREFIX)+"_free_event").c_str() )
{
    CHNL_MTX_INIT_( m_mutex ); // 02/10/2015 GL: initialize the channel lock
}


// destructor

sc_mutex::~sc_mutex()
{
    CHNL_MTX_DESTROY_( m_mutex ); // 02/10/2015 GL: destroy the channel lock
}

// interface methods

// blocks until mutex could be locked
// 08/19/2015 GL: modified for the OoO simulation

int
sc_mutex::lock( int seg_id )
{
    // 02/24/2015 GL: acquire the channel lock
    chnl_scoped_lock lock( m_mutex );

    if ( m_owner == sc_get_current_process_b()) return 0;
    while( in_use() ) {
	sc_core::wait( m_free, seg_id, sc_get_curr_simcontext() );
    }
    m_owner = sc_get_current_process_b();
    return 0;
    // 02/24/2015 GL: return releases the lock
}


// returns -1 if mutex could not be locked

int
sc_mutex::trylock()
{
    // 02/24/2015 GL: acquire the channel lock
    chnl_scoped_lock lock( m_mutex );

    if ( m_owner == sc_get_current_process_b()) return 0;
    if( in_use() ) {
	return -1;
    }
    m_owner = sc_get_current_process_b();
    return 0;
    // 02/24/2015 GL: return releases the lock
}


// returns -1 if mutex was not locked by caller

int
sc_mutex::unlock()
{
    // 02/24/2015 GL: acquire the channel lock
    chnl_scoped_lock lock( m_mutex );

    if( m_owner != sc_get_current_process_b() ) {
	return -1;
    }
    m_owner = 0;
    m_free.notify();
    return 0;
    // 02/24/2015 GL: return releases the lock
}

} // namespace sc_core

// $Log: sc_mutex.cpp,v $
// Revision 1.7  2011/08/26 20:45:40  acg
//  Andy Goodrich: moved the modification log to the end of the file to
//  eliminate source line number skew when check-ins are done.
//
// Revision 1.6  2011/03/28 13:02:29  acg
//  Andy Goodrich: removed sc_event in sc_mutex class from the object
//  hierarchy since it is considered a "kernel" event.
//
// Revision 1.5  2011/02/18 20:23:45  acg
//  Andy Goodrich: Copyright update.
//
// Revision 1.4  2010/11/02 16:31:01  acg
//  Andy Goodrich: changed object derivation to use sc_object rather than
//  sc_prim_channel as the parent class.
//
// Revision 1.3  2008/11/13 15:29:46  acg
//  David C. Black, ESLX, Inc: lock & trylock now allow owner to apply
//  lock more than once without incident. Previous behavior locked up the
//  owning process.
//
// Revision 1.2  2008/05/20 16:46:18  acg
//  Andy Goodrich: added checks for multiple writers.
//
// Revision 1.1.1.1  2006/12/15 20:20:04  acg
// SystemC 2.3
//
// Revision 1.4  2006/03/21 00:00:27  acg
//   Andy Goodrich: changed name of sc_get_current_process_base() to be
//   sc_get_current_process_b() since its returning an sc_process_b instance.
//
// Revision 1.3  2006/01/13 18:47:42  acg
// Added $Log command so that CVS comments are reproduced in the source.
//

// Taf!
