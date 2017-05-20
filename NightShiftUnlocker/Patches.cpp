//
//  Patches.cpp
//  NightShiftUnlocker
//
//  Created by FireWolf on 5/19/17.
//  Copyright © 2017 FireWolf. All rights reserved.
//

#include "Patches.hpp"

const uint32_t SectionActive = 1;

// 10.12.4~10.12.5: Processes
UserPatcher::ProcInfo processInfos[] =
{
    {"/usr/libexec/corebrightnessd", 28, SectionActive},
    
    {"/System/Library/CoreServices/NotificationCenter.app/Contents/MacOS/NotificationCenter", 85, SectionActive}
};

// 10.12.4~10.12.5: 16 Bytes Patches
const size_t numBytes = 18;

const uint8_t findBytes[] =
{
    0x41, 0x57,                     // push r15
    0x41, 0x56,                     // push r14
    0x41, 0x55,                     // push r13
    0x41, 0x54,                     // push r12
    0x53,                           // push rbx
    0x48, 0x83, 0xEC, 0x18,         // sub rsp, 0x18
    0xBF, 0x01, 0x00, 0x00, 0x00    // mov edi, 0x1
};

const uint8_t replaceBytes[] =
{
    0x48, 0x31, 0xC0,               // xor rax, rax
    0x48, 0xFF, 0xC0,               // inc rax
    0x5D,                           // pop rbp
    0xC3,                           // ret
    0x90, 0x90, 0x90, 0x90, 0x90,   // unused
    0x90, 0x90, 0x90, 0x90, 0x90    // unused
};

// 10.12.4~10.12.5: Binary Patches
UserPatcher::BinaryModPatch patches[] =
{
    {
        CPU_TYPE_X86_64,
        
        findBytes,
        
        replaceBytes,
        
        numBytes,
        
        0,
        
        1,
        
        UserPatcher::FileSegment::SegmentTextText,
        
        SectionActive
    }
};

// 10.12.4~10.12.5: Target Binary Info
UserPatcher::BinaryModInfo modInfos[] = {{"/System/Library/PrivateFrameworks/CoreBrightness.framework/Versions/A/CoreBrightness", patches, 1}};
