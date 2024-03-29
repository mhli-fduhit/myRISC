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

  sc_process.h -- Process base class support.

  Original Author: Andy Goodrich, Forte Design Systems, 04 August 2005


  CHANGE LOG AT THE END OF THE FILE
 *****************************************************************************/


#if !defined(sc_process_h_INCLUDED)
#define sc_process_h_INCLUDED

#include <string>
#include <cassert>
#include "sysc/utils/sc_iostream.h"
#include "sysc/kernel/sc_constants.h"
#include "sysc/kernel/sc_object.h"
#include "sysc/kernel/sc_kernel_ids.h"
#include "sysc/communication/sc_export.h"

#include <list> // 02/13/2015 GL
#include <set>
#include <unordered_set>
#ifndef SC_INCLUDE_WINDOWS_H // 02/20/2015 GL
#  define SC_INCLUDE_WINDOWS_H // include Windows.h, if needed
#endif
#include "sysc/kernel/sc_cmnhdr.h"

#if defined(WIN32) || defined(_WIN32)

#define CHNL_MTX_TYPE_ CRITICAL_SECTION

#define CHNL_MTX_INIT_( Mutex ) \
    InitializeCriticalSection( &(Mutex) )
#define CHNL_MTX_LOCK_( Mutex ) \
    EnterCriticalSection( &(Mutex) )
#define CHNL_MTX_UNLOCK_( Mutex ) \
    LeaveCriticalSection( &(Mutex) )
#define CHNL_MTX_TRYLOCK_( Mutex ) \
    ( TryEnterCriticalSection( &(Mutex) ) != 0 )
#define CHNL_MTX_DESTROY_( Mutex ) \
    DeleteCriticalSection( &(Mutex) )

#else // use pthread mutex

#include <pthread.h>
#define CHNL_MTX_TYPE_ pthread_mutex_t

#if defined(__hpux)
#  define CHNL_PTHREAD_NULL_ cma_c_null
#else // !defined(__hpux)
#  define CHNL_PTHREAD_NULL_ NULL
#endif

#define CHNL_MTX_INIT_( Mutex ) \
    pthread_mutex_init( &(Mutex), CHNL_PTHREAD_NULL_ )
#define CHNL_MTX_LOCK_( Mutex ) \
    pthread_mutex_lock( &(Mutex) )
#define CHNL_MTX_UNLOCK_( Mutex ) \
    pthread_mutex_unlock( &(Mutex) )

#ifdef _XOPEN_SOURCE
#   define CHNL_MTX_TRYLOCK_( Mutex ) \
       ( pthread_mutex_trylock( &(Mutex) ) == 0 )
#else // no try_lock available
#   define CHNL_MTX_TRYLOCK_( Mutex ) \
       ( false ) 
#endif

#define CHNL_MTX_DESTROY_( Mutex ) \
    pthread_mutex_destroy( &(Mutex) )

#endif

#include "sysc/kernel/sc_time.h" // 08/12/2015 GL: to include sc_time type

// 08/19/2015 GL: to include sc_dt::uint64
#include "sysc/datatypes/int/sc_nbdefs.h"
// 02/22/2016 ZC: to enable verbose display or not
#ifndef _SYSC_PRINT_VERBOSE_MESSAGE_ENV_VAR
#define _SYSC_PRINT_VERBOSE_MESSAGE_ENV_VAR "SYSC_PRINT_VERBOSE_MESSAGE"
#endif
namespace sc_core {

// Forward declarations:
class sc_process_handle;
class sc_thread_process;
class sc_reset;

const char* sc_gen_unique_name( const char*, bool preserve_first );
sc_process_handle sc_get_current_process_handle();
void sc_thread_cor_fn( void* arg );
bool timed_out( sc_simcontext* );

extern bool sc_allow_process_control_corners; // see sc_simcontext.cpp.


// Process handles as forward references:

typedef class sc_cthread_process* sc_cthread_handle;
typedef class sc_method_process*  sc_method_handle;
typedef class sc_thread_process*  sc_thread_handle;


// Standard process types:

enum sc_curr_proc_kind
{
    SC_NO_PROC_,
    SC_METHOD_PROC_,
    SC_THREAD_PROC_,
    SC_CTHREAD_PROC_
};

// Descendant information for process hierarchy operations:

enum sc_descendant_inclusion_info {
    SC_NO_DESCENDANTS=0,
    SC_INCLUDE_DESCENDANTS,
    SC_INVALID_DESCENDANTS
};

/**************************************************************************//**
 *  \class sc_process_host
 *
 *  \brief This is the base class for objects which may have processes defined 
 *  for their methods (e.g., sc_module).
 *****************************************************************************/

class sc_process_host 
{
  public:
    sc_process_host() {}
    virtual ~sc_process_host() { } // Needed for cast check for sc_module.
    void defunct() {}
};


/**************************************************************************//**
 *  \class sc_process_monitor
 *
 *  This class provides a way of monitoring a process' status (e.g., 
 *  waiting for a thread to complete its execution). This class is intended to 
 *  be a base class for classes which need to monitor a process or processes 
 *  (e.g., sc_join). Its methods should be overloaded where notifications are 
 *  desired.
 *****************************************************************************/

class sc_process_monitor {
  public:
    enum {
        spm_exit = 0
    };
    virtual ~sc_process_monitor() {}
    virtual void signal(sc_thread_handle thread_p, int type);
	//DM 05/24/2019 sc_method is same as sc_thread
    virtual void signal(sc_method_handle thread_p, int type);

};
inline void sc_process_monitor::signal(sc_thread_handle , int ) {}  
inline void sc_process_monitor::signal(sc_method_handle , int ) {}  

/**************************************************************************//**
 *  \def SC_USE_MEMBER_FUNC_PTR
*   
 *  \brief Process invocation method or function.
 *
 *  Define SC_USE_MEMBER_FUNC_PTR if we want to use member function pointers 
 *  to implement process dispatch. Otherwise, we'll use a hack that involves 
 *  creating a templated invocation object which will invoke the member
 *  function. This should not be necessary, but some compilers (e.g., VC++)
 *  do not allow the conversion from `void (callback_tag::*)()' to 
 *  `void (sc_process_host::*)()'. This is supposed to be OK as long as the 
 *  dynamic type is correct.  C++ Standard 5.4 "Explicit type conversion", 
 *  clause 7: a pointer to member of derived class type may be explicitly 
 *  converted to a pointer to member of an unambiguous non-virtual base class 
 *  type. 
 *****************************************************************************/

#if defined(_MSC_VER)
#if ( _MSC_VER > 1200 )
#   define SC_USE_MEMBER_FUNC_PTR
#endif
#else
#   define SC_USE_MEMBER_FUNC_PTR
#endif


// COMPILER DOES SUPPORT CAST TO void (sc_process_host::*)() from (T::*)():

#if defined(SC_USE_MEMBER_FUNC_PTR)

