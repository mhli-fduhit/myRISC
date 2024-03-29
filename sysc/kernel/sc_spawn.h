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

  sc_spawn.h -- Process spawning support.

  Original Authors: Andy Goodrich, Forte Design Systems, 17 June 2003
                    Stuart Swan, Cadence,
                    Bishnupriya Bhattacharya, Cadence Design Systems,
                    25 August, 2003

  CHANGE LOG AT THE END OF THE FILE
 *****************************************************************************/


#if !defined(sc_spawn_h_INCLUDED)
#define sc_spawn_h_INCLUDED

#include "sysc/kernel/sc_process_handle.h"
#include "sysc/kernel/sc_spawn_options.h"

namespace sc_core {

class sc_event;
class sc_port_base;
class sc_interface;
class sc_event_finder;
class sc_process_b;

/**************************************************************************//**
 *  \class sc_spawn_object<T>
 *
 *  This templated helper class allows an object to provide the execution 
 *  semantics for a process via its () operator. An instance of the supplied 
 *  execution object will be kept to provide the semantics when the process is 
 *  scheduled for execution. The () operator does not return a value. An example 
 *  of an object that might be used for this helper function would be void 
 *  SC_BOOST bound function or method. 
 *
 *  <pre>
 *  This class is derived from sc_process_host and overloads 
 *  sc_process_host::semantics to provide the actual semantic content. 
 *
 *   sc_spawn_object(T object, const char* name, const sc_spawn_options* opt_p)
 *     This is the object instance constructor for this class. It makes a
 *     copy of the supplied object. The tp_call constructor is called
 *     with an indication that this object instance should be reclaimed when
 *     execution completes.
 *         object   =  object whose () operator will be called to provide
 *                     the process semantics.
 *         name_p   =  optional name for object instance, or zero.
 *         opt_p    -> spawn options or zero.
 *
 *   virtual void semantics()
 *     This virtual method provides the execution semantics for its process.
 *     It performs a () operation on m_object.
 *  </pre>
 *****************************************************************************/
template<typename T>
class sc_spawn_object : public sc_process_host {
  public:
    sc_spawn_object( T object) : m_object(object)
    {
    }

    virtual void semantics()
    {
        m_object();
    }

