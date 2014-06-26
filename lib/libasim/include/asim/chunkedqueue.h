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

#ifndef _CHUNKEDQ_
#define _CHUNKEDQ_

// ASIM core
#include "asim/syntax.h"
#include "asim/mesg.h"
#include "asim/chunk.h"
#include "asim/arraylist.h"
#include "asim/hashtable.h"

// not using STL stack since this stack has some added functionality. Can we
// figure out how to use STL stack? Eric
#include "asim/stack.h"


// TO DO: Eric - instead of using CPU_INST, should use templates so this 
// isn't dependent on aranainst.h
typedef mmptr<class CPU_INST_CLASS> CPU_INST;

inline unsigned HashInst(CPU_INST addr)
{ return Hash((void*)addr); }

template <int CS, int NC, int NI = CS * NC>
class ChunkedQueue
{
public:
  typedef ArrayList<CPU_INST, NI> AgeListType;
  typedef Chunk<CPU_INST, CS> ChunkType;

public:
  ChunkType Array[NC];
  Stack<ChunkType*, NC> FreeChunks;

  AgeListType AgeList;
  struct InstData {
    typename AgeListType::Iterator iter;
    ChunkType* chunk;
    int index;
    UINT64 create_cycle;	
  };

  typedef HashTable<CPU_INST, InstData, NI*4, HashInst> HTABLE;
  HTABLE AgeMap;

public:
  ChunkedQueue();
  
  bool IsEmpty() const { return FreeChunks.IsFull(); }
  bool IsFull() const { return FreeChunks.IsEmpty(); }
  int GetFreeChunks() const { return FreeChunks.GetOccupancy(); }
  int GetMaxChunks() const { return FreeChunks.GetCapacity(); }
  int GetNC() const { return FreeChunks.GetFreeSpace(); }

  int GetFreeEntries() const;

  typename AgeListType::Iterator GetAgeIter();
  typename AgeListType::Iterator GetAgeIterEnd();
  ChunkType* GetFreeChunk();
  bool AddInst(ChunkedQueue<CS,NC,NI>::ChunkType* chunk, CPU_INST inst,
	UINT64 cycle = 0);
  bool RemoveInstruction(CPU_INST inst);
  int IfOldestGetCycle(CPU_INST inst) const;
  UINT64 GetCycleFilled(CPU_INST inst) const;
  int GetChunkOccupancy(CPU_INST inst) const;
};

template <int CS, int NC, int NI>
inline
ChunkedQueue<CS,NC,NI>::ChunkedQueue()
{
  for (unsigned count = 0; count < NC; count++)
    FreeChunks.Push(&Array[count]);
}

template <int CS, int NC, int NI>
inline int
ChunkedQueue<CS,NC,NI>::GetFreeEntries() const
{
  int freeEntries = 0;

  for (unsigned count = 0; count < NC; count++)
  {
    freeEntries += Array[count].GetFree();
  }
  return freeEntries;
}

template <int CS, int NC, int NI>
inline typename ChunkedQueue<CS,NC,NI>::AgeListType::Iterator
ChunkedQueue<CS,NC,NI>::GetAgeIter()
{
  typename AgeListType::Iterator i = AgeList.Begin();
  return i;
}

template <int CS, int NC, int NI>
inline typename ChunkedQueue<CS,NC,NI>::AgeListType::Iterator
ChunkedQueue<CS,NC,NI>::GetAgeIterEnd()
{
  typename AgeListType::Iterator i = AgeList.End();
  return i;
}

template <int CS, int NC, int NI>
inline typename ChunkedQueue<CS,NC,NI>::ChunkType*
ChunkedQueue<CS,NC,NI>::GetFreeChunk()
{
  ChunkType* chunk;

  if (!FreeChunks.Pop(chunk)) {
    return NULL;
  }

  return chunk;
}

template <int CS, int NC, int NI>
inline bool
ChunkedQueue<CS,NC,NI>::AddInst(ChunkedQueue::ChunkType* chunk, CPU_INST inst, UINT64 cycle)
{
  InstData data;

  if (chunk->IsFull())
    return false;

  typename AgeListType::Iterator i = AgeList.End();
  AgeList.Insert(i, inst);

  data.iter = --i;
  data.chunk = chunk;
  data.index = chunk->Add(inst);
  data.create_cycle = cycle;
  ASSERTX(data.index != -1);
  
  AgeMap.Insert(inst, data);

  return true;
}

template <int CS, int NC, int NI>
inline bool
ChunkedQueue<CS,NC,NI>::RemoveInstruction(CPU_INST inst)
{
  bool retVal;

  typename HTABLE::Iterator i = AgeMap.Find(inst);
  if (i == AgeMap.End())
    return false;

  InstData data = *i;

  (*data.chunk)[data.index] = NULL;
  retVal = data.chunk->Remove(data.index);
  ASSERTX(retVal);

  if (data.chunk->IsEmpty()) {
    bool ret = FreeChunks.Push(data.chunk);
    ASSERTX(ret);
  }

  ASSERTX(inst == *data.iter);
  AgeList.Remove(data.iter);
  AgeMap.Remove(i);

  return true;
}

// returns the number of instructions in the chunk which contains the
// supplied instruction. 
template <int CS, int NC, int NI>
inline int
ChunkedQueue<CS,NC,NI>::GetChunkOccupancy(CPU_INST inst) const
{

  typename HTABLE::ConstIterator i = AgeMap.Find(inst);
  ASSERTX(i != AgeMap.End());

  InstData data = *i;
  return data.chunk->GetUsed();
}

// returns the cycle that the chunk for the supplied instruction was 
// first filled.  In the case of an error, 0 is returned, since 
// This is an unlikely cycle for something to enter the queue.
template <int CS, int NC, int NI>
inline UINT64
ChunkedQueue<CS,NC,NI>::GetCycleFilled(CPU_INST inst) const
{

  typename HTABLE::ConstIterator i = AgeMap.Find(inst);
  ASSERTX(i != AgeMap.End());

  InstData data = *i;
  return data.create_cycle;
}

#endif //_CHUNKEDQ_