    typedef void (sc_process_host::*SC_ENTRY_FUNC)();
#   define SC_DECL_HELPER_STRUCT(callback_tag, func) /*EMPTY*/
#   define SC_MAKE_FUNC_PTR(callback_tag, func) \
        static_cast<sc_core::SC_ENTRY_FUNC>(&callback_tag::func)


// COMPILER NOT DOES SUPPORT CAST TO void (sc_process_host::*)() from (T::*)():

#else // !defined(SC_USE_MEMBER_FUNC_PTR)
    class sc_process_call_base {
      public:
        inline sc_process_call_base()
        {
        }

        virtual ~sc_process_call_base()
        {
        }

        virtual void invoke(sc_process_host* host_p)
        {
        }
    };
    extern sc_process_call_base sc_process_defunct;

    template<class T>
    class sc_process_call : public sc_process_call_base {
      public:
        sc_process_call( void (T::*method_p)() ) :
            sc_process_call_base()
        {
             m_method_p = method_p;
        }

        virtual ~sc_process_call()
        {
        }

        virtual void invoke(sc_process_host* host_p)
        {
            (((T*)host_p)->*m_method_p)();
        }

      protected:
        void (T::*m_method_p)();  // Method implementing the process.
    };

    typedef sc_process_call_base* SC_ENTRY_FUNC;
#   define SC_DECL_HELPER_STRUCT(callback_tag, func) /*EMPTY*/
#   define SC_MAKE_FUNC_PTR(callback_tag, func) \
        (::sc_core::SC_ENTRY_FUNC) (new \
        ::sc_core::sc_process_call<callback_tag>(&callback_tag::func))

#endif // !defined(SC_USE_MEMBER_FUNC_PTR)


extern void sc_set_stack_size( sc_thread_handle, std::size_t );

// 04/03/2015 GL: set the stack size of SC_METHODs
extern void sc_set_stack_size( sc_method_handle, std::size_t );

class sc_event;
class sc_event_list;
class sc_event_or_list;
class sc_name_gen;
class sc_spawn_options;
class sc_unwind_exception;

/**************************************************************************//**
 *  \class sc_throw_it<EXCEPT> 
 *   
 *  \brief Arbitrary exception class.
 *
 *  This class serves as a way of throwing an execption for an aribtrary type
 *  without knowing what that type is. A true virtual method in the base
 *  class is used to actually throw the execption. A pointer to the base 
 *  class is used internally removing the necessity of knowing what the type
 *  of EXCEPT is for code internal to the library.
 *
 *  Note the clone() true virtual method. This is used to allow instances
 *  of the sc_throw_it<EXCEPT> class to be easily garbage collected. Since
 *  an exception may be propogated to more than one process knowing when
 *  to garbage collect is non-trivial. So when a call is made to 
 *  sc_process_handle::throw_it() an instance of sc_throw_it<EXCEPT> is 
 *  allocated on the stack. For each process throwing the exception a copy is 
 *  made via clone(). That allows those objects to be deleted by the individual
 *  processes when they are no longer needed (in this implementation of SystemC 
 *  that deletion will occur each time a new exception is thrown ( see 
 *  sc_thread_process::suspend_me() ).
 *****************************************************************************/
class sc_throw_it_helper {
  public:
    virtual sc_throw_it_helper* clone() const = 0;
    virtual void throw_it() = 0;
    sc_throw_it_helper() {}
    virtual ~sc_throw_it_helper() {}
};

template<typename EXCEPT>
class sc_throw_it : public sc_throw_it_helper
{
    typedef sc_throw_it<EXCEPT> this_type;
  public:
    sc_throw_it( const EXCEPT& value ) : m_value(value) { }
    virtual ~sc_throw_it() {}
    virtual inline this_type* clone() const { return new this_type(m_value); }
    virtual inline void throw_it() { throw m_value; }
  protected:
    EXCEPT m_value;  // value to be thrown.
};

/**************************************************************************//**
 *  \class sc_acq_chnl_lock_queue
 *  
 *  \brief A list of channel locks acquired by a process.
 *
 *  This class implements the object to maintain a list of channel locks  
 *  acquired in the hierachical channels. The outer channel lock is at the 
 *  beginning of the list, and the inner channel lock is at the end of the 
 *  list. The order of the acquired channel locks is maintained as LIFO 
 *  (Last-In-First-Out), meaning that the outer channel lock is acquired first 
 *  and released last.
 *
 *  Notes:
 *    (1) In the lock_and_push method, it will first check whether the new 
 *        channel lock is the same as the last one in the list. If they are 
 *        the same (one method calls another in the current-level channel), it 
 *        will only increment the lock counter; if the new channel lock is not 
 *        in the list, then it will be locked and pushed into the end of the 
 *        list; if the new lock is in the list but not the last one (an inner 
 *        channel method calls an outer channel method), assertion fails and it
 *         stops the simulation. 
 *    (2) In the pop_and_unlock method, it will always try to unlock the last 
 *        channel lock in the list, otherwise it breaks the correct order to 
 *        unlock locks (may lead to deadlock issues). When the counter of the 
 *        last lock is larger than one, it will decrement the counter. If the 
 *        counter equals one, it pops the lock from the list and releases the 
 *        lock.
 *    (3) In lock_all and unlock_all methods, it will acquire all the locks 
 *        from the beginning to the end, and release all the locks in the 
 *        reverse order.
 *****************************************************************************/
// 02/13/2015 GL.
/**
 *  \brief A data structure to keep the state of a channel lock.
 */
struct sc_chnl_lock {
    /**
     *  \brief A pointer to the specific channel lock.
     */
    CHNL_MTX_TYPE_ *lock_p;

