/*
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


/*****************************************************************************
 *
 * @Brief Atomic counter type.  Useful for threadsafe reference counting
 *
 * @author FSF, Frank Mayhar, Kenneth C. Barr.  See note.
 * @brief Wrappers around atomic x86 code giving the effect of atomic increments/decrements/assignments.
 * @warning this is (temporarily) GPL'd.  When our build includes
 * atomicity.h and that file gets 64b extensions, you shouldn't need
 * to duplicate that file's code here.
 * 
 * @note 
 * NMSTL (nmstl.sourceforge.net) -- LGPL -- introduced me to the 32-bit version
 * of this class, but I think they just use code from recent libstdc++
 * (which is makes this GPL, too, I think)
 *
 * Adapted by Ken Barr based on code from Frank Mayhar (http://www.exit.com/blog/technotes/)
 * for 64 bit counters; had to special-case subtraction and merge a copy of
 * read_val into the add/sub for exchanges.
 *
 * @todo 
 *  is ASIM_ENABLE_MM_DEBUG the best way to turn on asserts?
 *  get these 64b changes into the official atomicity.h so that we can #include one file and not be "infected" by GPL here.
 *
 *****************************************************************************/

#ifndef ASIM_ATOMIC_H
#define ASIM_ATOMIC_H

#include "asim/syntax.h"
#include "asim/mesg.h"

#define ll_low(x)       *(((unsigned int*)&(x))+0)
#define ll_high(x)      *(((unsigned int*)&(x))+1)


//
// MemBarrier() is a simple barrier, currently for x86 only.  It
// guarantees memory synchronization using a locked reference to the stack.
// No sempahore is updated.  The function can be used on its own without
// a surrounding class since there is no need to allocate a specific
// memory location.
//
static inline void
__attribute__ ((__unused__))
MemBarrier(void)
{
#if __WORDSIZE < 64
    __asm__ __volatile__ ("lock; addl $0,0(%%esp)": : :"memory");
#else
    __asm__ __volatile__ ("lock; addl $0,0(%%rsp)": : :"memory");
#endif
}


//
// CpuPause() is a pause instruction for x86 only.  It reduces power in
// spin loops and gives priority to active threads among a group of
// hyperthreaded streams.
//
static inline void
__attribute__ ((__unused__))
CpuPause(void)
{
	__asm__ __volatile__("rep;nop": : :"memory");
}



//
// Compare and exchange.  All compare and exchange functions return a boolean
// result, true if the exchange happens.
//

inline bool
__attribute__ ((__unused__))
CompareAndExchangeU32(
    volatile UINT32 *mem,
    UINT32 oldValue,
    UINT32 newValue)
{
    return __sync_bool_compare_and_swap(mem, oldValue, newValue);
}

inline bool
__attribute__ ((__unused__))
CompareAndExchangeU64(
    volatile UINT64 *mem,
    UINT64 oldValue,
    UINT64 newValue)
{
    return __sync_bool_compare_and_swap(mem, oldValue, newValue);
}

#if defined INT128_AVAIL && !defined __ICC
inline bool
__attribute__ ((__unused__))
CompareAndExchangeU128(
    volatile UINT128 *mem,
    UINT128 oldValue,
    UINT128 newValue)
{
    UINT8 didXchg;

#if __WORDSIZE >= 64
   // GNU assembler doesn't know about cmpxchg16b but assembles the 8b
    // version as a cmpxchg16b when the rex64 prefix is set.  This can be
    // changed when gas is fixed.
    __asm__ __volatile__("lock; rex64 cmpxchg8b (%1)\n\t"
                         "sete %0"
                             : "=a"(didXchg), "+S"(mem)
                             : "b"(UINT64(newValue)),
                               "c"(UINT64(newValue >> 64)),
                               "a"(UINT64(oldValue)),
                               "d"(UINT64(oldValue >> 64))
                             : "memory", "cc");
#else
    ASIMERROR("CompareAndExchangeU128 doesn't work in 32 bit mode");
#endif
    return didXchg;
}

#endif // INT128_AVAIL

static inline bool
CompareAndExchange(
    UINT32 *mem,
    UINT32 oldValue,
    UINT32 newValue)
{
    return CompareAndExchangeU32(mem, oldValue, newValue);
}

static inline bool
CompareAndExchange(
    UINT64 *mem,
    UINT64 oldValue,
    UINT64 newValue)
{
    return CompareAndExchangeU64(mem, oldValue, newValue);
}

