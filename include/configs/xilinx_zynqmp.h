/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Configuration for Xilinx ZynqMP
 * (C) Copyright 2014 - 2015 Xilinx, Inc.
 * Michal Simek <michal.simek@xilinx.com>
 *
 * Based on Configuration for Versatile Express
 */

#ifndef __XILINX_ZYNQMP_H
#define __XILINX_ZYNQMP_H

/* Generic Interrupt Controller Definitions */
#define GICD_BASE	0xF9010000
#define GICC_BASE	0xF9020000

#define CONFIG_SYS_INIT_SP_ADDR		CONFIG_SYS_TEXT_BASE

/* Generic Timer Definitions - setup in EL3. Setup by ATF for other cases */
#if !defined(COUNTER_FREQUENCY)
# define COUNTER_FREQUENCY		100000000
#endif

/* Serial setup */
#define CONFIG_SYS_BAUDRATE_TABLE \
	{ 4800, 9600, 19200, 38400, 57600, 115200 }

#ifdef CONFIG_NAND_ARASAN
# define CONFIG_SYS_MAX_NAND_DEVICE	1
#endif

/* Miscellaneous configurable options */

#if defined(CONFIG_ZYNQMP_USB)
#define DFU_DEFAULT_POLL_TIMEOUT	300
#define CONFIG_THOR_RESET_OFF

#ifndef CONFIG_SPL_BUILD
# define PARTS_DEFAULT \
	"partitions=uuid_disk=${uuid_gpt_disk};" \
	"name=""boot"",size=16M,uuid=${uuid_gpt_boot};" \
	"name=""Linux"",size=-M,uuid=${uuid_gpt_Linux}\0"
#endif
#endif

#if !defined(PARTS_DEFAULT)
# define PARTS_DEFAULT
#endif

/* Monitor Command Prompt */
/* Console I/O Buffer Size */
#define CONFIG_SYS_CBSIZE		2048
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE
#define CONFIG_SYS_MAXARGS		64

/* Ethernet driver */
#if defined(CONFIG_ZYNQ_GEM)
# define PHY_ANEG_TIMEOUT       20000
#endif

#define CONFIG_SYS_BOOTM_LEN	(60 * 1024 * 1024)

#define ENV_MEM_LAYOUT_SETTINGS \
	"fdt_addr_r=0x40000000\0" \
	"fdt_size_r=0x400000\0" \
	"pxefile_addr_r=0x10000000\0" \
	"kernel_addr_r=0x18000000\0" \
	"kernel_size_r=0x10000000\0" \
	"kernel_comp_addr_r=0x30000000\0" \
	"kernel_comp_size=0x3C00000\0" \
	"scriptaddr=0x20000000\0" \
	"ramdisk_addr_r=0x02100000\0" \
	"script_size_f=0x80000\0" \

#if defined(CONFIG_MMC_SDHCI_ZYNQ)
# define BOOT_TARGET_DEVICES_MMC(func)	func(MMC, mmc, 0) func(MMC, mmc, 1)
#else
# define BOOT_TARGET_DEVICES_MMC(func)
#endif

#if defined(CONFIG_SATA_CEVA)
# define BOOT_TARGET_DEVICES_SCSI(func)	func(SCSI, scsi, 0)
#else
# define BOOT_TARGET_DEVICES_SCSI(func)
#endif

#if defined(CONFIG_ZYNQMP_USB)
# define BOOT_TARGET_DEVICES_USB(func)	func(USB, usb, 0) func(USB, usb, 1)
#else
# define BOOT_TARGET_DEVICES_USB(func)
#endif

#if defined(CONFIG_CMD_PXE) && defined(CONFIG_CMD_DHCP)
# define BOOT_TARGET_DEVICES_PXE(func)	func(PXE, pxe, na)
#else
# define BOOT_TARGET_DEVICES_PXE(func)
#endif

