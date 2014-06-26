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

#ifndef _DRALDB_DBGRAPH_H
#define _DRALDB_DBGRAPH_H

#include "asim/draldb_syntax.h"
#include "asim/LogMgr.h"
#include "asim/DBGraphEdge.h"
#include "asim/DBGraphNode.h"
#include "asim/DRALTag.h"

/**
  * @brief
  * This class stores all the nodes and edges of the DRAL graph.
  *
  * @description
  * This class is used to store all the nodes and edges that
  * are used in the DRAL trace. Both nodes and edges can be
  * found using their ids and the nodes can be indexed using
  * their name too.
  *
  * @author Federico Ardanaz
  * @date started at 2002-07-15
  * @version 0.1
  */
class DBGraph
{
    public:
        static DBGraph* getInstance ();
        static void destroy();

    public:
        bool addNode (
                     QString name,
                     UINT16 node_id,
                     UINT16 parent_id,
                     UINT16 instance
                     );

        bool addEdge (
                     UINT16 source_node,
                     UINT16 destination_node,
                     UINT16 edge_id,
                     UINT32 bandwidth,
                     UINT32 latency,
                     QString  name
                     ) ;

        inline INT32 getNumEdges();
        inline INT32 getNumNodes();

        inline DBGraphNode* getNode(UINT16 node_id);
        inline DBGraphNode* getNode(QString name);
        inline INT32 getNodeCapacity (UINT16 node_id);
        inline INT32 getNodeInputBW  (UINT16 node_id);
        inline INT32 getNodeOutputBW (UINT16 node_id);
        bool setNodeLayout    (UINT16 node_id, UINT16 dimensions, const UINT32 capacities []);

        inline DBGraphEdge* getEdge (UINT16 edge_id);
        DBGraphEdge* findEdgeByNameFromTo(QString name,QString fromstr,QString tostr);
        inline INT32 getEdgeBandwidth (UINT16 edge_id);
        inline INT32 getEdgeLatency   (UINT16 edge_id);

        // for debugging
        QString getGraphDescription();

        static QString normalizeNodeName(QString name);
        static QString slotedNodeName(QString name,UINT16 level,  const UINT32 list []);
        static QString slotedNodeName(QString name,NodeSlot nslot);
        static bool decodeNodeSlot(QString strnodeslot,QString* nodename,NodeSlot* nslot);
        //static QString slotedEdgeName(DBGraphEdge* edge, INT32 pos);
        //static bool decodeEdgeSlot(QString stredgeslot,QString* ename,QString* from, QString *to, INT32 *pos);

		void reset();
		
    protected:
        DBGraph();
        ~DBGraph();
		void init();
		void freeAll();


    private:
        DBGNList*       dbgnList; // List used to obtain the nodes indexing with their ids.
        DBGNListByName* dbgnListByName; // List used to obtain the nodes indexing with their names.

        DBGEList* dbgeList; // List used to obtain the edges indexing with their ids.

        LogMgr* myLogMgr; // Log file.

    private:
       static DBGraph* _myInstance; // Pointer to the singleton instance of the class.
};

/**
 * Searches the node with id node_id using the node list indexed
 * with integers.
 *
 * @return the node with id node_id.
 */
DBGraphNode*
DBGraph::getNode(UINT16 node_id)
{
    return dbgnList->find((long)node_id);
}

/**
 * Searches the node with name name using the node list indexed
 * with names.
 *
 * @return the node with name name.
 */
DBGraphNode*
DBGraph::getNode(QString name)
{
    return dbgnListByName->find(name);
}

/**
 * Searches the node with id node_id using the node list indexed
 * with integers. Then returns the capacity of this node.
 *
 * @return the node capcity if the node exists. -1 otherwise.
 */
INT32
DBGraph::getNodeCapacity (UINT16 node_id)
{
    DBGraphNode* node = dbgnList->find((long)node_id);
    if (node==NULL) return -1;
    return node->getCapacity();
}

/**
 * Searches the node with id node_id using the node list indexed
 * with integers. Then returns the input bandwidth of this node.
 *
 * @return the node input bandwidth if the node exists. -1 otherwise.
 */
INT32
DBGraph::getNodeInputBW  (UINT16 node_id)
{
    DBGraphNode* node = dbgnList->find((long)node_id);
    if (node==NULL) return -1;
    return node->getInputBW();
}

/**
 * Searches the node with id node_id using the node list indexed
 * with integers. Then returns the output bandwidth of this node.
 *
 * @return the node output bandwidth if the node exists. -1 otherwise.
 */
INT32
DBGraph::getNodeOutputBW (UINT16 node_id)
{
    DBGraphNode* node = dbgnList->find((long)node_id);
    if (node==NULL) return -1;
    return node->getOutputBW();
}

/**
 * Searches the edge with id edge_id using the edge list indexed
 * with integers.
 *
 * @return the edge with id edge_id.
 */
DBGraphEdge*
DBGraph::getEdge (UINT16 edge_id)
{
    return dbgeList->find((long)edge_id);
}


/**
 * Searches the edge with id edge_id using the edge list indexed
 * with integers. Then returns the bandwidth of this edge.
 *
 * @return the edge bandwidth if the edge exists. -1 otherwise.
 */
INT32
DBGraph::getEdgeBandwidth (UINT16 edge_id)
{
    DBGraphEdge* edge = dbgeList->find((long)edge_id);
    if (edge==NULL) { return -1; }
    return (INT32)edge->getBandwidth();
}

/**
 * Searches the edge with id edge_id using the edge list indexed
 * with integers. Then returns the latency of this edge.
 *
 * @return the edge latency if the edge exists. -1 otherwise.
 */
INT32
DBGraph::getEdgeLatency   (UINT16 edge_id)
{
    DBGraphEdge* edge = dbgeList->find((long)edge_id);
    if (edge==NULL) { return -1; }
    return (INT32)edge->getLatency();
}

#endif