    /**
     *  \brief The lock counter.
     */
    unsigned int counter;

    /**
     *  \brief The constructor initializes the lock pointer and counter.
     */
    sc_chnl_lock( CHNL_MTX_TYPE_ *m ): lock_p( m ), counter( 1 ) {}
};

class sc_acq_chnl_lock_queue {
  public:
    /**
     *  \brief Acquire a new channel lock or increment the lock counter.
     *
     *  In the lock_and_push method, it will first check whether the new 
     *  channel lock is the same as the last one in the list. If they are 
     *  the same (one method calls another in the current-level channel), it 
     *  will only increment the lock counter; if the new channel lock is not 
     *  in the list, then it will be locked and pushed into the end of the 
     *  list; if the new lock is in the list but not the last one (an inner 
     *  channel method calls an outer channel method), assertion fails and it
     *  stops the simulation. 
     */
    void lock_and_push( CHNL_MTX_TYPE_ *lock );

    /**
     *  \brief Release a channel lock or decrement the lock counter.
     *
     *  In the pop_and_unlock method, it will always try to unlock the last 
     *  channel lock in the list, otherwise it breaks the correct order to 
     *  unlock locks (may lead to deadlock issues). When the counter of the 
     *  last lock is larger than one, it will decrement the counter. If the 
     *  counter equals one, it pops the lock from the list and releases the 
     *  lock.
     */
    void pop_and_unlock( CHNL_MTX_TYPE_ *lock );

    /**
     *  \brief Acquire all the channel locks in the list.
     *
     *  Acquire all the locks from the beginning to the end.
     */ 
    void lock_all( void );

    /**
     *  \brief Release all the channel locks in the list.
     *
     *  Release all the locks from the end to the beginning.
     */
    void unlock_all( void );

  private:
    /**
     *  \brief A list of acquired channel locks.
     */
    std::list<sc_chnl_lock*> queue;
};

/**************************************************************************//**
 *  \class sc_timestamp
 *
 *  \brief A time stamp combining timed cycles and delta cycles.
 *
 *  This class implements the time stamp of a process which contains both timed
 *  cycle counts and delta cycle counts. It records the local time of a process
 *  and is used in the Out-of-Order simulation.
 *****************************************************************************/
// 08/06/2015 GL.
class sc_timestamp
{
public:

    /**
     *  \brief The data type of delta cycles.
     */
    typedef sc_dt::uint64 value_type;

    // constructors

    sc_timestamp();
    sc_timestamp( sc_time, value_type );
    sc_timestamp( long long, int );
    sc_timestamp( const sc_timestamp& );

    // assignment operator

    /**
     *  \brief Overload assignment operator.
     */
    sc_timestamp& operator = ( const sc_timestamp& );

    // relational operators

    /**
     *  \brief Overload == operator.
     */
    bool operator == ( const sc_timestamp& ) const;

    /** 
     *  \brief Overload != operator
     */
    bool operator != ( const sc_timestamp& ) const;

    /**
     *  \brief Overload < operator.
     */
    bool operator <  ( const sc_timestamp& ) const;

    /**
     *  \brief Overload <= operator.
     */
    bool operator <= ( const sc_timestamp& ) const;

    /**
     *  \brief Overload > operator.
     */
    bool operator >  ( const sc_timestamp& ) const;

    /**
     *  \brief Overload >= operator.
     */
    bool operator >= ( const sc_timestamp& ) const;
 
    // arithmetic operator

    /**
     *  \brief Overload + operator.
     */
    sc_timestamp operator + ( const sc_timestamp& );

    // get member variables

    /**
     *  \brief Get the value of timed cycles.
     */
    const sc_time& get_time_count() const;

    /**
     *  \brief Get the value of delta cycles.
     */
    value_type get_delta_count() const;

    /**
     *  \brief Check whether the time stamp is infinite.
     */
    bool get_infinite() const;

    void show() const;
    std::string to_string() const;
    /**
     *  \brief The value of timed cycles.
     */
    sc_time    m_time_count;

    /**
     *  \brief The value of delta cycles.
     */
    value_type m_delta_count;

private:

    