#if defined(CONFIG_CMD_DHCP)
# define BOOT_TARGET_DEVICES_DHCP(func)	func(DHCP, dhcp, na)
#else
# define BOOT_TARGET_DEVICES_DHCP(func)
#endif

#if defined(CONFIG_ZYNQMP_GQSPI)
# define BOOT_TARGET_DEVICES_QSPI(func)	func(QSPI, qspi, 0)
#else
# define BOOT_TARGET_DEVICES_QSPI(func)
#endif

#if defined(CONFIG_NAND_ARASAN)
# define BOOT_TARGET_DEVICES_NAND(func)	func(NAND, nand, 0)
#else
# define BOOT_TARGET_DEVICES_NAND(func)
#endif

#define BOOTENV_DEV_QSPI(devtypeu, devtypel, instance) \
	"bootcmd_" #devtypel #instance "=sf probe " #instance " 0 0 && " \
		       "sf read $scriptaddr $script_offset_f $script_size_f && " \
		       "echo QSPI: Trying to boot script at ${scriptaddr} && " \
		       "source ${scriptaddr}; echo QSPI: SCRIPT FAILED: continuing...;\0"

#define BOOTENV_DEV_NAME_QSPI(devtypeu, devtypel, instance) \
	#devtypel #instance " "

#define BOOTENV_DEV_NAND(devtypeu, devtypel, instance) \
	"bootcmd_" #devtypel #instance "= nand info && " \
		       "nand read $scriptaddr $script_offset_f $script_size_f && " \
		       "echo NAND: Trying to boot script at ${scriptaddr} && " \
		       "source ${scriptaddr}; echo NAND: SCRIPT FAILED: continuing...;\0"

#define BOOTENV_DEV_NAME_NAND(devtypeu, devtypel, instance) \
	#devtypel #instance " "

#define BOOT_TARGET_DEVICES_JTAG(func)	func(JTAG, jtag, na)

#define BOOTENV_DEV_JTAG(devtypeu, devtypel, instance) \
	"bootcmd_jtag=echo JTAG: Trying to boot script at ${scriptaddr} && " \
		"source ${scriptaddr}; echo JTAG: SCRIPT FAILED: continuing...;\0"

#define BOOTENV_DEV_NAME_JTAG(devtypeu, devtypel, instance) \
	"jtag "

#define BOOT_TARGET_DEVICES_USB_DFU(func) \
	func(USB_DFU, usb_dfu, 0) func(USB_DFU, usb_dfu, 1)

#define BOOTENV_DEV_USB_DFU(devtypeu, devtypel, instance) \
	"bootcmd_" #devtypel #instance "=setenv dfu_alt_info boot.scr ram " \
	"$scriptaddr $script_size_f && " \
	"dfu " #instance " ram " #instance " 60 && " \
	"echo DFU" #instance ": Trying to boot script at ${scriptaddr} && " \
	"source ${scriptaddr}; " \
	"echo DFU" #instance ": SCRIPT FAILED: continuing...;\0"

#define BOOTENV_DEV_NAME_USB_DFU(devtypeu, devtypel, instance) \
	""

#define BOOT_TARGET_DEVICES_USB_THOR(func) \
	func(USB_THOR, usb_thor, 0) func(USB_THOR, usb_thor, 1)

#define BOOTENV_DEV_USB_THOR(devtypeu, devtypel, instance) \
	"bootcmd_" #devtypel #instance "=setenv dfu_alt_info boot.scr ram " \
	"$scriptaddr $script_size_f && " \
	"thordown " #instance " ram " #instance " && " \
	"echo THOR" #instance ": Trying to boot script at ${scriptaddr} && " \
	"source ${scriptaddr}; " \
	"echo THOR" #instance ": SCRIPT FAILED: continuing...;\0"

#define BOOTENV_DEV_NAME_USB_THOR(devtypeu, devtypel, instance) \
	""

