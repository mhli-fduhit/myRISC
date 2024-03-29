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

  sc_wait.h -- Wait() and related functions.

  Original Author: Stan Y. Liao, Synopsys, Inc.
                   Martin Janssen, Synopsys, Inc.

  CHANGE LOG AT THE END OF THE FILE
 *****************************************************************************/

#ifndef SC_WAIT_H
#define SC_WAIT_H


#include "sysc/kernel/sc_simcontext.h"
// 02/22/2016 ZC: to enable verbose display or not
#ifndef _SYSC_PRINT_VERBOSE_MESSAGE_ENV_VAR
#define _SYSC_PRINT_VERBOSE_MESSAGE_ENV_VAR "SYSC_PRINT_VERBOSE_MESSAGE"
#endif
namespace sc_core {

class sc_event;
class sc_event_and_list;
class sc_event_or_list;
class sc_simcontext;

extern sc_simcontext* sc_get_curr_simcontext();

void message_test();
// static sensitivity for SC_THREADs and SC_CTHREADs

/**
 *  \brief A new parameter segment ID is added for the out-of-order 
 *         simulation.
 */
 
extern 
void 
aux_seg_bound(sc_simcontext* simc);
 
// 06/12/2015 GL: modified for the OoO simulation
extern
void
wait( int = -1,
      sc_simcontext* = sc_get_curr_simcontext() );


// dynamic sensitivity for SC_THREADs and SC_CTHREADs

/**
 *  \brief A new parameter segment ID is added for the out-of-order 
 *         simulation.
 */
// 06/12/2015 GL: modified for the OoO simulation
extern
void
wait( const sc_event&,
      int = -1,
      sc_simcontext* = sc_get_curr_simcontext() );

/**
 *  \brief A new parameter segment ID is added for the out-of-order 
 *         simulation.
 */
// 06/12/2015 GL: modified for the OoO simulation
extern
void
wait( const sc_event_or_list&,
      int = -1,
      sc_simcontext* = sc_get_curr_simcontext() );

/**
 *  \brief A new parameter segment ID is added for the out-of-order 
 *         simulation.
 */
// 06/12/2015 GL: modified for the OoO simulation
extern
void
wait( const sc_event_and_list&,
      int = -1,
      sc_simcontext* = sc_get_curr_simcontext() );

/**
 *  \brief A new parameter segment ID is added for the out-of-order 
 *         simulation.
 */
// 06/12/2015 GL: modified for the OoO simulation
extern
void
wait( const sc_time&,
      int = -1,
      sc_simcontext* = sc_get_curr_simcontext() );

/**
 *  \brief A new parameter segment ID is added for the out-of-order 
 *         simulation.
 */
// 06/12/2015 GL: modified for the OoO simulation
inline
void
wait( double v, sc_time_unit tu,
      int seg_id = -1,
      sc_simcontext* simc = sc_get_curr_simcontext() )
{
    wait( sc_time( v, tu, simc ), seg_id, simc );
}

/**
 *  \brief A new parameter segment ID is added for the out-of-order 
 *         simulation.
 */
// 06/12/2015 GL: modified for the OoO simulation
extern
void
wait( const sc_time&,
      const sc_event&,
      int = -1,
      sc_simcontext* = sc_get_curr_simcontext() );

/**
 *  \brief A new parameter segment ID is added for the out-of-order 
 *         simulation.
 */
// 06/12/2015 GL: modified for the OoO simulation
inline
void
wait( double v, sc_time_unit tu,
      const sc_event& e,
      int seg_id = -1,
      sc_simcontext* simc = sc_get_curr_simcontext() )
{
    wait( sc_time( v, tu, simc ), e, seg_id, simc );
}

/**
 *  \brief A new parameter segment ID is added for the out-of-order 
 *         simulation.
 */
// 06/12/2015 GL: modified for the OoO simulation
extern
void
wait( const sc_time&,
      const sc_event_or_list&,
      int = -1,
      sc_simcontext* = sc_get_curr_simcontext() );

/**
 *  \brief A new parameter segment ID is added for the out-of-order 
 *         simulation.
 */
// 06/12/2015 GL: modified for the OoO simulation
inline
void
wait( double v, sc_time_unit tu,
      const sc_event_or_list& el,
      int seg_id = -1,
      sc_simcontext* simc = sc_get_curr_simcontext() )
{
    wait( sc_time( v, tu, simc ), el, seg_id, simc );
}

/**
 *  \brief A new parameter segment ID is added for the out-of-order 
 *         simulation.
 */
// 06/12/2015 GL: modified for the OoO simulation
extern
void
wait( const sc_time&,
      const sc_event_and_list&,
      int = -1,
      sc_simcontext* = sc_get_curr_simcontext() );

/**
 *  \brief A new parameter segment ID is added for the out-of-order 
 *         simulation.
 */
// 06/12/2015 GL: modified for the OoO simulation
inline
void
wait( double v, sc_time_unit tu,
      const sc_event_and_list& el,
      int seg_id = -1,
      sc_simcontext* simc = sc_get_curr_simcontext() )
{
    wait( sc_time( v, tu, simc ), el, seg_id, simc );
}


// static sensitivity for SC_METHODs

/**
 *  \brief A new parameter segment ID is added for the out-of-order 
 *         simulation.
 */
// 06/12/2015 GL: modified for the OoO simulation
extern
void
next_trigger( sc_simcontext* = sc_get_curr_simcontext() );


// dynamic sensitivity for SC_METHODs

/**
 *  \brief A new parameter segment ID is added for the out-of-order 
 *         simulation.
 */
// 06/12/2015 GL: modified for the OoO simulation
extern
void
next_trigger( const sc_event&,
	      sc_simcontext* = sc_get_curr_simcontext() );

/**
 *  \brief A new parameter segment ID is added for the out-of-order 
 *         simulation.
 */
// 06/12/2015 GL: modified for the OoO simulation
extern
void
next_trigger( const sc_event_or_list&,
	      sc_simcontext* = sc_get_curr_simcontext() );

/**
 *  \brief A new parameter segment ID is added for the out-of-order 
 *         simulation.
 */
// 06/12/2015 GL: modified for the OoO simulation
extern
void
next_trigger( const sc_event_and_list&,
	      sc_simcontext* = sc_get_curr_simcontext() );

/**
 *  \brief A new parameter segment ID is added for the out-of-order 
 *         simulation.
 */
// 06/12/2015 GL: modified for the OoO simulation
extern
void
next_trigger( const sc_time&,
	      sc_simcontext* = sc_get_curr_simcontext() );

/**
 *  \brief A new parameter segment ID is added for the out-of-order 
 *         simulation.
 */
// 06/12/2015 GL: modified for the OoO simulation
inline
void
next_trigger( double v, sc_time_unit tu,
	      sc_simcontext* simc = sc_get_curr_simcontext() )
{
    next_trigger( sc_time( v, tu, simc ), simc );
}

/**
 *  \brief A new parameter segment ID is added for the out-of-order 
 *         simulation.
 */
// 06/12/2015 GL: modified for the OoO simulation
extern
void
next_trigger( const sc_time&,
	      const sc_event&,
	      sc_simcontext* = sc_get_curr_simcontext() );

/**
 *  \brief A new parameter segment ID is added for the out-of-order 
 *         simulation.
 */
// 06/12/2015 GL: modified for the OoO simulation
inline
void
next_trigger( double v, sc_time_unit tu,
	      const sc_event& e,
	      sc_simcontext* simc = sc_get_curr_simcontext() )
{
    next_trigger( sc_time( v, tu, simc ), e, simc );
}

/**
 *  \brief A new parameter segment ID is added for the out-of-order 
 *         simulation.
 */
// 06/12/2015 GL: modified for the OoO simulation
extern
void
next_trigger( const sc_time&,
	      const sc_event_or_list&,
	      sc_simcontext* = sc_get_curr_simcontext() );

/**
 *  \brief A new parameter segment ID is added for the out-of-order 
 *         simulation.
 */
// 06/12/2015 GL: modified for the OoO simulation
inline
void
next_trigger( double v, sc_time_unit tu,
	      const sc_event_or_list& el,
	      sc_simcontext* simc = sc_get_curr_simcontext() )
{
    next_trigger( sc_time( v, tu, simc ), el, simc );
}

/**
 *  \brief A new parameter segment ID is added for the out-of-order 
 *         simulation.
 */
// 06/12/2015 GL: modified for the OoO simulation
extern
void
next_trigger( const sc_time&,
	      const sc_event_and_list&,
	      sc_simcontext* = sc_get_curr_simcontext() );

/**
 *  \brief A new parameter segment ID is added for the out-of-order 
 *         simulation.
 */
// 06/12/2015 GL: modified for the OoO simulation
inline
void
next_trigger( double v, sc_time_unit tu,
	      const sc_event_and_list& el,
	      sc_simcontext* simc = sc_get_curr_simcontext() )
{
    next_trigger( sc_time( v, tu, simc ), el, simc );
}


// for SC_METHODs and SC_THREADs and SC_CTHREADs

extern
bool
timed_out( sc_simcontext* = sc_get_curr_simcontext() );

// misc.

extern
void
sc_set_location( const char*,
		 int,
		 sc_simcontext* = sc_get_curr_simcontext() );

} // namespace sc_core

/*
$Log: sc_wait.h,v $
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

Revision 1.2  2006/01/03 23:18:45  acg
Changed copyright to include 2006.

Revision 1.1.1.1  2005/12/19 23:16:44  acg
First check in of SystemC 2.1 into its own archive.

Revision 1.10  2005/07/30 03:45:05  acg
Changes from 2.1, including changes for sc_process_handle.

Revision 1.9  2005/04/04 00:16:08  acg
Changes for directory name change to sys from systemc.
Changes for sc_string going to std::string.
Changes for sc_pvector going to std::vector.
Changes for reference pools for bit and part selections.
Changes for const sc_concatref support.

Revision 1.6  2004/10/13 18:13:22  acg
sc_ver.h - updated version number. sc_wait.h remove inclusion of
sysc/kernel/sc_event.h because it is not necessary.

Revision 1.5  2004/09/27 20:49:10  acg
Andy Goodrich, Forte Design Systems, Inc.
   - Added a $Log comment so that CVS checkin comments appear in the
        checkout source.

*/

#endif

// Taf!
