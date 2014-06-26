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

#ifndef _DRALDB_DBGRAPHEDGE_H
#define _DRALDB_DBGRAPHEDGE_H

// Qt library
#include <qstring.h>
#include <q3intdict.h>

#include "asim/draldb_syntax.h"
#include "asim/DBGraphNode.h"
#include "asim/DralDBDefinitions.h"

/**
  * @brief
  * This class represents a DRAL graph edge.
  *
  * @description
  * A DRAL graph edge is used to store all the information of  
  *
  * @author Federico Ardanaz
  * @date started at 2002-07-15
  * @version 0.1
  */
class DBGraphEdge
{
    public:
        DBGraphEdge (
                      DBGraphNode* source_node,
                      DBGraphNode* destination_node,
                      UINT16 edge_id,
                      UINT32 bandwidth,
                      UINT32 latency,
                      QString name
                      );

        ~DBGraphEdge();

        inline DBGraphNode* getSourceNode();
        inline DBGraphNode* getDestinationNode();
        inline QString getSourceNodeName();
        inline QString getDestinationNodeName();
        inline UINT16 getEdgeId();
        inline UINT32 getBandwidth();
        inline UINT32 getLatency();
        inline QString  getName();
        inline QString  getCompleteName();

    private:

        // DRAL edge attributes
        UINT32 bandwidth; // Maximum bandwidth of the egdes (items per cycle).
        UINT32 latency; // Latency of the edge.
        DBGraphNode* source_node; // Source node of the edge.
        DBGraphNode* destination_node; // Destination node of the edge.
        UINT16 edge_id; // Holds the id of the edge. Unique value.
        QString edgeName; // The name of the edge.
};

/**
  * @typedef DBGEList
  * @brief
  * This struct is a dictionary of edges indexed by integers.
  */
typedef Q3IntDict<DBGraphEdge> DBGEList;

/**
 * Returns the source node of the edge.
 *
 * @return the source node.
 */
DBGraphNode*
DBGraphEdge::getSourceNode()
{
	return source_node;
}

/**
 * Returns the destination node of the edge.
 *
 * @return the destination node.
 */
DBGraphNode*
DBGraphEdge::getDestinationNode() 
{
	return destination_node;
}

/**
 * Returns the source node name of the edge.
 *
 * @return the source node name.
 */
QString
DBGraphEdge::getSourceNodeName()
{
	return (source_node->getName());
}

/**
 * Returns the destination node name of the edge.
 *
 * @return the destination node name.
 */
QString
DBGraphEdge::getDestinationNodeName()
{
	return (destination_node->getName());
}

/**
 * Returns the id of the edge.
 *
 * @return the id.
 */
UINT16
DBGraphEdge::getEdgeId()
{
	return edge_id;
}

/**
 * Returns the bandwidth of the edge.
 *
 * @return the bandwidth.
 */
UINT32
DBGraphEdge::getBandwidth()
{
	return bandwidth;
}

/**
 * Returns the latency of the edge.
 *
 * @return the latency.
 */
UINT32
DBGraphEdge::getLatency()
{
	return latency;
}

/**
 * Returns the name of the edge.
 *
 * @return the name.
 */
QString
DBGraphEdge::getName()
{
	return edgeName;
}

/**
 * Returns the name of the edge followed by the name of the source
 * node plus the name of the destination node.
 *
 * @return the complete name.
 */
QString
DBGraphEdge::getCompleteName()
{
	return edgeName+";"+source_node->getName()+";"+destination_node->getName();
}

#endif