#define BOOT_TARGET_DEVICES(func) \
	BOOT_TARGET_DEVICES_JTAG(func) \
	BOOT_TARGET_DEVICES_MMC(func) \
	BOOT_TARGET_DEVICES_QSPI(func) \
	BOOT_TARGET_DEVICES_NAND(func) \
	BOOT_TARGET_DEVICES_USB_DFU(func) \
	BOOT_TARGET_DEVICES_USB_THOR(func) \
	BOOT_TARGET_DEVICES_USB(func) \
	BOOT_TARGET_DEVICES_SCSI(func) \
	BOOT_TARGET_DEVICES_PXE(func) \
	BOOT_TARGET_DEVICES_DHCP(func)

#include <config_distro_bootcmd.h>

/* Initial environment variables */
#ifndef CONFIG_EXTRA_ENV_SETTINGS
#define CONFIG_EXTRA_ENV_SETTINGS \
	ENV_MEM_LAYOUT_SETTINGS \
	BOOTENV
#endif

/* SPL can't handle all huge variables - define just DFU */
#if defined(CONFIG_SPL_BUILD) && defined(CONFIG_SPL_DFU)
#undef CONFIG_EXTRA_ENV_SETTINGS
# define CONFIG_EXTRA_ENV_SETTINGS \
	"dfu_alt_info_ram=uboot.bin ram 0x8000000 0x1000000;" \
			  "atf-uboot.ub ram 0x10000000 0x1000000;" \
			  "Image ram 0x80000 0x3f80000;" \
			  "system.dtb ram 0x4000000 0x100000\0" \
	"dfu_bufsiz=0x1000\0"
#endif

#define CONFIG_SPL_STACK		0xfffffffc
#define CONFIG_SPL_MAX_SIZE		0x40000

/* Just random location in OCM */
#define CONFIG_SPL_BSS_START_ADDR	0x0
#define CONFIG_SPL_BSS_MAX_SIZE		0x80000

#if defined(CONFIG_SPL_SPI_FLASH_SUPPORT)
# define CONFIG_SYS_SPI_KERNEL_OFFS	0x80000
# define CONFIG_SYS_SPI_ARGS_OFFS	0xa0000
# define CONFIG_SYS_SPI_ARGS_SIZE	0xa0000
#endif

/* u-boot is like dtb */
#define CONFIG_SPL_FS_LOAD_ARGS_NAME	"u-boot.bin"
#define CONFIG_SYS_SPL_ARGS_ADDR	0x8000000

/* ATF is my kernel image */
#define CONFIG_SPL_FS_LOAD_KERNEL_NAME	"atf-uboot.ub"

/* MMC support */
#ifdef CONFIG_MMC_SDHCI_ZYNQ
# define CONFIG_SYS_MMCSD_RAW_MODE_ARGS_SECTOR	0 /* unused */
# define CONFIG_SYS_MMCSD_RAW_MODE_ARGS_SECTORS	0 /* unused */
# if defined(CONFIG_SPL_LOAD_FIT)
#  define CONFIG_SPL_FS_LOAD_PAYLOAD_NAME	"u-boot.itb"
# else
#  define CONFIG_SPL_FS_LOAD_PAYLOAD_NAME	"u-boot.img"
# endif
#endif

#if defined(CONFIG_SPL_BUILD) && defined(CONFIG_SPL_DFU)
# define CONFIG_SPL_HASH
# define CONFIG_ENV_MAX_ENTRIES	10
#endif

#define CONFIG_SYS_SPL_MALLOC_START	0x20000000
#define CONFIG_SYS_SPL_MALLOC_SIZE	0x1000000

#ifdef CONFIG_SPL_SYS_MALLOC_SIMPLE
# error "Disable CONFIG_SPL_SYS_MALLOC_SIMPLE. Full malloc needs to be used"
#endif

#endif /* __XILINX_ZYNQMP_H */
