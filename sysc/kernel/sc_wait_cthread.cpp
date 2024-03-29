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

  sc_wait_cthread.cpp -- Wait() and related functions for SC_CTHREADs.

  Original Author: Stan Y. Liao, Synopsys, Inc.
                   Martin Janssen, Synopsys, Inc.

  CHANGE LOG AT THE END OF THE FILE
 *****************************************************************************/



#include "sysc/kernel/sc_kernel_ids.h"
#include "sysc/kernel/sc_cthread_process.h"
#include "sysc/kernel/sc_simcontext_int.h"
#include "sysc/kernel/sc_wait_cthread.h"
#include "sysc/communication/sc_port.h"
#include "sysc/kernel/sc_wait.h"
namespace sc_core 
{

// 08/19/2015 GL: modified for the OoO simulation

// for SC_CTHREADs

void
halt( int seg_id, sc_simcontext* simc )
{
    sc_process_b* cp = simc->get_curr_proc();
    switch( cp->proc_kind() ) {
    case SC_CTHREAD_PROC_: {
	RCAST<sc_cthread_handle>( cp )->wait_halt( seg_id );
	break;
    }
    default:
	SC_REPORT_ERROR( SC_ID_HALT_NOT_ALLOWED_, 0 );
	break;
    }
}


void
wait( int n, int seg_id, sc_simcontext* simc )
{
    sc_process_b* cp = simc->get_curr_proc();
    if( n <= 0 ) {
	char msg[BUFSIZ];
	std::sprintf( msg, "n = %d", n );
	SC_REPORT_ERROR( SC_ID_WAIT_N_INVALID_, msg );
    }
    switch( cp->proc_kind() ) {
      case SC_THREAD_PROC_: 
      case SC_CTHREAD_PROC_: 
	RCAST<sc_cthread_handle>( cp )->wait_cycles( seg_id, n );
        break;
      default:
        SC_REPORT_ERROR( SC_ID_WAIT_NOT_ALLOWED_, "\n        "
	                 "in SC_METHODs use next_trigger() instead" );
        break;
    }
}


void
at_posedge( const sc_signal_in_if<bool>& s, int seg_id, sc_simcontext* simc )
{
    if( s.read() == true ) 
        do { wait( seg_id, simc ); } while ( s.read() == true );
    do { wait( seg_id, simc ); } while ( s.read() == false );
}

void
at_posedge( const sc_signal_in_if<sc_dt::sc_logic>& s, 
            int seg_id, sc_simcontext* simc )
{
    if( s.read() == '1' ) 
        do { wait( seg_id, simc ); } while ( s.read() == '1' );
    do { wait( seg_id, simc ); } while ( s.read() == '0' );
}

void
at_negedge( const sc_signal_in_if<bool>& s, int seg_id, sc_simcontext* simc )
{
    if( s.read() == false ) 
        do { wait( seg_id, simc ); } while ( s.read() == false );
    do { wait( seg_id, simc ); } while ( s.read() == true );
}

void
at_negedge( const sc_signal_in_if<sc_dt::sc_logic>& s, 
            int seg_id, sc_simcontext* simc )
{
    if( s.read() == '0' ) 
        do { wait( seg_id, simc ); } while ( s.read() == '0' );
    do { wait( seg_id, simc ); } while ( s.read() == '1' );
}


} // namespace sc_core

/* 
$Log: sc_wait_cthread.cpp,v $
Revision 1.6  2011/08/26 20:46:11  acg
 Andy Goodrich: moved the modification log to the end of the file to
 eliminate source line number skew when check-ins are done.

Revision 1.5  2011/02/18 20:27:14  acg
 Andy Goodrich: Updated Copyrights.

Revision 1.4  2011/02/13 21:47:38  acg
 Andy Goodrich: update copyright notice.

Revision 1.3  2009/10/14 19:07:42  acg
 Andy Goodrich: added an error message for wait(n) being called from an
 SC_METHOD.

Revision 1.2  2008/05/22 17:06:27  acg
 Andy Goodrich: updated copyright notice to include 2008.

Revision 1.1.1.1  2006/12/15 20:20:05  acg
SystemC 2.3

Revision 1.3  2006/03/13 20:26:51  acg
 Andy Goodrich: Addition of forward class declarations, e.g.,
 sc_reset, to keep gcc 4.x happy.

Revision 1.2  2006/01/03 23:18:45  acg
Changed copyright to include 2006.

Revision 1.1.1.1  2005/12/19 23:16:44  acg
First check in of SystemC 2.1 into its own archive.

Revision 1.10  2005/09/15 23:02:18  acg
Added std:: prefix to appropriate methods and types to get around
issues with the Edison Front End.

Revision 1.9  2005/09/02 19:03:30  acg
Changes for dynamic processes. Removal of lambda support.

Revision 1.8  2005/04/04 00:16:08  acg
Changes for directory name change to sys from systemc.
Changes for sc_string going to std::string.
Changes for sc_pvector going to std::vector.
Changes for reference pools for bit and part selections.
Changes for const sc_concatref support.

Revision 1.5  2004/09/27 20:49:10  acg
Andy Goodrich, Forte Design Systems, Inc.
   - Added a $Log comment so that CVS checkin comments appear in the
     checkout source.

*/

// Taf!