  protected:
    T                        m_object;
};


/**************************************************************************//**
 *  \fn sc_spawn(T object, const char *name_p=0, 
 *      const sc_spawn_options *opt_p=0)
 *
 *  \brief Semantic object with no return value.
 *
 *  Note: This function is not supported by the out-of-order simulation in the
 *  current release.
 * 
 *  This inline function spawns a process for execution. The execution 
 *  semantics for the process being spawned will be provided by the supplied 
 *  object instance via its () operator. (E.g., a SC_BOOST bound function) 
 * 
 *  <pre>
 *  After creating the process it is registered with the simulator.
 *     object   =   object instance providing the execution semantics via its 
 *                  () operator.
 *     name_p   =   optional name for object instance, or zero.
 *     opt_p    ->  optional spawn options for process, or zero for the default.
 *  </pre>
 *****************************************************************************/
template <typename T>
inline sc_process_handle sc_spawn( 
    T object, 
    const char* name_p = 0,
    const sc_spawn_options* opt_p = 0)
{
    sc_simcontext*      context_p;
    sc_spawn_object<T>* spawn_p;

    //assert( 0 ); // 08/20/2015 GL: to support sc_spawn in the future
    
    context_p = sc_get_curr_simcontext();
    spawn_p = new sc_spawn_object<T>(object);
    if ( !opt_p || !opt_p->is_method() )
    {
        // 05/25/2015 GL: sc_kernel_lock constructor acquires the kernel lock
        sc_kernel_lock lock;

        // 05/27/2015 GL: we may or may not have acquired the kernel lock 
        //                (static vs. dynamic spawn)

        sc_process_handle thread_handle = context_p->create_thread_process( 
            name_p, true,
            SC_MAKE_FUNC_PTR(sc_spawn_object<T>,semantics), 
            spawn_p, opt_p,
            -2, // 08/20/2015 GL: fake segment ID 
            -2  // 09/02/2015 GL: fake instance ID
        );
        return thread_handle;
        // 05/25/2015 GL: sc_kernel_lock destructor releases the kernel lock
    }
    else
    {
        // 05/25/2015 GL: sc_kernel_lock constructor acquires the kernel lock
        sc_kernel_lock lock;

        // 05/27/2015 GL: we may or may not have acquired the kernel lock 
        //                (static vs. dynamic spawn)

        sc_process_handle method_handle = context_p->create_method_process( 
            name_p, true,
            SC_MAKE_FUNC_PTR(sc_spawn_object<T>,semantics), 
            spawn_p, opt_p,
            -2, // 08/20/2015 GL: fake segment ID
            -2  // 09/02/2015 GL: fake instance ID
        );
        return method_handle;
        // 05/25/2015 GL: sc_kernel_lock destructor releases the kernel lock
    }
}

//=============================================================================
// CLASS sc_spawn_object_v<T> for all compilers except HP aCC
//              or
// CLASS sc_spawn_object_v<T, R> for HP aCC which tries to match this 
// one template argument class when the sc_spawn() declared above is
// invoked with 3 arguments or 2 arguments, and generates compiler errors.
//
// This templated helper class allows an object to provide the execution 
// semantics for a process via its () operator. An instance of the supplied 
// object will be kept to provide the semantics when the process is scheduled 
// for execution. The () operator returns a value, which will be stored at the 
// location specified by the supplied pointer. An example of an object that 
// might be used for this helper function would be valued SC_BOOST bound 
// function or method. 
//
//   sc_spawn_object_v( typename F::result_type* r_p, T f, const char* name_p,
//                      const sc_spawn_options* opt_p )
//       r_p      -> where to place the result of the function invocation.
//       f        =  information to be executed.
//       name_p   =  optional name for object instance, or zero.
//       opt_p    -> optional spawn options for process, or zero for the default
//     This is the object instance constructor for this class. It makes a
//     copy of the supplied object. The tp_call constructor is called
//     with an indication that this object instance should be reclaimed when
//     execution completes.
//         result_p -> where to place the value of the () operator.
//         object   =  object whose () operator will be called to provide
//                     the process semantics.
//
//   virtual void semantics()
//     This virtual method provides the execution semantics for its process.
//     It performs a () operation on m_object, placing the result at m_result_p.
//=============================================================================

/**************************************************************************//**
 *  \class sc_spawn_object_v
 * 
 *  \brief Semantic object with return value.
 *
 *  This inline function spawns a process for execution. The execution 
 *  semantics for the process being spawned will be provided by the supplied 
 *  object instance via its () operator. (E.g., a SC_BOOST bound function) That
 *  operator returns a value, which will be placed in the supplied return 
 *  location. 
 *
 *  <pre>
 *  After creating the process it is registered with the simulator.
 *     object   =  object instance providing the execution semantics via its () 
 *                 operator.
 *     r_p      -> where to place the value of the () operator.
 *     name_p   =  optional name for object instance, or zero.
 *     opt_p    -> optional spawn options for process, or zero for the default.
 *  </pre>
 *****************************************************************************/

#if !defined (__HP_aCC)

template<typename T>
class sc_spawn_object_v : public sc_process_host {
  public:
    sc_spawn_object_v( typename T::result_type* r_p, T object ) :
        m_object(object), m_result_p(r_p)
    {
    }

    virtual void semantics()
    {
        *m_result_p = m_object();
    }

  protected:
    T                        m_object;
    typename T::result_type* m_result_p;
};

/**
 *  \brief This function is not supported by the out-of-order simulation in the
 *         current release.
 */
// 08/20/2015 GL.
template <typename T>
inline sc_process_handle sc_spawn( 
    typename T::result_type* r_p, 
    T object, 
    const char* name_p = 0,
    const sc_spawn_options* opt_p = 0)
{
    sc_simcontext*      context_p;
    sc_spawn_object_v<T>* spawn_p;

    assert( 0 ); // 08/20/2015 GL: to support sc_spawn in the future
    
    context_p = sc_get_curr_simcontext();
    
    spawn_p = new sc_spawn_object_v<T>(r_p, object);
    if ( !opt_p || !opt_p->is_method() )
    {
        // 05/25/2015 GL: sc_kernel_lock constructor acquires the kernel lock
        sc_kernel_lock lock;

        // 05/27/2015 GL: we may or may not have acquired the kernel lock 
        //                (static vs. dynamic spawn)

        sc_process_handle thread_handle = context_p->create_thread_process( 
            name_p, true,
            SC_MAKE_FUNC_PTR(sc_spawn_object_v<T>,semantics), 
            spawn_p, opt_p,
            -2, // 08/20/2015 GL: fake segment ID 
            -2  // 09/02/2015 GL: fake instance ID
        );
        return thread_handle;
        // 05/25/2015 GL: sc_kernel_lock destructor releases the kernel lock
    }
    else
    {
        // 05/25/2015 GL: sc_kernel_lock constructor acquires the kernel lock
        sc_kernel_lock lock; 

        // 05/27/2015 GL: we may or may not have acquired the kernel lock 
        //                (static vs. dynamic spawn)

        sc_process_handle method_handle = context_p->create_method_process( 
            name_p, true,
            SC_MAKE_FUNC_PTR(sc_spawn_object_v<T>,semantics), 
            spawn_p, opt_p,
            -2, // 08/20/2015 GL: fake segment ID 
            -2  // 09/02/2015 GL: fake instance ID
        );
        return method_handle;
        // 05/25/2015 GL: sc_kernel_lock destructor releases the kernel lock
    }
}

#else
// for HP aCC
template<typename T, typename R>
class sc_spawn_object_v : public sc_process_host {
  public:
    sc_spawn_object_v( R* r_p, T object) :
        m_object(object), m_result_p(r_p)
    {
    }

