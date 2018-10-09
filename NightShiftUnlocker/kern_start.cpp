//
//  kern_start.cpp
//  NightShiftUnlocker
//
//  Created by FireWolf on 2017-05-19.
//  Copyright © 2017 FireWolf. All rights reserved.
//

#include <Headers/plugin_start.hpp>
#include <Headers/kern_api.hpp>
#include <Headers/kern_file.hpp>
#include <Headers/kern_mach.hpp>
#include <sys/vnode.h>

/**
    Read bytes from the given path
 
    @param fromPath The path of a file
    @param atOffset Start to read at the given offset
    @param numBytes Total number of bytes to read
    @return An array of bytes on success, `NULL` otherwise.
 */
static uint8_t* readBytes(const char* fromPath, off_t atOffset, size_t numBytes)
{
    vnode_t node = NULL;
    
    vfs_context_t context = vfs_context_create(NULL);
    
    if (vnode_lookup(fromPath, 0, &node, context) != 0)
    {
        IOLog("NightShiftUnlocker::readBytes() Error: Failed to read bytes.\n");
        
        vfs_context_rele(context);
        
        return NULL;
    }
    
    uint8_t* buffer = new uint8_t[numBytes];
    
    if (FileIO::readFileData(buffer, atOffset, numBytes, node, context) != 0)
    {
        IOLog("NightShiftUnlocker::readBytes() Error: Failed to read bytes.\n");
        
        delete [] buffer;
        
        buffer = NULL;
    }
    
    vnode_put(node);
    
    vfs_context_rele(context);
    
    return buffer;
}

static void lmemcpy(void* dst, const void* src, size_t length)
{
    for (int index = 0; index < length; index++)
    {
        ((uint8_t*) dst)[index] = ((uint8_t*) src)[index];
    }
}

// MARK: Boot Arguments
static const char* bootArgDisable[] = {"-nsuoff"};

static const char* bootArgDebug[] = {"-nsudbg"};

static const char* bootArgBeta[] = {"-nsubeta"};

// MARK: Night Shift Unlocker
static const uint32_t SectionActive = 1;

static const char* CoreBrightnessBinaryAA = "/System/Library/PrivateFrameworks/CoreBrightness.framework/Versions/A/CoreBrightness";

static const char* symbolAA = "_CBU_IsNightShiftSupported";

static const size_t numProcessesAA = 3;

using ProcFlags = UserPatcher::ProcInfo::ProcFlags;

static UserPatcher::ProcInfo processesAA[] =
{
    {"/usr/libexec/corebrightnessd", 28, SectionActive, UserPatcher::ProcInfo::ProcFlags::MatchExact},
    
    {"/System/Library/CoreServices/NotificationCenter.app/Contents/MacOS/NotificationCenter", 85, SectionActive, UserPatcher::ProcInfo::ProcFlags::MatchExact},
    
    {"/Applications/System Preferences.app/Contents/MacOS/System Preferences", 70, SectionActive, UserPatcher::ProcInfo::ProcFlags::MatchExact}
};

static const size_t numBytesBufferAA = 32;

static const size_t numBytesPatchAA = 5;

static uint8_t findBytesAA[numBytesBufferAA] = {};

// TODO: MORE TEST REQUIRED
static uint8_t replBytesAA[numBytesBufferAA] =
{
    0x31, 0xC0,     // xor eax, eax
    0xFF, 0xC0,     // inc eax,
    0xC3            // ret
};

static UserPatcher::BinaryModPatch patchAA =
{
    CPU_TYPE_X86_64,
    
    findBytesAA,
    
    replBytesAA,
    
    numBytesBufferAA,
    
    0,
    
    1,
    
    UserPatcher::FileSegment::SegmentTextText,
    
    SectionActive
};

static const size_t numModsAA = 1;

static UserPatcher::BinaryModInfo modAA =
{
    CoreBrightnessBinaryAA,
    
    &patchAA,
    
    1
};

/**
    Dynamically generate binary patches for the given magic bag
 
    @param forBag A Night Shift magic bag
    @param patcher The kernel patchere instance
 */
