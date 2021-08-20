[TOC]

##  Booting via JTAG

Set DIP switch (SW6) to JTAG boot (on, on, on, on).

Start the Xilinx debug tool (shipped with Vivado SDK):

    xsct -nodisp

Upload files to the board using XSCT prompt:

```
connect
targets -set -filter {name =~ "PSU"}
mwr 0xffca0038 0x1ff
targets -set -filter {name =~ "MicroBlaze*"}
# /home/zynq/work/linux_app/project_pmu/Debug/project_pmu.elf
dow pmufw.elf
con
target -set -filter {name =~ "Cortex-A53 #0*"}
rst -processor
# /home/zynq/work/linux_app/project_2/Debug/project_2.elf
dow fsbl.elf
con
# /home/zynq/work/linux_app/arm-trusted-firmware/build/zynqmp/release/bl31/bl31.elf
dow bl31.elf
con
# /home/zynq/work/linux_app/u-boot-xlnx/u-boot.elf
dow u-boot.elf
con
# 下载seL4镜像（ELF格式）/home/zynq/work/linux_app/elfloader
dow elfloader/elfloader
con
```