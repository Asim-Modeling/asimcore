/*
 * Copyright (C) 2003-2006 Intel Corporation
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/**
 * @file
 * @author David Goodwin, Shubu Mukherjee, Artur Klauser
 * @brief Printing of warnings, error messages, and assertions.
 */

#ifndef ASIM_MESG_H
#define ASIM_MESG_H

// generic
#include <iostream>
#include <fstream>

#include <pthread.h>

// Un-comment the following line in case you want thread-safe ASIM_MESG
//#define THREAD_SAFE_ASIM_MESG

#ifdef THREAD_SAFE_ASIM_MESG
    #define lockMesgMutex(p)   pthread_mutex_lock(p)
    #define unlockMesgMutex(p) pthread_mutex_unlock(p)
#else
    #define lockMesgMutex(p)
    #define unlockMesgMutex(p)
#endif

// ASIM core
#include "asim/syntax.h"
#include "asim/ioformat.h"

namespace iof = IoFormat;
using namespace iof;
using namespace std;

/*
 * Class ASIM_MESG
 *
 * Class for outputing varargs strings.
 *
 */
typedef class ASIM_MESG_CLASS *ASIM_MESG;
class ASIM_MESG_CLASS
{
  private:
    // variables
    ostream & out;             ///< output file stream for messages
    const string prefix;       ///< prefix string for each message
    const bool printLocation;  ///< prepend message location file:line
    const bool terminate;      ///< abort program after message
    bool privateStream;        ///< this object owns the out stream

    // constructors / destructors / initializers

    /// Calling the default constructor on this object is meaningless,
    /// so we protect against it by making it private.
    ASIM_MESG_CLASS ();

    /// Return an output stream to use for filename.
    ostream & FilenameToOstream (const string file);

  public:
    // constructors / destructors / initializers

    /// ostream constructor
    ASIM_MESG_CLASS (
        ostream & _out,
        const string & _prefix = "",
        const bool _printLocation = false,
        const bool _terminate = false);

    /// filename constructor
    ASIM_MESG_CLASS (
        const string & _name,
        const string & _prefix = "",
        const bool _printLocation = false,
        const bool _terminate = false);

    ~ASIM_MESG_CLASS ();

    // message print methods

    /// prepare printing of a message
    ostream & Prepare (const char * const file = NULL, const INT32 line = -1);
    /// finish printing of a message
    void Finish(void);
};

// Global ASIM message objects for errors, warnings, and assertions
extern ASIM_MESG_CLASS asim_warn;
extern ASIM_MESG_CLASS asim_error;
extern ASIM_MESG_CLASS asim_assert;

// Global ASIM message mutex for thread safe logging
extern pthread_mutex_t asim_mesg_mutex;

//------------------------------------------------------------------------------
//
// Assertion macros
//
// Usage:
//
// WARN( cond )
// WARN( cond, msg )
// ASSERT( cond )
// ASSERT( cond, msg )
// VERIFY( cond )
// VERIFY( cond, msg )
// ASIMWARNING( mgs )
// ASIMERROR( msg )
//
// When 'condition' is not true, these print
// the file, line number, current cycle, msg (if provided)
// and exit (except WARN-ings).
//
// ASSERT is checked only when compiled with ASIM_ENABLE_ASSERTIONS defined.
// VERIFY is always checked.
//
// For backwards compatibility:
// WARNX( cond )     - same as WARN( cond )
// ASSERTX( cond )   - same as ASSERT( cond )
// VERIFYX( cond )   - same as VERIYX( cond )
//
//------------------------------------------------------------------------------
#define ASSERTION_COND_MSG( type, condition, ... )  if (! (condition)) ASSERTION_MSG( type, ## __VA_ARGS__ )
#define ASSERTION_MSG( ... )  MSG_ADDON_CHOOSER( __VA_ARGS__, MSG_WITH_ADDON, MSG_WITHOUT_ADDON )( __VA_ARGS__ )
#define MSG_ADDON_CHOOSER( type, skip, impl, ... )  impl
#define MSG_WITH_ADDON( type, msg )  ASSERTION_MSG_DUMP( type, << msg << endl )
#define MSG_WITHOUT_ADDON( type )    ASSERTION_MSG_DUMP( type, )
#define ASSERTION_MSG_DUMP( type, addon ) \
    { \
        asim_ ## type.Prepare(__FILE__,__LINE__) addon; \
        asim_ ## type.Finish(); \
    }

// Assertion macros implementation
#ifdef ASIM_ENABLE_ASSERTIONS

#define WARN( condition, ... )     ASSERTION_COND_MSG( warn, condition, ## __VA_ARGS__ )
#define ASSERT( condition, ... )   ASSERTION_COND_MSG( assert, condition, ## __VA_ARGS__ )
// Backwards compatibility
#define WARNX( condition )         ASSERTION_COND_MSG( warn, condition )
#define ASSERTX( condition )       ASSERTION_COND_MSG( assert, condition )

#else // ASIM_ENABLE_ASSERTIONS

#define WARN( condition, ... )
#define ASSERT( condition, ... )
// Backwards compatibility
#define WARNX( condition )
#define ASSERTX( condition )

#endif // ASIM_ENABLE_ASSERTIONS

// same as ASSERT macros but can't be turned off
#define VERIFY( condition, ... )   ASSERTION_COND_MSG( assert, condition, ## __VA_ARGS__ )
// Backwards compatibility
#define VERIFYX( condition )       ASSERTION_COND_MSG( assert, condition )

// Macros for errors and warnings.
#define ASIMERROR( msg )           ASSERTION_MSG( error, msg )
#define ASIMWARNING( msg )         ASSERTION_MSG( warn, msg )

//------------------------------------------------------------------------------
#endif /* ASIM_MESG_H */
