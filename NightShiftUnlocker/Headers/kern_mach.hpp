//
//  kern_mach.hpp
//  Lilu
//
//  Certain parts of code are the subject of
//   copyright © 2011, 2012, 2013, 2014 fG!, reverser@put.as - http://reverse.put.as
//  Copyright © 2016-2017 vit9696. All rights reserved.
//

#ifndef kern_mach_hpp
#define kern_mach_hpp

#include <Headers/kern_config.hpp>
#include <Headers/kern_util.hpp>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/vnode.h>
#include <mach-o/loader.h>
#include <mach/vm_param.h>
#include <libkern/c++/OSDictionary.h>

class MachInfo {
	mach_vm_address_t running_text_addr {0}; // the address of running __TEXT segment
	mach_vm_address_t disk_text_addr {0};    // the same address at from a file
	mach_vm_address_t kaslr_slide {0};       // the kernel aslr slide, computed as the difference between above's addresses
	uint8_t *file_buf {nullptr};             // read file data
	OSDictionary *prelink_dict {nullptr};    // read prealinked kext dictionary
	uint8_t *prelink_addr {nullptr};         // prelink text base address
	mach_vm_address_t prelink_vmaddr {0};    // prelink text base vm address (for kexts this is their actual slide)
	uint32_t file_buf_size {0};              // read file data size
	uint8_t *linkedit_buf {nullptr};         // pointer to __LINKEDIT buffer containing symbols to solve
	uint64_t linkedit_fileoff {0};           // __LINKEDIT file offset so we can read
	uint64_t linkedit_size {0};
	uint32_t symboltable_fileoff {0};        // file offset to symbol table - used to position inside the __LINKEDIT buffer
	uint32_t symboltable_nr_symbols {0};
	uint32_t stringtable_fileoff {0};        // file offset to string table
	mach_header_64 *running_mh {nullptr};    // pointer to mach-o header of running kernel item
	off_t fat_offset {0};                    // additional fat offset
	size_t memory_size {HeaderSize};         // memory size
	bool kaslr_slide_set {false};            // kaslr can be null, used for disambiguation
	bool allow_decompress {true};            // allows mach decompression
	bool prelink_slid {false};               // assume kaslr-slid kext addresses

	/**
	 *  16 byte IDT descriptor, used for 32 and 64 bits kernels (64 bit capable cpus!)
	 */
	struct descriptor_idt {
		uint16_t offset_low;
		uint16_t seg_selector;
		uint8_t reserved;
		uint8_t flag;
		uint16_t offset_middle;
		uint32_t offset_high;
		uint32_t reserved2;
	};
	
	/**
	 *  Retrieve the address of the IDT
	 *
	 *  @return always returns the IDT address
	 */
	mach_vm_address_t getIDTAddress();
	
	/**
	 *  Calculate the address of the kernel int80 handler
	 *
	 *  @return always returns the int80 handler address
	 */
	mach_vm_address_t calculateInt80Address();
	
	/**
	 *  Retrieve LC_UUID command value from a mach header
	 *
	 *  @param header mach header pointer
	 *
	 *  @return UUID or nullptr
	 */
	uint64_t *getUUID(void *header);
	
	/**
	 *  Enable/disable the Write Protection bit in CR0 register
	 *
	 *  @param enable the desired value
	 *
	 *  @return KERN_SUCCESS if succeeded
	 */
	static kern_return_t setWPBit(bool enable);
	
	/**
	 *  Retrieve the first pages of a binary at disk into a buffer
	 *  Version that uses KPI VFS functions and a ripped uio_createwithbuffer() from XNU
	 *
	 *  @param buffer     allocated buffer sized no less than HeaderSize
	 *  @param vnode      file node
	 *  @param ctxt       filesystem context
	 *  @param decompress enable decompression
	 *  @param off        fat offset or 0
	 *
	 *  @return KERN_SUCCESS if the read data contains 64-bit mach header
	 */
	kern_return_t readMachHeader(uint8_t *buffer, vnode_t vnode, vfs_context_t ctxt, off_t off=0);

	/**
	 *  Retrieve the whole linkedit segment into target buffer from kernel binary at disk
	 *
	 *  @param vnode file node
	 *  @param ctxt  filesystem context
	 *
	 *  @return KERN_SUCCESS on success
	 */
	kern_return_t readLinkedit(vnode_t vnode, vfs_context_t ctxt);
	
	/**
	 *  Retrieve necessary mach-o header information from the mach header
	 *
	 *  @param header read header sized no less than HeaderSize
	 */
	void processMachHeader(void *header);

	/**
	 *  Load kext info dictionary and addresses if they were not loaded previously
	 */
	void updatePrelinkInfo();

	/**
	 *  Lookup mach image in prelinked image
	 *
	 *  @param identifier  identifier
	 *  @param imageSize   size of the returned buffer
	 *  @param slide       actual slide for symbols (normally kaslr or 0)
	 *  @param missing     set to true on successful prelink parsing when image is not needed
	 *
	 *  @return pointer to const buffer on success or nullptr
	 */
	uint8_t *findImage(const char *identifier, uint32_t &imageSize, mach_vm_address_t &slide, bool &missing);
	
