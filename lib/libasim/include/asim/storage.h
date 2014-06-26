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

/**
 * @file
 * @author Nathan Binkert
 * @brief
 */

#ifndef _STORAGE_TEMPL_
#define _STORAGE_TEMPL_

// Note.  This data structure stores data backwards from the standard
// notion.  If location 7 was the location of the first element,
// then location 6 would be the location of the next element.  This
// simplifies index calculations.

template <class Type, int Capacity>
class Storage
{
protected:
  Type data[Capacity];

  unsigned size;
  unsigned first;

  unsigned CalcLast() const;
  unsigned CalcNth(unsigned n) const;

  void Next();
  void Prev();

public:
  Storage();
  ~Storage() {}

  bool IsFull() const;
  bool IsEmpty() const;

  unsigned GetSize() const;
  unsigned GetCapacity() const;
  unsigned GetFreeSpace() const;

  bool GetFirstElement(Type &d) const;
  bool SetFirstElement(const Type &d);
  bool GetLastElement(Type &d) const;
  bool SetLastElement(const Type &d);
  bool GetNthElement(unsigned e, Type &d) const;
  bool SetNthElement(unsigned e, const Type &d);
};

// Simple constructor to initialize the storage structure
template <class Type, int Capacity>
inline Storage<Type, Capacity>::Storage()
{
  size = 0;
  first = 0;
}

// Protected Functions:
//

// Calculates the index into the array for the last element
template <class Type, int Capacity>
inline unsigned Storage<Type, Capacity>::CalcLast() const
{ return (first + size - 1) % Capacity; }

// Calculates the index into the array for the Nth element
// from the front of the array
template <class Type, int Capacity>
inline unsigned Storage<Type, Capacity>::CalcNth(unsigned n) const
{ return (first + n - 1) % Capacity; }

// Advance the front to the next position
template <class Type, int Capacity>
inline void Storage<Type, Capacity>::Next()
{ if (first == 0) first = Capacity; first--; }

// Set the front pointer to the previous position
template <class Type, int Capacity>
inline void Storage<Type, Capacity>::Prev()
{ first = (first + 1) % Capacity; }

// Public Functions:
//

// Returns true if the storage space is full
template <class Type, int Capacity>
inline bool Storage<Type, Capacity>::IsFull() const
{ return (size == Capacity); }

// Returns true if the storage space is empty
template <class Type, int Capacity>
inline bool Storage<Type, Capacity>::IsEmpty() const
{ return (size == 0); }

// Returns the current size of the storages space
template <class Type, int Capacity>
inline unsigned Storage<Type, Capacity>::GetSize() const
{ return size; }

// Returns the capacity of the storage space
template <class Type, int Capacity>
inline unsigned Storage<Type, Capacity>::GetCapacity() const
{ return Capacity; }

// Returns the free space in number of elements for the
// storage space
template <class Type, int Capacity>
inline unsigned Storage<Type, Capacity>::GetFreeSpace() const
{ return Capacity - size; }


// Access functions
//
template <class Type, int Capacity>
inline bool Storage<Type, Capacity>::GetFirstElement(Type &d) const
{
  if (IsEmpty()) return false;
  d = data[first];
  return true;
}

template <class Type, int Capacity>
inline bool Storage<Type, Capacity>::SetFirstElement(const Type &d)
{
  if (IsEmpty()) return false;
  data[first] = d;
  return true;
}

template <class Type, int Capacity>
inline bool Storage<Type, Capacity>::GetLastElement(Type &d) const
{
  if (IsEmpty()) return false;
  d = data[CalcLast()];
  return true;
}


template <class Type, int Capacity>
inline bool Storage<Type, Capacity>::SetLastElement(const Type &d)
{
  if (IsEmpty()) return false;
  data[CalcLast()] = d;
  return true;
}


template <class Type, int Capacity>
inline bool Storage<Type, Capacity>::GetNthElement(unsigned e, Type &d) const
{
  if (IsEmpty()) return false;
  d = data[CalcNth(e)];
  return true;
}


template <class Type, int Capacity>
inline bool Storage<Type, Capacity>::SetNthElement(unsigned e, const Type &d)
{
  if (IsEmpty()) return false;
  data[CalcNth(e)] = d;
  return true;
}

#endif //_STORAGE_TEMPL_
