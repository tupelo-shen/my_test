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
# dow /home/zynq/work/linux_app/project_pmu/Debug/project_pmu.elf
dow pmufw.elf
con
targets -set -filter {name =~ "Cortex-A53 #0*"}
rst -processor
# dow /home/zynq/work/linux_app/project_2/Debug/project_2.elf
dow fsbl.elf
con
# dow /home/zynq/work/linux_app/arm-trusted-firmware/build/zynqmp/release/bl31/bl31.elf
dow bl31.elf
con
# dow /home/zynq/work/linux_app/u-boot-xlnx/u-boot.elf
dow u-boot.elf
con
# 下载seL4镜像（ELF格式）dow /home/zynq/work/linux_app/elfloader
dow elfloader/elfloader
con
```