    /**
     *  \brief The flag of infinite time stamp.
     */
    bool       m_infinite;
};

bool heap_cmp(sc_process_b*& s, sc_process_b*& t); //DM 08/15/2019 for event delivery

/**************************************************************************//**
 *  \class sc_process_b
 *
 *  \brief User initiated dynamic process support.
 *
 *  This class implements the base class for a threaded process_base process 
 *  whose semantics are provided by the true virtual method semantics(). 
 *  Classes derived from this one will provide a version of semantics which 
 *  implements the desired semantics. See the sc_spawn_xxx classes below.
 *
 *  Notes:
 *    (1) Object instances of this class maintain a reference count of 
 *        outstanding handles. When the handle count goes to zero the 
 *        object will be deleted. 
 *    (2) Descriptions of the methods and operators in this class appear with
 *        their implementations.
 *    (3) The m_sticky_reset field is used to handle synchronous resets that
 *        are enabled via the sc_process_handle::sync_reset_on() method. These
 *        resets are not generated by a signal, but rather are modal by 
 *        method call: sync_reset_on - sync_reset_off.    
 *****************************************************************************/
class sc_process_b : public sc_object { 
    friend class Invoker; //DM 05/16/2019

    friend class sc_simcontext;      // Allow static processes to have base.
    friend class sc_cthread_process; // Child can access parent.
    friend class sc_method_process;  // Child can access parent.
    friend class sc_process_handle;  // Allow handles to modify ref. count.
    friend class sc_thread_process;  // Child can access parent.

    friend class sc_object;
    friend class sc_port_base;
    friend class sc_runnable;
    friend class sc_sensitive;
    friend class sc_sensitive_pos;
    friend class sc_sensitive_neg;
    friend class sc_module;

    // 04/07/2015 GL: a new sc_channel class is derived from sc_module
    friend class sc_channel;

    friend class sc_report_handler;
    friend class sc_reset;
    friend class sc_reset_finder;
    friend class sc_unwind_exception;

    friend const char* sc_gen_unique_name( const char*, bool preserve_first );
    friend sc_process_handle sc_get_current_process_handle();
    friend void sc_thread_cor_fn( void* arg );
    friend bool timed_out( sc_simcontext* );
	
    friend bool heap_cmp(sc_process_b*&, sc_process_b*&);
	

  public:
  
  
  //19:23 2018/7/7 ZC
  //for both sc_event_and_list and sc_event_or_list
  
  /*
  / if any event in event_list is triggered
  / initially false;
  / when the thread wakes up, set it to false again
  / this is used to set the initial value of wake_up_time_for_event_list,
  / which is the first event's time
  */
  
  bool event_list_member_triggered;
  
  /*
  / used for both andlist and orlist
  / 1) for andlist, it is the max value of all the events
  / 2) for orlist, it is the min value of all the events
  */

  sc_timestamp wake_up_time_for_event_list;  
  
  //end
  

	/**
     * \brief sets the upcoming segment ids
     *        TS 07/08/17
     */
    void set_upcoming_segment_ids(int *segment_ids)
    {
      this->segment_ids = segment_ids;
    }

    /**
     * \brief returns the upcoming segment ids
     *        TS 07/08/17
     */
    int* get_upcoming_segment_ids()
    {
      return segment_ids;
    }

    /**
     * \brief stores the upcoming segment ids
     *        TS 07/08/17
     */
    int *segment_ids;


    /**
     * \brief sets the upcoming socket id
     *        ZC 10:30 2018/10/31
     */
    void set_upcoming_socket_id(int socket_id)
    {
      this->socket_id_ = socket_id;
    }

    /**
     * \brief returns the upcoming socket id
     *        ZC 10:31 2018/10/31
     */
    int get_upcoming_socket_id()
    {
      return this->socket_id_;
    }

    /**
     * \brief stores the upcoming socket id
     *        ZC 10:31 2018/10/31
     */
    int socket_id_;


    /**
     * \brief increase the offset
     *        ZC 10:30 2018/10/31
     */
    void increase_offset(int offset)
    {
      this->seg_offset_ += offset;
    }

    /**
     * \brief decrease the offset
     *        ZC 10:31 2018/10/31
     */
    void decrease_offset(int offset)
    {
      this->seg_offset_ -= offset;
    }

    /**
     * \brief returns the offset
     *        ZC 10:31 2018/10/31
     */
    int get_offset()
    {
      return this->seg_offset_;
    }
	
    
    enum process_throw_type {
        THROW_NONE = 0,
	THROW_KILL,
        THROW_USER,
        THROW_ASYNC_RESET,
        THROW_SYNC_RESET
    };

    enum process_state {
	ps_bit_disabled = 1,      // process is disabled.
	ps_bit_ready_to_run = 2,  // process is ready to run.
        ps_bit_suspended = 4,     // process is suspended.
	ps_bit_zombie = 8,        // process is a zombie.
        ps_normal = 0             // must be zero.
    };

    enum reset_type {             // types for sc_process_b::reset_process()
        reset_asynchronous = 0,   // asynchronous reset.
        reset_synchronous_off,    // turn off synchronous reset sticky bit.
        reset_synchronous_on      // turn on synchronous reset sticky bit.
    };

    enum trigger_t
    {
        STATIC,
        EVENT,
        OR_LIST,
        AND_LIST,
        TIMEOUT,
        EVENT_TIMEOUT,
        OR_LIST_TIMEOUT,
        AND_LIST_TIMEOUT
    };

  public: 
    sc_process_b( const char* name_p, bool is_thread, bool free_host, 
        SC_ENTRY_FUNC method_p, sc_process_host* host_p, 
        const sc_spawn_options* opt_p );

  protected:
    // may not be deleted manually (called from destroy_process())
    virtual ~sc_process_b();

