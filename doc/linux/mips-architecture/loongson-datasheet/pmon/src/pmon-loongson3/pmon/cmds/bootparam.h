#ifndef __ASM_MACH_LOONGSON_BOOT_PARAM_H_
#define __ASM_MACH_LOONGSON_BOOT_PARAM_H_
#include<sys/linux/types.h>

//typedef unsigned long long u64;//the temporary modification,or need to modify 44 branch.This is not the spec.

#define SPEC_VERS	0x2
#define SYSTEM_RAM_LOW		1
#define SYSTEM_RAM_HIGH		2
#define MEM_RESERVED		3
#define PCI_IO			4
#define PCI_MEM			5
#define LOONGSON_CFG_REG	6
#define VIDEO_ROM		7
#define ADAPTER_ROM		8
#define ACPI_TABLE		9
#define SMBIOS_TABLE		10
#define UMA_VIDEO_RAM		11
#define VUMA_VIDEO_RAM          12
#define MAX_MEMORY_TYPE         13

#define VRAM_TYPE_SP	0
#define VRAM_TYPE_UMA	1

#define DMA64_SUPPORT 0x2

#define LOONGSON3_BOOT_MEM_MAP_MAX 128
struct efi_memory_map_loongson{
	u16 vers;	/* version of efi_memory_map */
	u32 nr_map;	/* number of memory_maps */
	u32 mem_freq;	/* memory frequence */
	struct mem_map{
		u32 node_id;	/* node_id which memory attached to */
		u32 mem_type;	/* system memory, pci memory, pci io, etc. */
		u64 mem_start;	/* memory map start address */
		u32 mem_size;	/* each memory_map size, not the total size */
	}map[LOONGSON3_BOOT_MEM_MAP_MAX];
}__attribute__((packed));

enum loongson_cpu_type
{
	Legacy_2F = 0x0,
	Legacy_2E = 0x1,
	Legacy_3A = 0x2,
	Legacy_3B = 0x3,
	Legacy_1A = 0x4,
	Legacy_1B = 0x5,
	Legacy_2G = 0x6,
	Legacy_2H = 0x7,
	Loongson_1A = 0x100,
	Loongson_1B = 0x101,
	Loongson_2E = 0x200,
	Loongson_2F = 0x201,
	Loongson_2G = 0x202,
	Loongson_2H = 0x203,
	Loongson_3A = 0x300,
	Loongson_3B = 0x301
};

/*
 * Capability and feature descriptor structure for MIPS CPU
 */
struct efi_cpuinfo_loongson {
	u16 vers;     /* version of efi_cpuinfo_loongson */
	u32 processor_id; /* PRID, e.g. 6305, 6306 */
	enum loongson_cpu_type cputype; /* 3A, 3B, etc. */
	u32 total_node;   /* num of total numa nodes */
	u16 cpu_startup_core_id; /* Core id */
	u16 reserved_cores_mask;
	u32 cpu_clock_freq; /* cpu_clock */
	u32 nr_cpus;
	u8 cpuname[64]; /*cpu name*/
	
}__attribute__((packed));

#define MAX_UARTS 64
struct uart_device {
	u32 iotype; /* see include/linux/serial_core.h */
	u32 uartclk;
	u32 int_offset;
	u64 uart_base;
}__attribute__((packed));

#define MAX_SENSORS 64
#define SENSOR_TEMPER  0x00000001
#define SENSOR_VOLTAGE 0x00000002
#define SENSOR_FAN     0x00000004
struct sensor_device {
	char name[32];  /* a formal name */
	char label[64]; /* a flexible description */
	u32 type;       /* SENSOR_* */
	u32 id;         /* instance id of a sensor-class */
	u32 fan_policy; /* see arch/mips/include/asm/mach-loongson/loongson_hwmon.h */
	u32 fan_percent;/* only for constant speed policy */
	u64 base_addr;  /* base address of device registers */
}__attribute__((packed));

struct system_loongson{
	u16 vers;     /* version of system_loongson */
	u32 ccnuma_smp; /* 0: no numa; 1: has numa */
	u32 sing_double_channel; /* 1:single; 2:double */
	u32 nr_uarts;
	struct uart_device uarts[MAX_UARTS];
	u32 nr_sensors;
	struct sensor_device sensors[MAX_SENSORS];
	char has_ec;
	char ec_name[32];
	u64 ec_base_addr;
	char has_tcm;
	char tcm_name[32];
	u64 tcm_base_addr;
	u64 workarounds; /* see workarounds.h */
	u64 of_dtb_addr
}__attribute__((packed));