	MachInfo(bool asKernel, const char *id) : isKernel(asKernel), objectId(id) {
		DBGLOG("mach", "MachInfo asKernel %d object constructed", asKernel);
	}
	MachInfo(const MachInfo &) = delete;
	MachInfo &operator =(const MachInfo &) = delete;
	
public:

	/**
	 *  Each header is assumed to fit two pages
	 */
	static constexpr size_t HeaderSize {PAGE_SIZE_64*2};
	
	/**
	 *  Representation mode (kernel/kext)
	 */
	EXPORT const bool isKernel;
	
	/**
	 *  Specified file identifier
	 */
	EXPORT const char *objectId {nullptr};

	/**
	 *  MachInfo object generator
	 *
	 *  @param asKernel this MachInfo represents a kernel
	 *  @param id       kinfo identifier (e.g. CFBundleIdentifier)
	 *
	 *  @return MachInfo object or nullptr
	 */
	static MachInfo *create(bool asKernel=false, const char *id=nullptr) { return new MachInfo(asKernel, id); }
	static void deleter(MachInfo *i) { delete i; }

	/**
	 *  Resolve mach data in the kernel
	 *
	 *  @param paths      filesystem paths for lookup
	 *  @param num        the number of paths passed
	 *  @param prelink    prelink information source (i.e. Kernel MachInfo)
	 *  @param fsfallback fallback to reading from filesystem if prelink failed
	 *
	 *  @return KERN_SUCCESS if loaded
	 */
	EXPORT kern_return_t init(const char * const paths[], size_t num = 1, MachInfo *prelink=nullptr, bool fsfallback=false);
	
	/**
	 *  Release the allocated memory, must be called regardless of the init error
	 */
	EXPORT void deinit();

	/**
	 *  retrieve the mach header and __TEXT addresses
	 *
	 *  @param slide load slide if calculating for kexts
	 *  @param size  memory size
	 *  @param force force address recalculation
	 *
	 *  @return KERN_SUCCESS on success
	 */
	EXPORT kern_return_t getRunningAddresses(mach_vm_address_t slide=0, size_t size=0, bool force=false);
	
	/**
	 *  Set the mach header address
	 *
	 *  @param slide load address
	 *  @param size  memory size
	 *
	 *  @return KERN_SUCCESS on success
	 */
	EXPORT kern_return_t setRunningAddresses(mach_vm_address_t slide=0, size_t size=0);

	/**
	 *  retrieve running mach positions
	 *
	 *  @param header pointer to header
	 *  @param size   file size
	 */
	EXPORT void getRunningPosition(uint8_t * &header, size_t &size);

	/**
	 *  solve a mach symbol (running addresses must be calculated)
	 *
	 *  @param symbol symbol to solve
	 *
	 *  @return running symbol address or 0
	 */
	EXPORT mach_vm_address_t solveSymbol(const char *symbol);

	/**
	 *  find the kernel base address (mach-o header)
	 *  by searching backwards using the int80 handler as starting point
	 *
	 *  @return kernel base address or 0
	 */
	EXPORT mach_vm_address_t findKernelBase();

	/**
	 *  enable/disable interrupt handling
	 *  this is similar to ml_set_interrupts_enabled except the return value
	 *
	 *  @param enable the desired value
	 *
	 *  @return true if changed the value and false if it is unchanged
	 */
	EXPORT static bool setInterrupts(bool enable);
	
	/**
	 *  enable/disable kernel memory write protection
	 *
	 *  @param enable  the desired value
	 *  @param lock    use spinlock to disable cpu preemption (see KernelPatcher::kernelWriteLock)
	 *
	 *  @return KERN_SUCCESS if succeeded
	 */
	EXPORT static kern_return_t setKernelWriting(bool enable, IOSimpleLock *lock);
	
	/**
	 *  Compare the loaded kernel with the passed kernel header
	 *
	 *  @param kernel_header 64-bit mach header of at least HeaderSize size
	 *
	 *  @return true if the kernel uuids match
	 */
	EXPORT bool isCurrentKernel(void *kernelHeader);
	
	/**
	 *  Find section bounds in a passed binary for provided cpu
	 *
	 *  @param ptr         pointer to a complete mach binary
	 *  @param vmsegment   returned vm segment pointer
	 *  @param vmsection   returned vm section pointer
	 *  @param sectionptr  returned section pointer
	 *  @param size        returned section size or 0 on failure
	 *  @param segmentName segment name
	 *  @param sectionName section name
	 *  @param cpu         cpu to look for in case of fat binaries
	 */
	EXPORT static void findSectionBounds(void *ptr, vm_address_t &vmsegment, vm_address_t &vmsection, void *&sectionptr, size_t &size, const char *segmentName="__TEXT", const char *sectionName="__text", cpu_type_t cpu=CPU_TYPE_X86_64);

	/**
	 *  Request to free file buffer resources (not including linkedit symtable)
	 */
	void freeFileBufferResources();
};

#endif /* kern_mach_hpp */
