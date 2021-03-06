/**************************************************************************
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
 * @file dralServerAscii.cpp
 * @author Pau Cabre 
 * @brief dral server implementation using ASCII output
 */

#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <iostream>
using namespace std;

#include "asim/dralServerAscii.h"


DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::
    DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS(
        UINT16 buffer_size, bool compression) :
    DRAL_SERVER_IMPLEMENTATION_CLASS(buffer_size,compression) {}

/*
 * Private methods used to convert an array of values
 * to a string with this values in ascii format
 */
void
DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::Fill_values (
    ostringstream * value_list,UINT32 nval, const UINT64 value[])
{
    *value_list << value[0];
    for (UINT32 i=1;i<nval;i++)
    {
        *value_list << "," << value[i];
    }
}

void
DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::Fill_values_and_positions (
    ostringstream * value_list,UINT32 nval, const UINT32 value[], const UINT32 position [])
{
    if (position == NULL)
    {
        *value_list << value[0];
        for (UINT32 i=1;i<nval;i++)
        {
            *value_list << "," << value[i];
        }
    }
    else
    {
        *value_list << value[0] << '@' << position[0];
        for (UINT32 i=1;i<nval;i++)
        {
            *value_list << "," << value[i] << '@' << position[i];
        }
    }
}

void
DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::Write_single_position_list (
    ostringstream * sout,UINT16 dim, const UINT32 positions [])
{
    for (UINT16 i=0; i<dim; ++i)
    {
        if (i!=0)
        {
            *sout << ',';
        }
        
        if (positions[i] == DRAL_ANY)
        {
            *sout << "DRAL_ANY";
        }
        else
        {
            *sout << positions[i];
        }
    }
}

void
DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::Write_multiple_list (
    ostringstream * sout,UINT16 dim, const UINT32 positions [])
{
    for (UINT16 i=0; i<dim; ++i)
    {
        if (i!=0)
        {
            *sout << ',';
        }
        
        switch (positions[i])
        {
          case DRAL_ANY:
            *sout << "DRAL_ANY";
            break;
          case DRAL_ALL:
            *sout << "DRAL_ALL";
            break;
          case DRAL_EMPTY:
            *sout << "DRAL_EMPTY";
            break;
          default:
            *sout << positions[0];
        }
    }
}



/*
 * public methods to write events to the file descriptor
 * The methods addNode and AddEdge are used to define the graph.
 * The other ones are used to add items, their tags and to move
 * them over the graph
 */
void
DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::Cycle (
    UINT64 n)
{
    ostringstream sout;
    sout << "cycle " << n << endl;
    const char * buffer=sout.str().c_str();
    dralWrite->Write(buffer,strlen(buffer));
}


void
DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::NewItem (
    UINT32 item_id)
{
    ostringstream sout;
    sout << "newitem " << item_id << endl;
    const char * buffer=sout.str().c_str();
    dralWrite->Write(buffer,strlen(buffer));
}

void
DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::SetItemTag (
    UINT32 item_id, const char tag_name [], UINT16, UINT64 value)
{
    ostringstream sout;
    sout << "setItemTag " << item_id << " " << tag_name << " = "
    << value << endl;
    const char * buffer=sout.str().c_str();
    dralWrite->Write(buffer,strlen(buffer));
}


void
DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::SetItemTag (
    UINT32 item_id, const char tag_name[], UINT16,
    const char str[], UINT16)
{
    ostringstream sout;
    sout << "setItemTagString " << item_id << " " << tag_name << " = "
    << str << endl;
    const char * buffer=sout.str().c_str();
    dralWrite->Write(buffer,strlen(buffer));
}


void
DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::SetItemTag (
    UINT32 item_id, const char tag_name[],UINT16, UINT32 nval,
    UINT64 value[])
{
    ostringstream sout;
    sout << "setItemTagSet " << item_id << " " << tag_name << " = { ";
    Fill_values(&sout,nval,value);
    sout << " }" << endl;
    const char * buffer=sout.str().c_str();
    dralWrite->Write(buffer,strlen(buffer));
}