struct irq_source_routing_table {
	u16 vers;
	u16 size;
	u16 rtr_bus;
	u16 rtr_devfn;
	u32 vendor;
	u32 device;
	u32 PIC_type;   /* conform use HT or PCI to route to CPU-PIC */
	u64 ht_int_bit; /* 3A: 1<<24; 3B: 1<<16 */
	u64 ht_enable;  /* irqs used in this PIC */
	u32 node_id;    /* node id: 0x0-0; 0x1-1; 0x10-2; 0x11-3 */
	u64 pci_mem_start_addr;
	u64 pci_mem_end_addr;
	u64 pci_io_start_addr;
	u64 pci_io_end_addr;
	u64 pci_config_addr;
	u16 dma_mask_bits;
	u16 dma_noncoherent;/* 0:cache DMA ; 1:uncache DMA */
}__attribute__((packed));

struct interface_info{
	u16 vers; /* version of the specificition */
	u16 size;
	u8  flag;
	char description[64];
}__attribute__((packed));

#define MAX_RESOURCE_NUMBER 128
struct resource_loongson {
	u64 start; /* resource start address */
	u64 end;   /* resource end address */
	char name[64];
	u32 flags;
};

struct archdev_data {};  /* arch specific additions */

struct board_devices{
	char name[64];    /* hold the device name */
	u32 num_resources; /* number of device_resource */
	struct resource_loongson resource[MAX_RESOURCE_NUMBER]; /* for each device's resource */
	/* arch specific additions */
	struct archdev_data archdata;
};

struct loongson_special_attribute{
	u16 vers;     /* version of this special */
	char special_name[64]; /* special_atribute_name */
	u32 loongson_special_type; /* type of special device */
	struct resource_loongson resource[MAX_RESOURCE_NUMBER]; /* for each device's resource */
};

struct loongson_params{
	u64 memory_offset;	/* efi_memory_map_loongson struct offset */
	u64 cpu_offset;		/* efi_cpuinfo_loongson struct offset */
	u64 system_offset;	/* system_loongson struct offset */
	u64 irq_offset; 	/* irq_source_routing_table struct offset */
	u64 interface_offset;	/* interface_info struct offset */
	u64 special_offset;	/* loongson_special_attribute struct offset */
	u64 boarddev_table_offset;  /* board_devices offset */
};

struct smbios_tables {
	u16 vers;     /* version of smbios */
	u64 vga_bios; /* vga_bios address */
	struct loongson_params lp;
};

struct efi_reset_system_t{
	u64 ResetCold;
	u64 ResetWarm;
	u64 ResetType;
	u64 Shutdown;
	u64 DoSuspend; /* NULL if not support */
};

struct efi_loongson {
	u64 mps;	/* MPS table */
	u64 acpi;	/* ACPI table (IA64 ext 0.71) */
	u64 acpi20;	/* ACPI table (ACPI 2.0) */
	struct smbios_tables smbios;	/* SM BIOS table */
	u64 sal_systab;	/* SAL system table */
	u64 boot_info;	/* boot info table */
};

struct boot_params{
	struct efi_loongson efi;
	struct efi_reset_system_t reset_system;
};

extern u32 nr_cpus_loongson;
extern u32 nr_nodes_loongson;
extern int cores_per_node;
extern int cores_per_package;
extern enum loongson_cpu_type cputype;
extern u16 loongson_boot_cpu_id;
extern u16 loongson_reserved_cpus_mask;
extern struct efi_memory_map_loongson *emap;
extern u64 ht_control_base;
extern u64 pci_mem_start_addr, pci_mem_end_addr;
extern u64 loongson_pciio_base;
extern u64 vgabios_addr;
extern u64 low_physmem_start;
extern u64 high_physmem_start;
extern u32 vram_type;
extern u64 uma_vram_addr;
extern u64 uma_vram_size;

extern u32 loongson_nr_uarts;
extern struct uart_device loongson_uarts[MAX_UARTS];
extern u32 loongson_ec_sci_irq;
extern char loongson_ecname[32];
extern u32 loongson_nr_sensors;
extern struct sensor_device loongson_sensors[MAX_SENSORS];
#endif