#ifdef INT128_AVAIL 
static inline bool
CompareAndExchange(
    UINT128 *mem,
    UINT128 oldValue,
    UINT128 newValue)
{
#ifdef __ICC
    UINT8 didXchg;
    __asm__ __volatile__("lock; rex64 cmpxchg8b (%1)\n\t"
                         "sete %0"
                             : "=a"(didXchg), "+S"(mem)
                             : "b"(newValue.low),
                               "c"(newValue.high),
                               "a"(oldValue.low),
                               "d"(oldValue.high)
                             : "memory", "cc");
    return didXchg;
#else
    return CompareAndExchangeU128(mem, oldValue, newValue);
#endif
}
#endif

template <class T>
static inline bool
CompareAndExchange(
    T *mem,
    T oldValue,
    T newValue)
{
    if (sizeof(T) == 8)
    {
        return CompareAndExchangeU64((UINT64 *)mem,
                                     (UINT64)oldValue,
                                     (UINT64)newValue);
    }
    else if (sizeof(T) == 4)
    {
        return CompareAndExchangeU32((UINT32 *)mem,
                                     (UINT32)oldValue,
                                     (UINT32)newValue);
    }
}


#if defined(__GLIBCPP__) && __GLIBCPP__ >= 20020814
#  include <bits/atomicity.h>
#else

// Routine from STL: bits/atomicity.h.  Currently for
// i386 only
typedef int _Atomic_word;


static inline _Atomic_word 
__attribute__ ((__unused__))
__exchange_and_add (volatile _Atomic_word *__mem, int __val)
{
    _Atomic_word __result;
    __asm__ __volatile__ ("lock; xaddl %0,%2"
                          : "=r" (__result) 
                          : "0" (__val), "m" (*__mem) 
                          : "memory");
    return __result;
}


static inline void
__attribute__ ((__unused__))
__atomic_add (volatile _Atomic_word* __mem, int __val)
{
    __asm__ __volatile__ ("lock; addl %0,%1"
                          : : "ir" (__val), "m" (*__mem) : "memory");
}

#endif


typedef INT64 _Atomic64;

static inline void
__attribute__ ((__unused__))
__atomic_add (volatile _Atomic64* __mem, _Atomic64 __val)
{
#if __WORDSIZE >= 64
    __asm__ __volatile__ ("lock; addq %0,%1"
                          : : "ir" (__val), "m" (*__mem) : "memory");
#else
    __asm__ __volatile__ ( "movl  %0, %%eax\n\t" 
                           "movl  %1, %%edx\n\t" 
                           "1:\t movl %2, %%edi\n\t"
                           "xorl %%ecx, %%ecx\n" 
                           "addl %%eax, %%edi\n\t" 
                           "adcl %%edx, %%ecx\n\t"
                           "lock;  cmpxchg8b       %0\n\t"
                           "jnz             1b"
                           : "+o" (ll_low(*__mem)), "+o" (ll_high(*__mem))
                           : "m" (__val) 
                           : "memory", "eax", "edi", "ecx", "edx", "cc"); 
#endif
}

union _Atomic64Split {
    _Atomic64 comb;
    unsigned int split[2];
};

static inline _Atomic64 
__attribute__ ((__unused__))
__exchange_and_add (volatile _Atomic64 *__mem, _Atomic64 __val)
{
    _Atomic64Split __result;

#if __WORDSIZE >= 64
    __asm__ __volatile__ ("lock; xaddq %0,%2"
                          : "=r" (__result.comb) 
                          : "0" (__val), "m" (*__mem) 
                          : "memory");
#else
    __asm__ __volatile__("movl  %0, %%eax\n\t" 
                         "movl  %1, %%edx\n\t"
                         "1:\t movl %4, %%edi\n\t"
                         "xorl %%ecx, %%ecx\n"
                         "addl %%eax, %%edi\n\t"
                         "adcl %%edx, %%ecx\n\t" 
                         "lock;  cmpxchg8b       %0\n\t"
                         "jnz             1b\n\t"
                         "movl            %%eax, %2\n\t" 
                         "movl            %%edx, %3\n" 
                         : "+o" (ll_low(*__mem)), "+o" (ll_high(*__mem)), "=m" (__result.split[0]),  "=m" (__result.split[1])
                         : "m" (__val) 
                         : "memory", "eax", "edi", "ecx", "edx", "cc"); 
#endif
    return __result.comb;
}

 
static inline void
__attribute__ ((__unused__))
__atomic_decr (volatile _Atomic64* __mem, _Atomic64 __val)
{
#if __WORDSIZE >= 64
    return __atomic_add(__mem, -__val);
#else
    __asm__ __volatile__ ( "movl  %0, %%eax\n\t" 
                           "movl  %1, %%edx\n\t" 
                           "1:\t movl %2, %%edi\n\t"
                           "movl $0xffffffff, %%ecx\n" 
                           "addl %%eax, %%edi\n\t" 
                           "adcl %%edx, %%ecx\n\t" 
                           "lock;  cmpxchg8b       %0\n\t"
                           "jnz             1b"
                           : "+o" (ll_low(*__mem)), "+o" (ll_high(*__mem))
                           : "m" (__val) 
                           : "memory", "eax", "edi", "ecx", "edx", "cc"); 
#endif
}
    

