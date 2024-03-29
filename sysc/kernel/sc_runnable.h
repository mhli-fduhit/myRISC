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

  sc_runnable.h --

  Original Author: Martin Janssen, Synopsys, Inc., 2001-05-21

  CHANGE LOG AT THE END OF THE FILE
 *****************************************************************************/


#ifndef SC_RUNNABLE_H
#define SC_RUNNABLE_H


#include "sysc/kernel/sc_process.h"

namespace sc_core {

/**************************************************************************//**
 *  \class sc_runnable
 *
 *  \brief Class that manages the ready-to-run queues.
 *****************************************************************************/

class sc_runnable
{

  public:
    sc_runnable();
    ~sc_runnable();

    inline void init();
    inline void toggle_methods();
    inline void toggle_threads();

    inline void remove_method( sc_method_handle );
    inline void remove_thread( sc_thread_handle );

    inline void execute_method_next( sc_method_handle );
    inline void execute_thread_next( sc_thread_handle );

    inline void push_back_method( sc_method_handle );
    inline void push_back_thread( sc_thread_handle );
    inline void push_front_method( sc_method_handle );
    inline void push_front_thread( sc_thread_handle );

    inline bool is_initialized() const;
    inline bool is_empty() const;

    inline sc_method_handle pop_method();
    inline sc_thread_handle pop_thread();

    /**
     *  \brief Return the first method of methods push queue.
     */
    // 08/17/2015 GL.
    inline sc_method_handle get_methods_push_first();

    /**
     *  \brief Check whether it is the end of methods push queue.
     */
    // 08/17/2015 GL.
    inline bool is_methods_push_end( sc_method_handle );

    /**
     *  \brief Return the first method of methods pop queue.
     */
    // 08/17/2015 GL.
    inline sc_method_handle get_methods_pop_first();

    /**
     *  \brief Check whether it is the end of methods pop queue.
     */
    // 08/17/2015 GL.
    inline bool is_methods_pop_end( sc_method_handle );

    /**
     *  \brief Return the first thread of threads push queue.
     */
    // 08/17/2015 GL.
    inline sc_thread_handle get_threads_push_first();

    /**
     *  \brief Check whether it is the end of threads push queue.
     */
    // 08/17/2015 GL.
    inline bool is_threads_push_end( sc_thread_handle );

    /**
     *  \brief Return the first thread of threads pop queue.
     */
    // 08/17/2015 GL.
    inline sc_thread_handle get_threads_pop_first();

    /**
     *  \brief Check whether it is the end of threads pop queue.
     */
    // 08/17/2015 GL.
    inline bool is_threads_pop_end( sc_thread_handle );

  public: // diagnostics:
    void dump() const;

  private:
    sc_method_handle m_methods_push_head;
    sc_method_handle m_methods_push_tail;
    sc_method_handle m_methods_pop;
    sc_thread_handle m_threads_push_head;
    sc_thread_handle m_threads_push_tail;
    sc_thread_handle m_threads_pop;

  private:
    // disabled
    sc_runnable( const sc_runnable& );
    sc_runnable& operator = ( const sc_runnable& );
};

} // namespace sc_core

#endif

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date: Andy Goodrich, 30 June 2003, Forte Design Systems
  Description of Modification: Total rewrite using linked list rather than 
                               fixed vector.


      Name, Affiliation, Date: Bishnupriya Bhattacharya, Cadence Design Systems,
                               25 August, 2003
  Description of Modification: Add tail pointers for m_methods_push and
                               m_threads_push to maintain the same scheduler
                               ordering as 2.0.1

 *****************************************************************************/

// $Log: sc_runnable.h,v $
// Revision 1.9  2011/08/26 20:46:10  acg
//  Andy Goodrich: moved the modification log to the end of the file to
//  eliminate source line number skew when check-ins are done.
//
// Revision 1.8  2011/04/08 18:26:07  acg
//  Andy Goodrich: added execute_method_next() to handle method dispatch
//   for asynchronous notifications that occur outside the evaluation phase.
//
// Revision 1.7  2011/02/18 20:27:14  acg
//  Andy Goodrich: Updated Copyrights.
//
// Revision 1.6  2011/02/13 21:47:38  acg
//  Andy Goodrich: update copyright notice.
//
// Revision 1.5  2011/02/02 06:37:03  acg
//  Andy Goodrich: removed toggle() method since it is no longer used.
//
// Revision 1.4  2011/02/01 21:09:13  acg
//  Andy Goodrich: addition of toggle_methods() and toggle_threads() calls.
//
// Revision 1.3  2011/01/25 20:50:37  acg
//  Andy Goodrich: changes for IEEE 1666 2011.
//
// Revision 1.2  2008/05/22 17:06:26  acg
//  Andy Goodrich: updated copyright notice to include 2008.
//
// Revision 1.1.1.1  2006/12/15 20:20:05  acg
// SystemC 2.3
//
// Revision 1.3  2006/01/13 18:44:30  acg
// Added $Log to record CVS changes into the source.

// Taf!
