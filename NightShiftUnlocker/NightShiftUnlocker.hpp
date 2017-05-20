//
//  NightShiftUnlocker.hpp
//  NightShiftUnlocker
//
//  Created by FireWolf on 2017-05-19.
//  Copyright © 2017 FireWolf. All rights reserved.
//

#include <Headers/kern_user.hpp>

class NSUnlocker
{
public:
    NSUnlocker(int osVersion);
    
    ~NSUnlocker();
    
    bool unlock();
    
private:
    int osVersion;
    
    UserPatcher::ProcInfo* getProcessInfos(int* length);
    
    UserPatcher::BinaryModInfo* getBinaryInfos(int* length);
};
