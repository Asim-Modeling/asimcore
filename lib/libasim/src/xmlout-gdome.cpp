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
 * @brief Implementation of XML Output Object
 * @see @ref XMLOut_Documentation "XML Output Documentation"
 * @ingroup XMLOut
 */

// ASIM core
#include "asim/mesg.h"
#include "asim/xmlout.h"

/**
 * Creates a new XML Output object associated with an output file.
 * Debugging turns on synchronous output on all operations, which
 * helps determine the output state of the XMLOut object in case the
 * application crashes before the XMLOut object would normally be
 * written to disk.
 */
XMLOut::XMLOut (
    const char* filename,    ///< output file name
    const char* root,        ///< name of XML root element
    const char* dtdPublicId, ///< PUBLIC-ID of XML <!DOCTYPE
    const char* dtdSystemId, ///< SYSTEM-ID of XML <!DOCTYPE
    const bool debug)        ///< if true, turn on synchronous output, ie.
                             ///< the XML tree is written out anew after
                             ///< every operation
: domDoc(NULL), currentNode(NULL)
{
    this->filename = filename;
    this->debug = debug;

    // catch DOM exceptions and turn them into ASIMERROR
    try {
        // initialize XML DOM subsystem ...
        // create a document type
        DOM::DocumentType docType;

        if (dtdPublicId || dtdSystemId) {
            docType = domImpl.createDocumentType(root, dtdPublicId,
                dtdSystemId);
        }

        // create a document and get its root node
        char* namespaceURI = NULL; //"http://asim.org/namespace/stats";
        domDoc = domImpl.createDocument(namespaceURI, root, docType);
        currentNode = domDoc.get_documentElement();
        // add creator's comment to beginning of document
        DOM::Comment comment = domDoc.createComment("Created by ASIM XMLOut");
        domDoc.insertBefore(comment, domDoc.get_firstChild());
    } catch (DOM::DOMException exc) {
        ASIMERROR("DOM Exception: " << exc.code << " " << exc.msg << endl);
    }

    // do debugging operations if turned on
    ConditionalDebug();
}

/**
 * Dump the XML Object to its output file and destroy the object.
 */
XMLOut::~XMLOut ()
{
    // generate output
    DumpToFile();

    // free all assiciated objects
    currentNode = DOM::Element(NULL);
    domDoc.removeChild(domDoc.get_documentElement());
    domDoc = DOM::Document(NULL);
}

/**
 * Add an element node to the XML tree and make it the current node
 */
void
XMLOut::AddElement (
    const char* name)  ///< tag name of the new element
{
    // catch DOM exceptions and turn them into ASIMERROR
    try {
        // create a new element node, append it to the current node,
        // and make it the current node
        DOM::Element elem = domDoc.createElement(name);
        currentNode = currentNode.appendChild(elem);
    } catch (DOM::DOMException exc) {
        ASIMERROR("DOM Exception: " << exc.code << " " << exc.msg << endl);
    }

    // do debugging operations if turned on
    ConditionalDebug();
}

/**
 * Close the current node and make its parent the new current node
 */
void
XMLOut::CloseElement(void)
{
    // catch DOM exceptions and turn them into ASIMERROR
    try {
        // step back up to parent node
        currentNode = currentNode.get_parentNode();
    } catch (DOM::DOMException exc) {
        ASIMERROR("DOM Exception: " << exc.code << " " << exc.msg << endl);
    }

    // do debugging operations if turned on
    ConditionalDebug();
}

/**
 * Add an attribute node to the current node
 */
void
XMLOut::AddAttribute (
    const char* name,   ///< attribute name to add
    const char* value)  ///< attribute value to add
{
    // catch DOM exceptions and turn them into ASIMERROR
    try {
        currentNode.setAttribute(name, value);
    } catch (DOM::DOMException exc) {
        ASIMERROR("DOM Exception: " << exc.code << " " << exc.msg << endl);
    }

    // do debugging operations if turned on
    ConditionalDebug();
}

/**
 * Add a text node to the current node
 */
void
XMLOut::AddText (
    const char* text)  ///< text to add
{
    // catch DOM exceptions and turn them into ASIMERROR
    try {
        // put the text into a text node
        // and make that the child of the current node;
        DOM::Text domText = domDoc.createTextNode(text);
        currentNode.appendChild(domText);
    } catch (DOM::DOMException exc) {
        ASIMERROR("DOM Exception: " << exc.code << " " << exc.msg << endl);
    }

    // do debugging operations if turned on
    ConditionalDebug();
}

/**
 * Add a CDATA node to the current node
 */
void
XMLOut::AddCDATA (
    const char* text)  ///< text to add
{
    // catch DOM exceptions and turn them into ASIMERROR
    try {
        // put the text into a CDATA (unparsed Character DATA)
        // section and make that a child of the current node;
        DOM::CDATASection cdata = domDoc.createCDATASection(text);
        currentNode.appendChild(cdata);
    } catch (DOM::DOMException exc) {
        ASIMERROR("DOM Exception: " << exc.code << " " << exc.msg << endl);
    }

    // do debugging operations if turned on
    ConditionalDebug();
}

/**
 * Add a comment node to the current node
 */
void
XMLOut::AddComment (
    const char* text)  ///< text to add
{
    // catch DOM exceptions and turn them into ASIMERROR
    try {
        // put the text into a comment node
        // and make that a child of the current node;
        DOM::Comment comment = domDoc.createComment(text);
        currentNode.appendChild(comment);
    } catch (DOM::DOMException exc) {
        ASIMERROR("DOM Exception: " << exc.code << " " << exc.msg << endl);
    }

    // do debugging operations if turned on
    ConditionalDebug();
}

/**
 * Add a processing instruction. Processing instructions are always
 * added just before the root node, and do not affect the position of
 * the current node.
 */
void
XMLOut::AddProcessingInstruction (
    const char* target,  ///< target part of processing instruction to add
                         ///< (ie. the first string after opening <? )
    const char* data)    ///< data part of processing instruction
                         ///< (ie. everything between target and closing ?> )
{
    // catch DOM exceptions and turn them into ASIMERROR
    try {
        // put the tag into a processing instruction node
        DOM::ProcessingInstruction procInst =
            domDoc.createProcessingInstruction(target, data);
        // insert PI immediately before document root element
        domDoc.insertBefore(procInst, domDoc.get_documentElement());
    } catch (DOM::DOMException exc) {
        ASIMERROR("DOM Exception: " << exc.code << " " << exc.msg << endl);
    }

    // do debugging operations if turned on
    ConditionalDebug();
}

/**
 * Dump current XML tree to an output file
 */
void
XMLOut::DumpToFile (void)
{
    // catch DOM exceptions and turn them into ASIMERROR
    try {
        domImpl.saveDocumentToFile(domDoc, filename.c_str(),
            GDOME_SAVE_LIBXML_INDENT);
    } catch (DOM::DOMException exc) {
        ASIMERROR("DOM Exception: " << exc.code << " " << exc.msg << endl);
    }
}

/**
 * Dump current XML tree to an ostream
 */
void
XMLOut::DumpToStream (
    ostream& out)  ///< ostream to dump XML tree to
{
    ASIMERROR ("XMLOut::DumpToStream: unimplemented method!" << endl);
}