void
DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::MoveItems (
    UINT16 edge_id, UINT32 n, UINT32 item_id[], UINT32 position [])
{
    ostringstream sout;
    sout << "moveitems edge " << edge_id << " items ";
    Fill_values_and_positions(&sout,n,item_id,position);
    sout << endl;
    const char * buffer=sout.str().c_str();
    dralWrite->Write(buffer,strlen(buffer));
}


void
DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::EnterNode (
    UINT16 node_id, UINT32 item_id, UINT16 dimensions, UINT32 position [])
{
    ostringstream sout;
    sout << "enternode node " << node_id << " item " << item_id << " slot [";
    Write_single_position_list(&sout,dimensions,position);
    sout << "]" << endl;
    const char * buffer=sout.str().c_str();
    dralWrite->Write(buffer,strlen(buffer));
}


void
DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::ExitNode (
    UINT16 node_id, UINT32 item_id, UINT16 dimensions, UINT32 position [])
{
    ostringstream sout;
    sout << "exitnode node " << node_id << " item " << item_id << " slot [";
    Write_single_position_list(&sout,dimensions,position);
    sout << "]" << endl;
    const char * buffer=sout.str().c_str();
    dralWrite->Write(buffer,strlen(buffer));
}


void
DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::DeleteItem (
    UINT32 item_id)
{
    ostringstream sout;
    sout << "deleteitem " << item_id << endl;
    const char * buffer=sout.str().c_str();
    dralWrite->Write(buffer,strlen(buffer));
}


void
DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::NewNode (
    UINT16 node_id, const char name[], UINT16 ,
    UINT16 parent_id, UINT16 instance)
{
    ostringstream sout;
    sout << "newnode " << name << " id " << node_id << " parent " << parent_id
    << " instance " << instance << endl;
    const char * buffer=sout.str().c_str();
    dralWrite->Write(buffer,strlen(buffer));
}


void
DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::NewEdge (
    UINT16 edge_id, UINT16 source_node, UINT16 destination_node,
    UINT32 bandwidth, UINT32 latency, const char name[], UINT16)
{
    ostringstream sout;
    sout << "newedge " << name << " from node " << source_node << " to node "
    << destination_node << " id " << edge_id << " bw "
    << bandwidth << " lat " << latency << endl;
    const char * buffer=sout.str().c_str();
    dralWrite->Write(buffer,strlen(buffer));
}

void
DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::SetNodeLayout (
    UINT16 node_id,  UINT16 dimensions, const UINT32 capacity [])
{
    ostringstream sout;
    sout << "setnodelayout node " << node_id << " cap " << capacity[0];
    for (UINT16 i=1;i<dimensions;i++)
    {
        sout << " x " << capacity[i];
    }
    sout << endl;
    const char * buffer=sout.str().c_str();
    dralWrite->Write(buffer,strlen(buffer));
}


void
DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::Comment(
    UINT32 magic_num, const char * comment, UINT32 strlen )
{
    ostringstream sout;
    sout << "comment " << magic_num << " " << comment << endl;
    const char * buffer=sout.str().c_str();
    dralWrite->Write(buffer, strlen);
}

void
DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::CommentBin(
    UINT16 magic_num, const char * comment, UINT32 length)
{
    ostringstream sout;
    sout << "commentbin " << magic_num << " (" << length <<
    " bytes of binary contents not showed)" << endl;
    const char * buffer=sout.str().c_str();
    dralWrite->Write(buffer,strlen(buffer));
}

void
DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::Version (void)
{
    ostringstream sout;
    sout << "version major " << DRAL_SERVER_VERSION_MAJOR << " minor "
    << DRAL_SERVER_VERSION_MINOR << endl;
    const char * buffer=sout.str().c_str();
    dralWrite->Write(buffer,strlen(buffer));
}

void
DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::SetNodeTag(
    UINT16 node_id, const char tag_name [], UINT16 , UINT64 val,
    UINT16 levels, const UINT32 list [])
{
    ostringstream sout;
    sout << "setNodeTag " << node_id << " " << tag_name << " = "
    << val << " at [";
    Write_multiple_list(&sout,levels,list);
    sout << "]" << endl;
    const char * buffer=sout.str().c_str();
    dralWrite->Write(buffer,strlen(buffer));
}

