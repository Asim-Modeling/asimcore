/*
 *Copyright (C) 2003-2006 Intel Corporation
 *
 *This program is free software; you can redistribute it and/or
 *modify it under the terms of the GNU General Public License
 *as published by the Free Software Foundation; either version 2
 *of the License, or (at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program; if not, write to the Free Software
 *Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/**
 * @file
 * @author David Goodwin
 * @brief Base class for ASIM module abstraction.
 */

// ASIM core
#include "asim/syntax.h"
#include "asim/module.h"
#include "asim/state.h"
#include "asim/stateout.h"

// ASIM public modules -- BAD! in asim-core
//#include "asim/provides/controller.h"

list<module_regex> ASIM_MODULE_CLASS::regexList;

ASIM_DRAL_NODE_CLASS::ASIM_DRAL_NODE_CLASS(ASIM_MODULE parent, const char* const name):
    active(true)
{
    // Assign event id for this node
    // Notify that a new dral node has been created. Such notification is required 
    // for the events to work properly.
    if (runWithEventsOn)
    {
        EVENT(uniqueId = DRALEVENT(NewNode(name, (parent ? parent->GetUniqueId() : 0), 0, true)));
    }
}

ASIM_MODULE_CLASS::ASIM_MODULE_CLASS(ASIM_MODULE       p,
                                     const char *const n,
				     ASIM_EXCEPT       e,
				     bool              create_thread) :
    ASIM_REGISTRY_CLASS(),
    ASIM_DRAL_NODE_CLASS(p,n),
    ASIM_CLOCKABLE_CLASS((ASIM_CLOCKABLE)p, /*create_thread=*/false), // handled in SetThreadHandle()
    ASIM_ADF_NODE_CLASS(p,n),
    name(strdup(n)), parent(p), eMod(e), contained(NULL),
    createThread(create_thread)
/*
 * Initialize...
 */
{
    const char *ppath = "";
    
    if (parent != NULL)
    {
        parent->contained = new ASIM_MODULELINK_CLASS(this, parent->contained);
        ppath = (char *)parent->Path();
    }

    path = new char[strlen(ppath)+strlen(name)+4];
    strcpy(path, ppath);
    strcat(path, "/");
    strcat(path, name);
    SetRegPath(path);

    traceName = path;

    for(int i = 0; traceName[i]; i++) {
        if(traceName[i] == '/') {
            traceName[i] = '.';
        }
    }
    if(traceName[0] == '.') {
        traceName.erase(0,1);
    }
    SetTraceableName(traceName);

    // Check if a new thread handle has to be created.
    // If not, set the thread handle to null.

    if ( !SetThreadHandle() )
    {
        SetHostThread (NULL);
    }
}

ASIM_MODULE_CLASS::~ASIM_MODULE_CLASS ()
{
    if (name) {
      free (const_cast<char*> (name));
    }
    if (path)
    {
        delete [] path;
    }
    while (contained)
    {
        ASIM_MODULELINK_CLASS * tmp = contained;
        contained = contained->next;
        delete tmp;
    }

    if (!didModuleCreateThread) {
        SetHostThread( NULL );
    }
}


ASIM_STATE
ASIM_MODULE_CLASS::GetState (const char *stateStr)
/*
 * Return the ASIM_STATE object for the state specified by
 * 'stateStr'. Return NULL is no state exists.
 */
{
    //
    // Check the first "entry" in 'stateStr' to see if it matches this
    // module's name. If it does, then pass it on to each contained module.
    //
    // Strip leading '/' if there is one.

    if (*stateStr == '/')
        stateStr++;

    if (strncasecmp(Name(), stateStr, strlen(Name())) != 0)
        return(NULL);

    //
    // 'stateStr' contains this module's name as the first entry. Strip the
    // name and pass 'stateStr' to each contained module.

    stateStr += strlen(Name());
    ASIM_MODULELINK scan = contained;
    while (scan != NULL)
    {
        ASIM_STATE state = scan->module->GetState(stateStr);
        if (state != NULL)
            return(state);

        scan = scan->next;
    }

    //
    // 'stateStr' was not found by any contained module, so check to see if
    // it is an actual state object in this module.  Returns NULL if none
    // is found. 
    //
    return (SearchStates(stateStr));
}

void
ASIM_MODULE_CLASS::PrintModuleStats (STATE_OUT stateOut)
/*
 * Print this modules statistics and then recursively the stats of all
 * contained modules.
 */
{
    stateOut->AddCompound("module", Name());

    DumpStats(stateOut);

    ASIM_MODULELINK scan = contained;
    while (scan != NULL)
    {
        scan->module->PrintModuleStats(stateOut);
        scan = scan->next;
    }

    stateOut->CloseCompound(); // module
}

void
ASIM_MODULE_CLASS::PrintModuleStats(char *filename)
{
    // create a STATE_OUT object for the stats file
    STATE_OUT stateOut = new STATE_OUT_CLASS(filename);

    if (! stateOut) 
    {
        ASIMERROR("Unable to create stats output file \"" << filename
            << "\", " << strerror(errno));
    }
    
    PrintModuleStats(stateOut);	

    // dump stats to file and delete object
    delete stateOut;
}

void
ASIM_MODULE_CLASS::ClearModuleStats ()
/*
 * Clear this module statistics and then recursively the stats of all
 * contained modules.
 */
{

    ClearStats();

    ASIM_MODULELINK scan = contained;
    while (scan != NULL)
    {
        scan->module->ClearModuleStats();
        scan = scan->next;
    }
}

