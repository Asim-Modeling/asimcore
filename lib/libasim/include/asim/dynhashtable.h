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
 * @author Shubu Mukherjee
 * @brief A bucket-based hash table that can be grown dynamically. 
 */


#ifndef _DYNHASHTABLE_
#define _DYNHASHTABLE_

// generic
#include <memory.h>
#include <math.h>
#include <iostream>

// ASIM core
#include "asim/syntax.h"
#include "asim/mesg.h"

template <class KeyType, class T, unsigned Table_Size>
class DynHashTable 
{
  public:
    class HashItem 
    {
      public:
	HashItem(KeyType k, HashItem *n = NULL, T *i = NULL)
	    : _next(n), _item(i)
	{

	    // do ___NOT___ move this into the parameter list, because
	    // LongUint somehow does not work for it.  It probably uses
	    // an operator different from "="

	    _key = k;	
	}

	~HashItem() {}
	KeyType  _key;
	HashItem *_next;
	T	 *_item;
    };


    class Iter 
    {
      public:
	T *item() 		{ return _hashItem->_item; }
	KeyType key()		{ return _hashItem->_key; }
	operator bool()		{ return (_hashItem != NULL); }

	void operator ++()
	{
	    _hashItem = _hashItem->_next;

	    if (_hashItem == NULL)
	    {
		advanceToNextBucket();
	    }
	}

	Iter(DynHashTable *t)
	    : _tblObj(t), _index(0), _hashItem(t->tbl[0])
	{
	    if (_hashItem == NULL)
	    {
		advanceToNextBucket();
	    }
	}

      private:

	void advanceToNextBucket();

	DynHashTable	*_tblObj;
	unsigned	 _index;
	HashItem	*_hashItem;
    };

    T *&Lookup(KeyType key);
    T *Remove(KeyType key);

    DynHashTable()
	: _numEntries(0)
    {
	memset(tbl, 0, sizeof(tbl));
    }

    UINT64 NumEntries()
    {
	return _numEntries; 
    }

    HashItem *tbl[Table_Size];

  private:
    INT64 _numEntries; 
};

// Lookup an entry.  If entry missing, then create new entry and return
// pointer to entry. 
template <class KeyType, class T, unsigned Table_Size>
T *&DynHashTable<KeyType, T, Table_Size>::Lookup(KeyType key)
{
    HashItem **bucketPtr = tbl + (key % Table_Size);
    HashItem *head = *bucketPtr;

    if (head == NULL)
    {
	++_numEntries;
	return (*bucketPtr = new HashItem(key))->_item;
    }
    else if (head->_key == key)
    {
	return head->_item;
    }
    else
    {
	HashItem **prevPtrPtr = bucketPtr;
	HashItem **ptrPtr = &(head->_next);
	HashItem *ptr = *ptrPtr;

	while (ptr != NULL && ptr->_key != key)
	{
	    prevPtrPtr = ptrPtr;
	    ptrPtr = &(ptr->_next);
	    ptr = *ptrPtr;
	}

	if (ptr == NULL)
	{
	    // not found... prepend to chain
	    ++_numEntries;
	    return (*bucketPtr = new HashItem(key, head))->_item;
	}
	else
	{
	    // found... move to head
	    *ptrPtr = ptr->_next;
	    ptr->_next = head;
	    return (*bucketPtr = ptr)->_item;
	}
    }
}

// Remove entry from hashtable
template <class KeyType, class T, unsigned Table_Size>
T *DynHashTable<KeyType, T, Table_Size>::Remove(KeyType key)
{
    HashItem **bucketPtr = tbl + (key % Table_Size);
    HashItem *head = *bucketPtr;

    if (head == NULL)
    {
	return (T *)NULL;
    }
    else if (head->_key == key)
    {
	T *p;
	*bucketPtr = head->_next;
	p = head->_item;
	--_numEntries;
	ASSERTX(_numEntries >= 0); 
	delete head;
	return p;
    }
    else
    {
	HashItem **prevPtrPtr = bucketPtr;
	HashItem **ptrPtr = &(head->_next);
	HashItem *ptr = *ptrPtr;

	while (ptr != NULL && ptr->_key != key)
	{
	    prevPtrPtr = ptrPtr;
	    ptrPtr = &(ptr->_next);
	    ptr = *ptrPtr;
	}

	if (ptr == NULL)
	{
	    return (T *)NULL;
	}
	else
	{
	    T *p;

	    // found... move to head
	    p = ptr->_item;
	    *ptrPtr = ptr->_next;

	    --_numEntries;
	    ASSERTX(_numEntries >= 0); 

	    delete ptr;
	    return p;
	}
    }
}

template <class KeyType, class T, unsigned Table_Size>
void DynHashTable<KeyType, T, Table_Size>::Iter::advanceToNextBucket()
{
    ASSERTX(_hashItem == NULL);

    while (++_index < Table_Size)
    {
	if (_tblObj->tbl[_index] != NULL)
	{
	    _hashItem = _tblObj->tbl[_index];
	    return;
	}
    }
}


#endif 