static inline _Atomic64 
__attribute__ ((__unused__))
__exchange_and_decr (volatile _Atomic64 *__mem, _Atomic64 __val)
{
#if __WORDSIZE >= 64
    return __exchange_and_add(__mem, -__val);
#else
    _Atomic64Split __result;

    __asm__ __volatile__("movl  %0, %%eax\n\t" 
                         "movl  %1, %%edx\n\t" 
                         "1:\t movl %4, %%edi\n\t"
                         "movl $0xffffffff, %%ecx\n" 
                         "addl %%eax, %%edi\n\t"
                         "adcl %%edx, %%ecx\n\t" 
                         "lock;  cmpxchg8b       %0\n\t"
                         "jnz             1b\n\t"
                         "movl            %%eax, %2\n\t" 
                         "movl            %%edx, %3\n" 
                         : "+o" (ll_low(*__mem)), "+o" (ll_high(*__mem)), "=m" (__result.split[0]),  "=m" (__result.split[1])
                         : "m" (__val) 
                         : "memory", "eax", "edi", "ecx", "edx", "cc"); 
    return __result.comb;
#endif
}


static inline _Atomic64 
__attribute__ ((__unused__)) 
__read_val(const _Atomic64 * target) 
{
    _Atomic64Split __out;

#if __WORDSIZE >= 64
    __out.comb = *target;
#else
    __asm__ __volatile__("       xorl            %%eax, %%eax\n"
                         "       xorl            %%edx, %%edx\n"
                         "       xorl            %%edi, %%edi\n"
                         "       xorl            %%ecx, %%ecx\n"
                         "lock;  cmpxchg8b       %2\n"
                         "       movl            %%eax, %0\n"
                         "       movl            %%edx, %1"
                         : "=m" (__out.split[0]), "=m" (__out.split[1])
                         : "o" (*target)
                         : "memory", "eax", "edi", "ecx", "edx", "cc");
#endif

    return __out.comb;
}



class ATOMIC32_CLASS {
  public:
    typedef _Atomic_word word;

  private:
    word val;

  public:

    /// Constructs an atomic integer with a given initial value.
    ATOMIC32_CLASS(int val = 0) : val(val) {}

    /// Atomically increases the value of the integer and returns its
    /// old value.
    inline word ExchangeAndAdd(int addend) {
        return __exchange_and_add(&val, addend);
    }

    /// Atomically increases the value of the integer.
    inline void Add(int addend) {
        __atomic_add(&val, addend);
    }

    /// Atomically increases the value of the integer.
    inline void operator += (int addend) {
        Add(addend);
    }

    /// Atomically decreases the value of the integer.
    inline void operator -= (int addend) {
        Add(-addend);
    }

    /// Atomically increments the value of the integer.
    inline void operator ++ () {
        Add(1);
    }

    /// Atomically decrements the value of the integer.
    inline void operator -- () {
        Add(-1);
    }

    /// Atomically increments the value of the integer and returns its
    /// old value.
    inline int operator ++ (int) {
        return ExchangeAndAdd(1);
    }

    /// Atomically decrements the value of the integer and returns its
    /// old value.
    inline int operator -- (int) {
        return ExchangeAndAdd(-1);
    }

    /// Returns the value of the integer.
    operator int() const { return val; }

    friend std::ostream & operator<<(std::ostream &, ATOMIC32_CLASS);
};