    virtual void semantics()
    {
        *m_result_p = m_object();
    }

  protected:
    T  m_object;
    R* m_result_p;
};

/**
 *  \brief This function is not supported by the out-of-order simulation in the
 *         current release.
 */
// 08/20/2015 GL.
template <typename T, typename R>
inline sc_process_handle sc_spawn( 
    R* r_p, 
    T object, 
    const char* name_p = 0,
    const sc_spawn_options* opt_p = 0)
{
    sc_simcontext*      context_p;
    sc_spawn_object_v<T,R>* spawn_p;

    assert( 0 ); // 08/20/2015 GL: to support sc_spawn in the future
    
    context_p = sc_get_curr_simcontext();
    
    spawn_p = new sc_spawn_object_v<T,R>(r_p, object);
    if ( !opt_p || !opt_p->is_method() )
    {
        // 05/25/2015 GL: sc_kernel_lock constructor acquires the kernel lock
        sc_kernel_lock lock; 

        // 05/27/2015 GL: we may or may not have acquired the kernel lock 
        //                (static vs. dynamic spawn)

        sc_process_handle thread_handle = context_p->create_thread_process( 
            name_p, true,
            static_cast<sc_core::SC_ENTRY_FUNC>(
                &sc_spawn_object_v<T,R>::semantics),
            spawn_p, opt_p,
            -2, // 08/20/2015 GL: fake segment ID 
            -2  // 09/02/2015 GL: fake instance ID 
        );
        return thread_handle;
        // 05/25/2015 GL: sc_kernel_lock destructor releases the kernel lock
    }
    else
    {
        // 05/25/2015 GL: sc_kernel_lock constructor acquires the kernel lock
        sc_kernel_lock lock;

        // 05/27/2015 GL: we may or may not have acquired the kernel lock 
        //                (static vs. dynamic spawn)

        sc_process_handle method_handle = context_p->create_method_process( 
            name_p, true,
            static_cast<sc_core::SC_ENTRY_FUNC>(
                &sc_spawn_object_v<T,R>::semantics), 
            spawn_p, opt_p,
            -2, // 08/20/2015 GL: fake segment ID 
            -2  // 09/02/2015 GL: fake instance ID
        );
        return method_handle;
        // 05/25/2015 GL: sc_kernel_lock destructor releases the kernel lock
    }
}

#endif // HP

} // namespace sc_core

// $Log: sc_spawn.h,v $
// Revision 1.7  2011/08/26 20:46:11  acg
//  Andy Goodrich: moved the modification log to the end of the file to
//  eliminate source line number skew when check-ins are done.
//
// Revision 1.6  2011/02/18 20:27:14  acg
//  Andy Goodrich: Updated Copyrights.
//
// Revision 1.5  2011/02/13 21:47:38  acg
//  Andy Goodrich: update copyright notice.
//
// Revision 1.4  2011/02/01 21:14:02  acg
//  Andy Goodrich: formatting.
//
// Revision 1.3  2009/07/28 01:10:53  acg
//  Andy Goodrich: updates for 2.3 release candidate.
//
// Revision 1.2  2008/05/22 17:06:26  acg
//  Andy Goodrich: updated copyright notice to include 2008.
//
// Revision 1.1.1.1  2006/12/15 20:20:05  acg
// SystemC 2.3
//
// Revision 1.6  2006/05/26 20:33:16  acg
//   Andy Goodrich: changes required by additional platform compilers (i.e.,
//   Microsoft VC++, Sun Forte, HP aCC).
//
// Revision 1.5  2006/05/08 18:01:44  acg
//  Andy Goodrich: changed the HP-specific implementations of sc_spawn() to
//  use a static_cast to create their entry functions rather than the
//  SC_MAKE_FUNC_PTR macro. The HP preprocessor does not parse template
//  arguments that contain a comma properly.
//
// Revision 1.4  2006/04/11 23:13:21  acg
//   Andy Goodrich: Changes for reduced reset support that only includes
//   sc_cthread, but has preliminary hooks for expanding to method and thread
//   processes also.
//
// Revision 1.3  2006/01/13 18:44:30  acg
// Added $Log to record CVS changes into the source.

#endif // !defined(sc_spawn_h_INCLUDED)
