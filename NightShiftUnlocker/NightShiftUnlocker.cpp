//
//  NightShiftUnlocker.cpp
//  NightShiftUnlocker
//
//  Created by FireWolf on 2017-05-19.
//  Copyright © 2017 FireWolf. All rights reserved.
//

#include <IOKit/IOLib.h>
#include <stdint.h>
#include <Headers/kern_util.hpp>
#include <Headers/kern_api.hpp>
#include "NightShiftUnlocker.hpp"
#include "Patches.hpp"

/// Constructor
NSUnlocker::NSUnlocker(int osVersion)
{
    // For future use
    this->osVersion = osVersion;
    
    IOLog("NSUnlocker::init() NSUnlocker started on macOS %d.\n", osVersion);
}

/// Destructor
NSUnlocker::~NSUnlocker()
{
    
}

/**
    Unlock the Night Shift for all Macs
 
    @return `true` on success, `false` otherwise.
 */
bool NSUnlocker::unlock()
{
    IOLog("NSUnlocker::unlock() Attempt to unlock the Night Shift...\n");
    
    int numInfos = 0;
    
    UserPatcher::ProcInfo* processes = this->getProcessInfos(&numInfos);
    
    if (processes == NULL)
    {
        IOLog("NSUnlocker::unlock() Failed to unlock: Unsupported processes.\n");
        
        return false;
    }
    
    int numMods = 0;
    
    UserPatcher::BinaryModInfo* mods = this->getBinaryInfos(&numMods);
    
    if (mods == NULL)
    {
        IOLog("NSUnlocker::unlock() Failed to unlock: Unsupported binary mods.\n");
        
        return false;
    }
    
    lilu.onProcLoad(processes, numInfos, NULL, NULL, mods, numMods);
    
    return true;
}

/**
    Get relevent process infos
 
    @param length The number of relevent processes on return
    @return An array of process infos, `NULL` if found an unsupported OS.
    @warning The caller is not required to release the returned pointer.
 */
UserPatcher::ProcInfo* NSUnlocker::getProcessInfos(int* length)
{
    *length = 0;
    
    switch (this->osVersion)
    {
        case 10124:
        case 10125:
            *length = 2;
            
            return processInfos;
            
        default:
            return NULL;
    }
}

/**
    Get the binary modification lists
 
    @param length The number of entries in the list on return
    @return A mod list, `NULL` if found an unsupported OS.
    @warning The caller is not required to release the returned pointer.
 */
UserPatcher::BinaryModInfo* NSUnlocker::getBinaryInfos(int* length)
{
    *length = 0;
    
    switch (this->osVersion)
    {
        case 10124:
        case 10125:
            *length = 1;
            
            return modInfos;
            
        default:
            return NULL;
    }
}
