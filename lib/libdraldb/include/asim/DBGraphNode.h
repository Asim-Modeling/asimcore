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

#ifndef _DRALDB_DBGRAPHNODE_H
#define _DRALDB_DBGRAPHNODE_H

// Qt library
#include <qstring.h>
#include <q3valuelist.h>
#include <q3dict.h>
#include <q3intdict.h>

#include "asim/draldb_syntax.h"
#include "asim/DralDBDefinitions.h"

/**
  * @brief
  * This class represents a DRAL graph node.
  *
  * @description
  * A DRAL graph node is used to store all the information that
  * a DRAL trace gives us about this node.
  *
  * @author Federico Ardanaz
  * @date started at 2002-07-15
  * @version 0.1
  */
class DBGraphNode
{
    public:
        DBGraphNode (
               QString name,
               UINT16 node_id,
               UINT16 parent_id,
               UINT16 instance
               );

        ~DBGraphNode();

        inline QString getName();
        inline UINT16 getNodeId();

        void   setLayout(UINT16 dimensions, const UINT32 capacities []);

        inline UINT16 getInputBW();
        inline UINT16 getOutputBW();
        inline UINT32 getCapacity();
        inline UINT16 getDimensions();
        inline UINT32* getCapacities();

    private:
        QString name; // Name of the node. This name is a unique string for identifying the node : node_name[instance]{node_slot}.  
        UINT32  node_id; // Holds the id of the node. 
        UINT16  parent_id; // Contains the id of this node's parent.
        UINT16  instance; // Instance of the node. Needed due the existence of different nodes with the same name.
        UINT16  dimensions; // Contains the number of dimensions of the node layout.
        UINT32* capacities; // Contains the value of each dimension of the node layout.
        UINT32  totalCapacity; // Contains the number of slots of the node.
        UINT16  inputbw; // Contains the input bandwidth of the node. Not used.
        UINT16  outputbw; // Contains the output bandwidth of the node. Not used.
};

/**
  * @typedef DGNList
  * @brief
  * This struct is a dictionary of nodes indexed by integers.
  */
typedef Q3IntDict<DBGraphNode> DBGNList;

/**
  * @typedef DGNListByName
  * @brief
  * This struct is a dictionary of nodes indexed by name (field name of this class).
  */
typedef Q3Dict<DBGraphNode> DBGNListByName;

/**
 * Returns the name of the node. 
 *
 * @return the name.
 */
QString
DBGraphNode::getName()
{
	return name;
}

/**
 * Returns the id of the node. 
 *
 * @return the id.
 */
UINT16
DBGraphNode::getNodeId()
{
	return node_id;
}

/**
 * Returns the input bandwidth of the node. 
 *
 * @return the input bandwidth.
 */
UINT16 
DBGraphNode::getInputBW()
{
	return inputbw;
}

/**
 * Returns the output bandwidth of the node. 
 *
 * @return the output bandwidth.
 */
UINT16
DBGraphNode::getOutputBW()
{
	return outputbw;
}

/**
 * Returns the capacity of the node. 
 *
 * @return the capacity.
 */
UINT32
DBGraphNode::getCapacity()
{
	return totalCapacity;
}

/**
 * Returns the dimensions of the node. 
 *
 * @return the dimensions.
 */
UINT16
DBGraphNode::getDimensions()
{
	return dimensions;
}

/**
 * Returns the capacities of the node. 
 *
 * @return the capacities.
 */
UINT32*
DBGraphNode::getCapacities()
{
	return capacities;
}

#endif