typedef ATOMIC32_CLASS *ATOMIC32;


class ATOMIC64_CLASS {
  public:
    typedef _Atomic64 word;

  private:
    word val;

  private:
    //disable a few ops that would aren't yet defined
    word operator+(int);
    word operator+(long long);
    word operator-(int);
    word operator-(long long);

  public:

    /// Constructs an atomic integer with a given initial value.
    ATOMIC64_CLASS(word val = 0) : val(val) {}
    
    /// Atomically increases the value of the integer and returns its
    /// old value.
    inline word ExchangeAndAdd(word addend) {
#ifdef ASIM_ENABLE_MM_DEBUG
        _Atomic64 a = __read_val((word *)&val);
        ASSERT(a != 0x7fffffffffffffffULL, "overflowing 64b counter");
#endif
        return __exchange_and_add((_Atomic64 *)&val, addend);
    }
    
    /// Atomically increases the value of the integer.
    inline void Add(word addend) {
#ifdef ASIM_ENABLE_MM_DEBUG
        _Atomic64 a = __read_val((_Atomic64 *)&val);
        ASSERT(a != 0x7fffffffffffffffULL, "overflowing 64b counter");
#endif
        __atomic_add((_Atomic64 *)&val, addend);
    }
    
    
    /// Atomically decreases the value of the integer and returns its
    /// old value.
    inline word ExchangeAndSub(word addend) {
        return __exchange_and_decr((_Atomic64 *)&val, addend);
    }

    /// Atomically decreases the value of the integer.
    inline void Sub(word addend) {
        __atomic_decr((_Atomic64 *)&val, addend);
    }


    /// Atomically increases the value of the integer.
    inline void operator += (word addend) {
        Add(addend);
    }

    /// Atomically decreases the value of the integer.
    inline void operator -= (word addend) {
        Sub(-addend);
    }

    //NOTE: cannot use these in prefix ops in expressions!
    /// Atomically increments the value of the integer.
    inline void operator ++ () {        
        Add(1);
    }

    /// Atomically decrements the value of the integer.
    inline void operator -- () {
        Sub(-1);
    }

    //postfix ops: "int" is a dummy arg
    /// Atomically increments the value of the integer and returns its
    /// old value.
    inline word operator ++ (int) {
        return ExchangeAndAdd(1);
    }

    /// Atomically decrements the value of the integer and returns its
    /// old value.
    inline word operator -- (int) {
        return ExchangeAndSub(-1);
    }

    /// Returns the value of the integer.
    operator word() const { return __read_val((_Atomic64 *)&val); }

    friend std::ostream & operator<<(std::ostream &, ATOMIC64_CLASS);
};

typedef ATOMIC64_CLASS *ATOMIC64;


/*
std::ostream & operator<< (std::ostream & os, atomic a){
    os << __read_val((_Atomic64 *)(&(a.val)));
    return os;
}
*/

#if MAX_PTHREADS > 1
typedef ATOMIC32_CLASS ATOMIC_INT32;
typedef ATOMIC64_CLASS ATOMIC_INT64;
#else
typedef INT32 ATOMIC_INT32;
typedef INT64 ATOMIC_INT64;
#endif

// Definition of UID_GEN types (unique identifier generator)
typedef ATOMIC_INT32 UID_GEN32;
typedef ATOMIC_INT64 UID_GEN64;


#if __WORDSIZE < 64
typedef _Atomic_word _Atomic_ptr_sized_int;
#else
typedef _Atomic64    _Atomic_ptr_sized_int;
#endif


//
// this class acquires a mutex in its constructor, and releases it
// in its destructor, so you can use it to create sequential sections
// of code by simply allocating an object of this class at the beginning
// of a block.  If it is allocated on the stack, the lock will automatically
// be released upon leaving the block, so no explicit "leave block" command
// is needed, e.g.:
//
// {
//   SEQUENTIAL foo( &my_mutex );
//   /* sequential code block */
// }
//
class SEQUENTIAL {
  private:
    // a mutex to use for this instance:
    pthread_mutex_t *m;
    // a global lock in case the user doesn't supply one:
    static pthread_mutex_t global_mutex;
  public:
    SEQUENTIAL( pthread_mutex_t &mtx = global_mutex )
      : m( &mtx )
    {   pthread_mutex_lock  ( m );   };
    ~SEQUENTIAL()
    {   pthread_mutex_unlock( m );   };
};


#endif