  public:
    inline int current_state() { return m_state; }
    bool dont_initialize() const { return m_dont_init; }
    virtual void dont_initialize( bool dont );
    std::string dump_state() const;
    const ::std::vector<sc_object*>& get_child_objects() const;
    inline sc_curr_proc_kind proc_kind() const;
    sc_event& reset_event();
    sc_event& terminated_event();

    /**
     *  \brief Acquire a new channel lock or increment the lock counter.
     * 
     *  Acquire a new channel lock and push it to the end of the list
     *  m_acq_chnl_lock_queue, or increment the lock counter of the 
     *  corresponding channel lock in the list.
     */
    // 02/16/2015 GL.
    void lock_and_push( CHNL_MTX_TYPE_ *lock );

    /**
     *  \brief Release a channel lock or decrement the lock counter.
     *
     *  Release the channel lock at the end of the list m_acq_chnl_lock_queue
     *  if its lock counter equals one, or decrement its lock counter.
     */
    // 02/16/2015 GL.
    void pop_and_unlock( CHNL_MTX_TYPE_ *lock );

    /**
     *  \brief Acquire all the channel locks.
     *
     *  Acquire all the channel locks in the list m_acq_chnl_lock_queue, from
     *  the beginning to the end.
     */
    // 02/16/2015 GL.
    void lock_all_channels( void );

    /**
     *  \brief Release all the channel locks.
     *
     *  Release all the channel locks in the list m_acq_chnl_lock_queue, from 
     *  the end of the beginning.
     */
    // 02/16/2015 GL.
    void unlock_all_channels( void );

    /**
     *  \brief Set the current segment ID of this process.
     */
    // 08/12/2015 GL.
    int get_segment_id();

    /** 
     *  \brief Get the current segment ID of this process.
     */
    // 08/12/2015 GL.
    void set_segment_id( int id );

    /**
     *  \brief Set the local time stamp of this process.
     */
    // 08/12/2015 GL.
    const sc_timestamp& get_timestamp();

    /**
     *  \brief Get the local time stamp of this process.
     */
    // 08/12/2015 GL.
    void set_timestamp( const sc_timestamp& ts );

    /**
     *  \brief Set the instance ID of this process.
     */
    // 09/01/2015 GL.
    int get_instance_id();

    /**
     *  \brief Get the instance ID of this process.
     */
    void set_instance_id( int id );

  public:
    static inline sc_process_handle last_created_process_handle();
    void add_sensitivity_event(const sc_event& e);
    std::string event_names();
  protected:
    virtual void add_child_object( sc_object* );
    void add_static_event( const sc_event& );
    bool dynamic() const { return m_dynamic_proc; }
    const char* gen_unique_name( const char* basename_, bool preserve_first );
    inline sc_report* get_last_report() { return m_last_report_p; }
    inline bool is_disabled() const;
    inline bool is_runnable() const;
    static inline sc_process_b* last_created_process_base();
    virtual bool remove_child_object( sc_object* );
    void remove_dynamic_events( bool skip_timeout = false );
    void remove_static_events();
    inline void set_last_report( sc_report* last_p )
        {  
            delete m_last_report_p;
            m_last_report_p = last_p;
        }
    inline bool timed_out() const;
    void report_error( const char* msgid, const char* msg = "" ) const;
    void report_immediate_self_notification() const;

  protected: // process control methods:
    virtual void disable_process(
        sc_descendant_inclusion_info descendants = SC_NO_DESCENDANTS ) = 0;
    void disconnect_process();
    virtual void enable_process(
        sc_descendant_inclusion_info descendants = SC_NO_DESCENDANTS ) = 0;
    inline void initially_in_reset( bool async );
    inline bool is_unwinding() const;
    inline bool start_unwinding();
    inline bool clear_unwinding();
    virtual void kill_process(
        sc_descendant_inclusion_info descendants = SC_NO_DESCENDANTS ) = 0;
    void reset_changed( bool async, bool asserted );
    void reset_process( reset_type rt,
        sc_descendant_inclusion_info descendants = SC_NO_DESCENDANTS );
    virtual void resume_process(
        sc_descendant_inclusion_info descendants = SC_NO_DESCENDANTS ) = 0;
    virtual void suspend_process(
        sc_descendant_inclusion_info descendants = SC_NO_DESCENDANTS ) = 0;
    virtual void throw_user( const sc_throw_it_helper& helper,
        sc_descendant_inclusion_info descendants = SC_NO_DESCENDANTS ) = 0;
    virtual void throw_reset( bool async ) = 0;
    virtual bool terminated() const;
    void trigger_reset_event();

  private:
    void        delete_process();
    inline void reference_decrement();
    inline void reference_increment();

  protected:
    inline void semantics();

    // debugging stuff:

  public:
    const char*                 file;
    int                         lineno;
    int                         proc_id;
	/**
     *  \brief The name of this process.
     */
    // 03/10/2017 ZC.
	//const char*				process_name; 		// not needed, use name() to get process name
	int							m_process_state; //0 running 1 ready 2 waiting 3 waitfor time
                                                 //4 finished 5 paused
    sc_event_or_list*            m_sensitivity_events;
  protected:
    int                          m_active_areset_n; // number of aresets active.
    int                          m_active_reset_n;  // number of resets active.
    bool                         m_dont_init;       // true: no initialize call.
    bool                         m_dynamic_proc;    // true: after elaboration.
    const sc_event*              m_event_p;         // Dynamic event waiting on.
	