void
DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::SetNodeTag(
    UINT16 node_id, const char tag_name [], UINT16 ,
    const char str [], UINT16, UINT16 levels, const UINT32 list [])
{
    ostringstream sout;
    sout << "setNodeTagString " << node_id << " " << tag_name << " = "
    << str << " at [";
    Write_multiple_list(&sout,levels,list);
    sout << "]" << endl;
    const char * buffer=sout.str().c_str();
    dralWrite->Write(buffer,strlen(buffer));
}

void
DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::SetNodeTag(
    UINT16 node_id, const char tag_name [], UINT16 , UINT16 nval,
    const UINT64 set [], UINT16 levels, const UINT32 list [])
{
    ostringstream sout;
    sout << "setNodeTagSet " << node_id << " " << tag_name << " = {";
    Fill_values(&sout,nval,set);
    sout << "} at [";
    Write_multiple_list(&sout,levels,list);
    sout << "]" << endl;
    const char * buffer=sout.str().c_str();
    dralWrite->Write(buffer,strlen(buffer));
}

void
DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::SetCycleTag (
    const char tag_name [], UINT16, UINT64 value)
{
    ostringstream sout;
    sout << "setCycleTag " << tag_name << " = " << value << endl;
    const char * buffer=sout.str().c_str();
    dralWrite->Write(buffer,strlen(buffer));
}


void
DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::SetCycleTag (
    const char tag_name[], UINT16, const char str[], UINT16)
{
    ostringstream sout;
    sout << "setCycleTagString " << tag_name << " = "  << str << endl;
    const char * buffer=sout.str().c_str();
    dralWrite->Write(buffer,strlen(buffer));
}


void
DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::SetCycleTag (
    const char tag_name[],UINT16, UINT32 nval, UINT64 value[])
{
    ostringstream sout;
    sout << "setCycleTagSet " << tag_name << " = { ";
    Fill_values(&sout,nval,value);
    sout << " }" << endl;
    const char * buffer=sout.str().c_str();
    dralWrite->Write(buffer,strlen(buffer));
}

void DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::SetNodeInputBandwidth (
    UINT16 nodeId, UINT32 bandwidth)
{
    ostringstream sout;
    sout << "setNodeInputBandwidth node " << nodeId << " bw " << bandwidth
    << endl;
    const char * buffer=sout.str().c_str();
    dralWrite->Write(buffer,strlen(buffer));
}

void DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::SetNodeOutputBandwidth (
    UINT16 nodeId, UINT32 bandwidth)
{
    ostringstream sout;
    sout << "setNodeOutputBandwidth node " << nodeId << " bw " << bandwidth
    << endl;
    const char * buffer=sout.str().c_str();
    dralWrite->Write(buffer,strlen(buffer));
}

void DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::StartActivity (UINT64 cycle)
{
    ostringstream sout;
    sout << "startactivity cycle " << cycle << endl;
    const char * buffer=sout.str().c_str();
    dralWrite->Write(buffer,strlen(buffer));
}

void DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::SetTagDescription (
    const char tag [], UINT16,
    const char description [], UINT16)
{
    ostringstream sout;
    sout << "settagdescription tag " << tag << " " << description << endl;
    const char * buffer=sout.str().c_str();
    dralWrite->Write(buffer,strlen(buffer));
}

void DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::SetNodeClock (
    UINT16 nodeId, UINT16 clockId)
{
    ostringstream sout;
    sout << "setnodeclock node " << nodeId << " clock " << clockId << endl;
    const char * buffer=sout.str().c_str();
    dralWrite->Write(buffer,strlen(buffer));
}

void DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::NewClock (
    UINT16 clockId, UINT64 freq, UINT16 skew, UINT16 divisions,
    const char name [], UINT16)
{
    ostringstream sout;
    sout << "newclock name " << name << " id " << clockId
    << " frequency " << freq << " skew " << skew << " divisions "
    << divisions << endl;
    const char * buffer=sout.str().c_str();
    dralWrite->Write(buffer,strlen(buffer));
}

void DRAL_SERVER_ASCII_IMPLEMENTATION_CLASS::Cycle (
    UINT16 clockId, UINT64 n, UINT16 phase)
{
    ostringstream sout;
    sout << "cycle id " << clockId << " number "
    << n << " phase " << phase << endl;
    const char * buffer=sout.str().c_str();
    dralWrite->Write(buffer,strlen(buffer));
}

