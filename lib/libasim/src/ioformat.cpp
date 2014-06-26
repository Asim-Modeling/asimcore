/*
 * ****************************************************************
 * Copyright (c) 2014, Intel Corporation
 *
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are 
 * met:
 * 
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright 
 *   notice, this list of conditions and the following disclaimer in the 
 *   documentation and/or other materials provided with the distribution.
 * - Neither the name of the Intel Corporation nor the names of its 
 *   contributors may be used to endorse or promote products derived from 
 *   this software without specific prior written permission.
 *  
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 * @author Artur Klauser
 * @brief Implementation of I/O Formatting Objects
 * @see @ref IoFormat_Documentation "I/O Format Documentation"
 * @ingroup IoFormat
 */

// generic
#include <stdlib.h>

// ASIM core
#include "asim/ioformat.h"
#include <stdlib.h>

namespace IoFormat {

  // commonly used formats
  /// global format for "%x" printing
  const Format fmt_x = "x";
  /// global format for "%p" printing
  const Format fmt_p = "p";
  /// global format for printing bool as true/false
  const Format fmt_b = Format().setf(ios_base::boolalpha);

  /**
   * @brief Parse printf-like format string.
   *
   * @anchor fmtstr
   * Set the state of this Format object from a printf-like format string.
   * @note
   * - All formatting options of this Format object are reset to their
   *   default values before the format string is parsed.
   * - Only some options of the format string are meaningful,
   *   i.e. not all printf-like format strings are allowed here, in
   *   particular the format string options that indicate argument type,
   *   rather than argument formatting are not supported.
   * 
   * @param fmtstr printf-like format string to parse\n
   *   allowed regexp: //([#0-+]*)([0-9]*)(\.[0-9]*)?([diuoXxEeFfGgp])?\n
   *   -# optional initial format flag modifier
   *   -# optional minimum field width
   *   -# optional dot followed by floating point precision
   *   -# optional trailing conversion specifier
   */
  Format&
  Format::format (
    const char* const fmtstr)
  {
    if (fmtstr == NULL) {
      throw Error::Fmtstr("missing format string");
    }

    const char *p = fmtstr;
    // set up default format of printf format string spec
    init();
    //
    // parse initial format flag modifier characters
    for (bool done = false; ! done; p++) {
      switch (*p) {
        case '#':
          showbase().showpoint();
          break;
        case '0':
          if ((f_flags & ios_base::adjustfield) != ios_base::left) {
            // left adjustment ignores 0-fill
            fill('0');
          }
          break;
        case '-':
          left().fill(' '); // ignore 0-fill if was set before
          break;
        case '+':
          showpos();
          break;
        default:
          done = true;
          break;
      }
    }
    p--; // unget the token we read too far
    //
    // parse optional field width specifier
    if (isdigit(*p)) {
      width (strtol(p, (char**) &p, 10));
    }
    //
    // parse optional precision specifier
    if (*p == '.') {
      ++p;
      precision (strtol(p, (char**) &p, 10));
    }
    if (*p == '\0') {
      return *this;
    }
    //
    // parse conversion specifier
    //
    // Note: we only understand the conversion specifiers that
    // are required to determine the format of output - we don't
    // care about the ones that declare argument types since C++
    // takes care of type safety by itself.
    // E.g. "x" is meaningfull because it sets hex output, but
    // "l" is not meaningful because we don't care about type (long)
    switch(*p++) {
      case 'd':
      case 'i':
      case 'u':
        dec();
        break;
      case 'o':
        oct();
        break;
      case 'X':
        uppercase();
        // intentional fallthrough
      case 'x':
        hex();
        break;
      case 'E':
        uppercase();
        // intentional fallthrough
      case 'e':
        scientific();
        break;
      case 'F':
        uppercase();
        // intentional fallthrough
      case 'f':
        fixed();
        break;
      case 'G':
        uppercase();
        // intentional fallthrough
      case 'g':
        // default behavior
        break;
      case 'p':
        showbase().hex();
        break;
      default:
        // unsupported type designation - complain
        throw Error::Fmtstr("invalid format string", fmtstr);
    }
    if (*p != '\0') {
      // trailing garbage left - complain
      throw Error::Fmtstr("trailing garbage in format string", fmtstr);
    }
    return *this;
  }
}
