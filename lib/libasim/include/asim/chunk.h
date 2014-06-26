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

#ifndef _CHUNK_
#define _CHUNK_

template<class T, int N>
class Chunk
{
private:
  struct {
    T Data;
    bool Valid;
  } Array[N];
  
  const T Dummy;

  int Used;

public:
  Chunk();
  Chunk(const Chunk<T,N>& chunk);
  ~Chunk();

  int GetUsed();
  int GetFree();
  int GetMaxSize();
  bool IsFull();
  bool IsEmpty();

  int Add(const T& data);
  bool Remove(const T& data);
  bool Remove(int index);

  T& operator[](int index);
  const Chunk<T,N>& operator=(const Chunk<T,N>& chunk);
};

template<class T, int N>
inline
Chunk<T,N>::Chunk() : Dummy(T())
{
    Used = 0;
    for (int count = 0; count < GetMaxSize(); count++)
	Array[count].Valid = false;
}

template<class T, int N>
inline
Chunk<T,N>::Chunk(const Chunk<T,N>& chunk) : Dummy(T())
{ operator=(chunk); }

template<class T, int N>
inline
Chunk<T,N>::~Chunk()
{}

template<class T, int N>
inline int
Chunk<T,N>::GetUsed()
{ return Used; }

template<class T, int N>
inline int
Chunk<T,N>::GetMaxSize()
{ return N; }

template<class T, int N>
inline int
Chunk<T,N>::GetFree()
{ return GetMaxSize() - GetUsed(); }

template<class T, int N>
inline bool
Chunk<T,N>::IsFull()
{ return GetUsed() == GetMaxSize(); }

template<class T, int N>
inline bool
Chunk<T,N>::IsEmpty()
{ return GetUsed() == 0; }

template<class T, int N>
inline int
Chunk<T,N>::Add(const T& data)
{
  if (GetFree() == 0)
    return -1;

  int count = 0;
  while (Array[count].Valid)
    ++count;

  Array[count].Data = data;
  Array[count].Valid = true;
  ++Used;

  return count;
}

template<class T, int N>
inline bool
Chunk<T,N>::Remove(const T& data)
{
  if (GetFree() == 0)
    return false;

  int count = 0;
  while (!Array[count].Valid || Array[count].Data != data)
    if (++count >= GetMaxSize())
      return false;

  Array[count].Valid = false;
  Array[count].Data = Dummy;
  --Used;

  return true;
}

template<class T, int N>
inline bool
Chunk<T,N>::Remove(int index)
{
  if (index < 0 || index >= GetMaxSize())
    return false;

  ASSERTX(Array[index].Valid);
  Array[index].Valid = false;
  Array[index].Data = Dummy;
  --Used;

  return true;
}

template<class T, int N>
inline T&
Chunk<T,N>::operator[](int index)
{
    ASSERTX(index >= 0 && index < GetMaxSize());
    return Array[index].Data;
}

template<class T, int N>
inline const Chunk<T,N>&
Chunk<T,N>::operator=(const Chunk<T,N>& chunk)
{
  for (int count = 0; count < GetMaxSize; count++) {
    Array[count].Valid = chunk.Array[count].Valid;
    if (Array[count].Valid)
      Array[count].Data = chunk.Array[count].Data;
  }

  Used = chunk.Used;
}
#endif //_CHUNK_