    int                          m_event_count;     // number of events.
    const sc_event_list*         m_event_list_p;    // event list waiting on.
    sc_process_b*                m_exist_p;         // process existence link.
    bool                         m_free_host;       // free sc_semantic_host_p.
    bool                         m_has_reset_signal;  // has reset_signal_is.
    bool                         m_has_stack;       // true is stack present.
    bool                         m_is_thread;       // true if this is thread.
    sc_report*                   m_last_report_p;   // last report this process.
    sc_name_gen*                 m_name_gen_p;      // subprocess name generator
    sc_curr_proc_kind            m_process_kind;    // type of process.
    int                          m_references_n;    // outstanding handles.
    std::vector<sc_reset*>       m_resets;          // resets for process.
    sc_event*                    m_reset_event_p;   // reset event.
    sc_event*                    m_resume_event_p;  // resume event.
    sc_process_b*                m_runnable_p;      // sc_runnable link
    sc_process_host*             m_semantics_host_p;   // host for semantics.
    SC_ENTRY_FUNC                m_semantics_method_p; // method for semantics.
    int                          m_state;           // process state.
    std::vector<const sc_event*> m_static_events;   // static events waiting on.
    bool                         m_sticky_reset;    // see note 3 above.
    sc_event*                    m_term_event_p;    // terminated event.
    sc_throw_it_helper*          m_throw_helper_p;  // what to throw.
    process_throw_type           m_throw_status;    // exception throwing status
    bool                         m_timed_out;       // true if we timed out.
    sc_event*                    m_timeout_event_p; // timeout event.
    trigger_t                    m_trigger_type;    // type of trigger using.
    bool                         m_unwinding;       // true if unwinding stack.

    /**
     *  \brief A list of channel locks acquired by this process.
     */
    // 02/16/2015 GL.
    sc_acq_chnl_lock_queue       m_acq_chnl_lock_queue;

    /**
     *  \brief The current segment ID of this process.
     */
    // 08/12/2015 GL.
    int                          m_segment_id;

    /**
     *  \brief The local time stamp of this process.
     */
    // 08/12/2015 GL.
    sc_timestamp                 m_timestamp;

    /**
     *  \brief The instance ID of this process.
     */
    // 09/01/2015 GL.
    int                          m_instance_id;
	
	 
    int seg_offset_;
	
	
  protected:
    static sc_process_b* m_last_created_process_p; // Last process created.
  public:
    sc_timestamp possible_wakeup_time;

    // 05/13/2019 DM
  public:
    bool invoker;
    sc_process_b* cur_invoker_method_handle;