static void generatePatches(void* unused, KernelPatcher &patcher)
{
    IOLog("NightShiftUnlocker::generatePatches() Patcher callback started.\n");
    
    MachInfo* info = MachInfo::create();
    
    if (info == NULL)
    {
        IOLog("NightShiftUnlocker::generatePatches() Error: Failed to create the mach info.\n");
        
        panic("NightShiftUnlocker::generatePatches() Error: Failed to create the mach info.\n");
        
        return;
    }
    
    if (info->init(&CoreBrightnessBinaryAA) != KERN_SUCCESS)
    {
        IOLog("NightShiftUnlocker::generatePatches() Error: Failed to initialize the mach info.\n");
        
        info->deinit();
        
        MachInfo::deleter(info);
        
        panic("NightShiftUnlocker::generatePatches() Error: Failed to initialize the mach info.\n");
        
        return;
    }
    
    if (info->setRunningAddresses(0) != KERN_SUCCESS)
    {
        IOLog("NightShiftUnlocker::generatePatches() Error: Failed to set the running address.\n");
        
        info->deinit();
        
        MachInfo::deleter(info);
        
        panic("NightShiftUnlocker::generatePatches() Error: Failed to set the running address.\n");
        
        return;
    }
    
    // TODO: Future: Multiple Symbols???
    mach_vm_address_t address = info->solveSymbol(symbolAA);
    
    if (address == 0)
    {
        IOLog("NightShiftUnlocker::generatePatches() Error: Failed to resolve the symbol.\n");
        
        info->deinit();
        
        MachInfo::deleter(info);
        
        panic("NightShiftUnlocker::generatePatches() Error: Failed to resolve the symbol.\n");
        
        return;
    }
    
    IOLog("NightShiftUnlocker::generatePatches() Found symbol @ %02llX.\n", address);
    
    size_t length = numBytesBufferAA;
    
    uint8_t* buffer = readBytes(CoreBrightnessBinaryAA, address, length);
    
    if (buffer != NULL)
    {
        IOLog("NightShiftUnlocker::generatePatches() %zu bytes have been read.\n", length);
        
        // TODO: Future: Multiple Patches???
        size_t offset = numBytesPatchAA;
        
        lmemcpy(&findBytesAA[0], buffer, length);
        
        lmemcpy(&replBytesAA[offset], buffer + offset, length - offset);
        
        IOLog("NightShiftUnlocker::generatePatches() Binary patches have been generated.\n");
        
        delete [] buffer;
    }
    else
    {
        IOLog("NightShiftUnlocker::generatePatches() Error: Failed to read the file.\n");
        
        panic("NightShiftUnlocker::generatePatches() Error: Failed to read the file.\n");
    }
    
    info->deinit();
    
    MachInfo::deleter(info);
}

static bool unlock()
{
    IOLog("NSUnlocker::unlock() Attempt to unlock the Night Shift...\n");
    
    // Register the patch generator
    LiluAPI::Error retVal = lilu.onPatcherLoad(generatePatches);
    
    if (retVal != LiluAPI::Error::NoError)
    {
        IOLog("NSUnlocker::unlock() Error: Unable to register the patch generator.\n");
        
        return false;
    }
    
    retVal = lilu.onProcLoad(processesAA, numProcessesAA, NULL, NULL, &modAA, numModsAA);
    
    return retVal == LiluAPI::Error::NoError;
}

// MARK: Start the unlocker
static void startUnlocker()
{
    IOLog("NightShiftUnlocker::startUnlocker() Started...\n");
    
    if (unlock())
    {
        IOLog("NightShiftUnlocker: Unlock request has been submitted.\n");
    }
    else
    {
        panic("NightShiftUnlocker: Failed to submit the unlock request.\n");
    }
}

// MARK: Define the plugin configuration
PluginConfiguration ADDPR(config) =
{
    xStringify(PRODUCT_NAME),
    
    parseModuleVersion(xStringify(MODULE_VERSION)),
    
    LiluAPI::RunningMode::AllowNormal,
    
    bootArgDisable,
    
    1,
    
    bootArgBeta,

    1,
    
    bootArgDebug,
    
    1,
    
    KernelVersion::Sierra,
    
    KernelVersion::Mojave,

    startUnlocker
};
