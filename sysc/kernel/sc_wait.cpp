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

  sc_wait.cpp -- Wait() and related functions.

  Original Author: Stan Y. Liao, Synopsys, Inc.
                   Martin Janssen, Synopsys, Inc.

  CHANGE LOG AT THE END OF THE FILE
 *****************************************************************************/


#include "sysc/kernel/sc_except.h"
#include "sysc/kernel/sc_kernel_ids.h"
#include "sysc/kernel/sc_cthread_process.h"
#include "sysc/kernel/sc_thread_process.h"
#include "sysc/kernel/sc_simcontext_int.h"
#include "sysc/kernel/sc_wait.h"
#include "sysc/utils/sc_utils_ids.h"

// 02/22/2016 ZC: to enable verbose display or not
#ifndef _SYSC_PRINT_VERBOSE_MESSAGE_ENV_VAR
#define _SYSC_PRINT_VERBOSE_MESSAGE_ENV_VAR "SYSC_PRINT_VERBOSE_MESSAGE"
#endif
namespace sc_core {

// 06/12/2015 GL: modified for the OoO simulation

// static sensitivity for SC_THREADs and SC_CTHREADs

void message_test(){
	printf("testing\n");
}

void warn_cthread_wait()
{
    static bool warn_wait = true;
    if ( warn_wait )
    {
        warn_wait = false;
        SC_REPORT_INFO(SC_ID_IEEE_1666_DEPRECATION_,
	    "all waits except wait() and wait(N)\n" \
	    "             are deprecated for SC_CTHREAD, " \
	    "use an SC_THREAD instead");
    }
}

void
wait( int seg_id, sc_simcontext* simc )
{
    if(seg_id == -1 && sc_core::verbosity_flag_6)
    {
        std::cout << "Warning: wait"
            << ", by thread : " << simc->get_curr_proc()->name()
            << ", with timestamp : " << simc->get_curr_proc()->get_timestamp().to_string()
            << ", and segment id : -1"
            << std::endl;
    }
    sc_process_b* cp = simc->get_curr_proc();
    switch( cp->proc_kind() ) {
    case SC_THREAD_PROC_: 

        RCAST<sc_thread_handle>( cp )->wait(*(cp->m_sensitivity_events), seg_id);
        break;
    case SC_CTHREAD_PROC_: {
	RCAST<sc_cthread_handle>( cp )->wait_cycles( seg_id );
        break;
    }
    default:
	SC_REPORT_ERROR( SC_ID_WAIT_NOT_ALLOWED_, "\n        "
			 "in SC_METHODs use next_trigger() instead" );
        break;
    }
    assert(seg_id != -1 && "in wait for nothing");
}

void aux_seg_bound(sc_simcontext* simc)
{
	
    sc_process_b* cp = simc->get_curr_proc();
	//printf("%s calling top wait\n",cp->name()); //ZC
    switch( cp->proc_kind() ) {
    case SC_THREAD_PROC_: {
		RCAST<sc_thread_handle>( cp )->aux_boundary( );
		break;
    }
    default:
	SC_REPORT_ERROR( SC_ID_WAIT_NOT_ALLOWED_, "\n        "
			 "in SC_METHODs use next_trigger() instead" );
        break;
    }
}
// dynamic sensitivity for SC_THREADs and SC_CTHREADs

void
wait( const sc_event& e, int seg_id, sc_simcontext* simc )
{
    if(seg_id == -1 && sc_core::verbosity_flag_6)
    {
        std::cout << "Warning: wait for event : " << e.name()
            << ", by thread : " << simc->get_curr_proc()->name()
            << ", with timestamp : " << simc->get_curr_proc()->get_timestamp().to_string()
            << ", and segment id : -1"
            << std::endl;
    }
    sc_process_b* cp = simc->get_curr_proc();
	//printf("%s calling top wait\n",cp->name()); //ZC
    switch( cp->proc_kind() ) {
    case SC_THREAD_PROC_: {
	RCAST<sc_thread_handle>( cp )->wait( e, seg_id );
	break;
    }
    case SC_CTHREAD_PROC_: {
        warn_cthread_wait();
	sc_cthread_handle cthread_h =
            RCAST<sc_cthread_handle>( cp );
	cthread_h->wait( e, seg_id );
	cthread_h->wait_cycles( seg_id + 1 );
	break;
    }
    default:
	SC_REPORT_ERROR( SC_ID_WAIT_NOT_ALLOWED_, "\n        "
			 "in SC_METHODs use next_trigger() instead" );
        break;
    }

    assert(seg_id != -1 && "in wait for event");
}

void
wait( const sc_event_or_list& el, int seg_id, sc_simcontext* simc )
{
    if(seg_id == -1 && sc_core::verbosity_flag_6)
    {
        std::cout << "Warning: wait for eventorlist"
            << ", by thread : " << simc->get_curr_proc()->name()
            << ", with timestamp : " << simc->get_curr_proc()->get_timestamp().to_string()
            << ", and segment id : -1"
            << std::endl;
    }
    sc_process_b* cp = simc->get_curr_proc();
    switch( cp->proc_kind() ) {
    case SC_THREAD_PROC_: {
	RCAST<sc_thread_handle>( cp )->wait( el, seg_id );
	break;
    }
    case SC_CTHREAD_PROC_: {
        warn_cthread_wait();
        SC_REPORT_INFO(SC_ID_IEEE_1666_DEPRECATION_,
	    "wait(event_list) is deprecated for SC_CTHREAD, use SC_THREAD");
	sc_cthread_handle cthread_h =
            RCAST<sc_cthread_handle>( cp );
	cthread_h->wait( el, seg_id );
	cthread_h->wait_cycles( seg_id + 1 );
	break;
    }
    default:
	SC_REPORT_ERROR( SC_ID_WAIT_NOT_ALLOWED_, "\n        "
			 "in SC_METHODs use next_trigger() instead" );
        break;
    }

    assert(seg_id != -1 && "in wait for eventorlist");
}

void
wait( const sc_event_and_list& el, int seg_id, sc_simcontext* simc )
{
    if(seg_id == -1 && sc_core::verbosity_flag_6)
    {
        std::cout << "Warning: wait for eventandlist"
            << ", by thread : " << simc->get_curr_proc()->name()
            << ", with timestamp : " << simc->get_curr_proc()->get_timestamp().to_string()
            << ", and segment id : -1"
            << std::endl;
    }
    sc_process_b* cp = simc->get_curr_proc();
    switch( cp->proc_kind() ) {
    case SC_THREAD_PROC_: {
	RCAST<sc_thread_handle>( cp )->wait( el, seg_id );
	break;
    }
    case SC_CTHREAD_PROC_: {
        warn_cthread_wait();
	sc_cthread_handle cthread_h =
            RCAST<sc_cthread_handle>( cp );
	cthread_h->wait( el, seg_id );
	cthread_h->wait_cycles( seg_id );
	break;
    }
    default:
	SC_REPORT_ERROR( SC_ID_WAIT_NOT_ALLOWED_, "\n        "
			 "in SC_METHODs use next_trigger() instead" );
        break;
    }

    assert(seg_id != -1 && "in wait for eventandlist");
}

void
wait( const sc_time& t, int seg_id, sc_simcontext* simc )
{
    if(seg_id == -1 && sc_core::verbosity_flag_6)
    {
        std::cout << "Warning: wait for time"
            << ", by thread : " << simc->get_curr_proc()->name()
            << ", with timestamp : " << simc->get_curr_proc()->get_timestamp().to_string()
            << ", and segment id : -1"
            << std::endl;
    }
    sc_process_b* cp = simc->get_curr_proc();
    switch( cp->proc_kind() ) {
    case SC_THREAD_PROC_: {
	RCAST<sc_thread_handle>( cp )->wait( t, seg_id );
	break;
    }
    case SC_CTHREAD_PROC_: {
        warn_cthread_wait();
	sc_cthread_handle cthread_h =
            RCAST<sc_cthread_handle>( cp );
	cthread_h->wait( t, seg_id );
	cthread_h->wait_cycles( seg_id + 1 );
	break;
    }
    default:
	SC_REPORT_ERROR( SC_ID_WAIT_NOT_ALLOWED_, "\n        "
			 "in SC_METHODs use next_trigger() instead" );
        break;
    }

    assert(seg_id != -1 && "in wait for time");
}

void
wait( const sc_time& t, const sc_event& e, 
      int seg_id, sc_simcontext* simc )
{
    if(seg_id == -1 && sc_core::verbosity_flag_6)
    {
        std::cout << "Warning: wait for timed event : " << e.name()
            << ", by thread : " << simc->get_curr_proc()->name()
            << ", with timestamp : " << simc->get_curr_proc()->get_timestamp().to_string()
            << ", and segment id : -1"
            << std::endl;
    }
   sc_process_b* cp = simc->get_curr_proc();
    switch( cp->proc_kind() ) {
    case SC_THREAD_PROC_: {
	RCAST<sc_thread_handle>( cp )->wait( t, e, seg_id );
	break;
    }
    case SC_CTHREAD_PROC_: {
        warn_cthread_wait();
	sc_cthread_handle cthread_h =
            RCAST<sc_cthread_handle>( cp );
	cthread_h->wait( t, e, seg_id );
	cthread_h->wait_cycles( seg_id + 1 );
	break;
    }
    default:
	SC_REPORT_ERROR( SC_ID_WAIT_NOT_ALLOWED_, "\n        "
			 "in SC_METHODs use next_trigger() instead" );
        break;
    }
    assert(seg_id != -1 && "in wait for timed event");
}

void
wait( const sc_time& t, const sc_event_or_list& el, 
      int seg_id, sc_simcontext* simc )
{
    if(seg_id == -1 && sc_core::verbosity_flag_6)
    {
        std::cout << "Warning: wait for timed eventorlist"
            << ", by thread : " << simc->get_curr_proc()->name()
            << ", with timestamp : " << simc->get_curr_proc()->get_timestamp().to_string()
            << ", and segment id : -1"
            << std::endl;
    }
    sc_process_b* cp = simc->get_curr_proc();
    switch( cp->proc_kind() ) {
    case SC_THREAD_PROC_: {
	RCAST<sc_thread_handle>( cp )->wait( t, el, seg_id );
	break;
    }
    case SC_CTHREAD_PROC_: {
        warn_cthread_wait();
	sc_cthread_handle cthread_h =
            RCAST<sc_cthread_handle>( cp );
	cthread_h->wait( t, el, seg_id );
	cthread_h->wait_cycles( seg_id + 1 );
	break;
    }
    default:
	SC_REPORT_ERROR( SC_ID_WAIT_NOT_ALLOWED_, "\n        "
			 "in SC_METHODs use next_trigger() instead" );
        break;
    } 
    assert(seg_id != -1 && "in wait for timed eventorlist");
}

void
wait( const sc_time& t, const sc_event_and_list& el, 
      int seg_id, sc_simcontext* simc )
{
    if(seg_id == -1 && sc_core::verbosity_flag_6)
    {
        std::cout << "Warning: wait for timed eventandlist"
            << ", by thread : " << simc->get_curr_proc()->name()
            << ", with timestamp : " << simc->get_curr_proc()->get_timestamp().to_string()
            << ", and segment id : -1"
            << std::endl;
    }
    sc_process_b* cp = simc->get_curr_proc();
    switch( cp->proc_kind() ) {
    case SC_THREAD_PROC_: {
	RCAST<sc_thread_handle>( cp )->wait( t, el, seg_id );
	break;
    }
    case SC_CTHREAD_PROC_: {
        warn_cthread_wait();
	sc_cthread_handle cthread_h =
            RCAST<sc_cthread_handle>( cp );
	cthread_h->wait( t, el, seg_id );
	cthread_h->wait_cycles( seg_id + 1 );
	break;
    }
    default:
	SC_REPORT_ERROR( SC_ID_WAIT_NOT_ALLOWED_, "\n        "
			 "in SC_METHODs use next_trigger() instead" );
        break;
    }
    assert(seg_id != -1 && "in wait for timed eventandlist");
}


// static sensitivity for SC_METHODs

void
next_trigger( sc_simcontext* simc )
{
    sc_process_b* cp = simc->get_curr_proc();
    if( cp->proc_kind() == SC_THREAD_PROC_ && cp->invoker ) {
	RCAST<sc_method_handle>( cp->cur_invoker_method_handle )->clear_trigger( );
    } else {
	SC_REPORT_ERROR( SC_ID_NEXT_TRIGGER_NOT_ALLOWED_, "\n        "
			 "in SC_THREADs and SC_CTHREADs use wait() instead" );
    }
}


// dynamic sensitivity for SC_METHODs

void
next_trigger( const sc_event& e, sc_simcontext* simc )
{
    sc_process_b* cp = simc->get_curr_proc();
    if( cp->proc_kind() == SC_THREAD_PROC_ && cp->invoker ) {
	RCAST<sc_method_handle>( cp->cur_invoker_method_handle )->next_trigger( e );
    } else {
	SC_REPORT_ERROR( SC_ID_NEXT_TRIGGER_NOT_ALLOWED_, "\n        "
			 "in SC_THREADs and SC_CTHREADs use wait() instead" );
    }
}

void
next_trigger( const sc_event_or_list& el, sc_simcontext* simc )
{
    sc_process_b* cp = simc->get_curr_proc();
    if( cp->proc_kind() == SC_THREAD_PROC_ && cp->invoker ) {
	RCAST<sc_method_handle>( cp->cur_invoker_method_handle )->next_trigger( el );
    } else {
	SC_REPORT_ERROR( SC_ID_NEXT_TRIGGER_NOT_ALLOWED_, "\n        "
			 "in SC_THREADs and SC_CTHREADs use wait() instead" );
    }
}

void
next_trigger( const sc_event_and_list& el, sc_simcontext* simc )
{
    sc_process_b* cp = simc->get_curr_proc();
    if( cp->proc_kind() == SC_THREAD_PROC_ && cp->invoker ) {
	RCAST<sc_method_handle>( cp->cur_invoker_method_handle )->next_trigger( el );
    } else {
	SC_REPORT_ERROR( SC_ID_NEXT_TRIGGER_NOT_ALLOWED_, "\n        "
			 "in SC_THREADs and SC_CTHREADs use wait() instead" );
    }
}

void
next_trigger( const sc_time& t, sc_simcontext* simc )
{
    sc_process_b* cp = simc->get_curr_proc();
    if( cp->proc_kind() == SC_THREAD_PROC_ && cp->invoker ) {
	RCAST<sc_method_handle>( cp->cur_invoker_method_handle )->next_trigger( t );
    } else {
	SC_REPORT_ERROR( SC_ID_NEXT_TRIGGER_NOT_ALLOWED_, "\n        "
			 "in SC_THREADs and SC_CTHREADs use wait() instead" );
    }
}

void
next_trigger( const sc_time& t, const sc_event& e,
              sc_simcontext* simc )
{
    sc_process_b* cp = simc->get_curr_proc();
    if( cp->proc_kind() == SC_THREAD_PROC_ && cp->invoker ) {
	RCAST<sc_method_handle>( cp->cur_invoker_method_handle )->next_trigger( t, e );
    } else {
	SC_REPORT_ERROR( SC_ID_NEXT_TRIGGER_NOT_ALLOWED_, "\n        "
			 "in SC_THREADs and SC_CTHREADs use wait() instead" );
    }
}

void
next_trigger( const sc_time& t, const sc_event_or_list& el,
              sc_simcontext* simc)
{
    sc_process_b* cp = simc->get_curr_proc();
    if( cp->proc_kind() == SC_THREAD_PROC_ && cp->invoker ) {
	RCAST<sc_method_handle>( cp->cur_invoker_method_handle )->next_trigger( t, el );
    } else {
	SC_REPORT_ERROR( SC_ID_NEXT_TRIGGER_NOT_ALLOWED_, "\n        "
			 "in SC_THREADs and SC_CTHREADs use wait() instead" );
    }
}

void
next_trigger(const sc_time& t, const sc_event_and_list& el,
             sc_simcontext* simc)
{
    sc_process_b* cp = simc->get_curr_proc();
    if( cp->proc_kind() == SC_THREAD_PROC_ && cp->invoker ) {
	RCAST<sc_method_handle>( cp->cur_invoker_method_handle )->next_trigger( t, el );
    } else {
	SC_REPORT_ERROR( SC_ID_NEXT_TRIGGER_NOT_ALLOWED_, "\n        "
			 "in SC_THREADs and SC_CTHREADs use wait() instead" );
    }
}


// for SC_METHODs and SC_THREADs and SC_CTHREADs

bool
timed_out( sc_simcontext* simc )
{
    static bool warn_timed_out=true; 
    if ( warn_timed_out )
    {
        warn_timed_out = false;
        SC_REPORT_INFO(SC_ID_IEEE_1666_DEPRECATION_,
	    "timed_out() function is deprecated" );
    }

    sc_process_b* cp = simc->get_curr_proc();
    return cp->timed_out();
}



// misc.

void
sc_set_location( const char* file, int lineno, sc_simcontext* simc )
{
    sc_process_b* handle = simc->get_curr_proc();
    handle->file = file;
    handle->lineno = lineno;
}

} // namespace sc_core

