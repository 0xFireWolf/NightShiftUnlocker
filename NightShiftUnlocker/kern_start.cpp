//
//  kern_start.cpp
//  NightShiftUnlocker
//
//  Created by FireWolf on 2017-05-19.
//  Copyright © 2017 FireWolf. All rights reserved.
//

#include <Headers/plugin_start.hpp>
#include <Headers/kern_api.hpp>
#include "NightShiftUnlocker.hpp"
#include "OSVersion.hpp"

// MARK: Boot Arguments
const char* bootArgDisable[] = {"-nsuoff"};

const char* bootArgDebug[] = {"-nsudbg"};

const char* bootArgBeta[] = {"-nsubeta"};

// MARK: Start the unlocker
void startUnlocker()
{
    OSVersion* osVersion = new OSVersion();
    
    if (!osVersion->parse())
    {
        IOLog("NightShiftUnlocker: Failed to get the current OS version.\n");
        
        IOLog("NightShiftUnlocker: Your OS version is probably not supported.\n");
        
        delete osVersion;
        
        return;
    }
    
    IOLog("NightShiftUnlocker: Started on macOS 10.%d.%d.\n", osVersion->getMajorVersion(), osVersion->getMinorVersion());
    
    NSUnlocker* unlocker = new NSUnlocker(osVersion->getVersion());
    
    if (unlocker->unlock())
    {
        IOLog("NightShiftUnlocker: Unlocked successfully.\n");
    }
    else
    {
        IOLog("NightShiftUnlocker: Failed to unlock.\n");
    }
    
    delete osVersion;
    
    delete unlocker;
}

// MARK: Define the plugin configuration
PluginConfiguration ADDPR(config) =
{
    xStringify(PRODUCT_NAME),
    
    parseModuleVersion(xStringify(MODULE_VERSION)),
    
    bootArgDisable,
    
    1,
    
    bootArgBeta,

    1,
    
    bootArgDebug,
    
    1,
    
    KernelVersion::Sierra,
    
    KernelVersion::Sierra,
    
    startUnlocker
};


