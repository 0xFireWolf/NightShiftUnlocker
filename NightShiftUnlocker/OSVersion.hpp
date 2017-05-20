//
//  OSVersion.hpp
//  NightShiftUnlocker
//
//  Created by FireWolf on 2017-05-20.
//  Copyright © 2017 FireWolf. All rights reserved.
//

#ifndef OSVersion_hpp
#define OSVersion_hpp

class OSVersion
{
public:
    OSVersion();
    
    ~OSVersion();
    
    bool parse();
    
    int getMajorVersion();
    
    int getMinorVersion();
    
    int getVersion();
    
private:
    int major;
    
    int minor;
};

#endif /* OSVersion_hpp */
