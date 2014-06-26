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
 * @file
 * @author Artur Klauser
 * @brief Implementation of stats output
 */

// generic
#include <string.h>
#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <cerrno>

// ASIM core
#include "asim/stateout.h"


// initialize class constants
const char * const STATE_OUT_CLASS::elementScalar   = "scalar";
const char * const STATE_OUT_CLASS::elementVector   = "vector";
const char * const STATE_OUT_CLASS::elementValue    = "value";
const char * const STATE_OUT_CLASS::elementCompound = "compound";
const char * const STATE_OUT_CLASS::elementText     = "text";
const char * const STATE_OUT_CLASS::elementType     = "type";
const char * const STATE_OUT_CLASS::elementName     = "name";
const char * const STATE_OUT_CLASS::elementDesc     = "desc";

/**
 * Create a new stats ouput object and associate it with output
 * filename. We also perform all necessary setup for the underlying
 * XML representation of this output object.
 */
STATE_OUT_CLASS::STATE_OUT_CLASS (
    const char* filename)
{
    // create an XMLOut object for the stats file
    xmlStats = new XMLOut(
        filename,
        "asim-stats",
        "-//ASIM//DTD STATS 0.2//EN",
        "http://asim.intel.com/home/asim/xml/dtd/asim-stats-0.2.dtd");

    if (! xmlStats) 
    {
	ASIMERROR("Unable to create XML stats output file \"" << filename
            << "\", " << strerror(errno));
    }

    //xmlStats->SetDebug(); // turn on if this sucker crashes before output

    // add the default XSLT stylesheet for HTML conversion
    xmlStats->AddProcessingInstruction(
        "xml-stylesheet",
        "type=\"text/xsl\" href=\"http://asim.intel.com/home/asim/xml/xsl/asim-stats-html-0.2.xsl\"");

    // add default namespace to root element
    // (r2r) note: some tools (e.g. xmllint) still have issues with
    // namespaces, so we leave them out for now until this is fully
    // understood;
    //xmlStats->AddAttribute("xmlns", "http://asim.org/xml/stats/namespace");

    // add (required) version attribute to root element
    xmlStats->AddAttribute("version", "0.2");
}

/**
 * Sync the stats output to disk and destroy the output object.
 */
STATE_OUT_CLASS::~STATE_OUT_CLASS ()
{
    if (xmlStats)
    {
        // dump stats to file and delete object
        delete xmlStats;
    }
}

/**
 * Add a compound element to the output. This method takes care of
 * implementing the <compound> syntax of the XML asim-stats DTD.
 */
void
STATE_OUT_CLASS::AddCompound (
    const char* type,   ///< type of the compound element
    const char* name,   ///< name of the compound element
    const char* desc)   ///< description of the compound element
{

    xmlStats->AddElement(elementCompound);
    AddCommonInfo(type, name, desc);
}

/**
 * Close the current compound element and make its parent the current
 * element.
 */
void
STATE_OUT_CLASS::CloseCompound (void)
{
    xmlStats->CloseElement(); // compound
}

/**
 * Add a scalar element to the output. This method takes care of
 * implementing the <scalar> syntax of the XML asim-stats DTD.
 */
void
STATE_OUT_CLASS::AddScalar (
    const char* type,   ///< type of the scalar element
    const char* name,   ///< name of the scalar element
    const char* desc,   ///< description of the scalar element
    const char* value)  ///< value of the scalar element
{
    xmlStats->AddElement(elementScalar);
    AddCommonInfo(type, name, desc);

    // add the value
    ASSERT(value, "missing value in scalar stats output for "
        << "type: " << (type ? type : "(NULL)") << ", "
        << "name: " << (name ? name : "(NULL)") << ", "
        << "desc: " << (desc ? desc : "(NULL)")
    );
    xmlStats->AddText(value);

    xmlStats->CloseElement(); // scalar
}

/**
 * This method allows to add arbitrary text strings to the output.
 * The string is wrapped into an XML CDATA section in order to avoid it
 * from potentially disrupting the output format with strings that
 * look like XML formatting.
 *
 * @note Output in this way, albeit now safely encapsulated and
 * unable to disrupt other well formatted output, is <b>deprecated
 * and strongly discouraged</b>. This method is only available to aid
 * transitioning from the old arbitrarily formatted stats output
 * method to the new strictly formatted method. All an XML parser will
 * get back for this is the same arbitrarily formatted text string
 * that is taken as an argument here.
 */
void
STATE_OUT_CLASS::AddText (
    const char* text)   ///< text to be printed
{
    xmlStats->AddElement(elementText);
    if (text)
    {
        // An unparsed CDATA section makes it safe to have text that
        // looks like formatting and could otherwise disrupt the
        // validity or well-formedness of the XML output.
        //
        // To prevent any s*head from putting the only illegal
        // sequence "]]>" into the text stream (that would be the XML
        // sequence that terminates the CDATA section), we replace all
        // such sequences with "&#93;]>", which will end up being the
        // same after the parser replaces the char reference "&#93;" by
        // "]", but will prevent the parser from thinking the CDATA
        // section itself was terminated with it - sigh.
        string txt(text);
        string::size_type pos = 0; // start at beginning of string
        while ((pos = txt.find("]]>", pos)) != string::npos)
        {
            txt.replace(pos, 1, "&#93;");
        }
        xmlStats->AddCDATA(txt.c_str());
    }
    xmlStats->CloseElement(); // text
}

/**
 * This method adds the common elements "type", "name", and "value"
 * into the current output context. Each element is only added if it
 * is non-empty.
 */
void
STATE_OUT_CLASS::AddCommonInfo (
    const char* type,    ///< type of current element
    const char* name,    ///< name of current element
    const char* desc)    ///< description of current element
{
    if (type)
    {
        xmlStats->AddElement(elementType);
        xmlStats->AddText(type);
        xmlStats->CloseElement(); // type
    }
    if (name)
    {
        xmlStats->AddElement(elementName);
        xmlStats->AddText(name);
        xmlStats->CloseElement(); // name
    }
    if (desc)
    {
        xmlStats->AddElement(elementDesc);
        xmlStats->AddText(desc);
        xmlStats->CloseElement(); // desc
    }
}
