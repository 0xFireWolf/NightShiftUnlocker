//
//  OSVersion.cpp
//  NightShiftUnlocker
//
//  Created by FireWolf on 2017-05-20.
//  Copyright © 2017 FireWolf. All rights reserved.
//

#include "OSVersion.hpp"
#include <IOKit/IOLib.h>
#include <libkern/version.h>

OSVersion::OSVersion()
{
    this->major = -1;
    
    this->minor = -1;
}

OSVersion::~OSVersion() {}

// Reference:
// https://en.wikipedia.org/wiki/MacOS_Sierra#Releases
bool OSVersion::parse()
{
    if (version_major == 16)
    {
        switch (version_minor)
        {
            case 0:
            case 1:
                this->minor = version_minor;
                
                break;
                
            case 3:
            case 4:
            case 5:
            case 6:
                this->minor = version_minor - 1;
                
                break;
                
            default:
                return false;
        }
    }
    else
    {
        // Currently not supported
        return false;
    }
    
    this->major = version_major - 4;
    
    return true;
}

int OSVersion::getMajorVersion()
{
    return this->major;
}

int OSVersion::getMinorVersion()
{
    return this->minor;
}

int OSVersion::getVersion()
{
    int major = this->major;
    
    if (major < 0)
    {
        return -1;
    }
    
    int minor = this->minor;
    
    if (minor < 0)
    {
        return -1;
    }
    
    return 10000 + major * 10 + minor;
}