  //DM 08/14/2019
  private:
    sc_process_b* PWT_defining_proc; //DM possible wakeup time defining process
    std::unordered_set<sc_process_b*> proc_defined; //DM 08/17/2019 processes that this proc defines
    sc_timestamp min_PWT_distance;
    int prev_seg_id;
    int get_previous_seg_id();
    int group_no;

};

typedef sc_process_b sc_process_b;  // For compatibility.


//------------------------------------------------------------------------------
//"sc_process_b::XXXX_child_YYYYY"
//
// These methods provide child object support.
//------------------------------------------------------------------------------
inline void
sc_process_b::add_child_object( sc_object* object_p )
{
    sc_object::add_child_object( object_p );
    reference_increment();
}



inline bool
sc_process_b::remove_child_object( sc_object* object_p )
{
    if ( sc_object::remove_child_object( object_p ) ) {
	    reference_decrement();
            return true;
    }
    else
    {
        return false;
    }
}

inline const ::std::vector<sc_object*>&
sc_process_b::get_child_objects() const
{
    return m_child_objects;
}


//------------------------------------------------------------------------------
//"sc_process_b::initially_in_reset"
//
// This inline method is a callback to indicate that a reset is active at
// start up. This is because the signal will have been initialized before
// a reset linkage for it is set up, so we won't get a reset_changed()
// callback.
//     async = true if this an asynchronous reset.
//------------------------------------------------------------------------------
inline void sc_process_b::initially_in_reset( bool async )
{
    if ( async ) 
        m_active_areset_n++;
    else
        m_active_reset_n++;
}

//------------------------------------------------------------------------------
//"sc_process_b::is_disabled"
//
// This method returns true if this process is disabled. 
//------------------------------------------------------------------------------
inline bool sc_process_b::is_disabled() const
{
    return (m_state & ps_bit_disabled) ? true : false;
}

//------------------------------------------------------------------------------
//"sc_process_b::is_runnable"
//
// This method returns true if this process is runnable. That is indicated
// by a non-zero m_runnable_p field.
//------------------------------------------------------------------------------
inline bool sc_process_b::is_runnable() const
{
    return m_runnable_p != 0;
}

//------------------------------------------------------------------------------
//"sc_process_b::is_unwinding"
//
// This method returns true if this process is unwinding from a kill or reset.
//------------------------------------------------------------------------------
inline bool sc_process_b::is_unwinding() const
{
    return m_unwinding;
}

//------------------------------------------------------------------------------
//"sc_process_b::start_unwinding"
//
// This method flags that this object instance should start unwinding if the
// current throw status requires an unwind. 
//
// Result is true if the flag is set, false if the flag is already set.
//------------------------------------------------------------------------------
inline bool sc_process_b::start_unwinding()
{
    if ( !m_unwinding )
    {
	switch( m_throw_status )
	{
	  case THROW_KILL:
	  case THROW_ASYNC_RESET:
	  case THROW_SYNC_RESET:
	    m_unwinding = true;
	     return true;
	  case THROW_USER:
	   default:
	     break;
	 }
    }
    return false;
}

//------------------------------------------------------------------------------
//"sc_process_b::clear_unwinding"
//
// This method clears this object instance's throw status and always returns
// true.
//------------------------------------------------------------------------------
inline bool sc_process_b::clear_unwinding()
{
    m_unwinding = false;
    return true;
}


//------------------------------------------------------------------------------
//"sc_process_b::last_created_process_base"
//
// This virtual method returns the sc_process_b pointer for the last
// created process. It is only used internally by the simulator.
//------------------------------------------------------------------------------
inline sc_process_b* sc_process_b::last_created_process_base()
{
    return m_last_created_process_p;
}



//------------------------------------------------------------------------------
//"sc_process_b::proc_kind"
//
// This method returns the kind of this process.
//------------------------------------------------------------------------------
inline sc_curr_proc_kind sc_process_b::proc_kind() const
{
    return m_process_kind;
}


//------------------------------------------------------------------------------
//"sc_process_b::reference_decrement"
//
// This inline method decrements the number of outstanding references to this 
// object instance. If the number of references goes to zero, this object
// can be deleted in "sc_process_b::delete_process()".
//------------------------------------------------------------------------------
inline void sc_process_b::reference_decrement()
{
    m_references_n--;
    if ( m_references_n == 0 ) delete_process();
}


//------------------------------------------------------------------------------
//"sc_process_b::reference_increment"
//
// This inline method increments the number of outstanding references to this 
// object instance.
//------------------------------------------------------------------------------
inline void sc_process_b::reference_increment()
{
    assert(m_references_n != 0);
    m_references_n++;
}

//------------------------------------------------------------------------------
//"sc_process_b::semantics"
//
// This inline method invokes the semantics for this object instance. 
// We check to see if we are initially in reset and then invoke the
// process semantics.
//
// Notes:
//   (1) For a description of the process reset mechanism see the top of 
//       the file sc_reset.cpp.
//------------------------------------------------------------------------------
struct scoped_flag
{
    scoped_flag( bool& b ) : ref(b){ ref = true;  }
    ~scoped_flag()                 { ref = false; }
    bool& ref;
};
inline void sc_process_b::semantics()
{

    // within this function, the process has a stack associated

    scoped_flag scoped_stack_flag( m_has_stack );

    assert( m_process_kind != SC_NO_PROC_ );

    // Determine the reset status of this object instance and potentially
    // trigger its notify event:

    // See if we need to trigger the notify event:

    if ( m_reset_event_p && 
         ( (m_throw_status == THROW_SYNC_RESET) || 
           (m_throw_status == THROW_ASYNC_RESET) )
    ) {
        trigger_reset_event();
    }

    // Set the new reset status of this object based on the reset counts:

    m_throw_status = m_active_areset_n ? THROW_ASYNC_RESET : 
        ( m_active_reset_n  ?  THROW_SYNC_RESET : THROW_NONE);

    // Dispatch the actual semantics for the process:

#   ifndef SC_USE_MEMBER_FUNC_PTR
        m_semantics_method_p->invoke( m_semantics_host_p );
#   else
        (m_semantics_host_p->*m_semantics_method_p)(); 
#   endif
}


//------------------------------------------------------------------------------
//"sc_process_b::terminated"
//
// This inline method returns true if this object has terminated.
//------------------------------------------------------------------------------
inline bool sc_process_b::terminated() const
{
    return (m_state & ps_bit_zombie) != 0;
}


//------------------------------------------------------------------------------
//"sc_process_b::timed_out"
//
// This inline method returns true if this object instance timed out.
//------------------------------------------------------------------------------
inline bool sc_process_b::timed_out() const
{
    return m_timed_out;
}


} // namespace sc_core

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date: Andy Goodrich, Forte Design Systems, 12 Aug 05
  Description of Modification: This is the rewrite of process support. It 
                               contains some code from the original
                               sc_process.h by Stan Liao, and the now-defunct
                               sc_process_b.h by Stan Liao and Martin
                               Janssen, all of Synopsys, Inc., It also contains
                               code from the original sc_process_b.h by 
                               Andy Goodrich of Forte Design Systems and
                               Bishnupriya Bhattacharya of Cadence Design
                               Systems.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/