/* 
$Log: sc_wait.cpp,v $
Revision 1.6  2011/08/26 20:46:11  acg
 Andy Goodrich: moved the modification log to the end of the file to
 eliminate source line number skew when check-ins are done.

Revision 1.5  2011/02/18 20:27:14  acg
 Andy Goodrich: Updated Copyrights.

Revision 1.4  2011/02/13 21:47:38  acg
 Andy Goodrich: update copyright notice.

Revision 1.3  2011/01/18 20:10:45  acg
 Andy Goodrich: changes for IEEE1666_2011 semantics.

Revision 1.2  2008/05/22 17:06:27  acg
 Andy Goodrich: updated copyright notice to include 2008.

Revision 1.1.1.1  2006/12/15 20:20:05  acg
SystemC 2.3

Revision 1.7  2006/02/02 20:20:39  acg
 Andy Goodrich: warnings for SC_THREAD waits.

Revision 1.6  2006/02/01 01:36:54  acg
 Andy Goodrich: addition of deprecation comments for SC_CTHREAD waits other
 than wait() and wait(N).

Revision 1.5  2006/01/31 22:17:40  acg
 Andy Goodrich: added deprecation warnings for SC_CTHREAD waits other than
 wait() and wait(N).

Revision 1.4  2006/01/25 00:31:20  acg
 Andy Goodrich: Changed over to use a standard message id of
 SC_ID_IEEE_1666_DEPRECATION for all deprecation messages.

Revision 1.3  2006/01/24 20:49:05  acg
Andy Goodrich: changes to remove the use of deprecated features within the
simulator, and to issue warning messages when deprecated features are used.

Revision 1.2  2006/01/03 23:18:45  acg
Changed copyright to include 2006.

Revision 1.1.1.1  2005/12/19 23:16:44  acg
First check in of SystemC 2.1 into its own archive.

Revision 1.10  2005/09/02 19:03:30  acg
Changes for dynamic processes. Removal of lambda support.

Revision 1.9  2005/07/30 03:45:05  acg
Changes from 2.1, including changes for sc_process_handle.

Revision 1.8  2005/04/04 00:16:07  acg
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