// Functional save
void
ASIM_MODULE_CLASS::DumpFunctionalState (ostream& saveFuncState)
/*
 * Dump the functional state of this module
 * Do we want recursive dump of all contained modules?
 */
{
    // This method will likely be overridden by modules to
    // dump their state as they like, however we provide
    // a default action of just printing a message here
    // This should be a NOP if the module does not implement
    // state save/restore
    
    //cout << "Saving state in DumpFunctionalState for " << Name() << "\n";
    
    ASIM_MODULELINK scan = contained;
    while (scan != NULL)
    {
        scan->module->DumpFunctionalState(saveFuncState);
        scan = scan->next;
    }
}

void
ASIM_MODULE_CLASS::LoadFunctionalState (istream& loadFuncState)
/*
 * Restore the functional state of this module
 * Do we want recursive dump of all contained modules?
 */
{
    // This method will likely be overridden by modules to
    // dump their state as they like, however we provide
    // a default action of just printing a message here
    // This should be a NOP if the module does not implement
    // state save/restore
    
    //cout << "Restoring state in LoadFunctionalState for " << Name() << endl; 
        
    ASIM_MODULELINK scan = contained;
    while (scan != NULL)
    {
        scan->module->LoadFunctionalState(loadFuncState);
        scan = scan->next;
    }
}


// Function to create a new thread handle when necessary.
bool ASIM_MODULE_CLASS::SetThreadHandle()
{
    didModuleCreateThread = false;

    // If PTHREAD_PER_<module> is set to 1 statically, create a 
    // pthread and return.
    if (createThread) {
        SetHostThread (new ASIM_SMP_THREAD_HANDLE_CLASS);
        return true;
    }
    
    list<module_regex>::iterator iter;
    
    // Loop through the list of regexes and match against the 
    // current module's name/path
    for (iter = regexList.begin(); iter != regexList.end(); ++iter) {
        
        // If there is no regex, return false.
        // We should not hit this condition.
        if ((*iter).regex == "") {
            return false;
        }
        
        // Build a regular expression for regex
        Regex *regex = new Regex();
        string m_regex = (*iter).regex;

        if(!regex->assign(m_regex.c_str(), false)) 
        {
             delete(regex);
             return false;
        }

        // Check if the regex matches the module name or path
        if ( regex->match(name) || regex->match(path) )
          {
              static ASIM_SMP_THREAD_HANDLE temp_handle = NULL;
              didModuleCreateThread = true;
            
              // Creating this temp_handle is not necessary
              // However, simulation fails if prevHandle is directly 
              // used.

              if (!temp_handle) {
                  temp_handle = new ASIM_SMP_THREAD_HANDLE_CLASS;
                  (*iter).prevHandle = temp_handle;
                  cout << "Running module " << path << " in a separate thread.\n";
              } 
              
              SetHostThread ( (*iter).prevHandle );
              
              if (++(*iter).count == (*iter).limit) {
                  temp_handle = NULL;
                  (*iter).prevHandle = temp_handle;
                  (*iter).count = 0;
              }
              return true;
          }
    }
    return false;
}

// Function to store the multi-threading regexes specified on 
// the command line.
void ASIM_MODULE_CLASS::SetModuleRegex(string regex, int limit)
{
    module_regex user_regex;

    user_regex.regex = regex;
    user_regex.limit = limit;
    user_regex.count = 0;
    user_regex.prevHandle = NULL;

    regexList.push_front (user_regex);
}

//
// To understand this function, see the color coding explanation in function
// FillUpdate in the tarantula workbench (in $ASIMDIR/config/wb/tarantula.cfg)
//
//
UINT32
ColorCoding(UINT32 nentries, UINT32 hwm, UINT32 max)
{
    if ( nentries == 0 ) return 0;
    if ( nentries == max ) return 4;
    if ( nentries >= hwm ) return 3;
    
    //
    // Two shade colors for how far we are from the HWM
    hwm /= 2;
    if ( nentries >= hwm ) return 2;
    return 1;
}



// ADF node constructor. it creates a linked list among all ADF_NODES.
ASIM_ADF_NODE_CLASS::ASIM_ADF_NODE_CLASS(ASIM_ADF_NODE parent, const char * const name)
{
    descendants = NULL;
    next = NULL;
    if (parent != NULL) {
        next = parent->descendants;
        parent->descendants = this;
    }
}

// ADF node destructor
ASIM_ADF_NODE_CLASS::~ASIM_ADF_NODE_CLASS()
{
}


// Get all general ADF definitions for all nodes
string 
ASIM_ADF_NODE_CLASS::GetWatchWindowADF()
{
    ASIM_ADF_NODE list = descendants;
    
    while (list) {
        watchwindow_adf << list->GetWatchWindowADF();
        list = list->next;
    }    

    return watchwindow_adf.str();
}

// Global list of tags used in the ADF file
set<string> ASIM_ADF_NODE_CLASS::stagList;

// Get all Watch window ADF tag desfriptor definitions for all nodes
string 
ASIM_ADF_NODE_CLASS::GetWatchWindowADFTagDescriptors()
{
    ostringstream adf;

    for(set<string>::iterator i = stagList.begin(); i != stagList.end(); i++) {
        adf << "    <tagdescriptor tagname=\"" << (*i) << "\" type=\"integer\"/>" << endl;
    }

    return adf.str();
}