// $Log: sc_process.h,v $
// Revision 1.36  2011/08/26 22:44:30  acg
//  Torsten Maehne: eliminate unused argument warning.
//
// Revision 1.35  2011/08/26 20:46:10  acg
//  Andy Goodrich: moved the modification log to the end of the file to
//  eliminate source line number skew when check-ins are done.
//
// Revision 1.34  2011/08/24 22:05:51  acg
//  Torsten Maehne: initialization changes to remove warnings.
//
// Revision 1.33  2011/08/15 16:43:24  acg
//  Torsten Maehne: changes to remove unused argument warnings.
//
// Revision 1.32  2011/07/24 11:20:03  acg
//  Philipp A. Hartmann: process control error message improvements:
//  (1) Downgrade error to warning for re-kills of processes.
//  (2) Add process name to process messages.
//  (3) drop some superfluous colons in messages.
//
// Revision 1.31  2011/04/13 02:44:26  acg
//  Andy Goodrich: added m_unwinding flag in place of THROW_NOW because the
//  throw status will be set back to THROW_*_RESET if reset is active and
//  the check for an unwind being complete was expecting THROW_NONE as the
//  clearing of THROW_NOW.
//
// Revision 1.30  2011/04/11 22:07:27  acg
//  Andy Goodrich: check for reset event notification before resetting the
//  throw_status value.
//
// Revision 1.29  2011/04/10 22:17:36  acg
//  Andy Goodrich: added trigger_reset_event() to allow sc_process.h to
//  contain the run_process() inline method. sc_process.h cannot have
//  sc_simcontext information because of recursive includes.
//
// Revision 1.28  2011/04/08 22:34:06  acg
//  Andy Goodrich: moved the semantics() method to this file and made it
//  an inline method. Added reset processing to the semantics() method.
//
// Revision 1.27  2011/04/08 18:24:48  acg
//  Andy Goodrich: moved reset_changed() to .cpp since it needs visibility
//  to sc_simcontext.
//
// Revision 1.26  2011/04/01 21:24:57  acg
//  Andy Goodrich: removed unused code.
//
// Revision 1.25  2011/03/28 13:02:51  acg
//  Andy Goodrich: Changes for disable() interactions.
//
// Revision 1.24  2011/03/20 13:43:23  acg
//  Andy Goodrich: added async_signal_is() plus suspend() as a corner case.
//
// Revision 1.23  2011/03/12 21:07:51  acg
//  Andy Goodrich: changes to kernel generated event support.
//
// Revision 1.22  2011/03/08 20:49:31  acg
//  Andy Goodrich: implement coarse checking for synchronous reset - suspend
//  interaction.
//
// Revision 1.21  2011/03/07 17:38:43  acg
//  Andy Goodrich: tightening up of checks for undefined interaction between
//  synchronous reset and suspend.
//
// Revision 1.20  2011/03/06 19:57:11  acg
//  Andy Goodrich: refinements for the illegal suspend - synchronous reset
//  interaction.
//
// Revision 1.19  2011/03/05 19:44:20  acg
//  Andy Goodrich: changes for object and event naming and structures.
//
// Revision 1.18  2011/02/19 08:30:53  acg
//  Andy Goodrich: Moved process queueing into trigger_static from
//  sc_event::notify.
//
// Revision 1.17  2011/02/18 20:27:14  acg
//  Andy Goodrich: Updated Copyrights.
//
// Revision 1.16  2011/02/18 20:10:44  acg
//  Philipp A. Hartmann: force return expression to be a bool to keep MSVC
//  happy.
//
// Revision 1.15  2011/02/17 19:52:45  acg
//  Andy Goodrich:
//    (1) Simplified process control usage.
//    (2) Changed dump_status() to dump_state() with new signature.
//
// Revision 1.14  2011/02/16 22:37:30  acg
//  Andy Goodrich: clean up to remove need for ps_disable_pending.
//
// Revision 1.13  2011/02/13 21:47:37  acg
//  Andy Goodrich: update copyright notice.
//
// Revision 1.12  2011/02/13 21:41:34  acg
//  Andy Goodrich: get the log messages for the previous check in correct.
//
// Revision 1.11  2011/02/13 21:32:24  acg
//  Andy Goodrich: moved sc_process_b::reset_process() implementation
//  from header to cpp file . Added dump_status() to print out the status of a
//  process.
//
// Revision 1.10  2011/02/11 13:25:24  acg
//  Andy Goodrich: Philipp A. Hartmann's changes:
//    (1) Removal of SC_CTHREAD method overloads.
//    (2) New exception processing code.
//
// Revision 1.9  2011/02/04 15:27:36  acg
//  Andy Goodrich: changes for suspend-resume semantics.
//
// Revision 1.8  2011/02/01 21:06:12  acg
//  Andy Goodrich: new layout for the process_state enum.
//
// Revision 1.7  2011/01/25 20:50:37  acg
//  Andy Goodrich: changes for IEEE 1666 2011.
//
// Revision 1.6  2011/01/19 23:21:50  acg
//  Andy Goodrich: changes for IEEE 1666 2011
//
// Revision 1.5  2011/01/18 20:10:45  acg
//  Andy Goodrich: changes for IEEE1666_2011 semantics.
//
// Revision 1.4  2010/07/22 20:02:33  acg
//  Andy Goodrich: bug fixes.
//
// Revision 1.3  2009/05/22 16:06:29  acg
//  Andy Goodrich: process control updates.
//
// Revision 1.2  2008/05/22 17:06:26  acg
//  Andy Goodrich: updated copyright notice to include 2008.
//
// Revision 1.1.1.1  2006/12/15 20:20:05  acg
// SystemC 2.3
//
// Revision 1.11  2006/05/08 17:58:10  acg
// Andy Goodrich: added David Long's forward declarations for friend
//   functions, methods, and operators to keep the Microsoft compiler happy.
//
// Revision 1.10  2006/04/28 23:29:01  acg
//  Andy Goodrich: added an sc_core:: prefix to SC_FUNC_PTR in the
//  SC_MAKE_FUNC_PTR macro to allow its transpareuse outside of the sc_core
//  namespace.
//
// Revision 1.9  2006/04/28 21:52:57  acg
//  Andy Goodrich: changed SC_MAKE_FUNC_PTR to use a static cast to address
//  and AIX issue wrt sc_module's inherited classes.
//
// Revision 1.8  2006/04/20 17:08:17  acg
//  Andy Goodrich: 3.0 style process changes.
//
// Revision 1.7  2006/04/11 23:13:21  acg
//   Andy Goodrich: Changes for reduced reset support that only includes
//   sc_cthread, but has preliminary hooks for expanding to method and thread
//   processes also.
//
// Revision 1.6  2006/03/13 20:26:50  acg
//  Andy Goodrich: Addition of forward class declarations, e.g.,
//  sc_reset, to keep gcc 4.x happy.
//
// Revision 1.5  2006/01/31 20:09:10  acg
//  Andy Goodrich: added explaination of static vs dynamic waits to
//  sc_process_b::trigger_static.
//
// Revision 1.4  2006/01/24 20:49:05  acg
// Andy Goodrich: changes to remove the use of deprecated features within the
// simulator, and to issue warning messages when deprecated features are used.
//
// Revision 1.3  2006/01/13 18:44:30  acg
// Added $Log to record CVS changes into the source.

#endif // !defined(sc_process_h_INCLUDED)
