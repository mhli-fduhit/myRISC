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

  sc_semaphore_if.h -- The sc_semaphore_if interface class.

  Original Author: Martin Janssen, Synopsys, Inc., 2001-05-21

  CHANGE LOG IS AT THE END OF THE FILE
 *****************************************************************************/

#ifndef SC_SEMAPHORE_IF_H
#define SC_SEMAPHORE_IF_H

#include "sysc/communication/sc_interface.h"

namespace sc_core {

/**************************************************************************//**
 *  \class sc_semaphore_if
 *
 *  \brief The sc_semaphore_if interface class.
 *****************************************************************************/

class sc_semaphore_if
: virtual public sc_interface
{
public:

    // the classical operations: wait(), trywait(), and post()

    // lock (take) the semaphore, block if not available

    /**
     *  \brief A new parameter segment ID is added for the out-of-order 
     *         simulation.
     */ 
    // 08/19/2015 GL: modified for the OoO simulation
    virtual int wait( int ) = 0;

    // lock (take) the semaphore, return -1 if not available
    virtual int trywait() = 0;

    // unlock (give) the semaphore
    virtual int post() = 0;

    // get the value of the semphore
    virtual int get_value() const = 0;

protected:

    // constructor

    sc_semaphore_if()
	{}

private:

    // disabled
    sc_semaphore_if( const sc_semaphore_if& );
    sc_semaphore_if& operator = ( const sc_semaphore_if& );
};

} // namespace sc_core

//$Log: sc_semaphore_if.h,v $
//Revision 1.3  2011/08/26 20:45:42  acg
// Andy Goodrich: moved the modification log to the end of the file to
// eliminate source line number skew when check-ins are done.
//
//Revision 1.2  2011/02/18 20:23:45  acg
// Andy Goodrich: Copyright update.
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
//Revision 1.8  2005/06/10 22:43:55  acg
//Added CVS change log annotation.
//

#endif

// Taf!
