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

  sc_mutex.h -- The sc_mutex primitive channel class.

  Original Author: Martin Janssen, Synopsys, Inc., 2001-05-21

  CHANGE LOG IS AT THE END OF THE FILE
 *****************************************************************************/

#ifndef SC_MUTEX_H
#define SC_MUTEX_H

#include "sysc/kernel/sc_event.h"
#include "sysc/kernel/sc_object.h"
#include "sysc/kernel/sc_wait.h"
#include "sysc/communication/sc_mutex_if.h"

// 02/24/2015 GL: to include the struct chnl_scoped_lock
#include "sysc/communication/sc_prim_channel.h"

namespace sc_core {

/**************************************************************************//**
 *  \class sc_mutex
 *
 *  \brief The sc_mutex primitive channel class.
 *****************************************************************************/

class sc_mutex
: public sc_mutex_if,
  public sc_object
{
public:

    // constructors and destructor

    sc_mutex();
    explicit sc_mutex( const char* name_ );
	virtual ~sc_mutex();


    // interface methods

    // blocks until mutex could be locked

    /**
     *  \brief A new parameter segment ID is added for the out-of-order 
     *         simulation.
     */ 
    // 08/19/2015 GL: modified for the OoO simulation
    virtual int lock( int = -1);

    // returns -1 if mutex could not be locked
    virtual int trylock();

    // returns -1 if mutex was not locked by caller
    virtual int unlock();

    virtual const char* kind() const
    { return "sc_mutex"; }

protected:

    // support methods

    bool in_use() const
    { return ( m_owner != 0 ); }

protected:

    sc_process_b* m_owner;
    sc_event      m_free;

    /**
     *  \brief A lock to protect concurrent communication through sc_mutex.
     */
    // 02/10/2015 GL.
    mutable CHNL_MTX_TYPE_ m_mutex;

private:

    // disabled
    sc_mutex( const sc_mutex& );
    sc_mutex& operator = ( const sc_mutex& );
};

} // namespace sc_core

//$Log: sc_mutex.h,v $
//Revision 1.4  2011/08/26 20:45:41  acg
// Andy Goodrich: moved the modification log to the end of the file to
// eliminate source line number skew when check-ins are done.
//
//Revision 1.3  2011/02/18 20:23:45  acg
// Andy Goodrich: Copyright update.
//
//Revision 1.2  2010/11/02 16:31:01  acg
// Andy Goodrich: changed object derivation to use sc_object rather than
// sc_prim_channel as the parent class.
//
//Revision 1.1.1.1  2006/12/15 20:20:04  acg
//SystemC 2.3
//
//Revision 1.2  2006/01/03 23:18:26  acg
//Changed copyright to include 2006.
//
//Revision 1.1.1.1  2005/12/19 23:16:43  acg
//First check in of SystemC 2.1 into its own archive.
//
//Revision 1.10  2005/09/15 23:01:51  acg
//Added std:: prefix to appropriate methods and types to get around
//issues with the Edison Front End.
//
//Revision 1.9  2005/06/10 22:43:55  acg
//Added CVS change log annotation.
//

#endif

// Taf!
