/*
 * THIS FILE AUTOMATICALLY GENERATED.  DO NOT EDIT.
 *
 * generated from:
 *		OpenBSD: pcidevs,v 1.443 2001/09/06 22:36:36 mickey Exp 
 */
/*	$NetBSD: pcidevs,v 1.30 1997/06/24 06:20:24 thorpej Exp $ 	*/

/*
 * Copyright (c) 1995, 1996 Christopher G. Demetriou
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Christopher G. Demetriou
 *	for the NetBSD Project.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * NOTE: a fairly complete list of PCI codes can be found at:
 *
 *	http://www.yourvote.com/pci/
 *	http://members.hyperlink.net.au/~chart/pci.htm
 *
 * There is a Vendor ID search engine available at:
 *
 *	http://www.pcisig.com/app/search/by_vendor_id/
 */

/*
 * List of known PCI vendors
 */

#define	PCI_VENDOR_MARTINMARIETTA	0x003d		/* Martin-Marietta */
#define	PCI_VENDOR_COMPAQ	0x0e11		/* Compaq */
#define	PCI_VENDOR_SYMBIOS	0x1000		/* Symbios Logic */
#define	PCI_VENDOR_ATI	0x1002		/* ATI */
#define	PCI_VENDOR_ULSI	0x1003		/* ULSI Systems */
#define	PCI_VENDOR_VLSI	0x1004		/* VLSI Technology */
#define	PCI_VENDOR_AVANCE	0x1005		/* Avance Logic */
#define	PCI_VENDOR_REPLY	0x1006		/* Reply Group */
#define	PCI_VENDOR_NETFRAME	0x1007		/* NetFrame Systems */
#define	PCI_VENDOR_EPSON	0x1008		/* Epson */
#define	PCI_VENDOR_PHOENIX	0x100a		/* Phoenix Technologies */
#define	PCI_VENDOR_NS	0x100b		/* National Semiconductor */
#define	PCI_VENDOR_TSENG	0x100c		/* Tseng Labs */
#define	PCI_VENDOR_AST	0x100d		/* AST Research */
#define	PCI_VENDOR_WEITEK	0x100e		/* Weitek */
#define	PCI_VENDOR_VIDEOLOGIC	0x1010		/* Video Logic */
#define	PCI_VENDOR_DEC	0x1011		/* DEC */
#define	PCI_VENDOR_MICRONICS	0x1012		/* Micronics */
#define	PCI_VENDOR_CIRRUS	0x1013		/* Cirrus Logic */
#define	PCI_VENDOR_IBM	0x1014		/* IBM */
#define	PCI_VENDOR_LSIL	0x1015		/* LSI Canada */
#define	PCI_VENDOR_ICLPERSONAL	0x1016		/* ICL Personal Systems */
#define	PCI_VENDOR_SPEA	0x1017		/* SPEA Software */
#define	PCI_VENDOR_UNISYS	0x1018		/* Unisys Systems */
#define	PCI_VENDOR_ELITEGROUP	0x1019		/* Elitegroup */
#define	PCI_VENDOR_NCR	0x101a		/* AT&T GIS */
#define	PCI_VENDOR_VITESSE	0x101b		/* Vitesse Semiconductor */
#define	PCI_VENDOR_WD	0x101c		/* Western Digital */
#define	PCI_VENDOR_AMI	0x101e		/* AMI */
#define	PCI_VENDOR_PICTURETEL	0x101f		/* PictureTel */
#define	PCI_VENDOR_HITACHICOMP	0x1020		/* Hitachi Computer Products */
#define	PCI_VENDOR_OKI	0x1021		/* OKI Electric Industry */
#define	PCI_VENDOR_AMD	0x1022		/* AMD */
#define	PCI_VENDOR_TRIDENT	0x1023		/* Trident */
#define	PCI_VENDOR_ZENITH	0x1024		/* Zenith Data Systems */
#define	PCI_VENDOR_ACER	0x1025		/* Acer */
#define	PCI_VENDOR_DELL	0x1028		/* Dell */
#define	PCI_VENDOR_SNI	0x1029		/* Siemens Nixdorf AG */
#define	PCI_VENDOR_LSILOGIC	0x102a		/* LSI Logic */
#define	PCI_VENDOR_MATROX	0x102b		/* Matrox */
#define	PCI_VENDOR_CHIPS	0x102c		/* Chips and Technologies */
#define	PCI_VENDOR_WYSE	0x102d		/* WYSE Technology */
#define	PCI_VENDOR_OLIVETTI	0x102e		/* Olivetti */
#define	PCI_VENDOR_TOSHIBA	0x102f		/* Toshiba */
#define	PCI_VENDOR_TMCRESEARCH	0x1030		/* TMC Research */
#define	PCI_VENDOR_MIRO	0x1031		/* Miro Computer Products */
#define	PCI_VENDOR_COMPAQ2	0x1032		/* Compaq */
#define	PCI_VENDOR_NEC	0x1033		/* NEC */
#define	PCI_VENDOR_BURNDY	0x1034		/* Burndy */
#define	PCI_VENDOR_COMPCOMM	0x1035		/* Comp. & Comm. Research Lab */
#define	PCI_VENDOR_FUTUREDOMAIN	0x1036		/* Future Domain */
#define	PCI_VENDOR_HITACHIMICRO	0x1037		/* Hitach Microsystems */
#define	PCI_VENDOR_AMP	0x1038		/* AMP */
#define	PCI_VENDOR_SIS	0x1039		/* SIS */
#define	PCI_VENDOR_SEIKOEPSON	0x103a		/* Seiko Epson */
#define	PCI_VENDOR_TATUNGAMERICA	0x103b		/* Tatung Co. of America */
#define	PCI_VENDOR_HP	0x103c		/* Hewlett-Packard */
#define	PCI_VENDOR_SOLLIDAY	0x103e		/* Solliday Engineering */
#define	PCI_VENDOR_LOGICMODELLING	0x103f		/* Logic Modeling */
#define	PCI_VENDOR_KPC	0x1040		/* Kubota Pacific */
#define	PCI_VENDOR_COMPUTREND	0x1041		/* Computrend */
#define	PCI_VENDOR_PCTECH	0x1042		/* PC Technology */
#define	PCI_VENDOR_ASUSTEK	0x1043		/* Asustek Computer */
#define	PCI_VENDOR_DPT	0x1044		/* DPT */
#define	PCI_VENDOR_OPTI	0x1045		/* Opti */
#define	PCI_VENDOR_IPCCORP	0x1046		/* IPC Corporation */
#define	PCI_VENDOR_GENOA	0x1047		/* Genoa Systems */
#define	PCI_VENDOR_ELSA	0x1048		/* Elsa */
#define	PCI_VENDOR_FOUNTAINTECH	0x1049		/* Fountain Technology */
#define	PCI_VENDOR_SGSTHOMSON	0x104a		/* SGS Thomson */
#define	PCI_VENDOR_BUSLOGIC	0x104b		/* BusLogic */
#define	PCI_VENDOR_TI	0x104c		/* Texas Instruments */
#define	PCI_VENDOR_SONY	0x104d		/* Sony */
#define	PCI_VENDOR_OAKTECH	0x104e		/* Oak Technology */
#define	PCI_VENDOR_COTIME	0x104f		/* Co-time Computer */
#define	PCI_VENDOR_WINBOND	0x1050		/* Winbond */
#define	PCI_VENDOR_ANIGMA	0x1051		/* Anigma */
#define	PCI_VENDOR_YOUNGMICRO	0x1052		/* Young Micro */
#define	PCI_VENDOR_HITACHI	0x1054		/* Hitachi */
#define	PCI_VENDOR_EFARMICRO	0x1055		/* Efar Microsystems */
#define	PCI_VENDOR_ICL	0x1056		/* ICL */
#define	PCI_VENDOR_MOT	0x1057		/* Motorola */
#define	PCI_VENDOR_ETR	0x1058		/* Electronics & Telec. RSH */
#define	PCI_VENDOR_TEKNOR	0x1059		/* Teknor Microsystems */
#define	PCI_VENDOR_PROMISE	0x105a		/* Promise */
#define	PCI_VENDOR_FOXCONN	0x105b		/* Foxconn */
#define	PCI_VENDOR_WIPRO	0x105c		/* Wipro Infotech */
#define	PCI_VENDOR_NUMBER9	0x105d		/* Number 9 */
#define	PCI_VENDOR_VTECH	0x105e		/* Vtech Computers */
#define	PCI_VENDOR_INFOTRONIC	0x105f		/* Infotronic America */
#define	PCI_VENDOR_UMC	0x1060		/* UMC */
#define	PCI_VENDOR_ITT	0x1061		/* I. T. T. */
#define	PCI_VENDOR_MASPAR	0x1062		/* MasPar Computer */
#define	PCI_VENDOR_OCEANOA	0x1063		/* Ocean Office Automation */
#define	PCI_VENDOR_ALCATEL	0x1064		/* Alcatel CIT */
#define	PCI_VENDOR_TEXASMICRO	0x1065		/* Texas Microsystems */
#define	PCI_VENDOR_PICOPOWER	0x1066		/* Picopower Technology */
#define	PCI_VENDOR_MITSUBISHI	0x1067		/* Mitsubishi */
#define	PCI_VENDOR_DIVERSIFIED	0x1068		/* Diversified Technology */
#define	PCI_VENDOR_MYLEX	0x1069		/* Mylex */
#define	PCI_VENDOR_ATEN	0x106a		/* Aten Research */
#define	PCI_VENDOR_APPLE	0x106b		/* Apple */
#define	PCI_VENDOR_HYUNDAI	0x106c		/* Hyundai */
#define	PCI_VENDOR_SEQUENT	0x106d		/* Sequent */
#define	PCI_VENDOR_DFI	0x106e		/* DFI */
#define	PCI_VENDOR_CITYGATE	0x106f		/* City Gate Development */
#define	PCI_VENDOR_DAEWOO	0x1070		/* Daewoo Telecom */
#define	PCI_VENDOR_MITAC	0x1071		/* Mitac */
#define	PCI_VENDOR_GIT	0x1072		/* GIT Co. */
#define	PCI_VENDOR_YAMAHA	0x1073		/* Yamaha */
#define	PCI_VENDOR_NEXGEN	0x1074		/* NexGen Microsystems */
#define	PCI_VENDOR_AIR	0x1075		/* Advanced Integration Research */
#define	PCI_VENDOR_CHAINTECH	0x1076		/* Chaintech Computer */
#define	PCI_VENDOR_QLOGIC	0x1077		/* QLogic */
#define	PCI_VENDOR_CYRIX	0x1078		/* Cyrix */
#define	PCI_VENDOR_IBUS	0x1079		/* I-Bus */
#define	PCI_VENDOR_NETWORTH	0x107a		/* NetWorth */
#define	PCI_VENDOR_GATEWAY	0x107b		/* Gateway 2000 */
#define	PCI_VENDOR_GOLDSTART	0x107c		/* Goldstar */
#define	PCI_VENDOR_LEADTEK	0x107d		/* LeadTek Research */
#define	PCI_VENDOR_INTERPHASE	0x107e		/* Interphase */
#define	PCI_VENDOR_DATATECH	0x107f		/* Data Technology Corporation */
#define	PCI_VENDOR_CONTAQ	0x1080		/* Contaq Microsystems */
#define	PCI_VENDOR_SUPERMAC	0x1081		/* Supermac Technology */
#define	PCI_VENDOR_EFA	0x1082		/* EFA */
#define	PCI_VENDOR_FOREX	0x1083		/* Forex Computer */
#define	PCI_VENDOR_PARADOR	0x1084		/* Parador */
#define	PCI_VENDOR_TULIP	0x1085		/* Tulip Computers */
#define	PCI_VENDOR_JBOND	0x1086		/* J. Bond Computer Systems */
#define	PCI_VENDOR_CACHECOMP	0x1087		/* Cache Computer */
#define	PCI_VENDOR_MICROCOMP	0x1088		/* Microcomputer Systems */
#define	PCI_VENDOR_DG	0x1089		/* Data General */
#define	PCI_VENDOR_BIT3	0x108a		/* Bit3 Computer Corp. */
#define	PCI_VENDOR_ELONEX	0x108c		/* Elonex PLC c/o Oakleigh Systems */
#define	PCI_VENDOR_OLICOM	0x108d		/* Olicom */
#define	PCI_VENDOR_SUN	0x108e		/* Sun */
#define	PCI_VENDOR_SYSTEMSOFT	0x108f		/* Systemsoft */
#define	PCI_VENDOR_ENCORE	0x1090		/* Encore Computer */
#define	PCI_VENDOR_INTERGRAPH	0x1091		/* Intergraph */
#define	PCI_VENDOR_DIAMOND	0x1092		/* Diamond Multimedia */
#define	PCI_VENDOR_NATIONALINST	0x1093		/* National Instruments */
#define	PCI_VENDOR_FICOMP	0x1094		/* First Int'l Computers */
#define	PCI_VENDOR_CMDTECH	0x1095		/* CMD Technology */
#define	PCI_VENDOR_ALACRON	0x1096		/* Alacron */
#define	PCI_VENDOR_APPIAN	0x1097		/* Appian Technology */
#define	PCI_VENDOR_QUANTUMDESIGNS	0x1098		/* Quantum Designs */
#define	PCI_VENDOR_SAMSUNGELEC	0x1099		/* Samsung Electronics */
#define	PCI_VENDOR_PACKARDBELL	0x109a		/* Packard Bell */
#define	PCI_VENDOR_GEMLIGHT	0x109b		/* Gemlight Computer */
#define	PCI_VENDOR_MEGACHIPS	0x109c		/* Megachips */
#define	PCI_VENDOR_ZIDA	0x109d		/* Zida Technologies */
#define	PCI_VENDOR_BROOKTREE	0x109e		/* Brooktree */
#define	PCI_VENDOR_TRIGEM	0x109f		/* Trigem Computer */
#define	PCI_VENDOR_MEIDENSHA	0x10a0		/* Meidensha */
#define	PCI_VENDOR_JUKO	0x10a1		/* Juko Electronics */
#define	PCI_VENDOR_QUANTUM	0x10a2		/* Quantum */
#define	PCI_VENDOR_EVEREX	0x10a3		/* Everex Systems */
#define	PCI_VENDOR_GLOBE	0x10a4		/* Globe Manufacturing Sales */
#define	PCI_VENDOR_RACAL	0x10a5		/* Racal Interlan */
#define	PCI_VENDOR_INFORMTECH	0x10a6		/* Informtech Industrial */
#define	PCI_VENDOR_BENCHMARQ	0x10a7		/* Benchmarq Microelectronics */
#define	PCI_VENDOR_SIERRA	0x10a8		/* Sierra Semiconductor */
#define	PCI_VENDOR_SGI	0x10a9		/* Silicon Graphics */
#define	PCI_VENDOR_ACC	0x10aa		/* ACC Microelectronics */
#define	PCI_VENDOR_DIGICOM	0x10ab		/* Digicom */
#define	PCI_VENDOR_HONEYWELL	0x10ac		/* Honeywell IASD */
#define	PCI_VENDOR_SYMPHONY	0x10ad		/* Symphony Labs */
#define	PCI_VENDOR_CORNERSTONE	0x10ae		/* Cornerstone Technology */
#define	PCI_VENDOR_MICROCOMPSON	0x10af		/* Micro Computer Systems (M) SON */
#define	PCI_VENDOR_CARDEXPER	0x10b0		/* CardExpert Technology */
#define	PCI_VENDOR_CABLETRON	0x10b1		/* Cabletron Systems */
#define	PCI_VENDOR_RAYETHON	0x10b2		/* Raytheon */
#define	PCI_VENDOR_DATABOOK	0x10b3		/* Databook */
#define	PCI_VENDOR_STB	0x10b4		/* STB Systems */
#define	PCI_VENDOR_PLX	0x10b5		/* PLX Technology */
#define	PCI_VENDOR_MADGE	0x10b6		/* Madge Networks */
#define	PCI_VENDOR_3COM	0x10b7		/* 3Com */
#define	PCI_VENDOR_SMC	0x10b8		/* SMC */
#define	PCI_VENDOR_ALI	0x10b9		/* Acer Labs */
#define	PCI_VENDOR_MITSUBISHIELEC	0x10ba		/* Mitsubishi Electronics */
#define	PCI_VENDOR_DAPHA	0x10bb		/* Dapha Electronics */
#define	PCI_VENDOR_ALR	0x10bc		/* Advanced Logic Research */
#define	PCI_VENDOR_SURECOM	0x10bd		/* Surecom Technology */
#define	PCI_VENDOR_TSENGLABS	0x10be		/* Tseng Labs International */
#define	PCI_VENDOR_MOST	0x10bf		/* Most */
#define	PCI_VENDOR_BOCA	0x10c0		/* Boca Research */
#define	PCI_VENDOR_ICM	0x10c1		/* ICM */
#define	PCI_VENDOR_AUSPEX	0x10c2		/* Auspex Systems */
#define	PCI_VENDOR_SAMSUNGSEMI	0x10c3		/* Samsung Semiconductors */
#define	PCI_VENDOR_AWARD	0x10c4		/* Award */
#define	PCI_VENDOR_XEROX	0x10c5		/* Xerox */
#define	PCI_VENDOR_RAMBUS	0x10c6		/* Rambus */
#define	PCI_VENDOR_MEDIAVIS	0x10c7		/* Media Vision */
#define	PCI_VENDOR_NEOMAGIC	0x10c8		/* Neomagic */
#define	PCI_VENDOR_DATAEXPERT	0x10c9		/* Dataexpert */
#define	PCI_VENDOR_FUJITSU	0x10ca		/* Fujitsu */
#define	PCI_VENDOR_OMRON	0x10cb		/* Omron */
#define	PCI_VENDOR_MENTOR	0x10cc		/* Mentor ARC */
#define	PCI_VENDOR_ADVSYS	0x10cd		/* Advansys */
#define	PCI_VENDOR_RADIUS	0x10ce		/* Radius */
#define	PCI_VENDOR_CITICORP	0x10cf		/* Citicorp TTI */
#define	PCI_VENDOR_FUJITSU2	0x10d0		/* Fujitsu */
#define	PCI_VENDOR_FUTUREPLUS	0x10d1		/* Future+ Systems */
#define	PCI_VENDOR_MOLEX	0x10d2		/* Molex */
#define	PCI_VENDOR_JABIL	0x10d3		/* Jabil Circuit */
#define	PCI_VENDOR_HAULON	0x10d4		/* Hualon Microelectronics */
#define	PCI_VENDOR_AUTOLOGIC	0x10d5		/* Autologic */
#define	PCI_VENDOR_CETIA	0x10d6		/* Cetia */
#define	PCI_VENDOR_BCM	0x10d7		/* BCM Advanced */
#define	PCI_VENDOR_APL	0x10d8		/* Advanced Peripherals Labs */
#define	PCI_VENDOR_MACRONIX	0x10d9		/* Macronix */
#define	PCI_VENDOR_THOMASCONRAD	0x10da		/* Thomas-Conrad */
#define	PCI_VENDOR_ROHM	0x10db		/* Rohm Research */
#define	PCI_VENDOR_CERN	0x10dc		/* CERN/ECP/EDU */
#define	PCI_VENDOR_ES	0x10dd		/* Evans & Sutherland */
#define	PCI_VENDOR_NVIDIA	0x10de		/* Nvidia */
#define	PCI_VENDOR_EMULEX	0x10df		/* Emulex */
#define	PCI_VENDOR_IMS	0x10e0		/* Integrated Micro Solutions */
#define	PCI_VENDOR_TEKRAM	0x10e1		/* Tekram Technology (1st ID) */
#define	PCI_VENDOR_APTIX	0x10e2		/* Aptix */
#define	PCI_VENDOR_NEWBRIDGE	0x10e3		/* Newbridge */
#define	PCI_VENDOR_TANDEM	0x10e4		/* Tandem */
#define	PCI_VENDOR_MICROINDUSTRIES	0x10e5		/* Micro Industries */
#define	PCI_VENDOR_GAINBERY	0x10e6		/* Gainbery Computer Products */
#define	PCI_VENDOR_VADEM	0x10e7		/* Vadem */
#define	PCI_VENDOR_AMCIRCUITS	0x10e8		/* Applied Micro Circuits */
#define	PCI_VENDOR_ALPSELECTIC	0x10e9		/* Alps Electric */
#define	PCI_VENDOR_INTERGRAPHICS	0x10ea		/* Integraphics Systems */
#define	PCI_VENDOR_ARTISTSGRAPHICS	0x10eb		/* Artists Graphics */
#define	PCI_VENDOR_REALTEK	0x10ec		/* Realtek */
#define	PCI_VENDOR_ASCIICORP	0x10ed		/* ASCII Corporation */
#define	PCI_VENDOR_XILINX	0x10ee		/* Xilinx */
#define	PCI_VENDOR_RACORE	0x10ef		/* Racore Computer Products */
#define	PCI_VENDOR_PERITEK	0x10f0		/* Peritek */
#define	PCI_VENDOR_TYAN	0x10f1		/* Tyan Computer */
#define	PCI_VENDOR_ACHME	0x10f2		/* Achme Computer */
#define	PCI_VENDOR_ALARIS	0x10f3		/* Alaris */
#define	PCI_VENDOR_SMOS	0x10f4		/* S-MOS Systems */
#define	PCI_VENDOR_NKK	0x10f5		/* NKK */
#define	PCI_VENDOR_CREATIVE	0x10f6		/* Creative Electronic Systems */
#define	PCI_VENDOR_MATSUSHITA	0x10f7		/* Matsushita */
#define	PCI_VENDOR_ALTOS	0x10f8		/* Altos India */
#define	PCI_VENDOR_PCDIRECT	0x10f9		/* PC Direct */
#define	PCI_VENDOR_TRUEVISIO	0x10fa		/* Truevision */
#define	PCI_VENDOR_THESYS	0x10fb		/* Thesys Ges. F. Mikroelektronik */
#define	PCI_VENDOR_IODATA	0x10fc		/* I-O Data Device */
#define	PCI_VENDOR_SOYO	0x10fd		/* Soyo Technology */
#define	PCI_VENDOR_FAST	0x10fe		/* Fast Electronic */
#define	PCI_VENDOR_NCUBE	0x10ff		/* NCube */
#define	PCI_VENDOR_JAZZ	0x1100		/* Jazz Multimedia */
#define	PCI_VENDOR_INITIO	0x1101		/* Initio */
#define	PCI_VENDOR_CREATIVELABS	0x1102		/* Creative Labs */
#define	PCI_VENDOR_TRIONES	0x1103		/* HighPoint */
#define	PCI_VENDOR_RASTEROPS	0x1104		/* RasterOps */
#define	PCI_VENDOR_SIGMA	0x1105		/* Sigma Designs */
#define	PCI_VENDOR_VIATECH	0x1106		/* VIA */
#define	PCI_VENDOR_STRATIS	0x1107		/* Stratus Computer */
#define	PCI_VENDOR_PROTEON	0x1108		/* Proteon */
#define	PCI_VENDOR_COGENT	0x1109		/* Cogent Data Technologies */
#define	PCI_VENDOR_SIEMENS	0x110a		/* Siemens AG / Siemens Nixdorf AG */
#define	PCI_VENDOR_XENON	0x110b		/* Xenon Microsystems */
#define	PCI_VENDOR_MINIMAX	0x110c		/* Mini-Max Technology */
#define	PCI_VENDOR_ZNYX	0x110d		/* Znyx Advanced Systems */
#define	PCI_VENDOR_CPUTECH	0x110e		/* CPU Technology */
#define	PCI_VENDOR_ROSS	0x110f		/* Ross Technology */
#define	PCI_VENDOR_POWERHOUSE	0x1110		/* Powerhouse Systems */
#define	PCI_VENDOR_SCO	0x1111		/* SCO */
#define	PCI_VENDOR_RNS	0x1112		/* RNS */
#define	PCI_VENDOR_ACCTON	0x1113		/* Accton Technology */
#define	PCI_VENDOR_ATMEL	0x1114		/* Atmel */
#define	PCI_VENDOR_DUPONT	0x1115		/* DuPont Pixel Systems */
#define	PCI_VENDOR_DATATRANSLATION	0x1116		/* Data Translation */
#define	PCI_VENDOR_DATACUBE	0x1117		/* Datacube */
#define	PCI_VENDOR_BERG	0x1118		/* Berg Electronics */
#define	PCI_VENDOR_VORTEX	0x1119		/* Vortex */
#define	PCI_VENDOR_EFFICIENTNETS	0x111a		/* Efficent Networks */
#define	PCI_VENDOR_TELEDYNE	0x111b		/* Teledyne */
#define	PCI_VENDOR_TRICORD	0x111c		/* Tricord Systems */
#define	PCI_VENDOR_IDT	0x111d		/* IDT */
#define	PCI_VENDOR_ELDEC	0x111e		/* Eldec */
#define	PCI_VENDOR_PDI	0x111f		/* Prescision Digital Images */
#define	PCI_VENDOR_EMC	0x1120		/* Emc */
#define	PCI_VENDOR_ZILOG	0x1121		/* Zilog */
#define	PCI_VENDOR_MULTITECH	0x1122		/* Multi-tech Systems */
#define	PCI_VENDOR_LEUTRON	0x1124		/* Leutron Vision */
#define	PCI_VENDOR_EUROCORE	0x1125		/* Eurocore/Vigra */
#define	PCI_VENDOR_VIGRA	0x1126		/* Vigra */
#define	PCI_VENDOR_FORE	0x1127		/* FORE Systems */
#define	PCI_VENDOR_FIRMWORKS	0x1129		/* Firmworks */
#define	PCI_VENDOR_HERMES	0x112a		/* Hermes Electronics */
#define	PCI_VENDOR_LINOTYPE	0x112b		/* Linotype */
#define	PCI_VENDOR_RAVICAD	0x112d		/* Ravicad */
#define	PCI_VENDOR_INFOMEDIA	0x112e		/* Infomedia Microelectronics */
#define	PCI_VENDOR_IMAGINGTECH	0x112f		/* Imaging Technlogy */
#define	PCI_VENDOR_COMPUTERVISION	0x1130		/* Computervision */
#define	PCI_VENDOR_PHILIPS	0x1131		/* Philips */
#define	PCI_VENDOR_MITEL	0x1132		/* Mitel */
#define	PCI_VENDOR_EICON	0x1133		/* Eicon Technology */
#define	PCI_VENDOR_MCS	0x1134		/* Mercury Computer Systems */
#define	PCI_VENDOR_FUJIXEROX	0x1135		/* Fuji Xerox */
#define	PCI_VENDOR_MOMENTUM	0x1136		/* Momentum Data Systems */
#define	PCI_VENDOR_CISCO	0x1137		/* Cisco */
#define	PCI_VENDOR_ZIATECH	0x1138		/* Ziatech */
#define	PCI_VENDOR_DYNPIC	0x1139		/* Dynamic Pictures */
#define	PCI_VENDOR_FWB	0x113a		/* FWB */
#define	PCI_VENDOR_CYCLONE	0x113c		/* Cyclone */
#define	PCI_VENDOR_LEADINGEDGE	0x113d		/* Leading Edge */
#define	PCI_VENDOR_SANYO	0x113e		/* Sanyo */
#define	PCI_VENDOR_EQUINOX	0x113f		/* Equinox */
#define	PCI_VENDOR_INTERVOICE	0x1140		/* Intervoice */
#define	PCI_VENDOR_CREST	0x1141		/* Crest Microsystem */
#define	PCI_VENDOR_ALLIANCE	0x1142		/* Alliance Semiconductor */
#define	PCI_VENDOR_NETPOWER	0x1143		/* NetPower */
#define	PCI_VENDOR_CINMILACRON	0x1144		/* Cincinnati Milacron */
#define	PCI_VENDOR_WORKBIT	0x1145		/* Workbit */
#define	PCI_VENDOR_FORCE	0x1146		/* Force */
#define	PCI_VENDOR_INTERFACE	0x1147		/* Interface */
#define	PCI_VENDOR_SCHNEIDERKOCH	0x1148		/* Schneider & Koch */
#define	PCI_VENDOR_WINSYSTEM	0x1149		/* Win System */
#define	PCI_VENDOR_VMIC	0x114a		/* VMIC */
#define	PCI_VENDOR_CANOPUS	0x114b		/* Canopus */
#define	PCI_VENDOR_ANNABOOKS	0x114c		/* Annabooks */
#define	PCI_VENDOR_IC	0x114d		/* IC Corporation */
#define	PCI_VENDOR_NIKON	0x114e		/* Nikon */
#define	PCI_VENDOR_DIGIINTERNAT	0x114f		/* Digi */
#define	PCI_VENDOR_TMC	0x1150		/* Thinking Machines */
#define	PCI_VENDOR_JAE	0x1151		/* JAE Electronics */
#define	PCI_VENDOR_MEGATEK	0x1152		/* Megatek */
#define	PCI_VENDOR_LANDWIN	0x1153		/* Land Win Electronic */
#define	PCI_VENDOR_MELCO	0x1154		/* Melco */
#define	PCI_VENDOR_PINETECH	0x1155		/* Pine Technology */
#define	PCI_VENDOR_PERISCOPE	0x1156		/* Periscope */
#define	PCI_VENDOR_AVSYS	0x1157		/* Avsys */
#define	PCI_VENDOR_VOARX	0x1158		/* Voarx R & D */
#define	PCI_VENDOR_MUTECH	0x1159		/* Mutech */
#define	PCI_VENDOR_HARLEQUIN	0x115a		/* Harlequin */
#define	PCI_VENDOR_PARALLAX	0x115b		/* Parallax Graphics */
#define	PCI_VENDOR_XIRCOM	0x115d		/* Xircom */
#define	PCI_VENDOR_PEERPROTO	0x115e		/* Peer Protocols */
#define	PCI_VENDOR_MAXTOR	0x115f		/* Maxtor */
#define	PCI_VENDOR_MEGASOFT	0x1160		/* Megasoft */
#define	PCI_VENDOR_PFU	0x1161		/* PFU Limited */
#define	PCI_VENDOR_OALAB	0x1162		/* OA Laboratory */
#define	PCI_VENDOR_SYNEMA	0x1163		/* Synema */
#define	PCI_VENDOR_APT	0x1164		/* Advanced Peripherals Technologies */
#define	PCI_VENDOR_IMAGRAPH	0x1165		/* Imagraph */
#define	PCI_VENDOR_RCC	0x1166		/* ServerWorks */
#define	PCI_VENDOR_MUTOH	0x1167		/* Mutoh Industries */
#define	PCI_VENDOR_THINE	0x1168		/* Thine Electronics */
#define	PCI_VENDOR_CDAC	0x1169		/* Centre for Dev. of Advanced Computing */
#define	PCI_VENDOR_POLARIS	0x116a		/* Polaris Communications */
#define	PCI_VENDOR_CONNECTWARE	0x116b		/* Connectware */
#define	PCI_VENDOR_WSTECH	0x116f		/* Workstation Technology */
#define	PCI_VENDOR_INVENTEC	0x1170		/* Inventec */
#define	PCI_VENDOR_LOUGHSOUND	0x1171		/* Loughborough Sound Images */
#define	PCI_VENDOR_ALTERA	0x1172		/* Altera */
#define	PCI_VENDOR_ADOBE	0x1173		/* Adobe Systems */
#define	PCI_VENDOR_BRIDGEPORT	0x1174		/* Bridgeport Machines */
#define	PCI_VENDOR_MIRTRON	0x1175		/* Mitron Computer */
#define	PCI_VENDOR_SBE	0x1176		/* SBE */
#define	PCI_VENDOR_SILICONENG	0x1177		/* Silicon Engineering */
#define	PCI_VENDOR_ALFA	0x1178		/* Alfa */
#define	PCI_VENDOR_TOSHIBA2	0x1179		/* Toshiba */
#define	PCI_VENDOR_ATREND	0x117a		/* A-Trend Technology */
#define	PCI_VENDOR_ATTO	0x117c		/* Atto Technology */
#define	PCI_VENDOR_TR	0x117e		/* T/R Systems */
#define	PCI_VENDOR_RICOH	0x1180		/* Ricoh */
#define	PCI_VENDOR_TELEMATICS	0x1181		/* Telematics International */
#define	PCI_VENDOR_FUJIKURA	0x1183		/* Fujikura */
#define	PCI_VENDOR_FORKS	0x1184		/* Forks */
#define	PCI_VENDOR_DATAWORLD	0x1185		/* Dataworld */
#define	PCI_VENDOR_DLINK	0x1186		/* D-Link Systems */
#define	PCI_VENDOR_ATL	0x1187		/* Advanced Techonoloy Labratories */
#define	PCI_VENDOR_SHIMA	0x1188		/* Shima Seiki Manufacturing */
#define	PCI_VENDOR_MATSUSHITA2	0x1189		/* Matsushita */
#define	PCI_VENDOR_HILEVEL	0x118a		/* HiLevel Technology */
#define	PCI_VENDOR_COROLLARY	0x118c		/* Corrollary */
#define	PCI_VENDOR_BITFLOW	0x118d		/* BitFlow */
#define	PCI_VENDOR_HERMSTEDT	0x118e		/* Hermstedt */
#define	PCI_VENDOR_ACARD	0x1191		/* Acard */
#define	PCI_VENDOR_DENSAN	0x1192		/* Densan */
#define	PCI_VENDOR_ZEINET	0x1193		/* Zeinet */
#define	PCI_VENDOR_TOUCAN	0x1194		/* Toucan Technology */
#define	PCI_VENDOR_RATOC	0x1195		/* Ratoc System */
#define	PCI_VENDOR_HYTEC	0x1196		/* Hytec Electronic */
#define	PCI_VENDOR_GAGE	0x1197		/* Gage Applied Sciences */
#define	PCI_VENDOR_LAMBDA	0x1198		/* Lambda Systems */
#define	PCI_VENDOR_DCA	0x1199		/* Digital Communications Associates */
#define	PCI_VENDOR_MINDSHARE	0x119a		/* Mind Share */
#define	PCI_VENDOR_OMEGA	0x119b		/* Omega Micro */
#define	PCI_VENDOR_ITI	0x119c		/* Information Technology Institute */
#define	PCI_VENDOR_BUG	0x119d		/* Bug Sapporo */
#define	PCI_VENDOR_FUJITSU3	0x119e		/* Fujitsu */
#define	PCI_VENDOR_BULL	0x119f		/* Bull Hn Information Systems */
#define	PCI_VENDOR_CONVEX	0x11a0		/* Convex Computer */
#define	PCI_VENDOR_HAMAMATSU	0x11a1		/* Hamamatsu Photonics */
#define	PCI_VENDOR_SIERRA2	0x11a2		/* Sierra Research & Technology */
#define	PCI_VENDOR_BARCO	0x11a4		/* Barco */
#define	PCI_VENDOR_MICROUNITY	0x11a5		/* MicroUnity Systems Engineering */
#define	PCI_VENDOR_PUREDATA	0x11a6		/* Pure Data */
#define	PCI_VENDOR_POWERCC	0x11a7		/* Power Computing */
#define	PCI_VENDOR_INNOSYS	0x11a9		/* InnoSys */
#define	PCI_VENDOR_ACTEL	0x11aa		/* Actel */
#define	PCI_VENDOR_GALILEO	0x11ab		/* Galileo Technology */
#define	PCI_VENDOR_CANNON	0x11ac		/* Cannon IS */
#define	PCI_VENDOR_LITEON	0x11ad		/* Lite-On */
#define	PCI_VENDOR_SCITEX	0x11ae		/* Scitex */
#define	PCI_VENDOR_PROLOG	0x11af		/* Pro-Log */
#define	PCI_VENDOR_V3	0x11b0		/* V3 Semiconductor */
#define	PCI_VENDOR_APRICOT	0x11b1		/* Apricot Computer */
#define	PCI_VENDOR_KODAK	0x11b2		/* Eastman Kodak */
#define	PCI_VENDOR_BARR	0x11b3		/* Barr Systems */
#define	PCI_VENDOR_LEITECH	0x11b4		/* Leitch Technology */
#define	PCI_VENDOR_RADSTONE	0x11b5		/* Radstone Technology */
#define	PCI_VENDOR_UNITEDVIDEO	0x11b6		/* United Video */
#define	PCI_VENDOR_MOT2	0x11b7		/* Motorola */
#define	PCI_VENDOR_XPOINT	0x11b8		/* Xpoint Technologies */
#define	PCI_VENDOR_PATHLIGHT	0x11b9		/* Pathlight Technology */
#define	PCI_VENDOR_VIDEOTRON	0x11ba		/* VideoTron */
#define	PCI_VENDOR_PYRAMID	0x11bb		/* Pyramid Technologies */
#define	PCI_VENDOR_NETPERIPH	0x11bc		/* Network Peripherals */
#define	PCI_VENDOR_PINNACLE	0x11bd		/* Pinnacle Systems */
#define	PCI_VENDOR_IMI	0x11be		/* International Microcircuts */
#define	PCI_VENDOR_LUCENT	0x11c1		/* AT&T/Lucent */
#define	PCI_VENDOR_NEC2	0x11c3		/* NEC */
#define	PCI_VENDOR_DOCTECH	0x11c4		/* Document Technologies */
#define	PCI_VENDOR_SHIVA	0x11c5		/* Shiva */
#define	PCI_VENDOR_DCMDATA	0x11c7		/* DCM Data Systems */
#define	PCI_VENDOR_DOLPHIN	0x11c8		/* Dolphin Interconnect Solutions */
#define	PCI_VENDOR_MRTMAGMA	0x11c9		/* Mesa Ridge Technologies (MAGMA) */
#define	PCI_VENDOR_LSISYS	0x11ca		/* LSI Systems */
#define	PCI_VENDOR_SPECIALIX	0x11cb		/* Specialix Research */
#define	PCI_VENDOR_MKC	0x11cc		/* Michels & Kleberhoff Computer */
#define	PCI_VENDOR_HAL	0x11cd		/* HAL Computer Systems */
#define	PCI_VENDOR_IRE	0x11d4		/* IRE */
#define	PCI_VENDOR_ZORAN	0x11de		/* Zoran */
#define	PCI_VENDOR_PIJNENBURG	0x11e3		/* Pijnenburg */
#define	PCI_VENDOR_COMPEX	0x11f6		/* Compex */
#define	PCI_VENDOR_PMCSIERRA	0x11f8		/* PMC-Sierra */
#define	PCI_VENDOR_CYCLADES	0x120e		/* Cyclades */
#define	PCI_VENDOR_ESSENTIAL	0x120f		/* Essential Communications */
#define	PCI_VENDOR_2MICRO	0x1217		/* 2 Micro Inc */
#define	PCI_VENDOR_3DFX	0x121a		/* 3DFX Interactive */
#define	PCI_VENDOR_ARIEL	0x1220		/* Ariel */
#define	PCI_VENDOR_AZTECH	0x122d		/* Aztech */
#define	PCI_VENDOR_3DO	0x1239		/* The 3D0 Company */
#define	PCI_VENDOR_CCUBE	0x123f		/* C-Cube */
#define	PCI_VENDOR_STALLION	0x124d		/* Stallion Technologies */
#define	PCI_VENDOR_LINEARSYS	0x1254		/* Linear Systems */
#define	PCI_VENDOR_ASIX	0x125b		/* ASIX */
#define	PCI_VENDOR_AURORA	0x125c		/* Aurora Technologies */
#define	PCI_VENDOR_ESSTECH	0x125d		/* ESS */
#define	PCI_VENDOR_NORTEL	0x126c		/* Nortel Networks */
#define	PCI_VENDOR_ENSONIQ	0x1274		/* Ensoniq */
#define	PCI_VENDOR_NETAPP	0x1275		/* Network Appliance */
#define	PCI_VENDOR_TRANSMETA	0x1279		/* Transmeta */
#define	PCI_VENDOR_ROCKWELL	0x127a		/* Rockwell */
#define	PCI_VENDOR_DAVICOM	0x1282		/* Davicom Technologies */
#define	PCI_VENDOR_ITEXPRESS	0x1283		/* ITExpress */
#define	PCI_VENDOR_PLATFORM	0x1285		/* Platform */
#define	PCI_VENDOR_LUXSONOR	0x1287		/* LuxSonor */
#define	PCI_VENDOR_TRITECH	0x1292		/* TriTech Microelectronics */
#define	PCI_VENDOR_KOFAX	0x1296		/* Kofax Image Products */
#define	PCI_VENDOR_RISCOM	0x12aa		/* RISCom */
#define	PCI_VENDOR_ALTEON	0x12ae		/* Alteon */
#define	PCI_VENDOR_USR	0x12b9		/* US Robotics */
#define	PCI_VENDOR_PICTUREEL	0x12c5		/* Picture Elements */
#define	PCI_VENDOR_STB2	0x12d2		/* NVidia/SGS-Thomson */
#define	PCI_VENDOR_AUREAL	0x12eb		/* Aureal */
#define	PCI_VENDOR_ADMTEK	0x1317		/* ADMtek */
#define	PCI_VENDOR_PE	0x1318		/* Packet Engines */
#define	PCI_VENDOR_FORTEMEDIA	0x1319		/* Forte Media */
#define	PCI_VENDOR_SIIG	0x131f		/* SIIG */
#define	PCI_VENDOR_PCTEL	0x134d		/* PCTEL */
#define	PCI_VENDOR_KAWASAKI	0x136b		/* Kawasaki */
#define	PCI_VENDOR_LMC	0x1376		/* LAN Media Corp */
#define	PCI_VENDOR_NETGEAR	0x1385		/* Netgear */
#define	PCI_VENDOR_LEVEL1	0x1394		/* Level 1 */
#define	PCI_VENDOR_HIFN	0x13a3		/* Hifn */
#define	PCI_VENDOR_TRIWARE	0x13c1		/* 3ware */
#define	PCI_VENDOR_SUNDANCE	0x13f0		/* Sundance */
#define	PCI_VENDOR_CMI	0x13f6		/* C-Media Electronics */
#define	PCI_VENDOR_LAVA	0x1407		/* Lava */
#define	PCI_VENDOR_OXFORD	0x14d2		/* Oxford */
#define	PCI_VENDOR_INVERTEX	0x14e1		/* Invertex */
#define	PCI_VENDOR_AIRONET	0x14b9		/* Aironet */
#define	PCI_VENDOR_COMPAL	0x14c0		/* COMPAL */
#define	PCI_VENDOR_TITAN	0x14d2		/* Titan */
#define	PCI_VENDOR_BROADCOM	0x14e4		/* Broadcom */
#define	PCI_VENDOR_CONEXANT	0x14f1		/* Conexant */
#define	PCI_VENDOR_DELTA	0x1500		/* Delta */
#define	PCI_VENDOR_TOPIC	0x151f		/* Topic/SmartLink */
#define	PCI_VENDOR_BLUESTEEL	0x15ab		/* Bluesteel Networks */
#define	PCI_VENDOR_VMWARE	0x15ad		/* VMware */
#define	PCI_VENDOR_EUMITCOM	0x1638		/* Eumitcom */
#define	PCI_VENDOR_NETSEC	0x1660		/* NetSec */
#define	PCI_VENDOR_GLOBALSUN	0x16ab		/* Global Sun */
#define	PCI_VENDOR_SYMPHONY2	0x1c1c		/* Symphony Labs */
#define	PCI_VENDOR_TEKRAM2	0x1de1		/* Tekram Technology */
#define	PCI_VENDOR_HINT	0x3388		/* Hint */
#define	PCI_VENDOR_3DLABS	0x3d3d		/* 3D Labs */
#define	PCI_VENDOR_AVANCE2	0x4005		/* Avance Logic */
#define	PCI_VENDOR_ADDTRON	0x4033		/* Addtron */
#define	PCI_VENDOR_INDCOMPSRC	0x494f		/* Industrial Computer Source */
#define	PCI_VENDOR_NETVIN	0x4a14		/* NetVin */
#define	PCI_VENDOR_BUSLOGIC2	0x4b10		/* Buslogic */
#define	PCI_VENDOR_S3	0x5333		/* S3 */
#define	PCI_VENDOR_NETPOWER2	0x5700		/* NetPower */
#define	PCI_VENDOR_C4T	0x6374		/* c't Magazin */
#define	PCI_VENDOR_QUANCM	0x8008		/* Quancm Electronic GmbH */
#define	PCI_VENDOR_INTEL	0x8086		/* Intel */
#define	PCI_VENDOR_TRIGEM2	0x8800		/* Trigem Computer */
#define	PCI_VENDOR_WINBOND2	0x8c4a		/* Winbond */
#define	PCI_VENDOR_COMPUTONE	0x8e0e		/* Computone */
#define	PCI_VENDOR_KTI	0x8e2e		/* KTI */
#define	PCI_VENDOR_ADP	0x9004		/* Adaptec */
#define	PCI_VENDOR_ADP2	0x9005		/* Adaptec */
#define	PCI_VENDOR_ATRONICS	0x907f		/* Atronics */
#define	PCI_VENDOR_ARC	0xedd8		/* ARC Logic */
#define	PCI_VENDOR_CHRYSALIS	0xcafe		/* Chrysalis-ITS */
#define	PCI_VENDOR_INVALID	0xffff		/* INVALID VENDOR ID */

/*
 * List of known products.  Grouped by vendor.
 */

/* PC Tech products */
#define	PCI_PRODUCT_PCTECH_RZ1000	0x1000		/* RZ1000 */

/* Force Computers products */
#define	PCI_PRODUCT_FORCE_SENTINEL	0x0001		/* Sentinel PCI-PCI Bridge */

/* VIA Technologies products */
#define	PCI_PRODUCT_VIATECH_VT82C586_PWR	0x0586		/* VT82C586 PCI-ISA */

/* 2 Micro Inc */
#define	PCI_PRODUCT_2MICRO_OZ6832	0x6832		/* OZ6832 CardBus */


/* 3Ware products */
#define	PCI_PRODUCT_TRIWARE_ESCALADE	0x1000		/* Escalade IDE RAID */
#define	PCI_PRODUCT_TRIWARE_ESCALADE_ASIC	0x1001		/* Escalade IDE RAID */

/* Aironet Products */
#define	PCI_PRODUCT_AIRONET_PCI352	0x0350		/* Cisco/Aironet PCI35x WLAN */
#define	PCI_PRODUCT_AIRONET_PC4500	0x4500		/* PC4500 Wireless */
#define	PCI_PRODUCT_AIRONET_PC4800	0x4800		/* PC4800 Wireless */

/* Acer Labs products */
#define	PCI_PRODUCT_ALI_M1523	0x1523		/* M1523 PCI-ISA */
#define	PCI_PRODUCT_ALI_M1543	0x1533		/* M1543 PCI-ISA */
#define	PCI_PRODUCT_ALI_M5219	0x5219		/* M5219 UDMA IDE */
#define	PCI_PRODUCT_ALI_M5229	0x5229		/* M5229 UDMA IDE */
#define	PCI_PRODUCT_ALI_M5237	0x5237		/* M5237 USB */
#define	PCI_PRODUCT_ALI_M5247	0x5247		/* M5247 AGP/PCI-PCI */
#define	PCI_PRODUCT_ALI_M5243	0x5243		/* M5243 AGP/PCI-PCI */
#define	PCI_PRODUCT_ALI_M7101	0x7101		/* M7101 Power Mgmt */
#define	PCI_PRODUCT_ALI_M3309	0x3309		/* M3309 MPEG Accelerator */

/* ADMtek products */
#define	PCI_PRODUCT_ADMTEK_AL981	0x0981		/* AL981 */
#define	PCI_PRODUCT_ADMTEK_AN983	0x0985		/* AN983 */
#define	PCI_PRODUCT_ADMTEK_AN985	0x1985		/* AN985 */

/* Adaptec products */
#define	PCI_PRODUCT_ADP_AIC7810	0x1078		/* AIC-7810 */
#define	PCI_PRODUCT_ADP_2940AU_CN	0x2178		/* AHA-2940AU/CN */
#define	PCI_PRODUCT_ADP_2930CU	0x3860		/* AHA-2930CU */
#define	PCI_PRODUCT_ADP_AIC7850	0x5078		/* AIC-7850 */
#define	PCI_PRODUCT_ADP_AIC7855	0x5578		/* AIC-7855 */
#define	PCI_PRODUCT_ADP_AIC5900	0x5900		/* AIC-5900 ATM */
#define	PCI_PRODUCT_ADP_AIC5905	0x5905		/* AIC-5905 ATM */
#define	PCI_PRODUCT_ADP_AIC7860	0x6078		/* AIC-7860 */
#define	PCI_PRODUCT_ADP_2940AU	0x6178		/* AHA-2940AU */
#define	PCI_PRODUCT_ADP_AIC7870	0x7078		/* AIC-7870 */
#define	PCI_PRODUCT_ADP_2940	0x7178		/* AHA-2940 */
#define	PCI_PRODUCT_ADP_3940	0x7278		/* AHA-3940 */
#define	PCI_PRODUCT_ADP_3985	0x7378		/* AHA-3985 */
#define	PCI_PRODUCT_ADP_2944	0x7478		/* AHA-2944 */
#define	PCI_PRODUCT_ADP_AIC7880	0x8078		/* AIC-7880 */
#define	PCI_PRODUCT_ADP_2940U	0x8178		/* AHA-2940U */
#define	PCI_PRODUCT_ADP_3940U	0x8278		/* AHA-3940U */
#define	PCI_PRODUCT_ADP_398XU	0x8378		/* AHA-398XU */
#define	PCI_PRODUCT_ADP_2944U	0x8478		/* AHA-2944U */
#define	PCI_PRODUCT_ADP_2940UWPro	0x8778		/* AHA-2940UWPro */
#define	PCI_PRODUCT_ADP_AIC6915	0x6915		/* AIC-6915 */
#define	PCI_PRODUCT_ADP_7895	0x7895		/* AIC-7895 */

#define	PCI_PRODUCT_ADP2_2940U2	0x0010		/* AHA-2940U2 U2 */
#define	PCI_PRODUCT_ADP2_2930U2	0x0011		/* AHA-2930U2 U2 */
#define	PCI_PRODUCT_ADP2_AIC7890	0x001f		/* AIC-7890/1 U2 */
#define	PCI_PRODUCT_ADP2_AIC7892	0x008f		/* AIC-7892 U160 */
#define	PCI_PRODUCT_ADP2_29160	0x0080		/* AHA-29160 U160 */
#define	PCI_PRODUCT_ADP2_19160B	0x0081		/* AHA-19160B U160 */
#define	PCI_PRODUCT_ADP2_3950U2B	0x0050		/* AHA-3950U2B U2 */
#define	PCI_PRODUCT_ADP2_3950U2D	0x0051		/* AHA-3950U2D U2 */
#define	PCI_PRODUCT_ADP2_AIC7896	0x005f		/* AIC-7896/7 U2 */
#define	PCI_PRODUCT_ADP2_3960D	0x00c0		/* AHA-3960D U160 */
#define	PCI_PRODUCT_ADP2_AIC7899B	0x00c1		/* AIC-7899B */
#define	PCI_PRODUCT_ADP2_AIC7899D	0x00c3		/* AIC-7899D */
#define	PCI_PRODUCT_ADP2_AIC7899F	0x00c5		/* AIC-7899F */
#define	PCI_PRODUCT_ADP2_AIC7899	0x00cf		/* AIC-7899 U160 */
#define	PCI_PRODUCT_ADP2_AAC2622	0x0282		/* AAC-2622 */
#define	PCI_PRODUCT_ADP2_AAC364	0x0364		/* AAC-364 */
#define	PCI_PRODUCT_ADP2_AAC3642	0x0365		/* AAC-3642 */
#define	PCI_PRODUCT_ADP2_PERC_2QC	0x1364		/* Dell PERC 2/QC */
/* XXX guess */
#define	PCI_PRODUCT_ADP2_PERC_3QC	0x1365		/* Dell PERC 3/QC */

/* AMD products */
#define	PCI_PRODUCT_AMD_PCNET_PCI	0x2000		/* 79c970 PCnet-PCI LANCE */
#define	PCI_PRODUCT_AMD_PCHOME_PCI	0x2001		/* 79c978 PChome-PCI LANCE */
#define	PCI_PRODUCT_AMD_PCSCSI_PCI	0x2020		/* 53c974 PCscsi-PCI SCSI */
#define	PCI_PRODUCT_AMD_PCNETS_PCI	0x2040		/* 79C974 PCnet-PCI Ether+SCSI */
#define	PCI_PRODUCT_AMD_ELANSC520	0x3000		/* ElanSC520 Host-PCI */
/* http://www.amd.com/products/cpg/athlon/techdocs/pdf/21910.pdf */
#define	PCI_PRODUCT_AMD_SC751_SC	0x7006		/* 751 System Controller */
#define	PCI_PRODUCT_AMD_SC751_PPB	0x7007		/* 751 PCI-PCI */
/* http://www.amd.com/products/cpg/athlon/techdocs/pdf/24462.pdf */
#define	PCI_PRODUCT_AMD_762_PCHB	0x700C		/* 762 Host-PCI */
#define	PCI_PRODUCT_AMD_762_PPB	0x700D		/* 762 PCI-PCI */
#define	PCI_PRODUCT_AMD_761_PCHB	0x700E		/* 761 Host-PCI */
#define	PCI_PRODUCT_AMD_761_PPB	0x700F		/* 761 PCI-PCI */
#define	PCI_PRODUCT_AMD_755_ISA	0x7400		/* 755 PCI-ISA */
#define	PCI_PRODUCT_AMD_755_IDE	0x7401		/* 755 IDE */
#define	PCI_PRODUCT_AMD_755_PMC	0x7403		/* 755 Power Mgmt */
#define	PCI_PRODUCT_AMD_755_USB	0x7404		/* 755 USB */
/* http://www.amd.com/products/cpg/athlon/techdocs/pdf/22548.pdf */
#define	PCI_PRODUCT_AMD_PBC756_ISA	0x7408		/* 756 PCI-ISA */
#define	PCI_PRODUCT_AMD_PBC756_IDE	0x7409		/* 756 IDE */
#define PCI_PRODUCT_AMD_CS5536_IDE  0x209A      /* CS5536 IDE */
#define	PCI_PRODUCT_AMD_PBC756_PMC	0x740b		/* 756 Power Mgmt */
#define	PCI_PRODUCT_AMD_PBC756_USB	0x740c		/* 756 USB Host */
#define	PCI_PRODUCT_AMD_766_ISA	0x7410		/* 766 PCI-ISA */
#define	PCI_PRODUCT_AMD_766_IDE	0x7411		/* 766 IDE */
#define	PCI_PRODUCT_AMD_766_USB	0x7412		/* 766 USB */
#define	PCI_PRODUCT_AMD_766_PMC	0x7413		/* 766 Power Mgmt */

/* AMI */
#define	PCI_PRODUCT_AMI_MEGARAID	0x1960		/* MegaRAID */
#define	PCI_PRODUCT_AMI_MEGARAID428	0x9010		/* MegaRAID Series 428 */
#define	PCI_PRODUCT_AMI_MEGARAID434	0x9060		/* MegaRAID Series 434 */

/* ATI Technologies */
#define	PCI_PRODUCT_ATI_MACH32	0x4158		/* Mach32 */
#define	PCI_PRODUCT_ATI_MACH64_CT	0x4354		/* Mach64 CT */
#define	PCI_PRODUCT_ATI_MACH64_CX	0x4358		/* Mach64 CX */
#define	PCI_PRODUCT_ATI_MACH64_ET	0x4554		/* Mach64 ET */
#define	PCI_PRODUCT_ATI_RAGEPRO	0x4742		/* Rage Pro */
#define	PCI_PRODUCT_ATI_MACH64_GD	0x4744		/* Mach64 GD */
#define	PCI_PRODUCT_ATI_MACH64_GI	0x4749		/* Mach64 GI */
#define	PCI_PRODUCT_ATI_MACH64_GM	0x474d		/* Mach64 GM */
#define	PCI_PRODUCT_ATI_MACH64_GP	0x4750		/* Mach64 GP */
#define	PCI_PRODUCT_ATI_MACH64_GQ	0x4751		/* Mach64 GQ */
#define	PCI_PRODUCT_ATI_RAGEXL	0x4752		/* Rage XL */
#define	PCI_PRODUCT_ATI_MACH64_GT	0x4754		/* Mach64 GT */
#define	PCI_PRODUCT_ATI_MACH64_GU	0x4755		/* Mach64 GU */
#define	PCI_PRODUCT_ATI_MACH64_GV	0x4756		/* Mach64 GV */
#define	PCI_PRODUCT_ATI_MACH64_GW	0x4757		/* Mach64 GW */
#define	PCI_PRODUCT_ATI_MACH64_GX	0x4758		/* Mach64 GX */
#define	PCI_PRODUCT_ATI_MACH64_GY	0x4759		/* Mach64 GY */
#define	PCI_PRODUCT_ATI_MACH64_GZ	0x475a		/* Mach64 GZ */
#define	PCI_PRODUCT_ATI_MACH64_LB	0x4c42		/* Mach64 LB */
#define	PCI_PRODUCT_ATI_MACH64_LD	0x4c44		/* Mach64 LD */
#define	PCI_PRODUCT_ATI_MOBILITY_M3	0x4c46		/* Mobility M3 */
#define	PCI_PRODUCT_ATI_MACH64_LG	0x4c47		/* Mach64 LG */
#define	PCI_PRODUCT_ATI_MACH64_LI	0x4c49		/* Mach64 LI */
#define	PCI_PRODUCT_ATI_MOBILITY_1	0x4c4d		/* Mobility 1 */
#define	PCI_PRODUCT_ATI_MACH64_LP	0x4c50		/* Mach64 LP */
#define	PCI_PRODUCT_ATI_RAGE_PM	0x4c52		/* Rage P/M */
#define	PCI_PRODUCT_ATI_RAGE_FURY	0x5046		/* Rage Fury */
#define	PCI_PRODUCT_ATI_RAGE128_PK	0x5052		/* Rage 128 PK */
#define	PCI_PRODUCT_ATI_RADEON_AIW	0x5144		/* AIW Radeon */
#define	PCI_PRODUCT_ATI_RAGE128_GL	0x5245		/* Rage 128 GL */
#define	PCI_PRODUCT_ATI_RAGE_MAGNUM	0x5246		/* Rage Magnum */
#define	PCI_PRODUCT_ATI_RAGE128_VR	0x524c		/* Rage 128 VR */
#define	PCI_PRODUCT_ATI_RAGE128_SH	0x5348		/* Rage 128 SH */
#define	PCI_PRODUCT_ATI_RAGE128_SK	0x534b		/* Rage 128 SK */
#define	PCI_PRODUCT_ATI_RAGE128_SL	0x534c		/* Rage 128 SL */
#define	PCI_PRODUCT_ATI_RAGE128_SM	0x534d		/* Rage 128 SM */
#define	PCI_PRODUCT_ATI_RAGE128	0x534e		/* Rage 128 */
#define	PCI_PRODUCT_ATI_MACH64_VT	0x5654		/* Mach64 VT */
#define	PCI_PRODUCT_ATI_MACH64_VU	0x5655		/* Mach64 VU */
#define	PCI_PRODUCT_ATI_MACH64_VV	0x5656		/* Mach64 VV */

/* Broadcom */
#define        PCI_PRODUCT_BROADCOM_BCM5752    0x1600          /* BCM5752 */
#define        PCI_PRODUCT_BROADCOM_BCM5752M   0x1601          /* BCM5752M */
#define        PCI_PRODUCT_BROADCOM_BCM5709    0x1639          /* BCM5709 */
#define        PCI_PRODUCT_BROADCOM_BCM5709S   0x163a          /* BCM5709S */
#define        PCI_PRODUCT_BROADCOM_BCM5716    0x163b          /* BCM5716 */
#define        PCI_PRODUCT_BROADCOM_BCM5716S   0x163c          /* BCM5716S */
#define	PCI_PRODUCT_BROADCOM_BCM5700	0x1644		/* BCM5700 */
#define        PCI_PRODUCT_BROADCOM_BCM5701    0x1645          /* BCM5701 */
#define        PCI_PRODUCT_BROADCOM_BCM5702    0x1646          /* BCM5702 */
#define        PCI_PRODUCT_BROADCOM_BCM5703    0x1647          /* BCM5703 */
#define        PCI_PRODUCT_BROADCOM_BCM5704C   0x1648          /* BCM5704C */
#define        PCI_PRODUCT_BROADCOM_BCM5704S_ALT       0x1649          /* BCM5704S Alt */
#define        PCI_PRODUCT_BROADCOM_BCM5706    0x164a          /* BCM5706 */
#define        PCI_PRODUCT_BROADCOM_BCM5708    0x164c          /* BCM5708 */
#define        PCI_PRODUCT_BROADCOM_BCM5702FE  0x164d          /* BCM5702FE */
#define        PCI_PRODUCT_BROADCOM_BCM57710   0x164e          /* BCM57710 */
#define        PCI_PRODUCT_BROADCOM_BCM57711   0x164f          /* BCM57711 */
#define        PCI_PRODUCT_BROADCOM_BCM57711E  0x1650          /* BCM57711E */
#define        PCI_PRODUCT_BROADCOM_BCM5705    0x1653          /* BCM5705 */
#define        PCI_PRODUCT_BROADCOM_BCM5705K   0x1654          /* BCM5705K */
#define        PCI_PRODUCT_BROADCOM_BCM5717    0x1655          /* BCM5717 */
#define        PCI_PRODUCT_BROADCOM_BCM5718    0x1656          /* BCM5718 */
#define        PCI_PRODUCT_BROADCOM_BCM5719    0x1657          /* BCM5719 */
#define        PCI_PRODUCT_BROADCOM_BCM5720    0x1658          /* BCM5720 */
#define        PCI_PRODUCT_BROADCOM_BCM5721    0x1659          /* BCM5721 */
#define        PCI_PRODUCT_BROADCOM_BCM5722    0x165a          /* BCM5722 */
#define        PCI_PRODUCT_BROADCOM_BCM5723    0x165b          /* BCM5723 */
#define        PCI_PRODUCT_BROADCOM_BCM5724    0x165c          /* BCM5724 */
#define        PCI_PRODUCT_BROADCOM_BCM5705M   0x165d          /* BCM5705M */
#define        PCI_PRODUCT_BROADCOM_BCM5705M_ALT       0x165e          /* BCM5705M Alt */
#define        PCI_PRODUCT_BROADCOM_BCM57712   0x1662          /* BCM57712 */
#define        PCI_PRODUCT_BROADCOM_BCM57712E  0x1663          /* BCM57712E */
#define        PCI_PRODUCT_BROADCOM_BCM5714    0x1668          /* BCM5714 */
#define        PCI_PRODUCT_BROADCOM_BCM5714S   0x1669          /* BCM5714S */
#define        PCI_PRODUCT_BROADCOM_BCM5780    0x166a          /* BCM5780 */
#define        PCI_PRODUCT_BROADCOM_BCM5780S   0x166b          /* BCM5780S */
#define        PCI_PRODUCT_BROADCOM_BCM5705F   0x166e          /* BCM5705F */
#define        PCI_PRODUCT_BROADCOM_BCM5754M   0x1672          /* BCM5754M */
#define        PCI_PRODUCT_BROADCOM_BCM5755M   0x1673          /* BCM5755M */
#define        PCI_PRODUCT_BROADCOM_BCM5756    0x1674          /* BCM5756 */
#define        PCI_PRODUCT_BROADCOM_BCM5750    0x1676          /* BCM5750 */
#define        PCI_PRODUCT_BROADCOM_BCM5751    0x1677          /* BCM5751 */
#define        PCI_PRODUCT_BROADCOM_BCM5715    0x1678          /* BCM5715 */
#define        PCI_PRODUCT_BROADCOM_BCM5715S   0x1679          /* BCM5715S */
#define        PCI_PRODUCT_BROADCOM_BCM5754    0x167a          /* BCM5754 */
#define        PCI_PRODUCT_BROADCOM_BCM5755    0x167b          /* BCM5755 */
#define        PCI_PRODUCT_BROADCOM_BCM5750M   0x167c          /* BCM5750M */
#define        PCI_PRODUCT_BROADCOM_BCM5751M   0x167d          /* BCM5751M */
#define        PCI_PRODUCT_BROADCOM_BCM5751F   0x167e          /* BCM5751F */
#define        PCI_PRODUCT_BROADCOM_BCM5787F   0x167f          /* BCM5787F */
#define        PCI_PRODUCT_BROADCOM_BCM5761E   0x1680          /* BCM5761E */
#define        PCI_PRODUCT_BROADCOM_BCM5761    0x1681          /* BCM5761 */
#define        PCI_PRODUCT_BROADCOM_BCM5764    0x1684          /* BCM5764 */
#define        PCI_PRODUCT_BROADCOM_BCM5761S   0x1688          /* BCM5761S */
#define        PCI_PRODUCT_BROADCOM_BCM5761SE  0x1689          /* BCM5761SE */
#define        PCI_PRODUCT_BROADCOM_BCM57760   0x1690          /* BCM57760 */
#define        PCI_PRODUCT_BROADCOM_BCM57788   0x1691          /* BCM57788 */
#define        PCI_PRODUCT_BROADCOM_BCM57780   0x1692          /* BCM57780 */
#define        PCI_PRODUCT_BROADCOM_BCM5787M   0x1693          /* BCM5787M */
#define        PCI_PRODUCT_BROADCOM_BCM57790   0x1694          /* BCM57790 */
#define        PCI_PRODUCT_BROADCOM_BCM5782    0x1696          /* BCM5782 */
#define        PCI_PRODUCT_BROADCOM_BCM5784    0x1698          /* BCM5784 */
#define        PCI_PRODUCT_BROADCOM_BCM5785G   0x1699          /* BCM5785G */
#define        PCI_PRODUCT_BROADCOM_BCM5786    0x169a          /* BCM5786 */
#define        PCI_PRODUCT_BROADCOM_BCM5787    0x169b          /* BCM5787 */
#define        PCI_PRODUCT_BROADCOM_BCM5788    0x169c          /* BCM5788 */
#define        PCI_PRODUCT_BROADCOM_BCM5789    0x169d          /* BCM5789 */
#define        PCI_PRODUCT_BROADCOM_BCM5785F   0x16a0          /* BCM5785F */
#define        PCI_PRODUCT_BROADCOM_BCM5702X   0x16a6          /* BCM5702X */
#define        PCI_PRODUCT_BROADCOM_BCM5703X   0x16a7          /* BCM5703X */
#define        PCI_PRODUCT_BROADCOM_BCM5704S   0x16a8          /* BCM5704S */
#define        PCI_PRODUCT_BROADCOM_BCM5706S   0x16aa          /* BCM5706S */
#define        PCI_PRODUCT_BROADCOM_BCM5708S   0x16ac          /* BCM5708S */
#define        PCI_PRODUCT_BROADCOM_BCM57761   0x16b0          /* BCM57761 */
#define        PCI_PRODUCT_BROADCOM_BCM57781   0x16b1          /* BCM57781 */
#define        PCI_PRODUCT_BROADCOM_BCM57791   0x16b2          /* BCM57791 */
#define        PCI_PRODUCT_BROADCOM_BCM57765   0x16b4          /* BCM57765 */
#define        PCI_PRODUCT_BROADCOM_BCM57785   0x16b5          /* BCM57785 */
#define        PCI_PRODUCT_BROADCOM_BCM57795   0x16b6          /* BCM57795 */
#define        PCI_PRODUCT_BROADCOM_SD 0x16bc          /* SD Host Controller */
#define        PCI_PRODUCT_BROADCOM_BCM5702_ALT        0x16c6          /* BCM5702 Alt */
#define        PCI_PRODUCT_BROADCOM_BCM5703_ALT        0x16c7          /* BCM5703 Alt */
#define        PCI_PRODUCT_BROADCOM_BCM5781    0x16dd          /* BCM5781 */
#define        PCI_PRODUCT_BROADCOM_BCM5753    0x16f7          /* BCM5753 */
#define        PCI_PRODUCT_BROADCOM_BCM5753M   0x16fd          /* BCM5753M */
#define        PCI_PRODUCT_BROADCOM_BCM5753F   0x16fe          /* BCM5753F */
#define        PCI_PRODUCT_BROADCOM_BCM5903M   0x16ff          /* BCM5903M */
#define        PCI_PRODUCT_BROADCOM_BCM4401B1  0x170c          /* BCM4401B1 */
#define        PCI_PRODUCT_BROADCOM_BCM5901    0x170d          /* BCM5901 */
#define        PCI_PRODUCT_BROADCOM_BCM5901A2  0x170e          /* BCM5901A2 */
#define        PCI_PRODUCT_BROADCOM_BCM5903F   0x170f          /* BCM5903F */
#define        PCI_PRODUCT_BROADCOM_BCM5906    0x1712          /* BCM5906 */
#define        PCI_PRODUCT_BROADCOM_BCM5906M   0x1713          /* BCM5906M */
#define        PCI_PRODUCT_BROADCOM_BCM4303    0x4301          /* BCM4303 */
#define        PCI_PRODUCT_BROADCOM_BCM4307    0x4307          /* BCM4307 */
#define        PCI_PRODUCT_BROADCOM_BCM4311    0x4311          /* BCM4311 */
#define        PCI_PRODUCT_BROADCOM_BCM4312    0x4312          /* BCM4312 */
#define        PCI_PRODUCT_BROADCOM_BCM4315    0x4315          /* BCM4315 */
#define        PCI_PRODUCT_BROADCOM_BCM4318    0x4318          /* BCM4318 */
#define        PCI_PRODUCT_BROADCOM_BCM4319    0x4319          /* BCM4319 */
#define        PCI_PRODUCT_BROADCOM_BCM4306    0x4320          /* BCM4306 */
#define        PCI_PRODUCT_BROADCOM_BCM4306_2  0x4321          /* BCM4306 */
#define        PCI_PRODUCT_BROADCOM_SERIAL_2   0x4322          /* Serial */
#define        PCI_PRODUCT_BROADCOM_BCM4309    0x4324          /* BCM4309 */
#define        PCI_PRODUCT_BROADCOM_BCM43XG    0x4325          /* BCM43XG */
#define        PCI_PRODUCT_BROADCOM_BCM4321    0x4328          /* BCM4321 */
#define        PCI_PRODUCT_BROADCOM_BCM4321_2  0x4329          /* BCM4321 */
#define        PCI_PRODUCT_BROADCOM_BCM4322    0x432b          /* BCM4322 */
#define        PCI_PRODUCT_BROADCOM_SERIAL     0x4333          /* Serial */
#define        PCI_PRODUCT_BROADCOM_SERIAL_GC  0x4344          /* Serial */
#define        PCI_PRODUCT_BROADCOM_BCM43224   0x4353          /* BCM43224 */
#define        PCI_PRODUCT_BROADCOM_BCM43225   0x4357          /* BCM43225 */
#define        PCI_PRODUCT_BROADCOM_BCM4401    0x4401          /* BCM4401 */
#define        PCI_PRODUCT_BROADCOM_BCM4401B0  0x4402          /* BCM4401B0 */
#define        PCI_PRODUCT_BROADCOM_BCM4313    0x4727          /* BCM4313 */
#define        PCI_PRODUCT_BROADCOM_5801       0x5801          /* 5801 */
#define        PCI_PRODUCT_BROADCOM_5802       0x5802          /* 5802 */
#define	PCI_PRODUCT_BROADCOM_5805	0x5805		/* 5805 */
#define	PCI_PRODUCT_BROADCOM_5820	0x5820		/* 5820 */
#define        PCI_PRODUCT_BROADCOM_5821       0x5821          /* 5821 */
#define        PCI_PRODUCT_BROADCOM_5822       0x5822          /* 5822 */
#define        PCI_PRODUCT_BROADCOM_5823       0x5823          /* 5823 */
#define        PCI_PRODUCT_BROADCOM_5825       0x5825          /* 5825 */
#define        PCI_PRODUCT_BROADCOM_5860       0x5860          /* 5860 */
#define        PCI_PRODUCT_BROADCOM_5861       0x5861          /* 5861 */
#define        PCI_PRODUCT_BROADCOM_5862       0x5862          /* 5862 */

/* Brooktree products */
#define	PCI_PRODUCT_BROOKTREE_BT848	0x0350		/* BT848 */
#define	PCI_PRODUCT_BROOKTREE_BT849	0x0351		/* BT849 */
#define	PCI_PRODUCT_BROOKTREE_BT878	0x036e		/* BT878 */
#define	PCI_PRODUCT_BROOKTREE_BT879	0x036f		/* BT879 */
#define	PCI_PRODUCT_BROOKTREE_BT878_DEV2	0x0878		/* BT878 (2nd device) */
#define	PCI_PRODUCT_BROOKTREE_BT8474	0x8474		/* Bt8474 Multichannel HDLC Controller */

/* CCUBE products */
#define	PCI_PRODUCT_CCUBE_CINEMASTER	0x8888		/* Cinemaster C 3.0 DVD Decoder */

/* Cirrus Logic products */
#define	PCI_PRODUCT_CIRRUS_CS4610	0x6001		/* CS4610 SoundFusion Audio */
#define	PCI_PRODUCT_CIRRUS_CS4614	0x6003		/* CS4614 */
#define	PCI_PRODUCT_CIRRUS_CS4615	0x6004		/* CS4615 */
#define	PCI_PRODUCT_CIRRUS_CS4280	0x6003		/* CS4280 CrystalClear Audio */
#define	PCI_PRODUCT_CIRRUS_CS4281	0x6005		/* CS4281 CrystalClear Audio */

/* CMD Technology products -- info gleaned from www.cmd.com */
#define	PCI_PRODUCT_CMDTECH_640	0x0640		/* PCI0640 */
#define	PCI_PRODUCT_CMDTECH_642	0x0642		/* PCI0642 */
#define	PCI_PRODUCT_CMDTECH_643	0x0643		/* PCI0643 */
#define	PCI_PRODUCT_CMDTECH_646	0x0646		/* PCI0646 */
#define	PCI_PRODUCT_CMDTECH_647	0x0647		/* PCI0647 */
#define	PCI_PRODUCT_CMDTECH_648	0x0648		/* PCI0648 */
#define	PCI_PRODUCT_CMDTECH_649	0x0649		/* PCI0649 */
/* Inclusion of 'A' in the following entry is probably wrong. */
/* No data on the CMD Tech. web site for the following as of Mar. 3 '98 */
#define	PCI_PRODUCT_CMDTECH_650A	0x0650		/* PCI0650A */
#define	PCI_PRODUCT_CMDTECH_670	0x0670		/* USB0670 */
#define	PCI_PRODUCT_CMDTECH_673	0x0673		/* USB0673 */

/* Cogent Data Technologies products */
#define	PCI_PRODUCT_COGENT_EM110TX	0x1400		/* EX110TX */

/* Creative Labs */
#define	PCI_PRODUCT_CREATIVELABS_SBLIVE	0x0002		/* SoundBlaster Live */
#define	PCI_PRODUCT_CREATIVELABS_DIGIN	0x7002		/* SoundBlaster Digital Input */
#define	PCI_PRODUCT_CREATIVELABS_EV1938	0x8938		/* Ectiva 1938 */

/* Cyclades products */
#define	PCI_PRODUCT_CYCLADES_CYCLOMY_1	0x0100		/* Cyclom-Y below 1M */
#define	PCI_PRODUCT_CYCLADES_CYCLOMY_2	0x0101		/* Cyclom-Y */
#define	PCI_PRODUCT_CYCLADES_CYCLOM4Y_1	0x0102		/* Cyclom-4Y below 1M */
#define	PCI_PRODUCT_CYCLADES_CYCLOM4Y_2	0x0103		/* Cyclom-4Y */
#define	PCI_PRODUCT_CYCLADES_CYCLOM8Y_1	0x0104		/* Cyclom-8Y below 1M */
#define	PCI_PRODUCT_CYCLADES_CYCLOM8Y_2	0x0105		/* Cyclom-8Y */
#define	PCI_PRODUCT_CYCLADES_CYCLOMZ_1	0x0200		/* Cyclom-Z below 1M */
#define	PCI_PRODUCT_CYCLADES_CYCLOMZ_2	0x0201		/* Cyclom-Z */

/* Cyrix/National Semiconductor products */
#define	PCI_PRODUCT_CYRIX_GXMPCI	0x0001		/* GXm Host-PCI */
#define	PCI_PRODUCT_CYRIX_GXMISA	0x0002		/* GXm PCI-ISA */
#define	PCI_PRODUCT_CYRIX_CX5530_PCIB	0x0100		/* Cx5530 South Bridge */
#define	PCI_PRODUCT_CYRIX_CX5530_SMI	0x0101		/* Cx5530 SMI/ACPI */
#define	PCI_PRODUCT_CYRIX_CX5530_IDE	0x0102		/* Cx5530 IDE */
#define	PCI_PRODUCT_CYRIX_CX5530_AUDIO	0x0103		/* Cx5530 XpressAUDIO */
#define	PCI_PRODUCT_CYRIX_CX5530_VIDEO	0x0104		/* Cx5530 Video */

/* D-Link products */
#define	PCI_PRODUCT_DLINK_550TX	0x1002		/* 550TX */
#define	PCI_PRODUCT_DLINK_530TXPLUS	0x1300		/* 530TX+ */
#define	PCI_PRODUCT_DLINK_DGE550T	0x4000		/* DGE-550T */

/* DEC products */
#define	PCI_PRODUCT_DEC_21050	0x0001		/* 21050 PCI-PCI */
#define	PCI_PRODUCT_DEC_21040	0x0002		/* 21040 */
#define	PCI_PRODUCT_DEC_21030	0x0004		/* 21030 */
#define	PCI_PRODUCT_DEC_21140	0x0009		/* 21140 */
#define	PCI_PRODUCT_DEC_PBXGB	0x000d		/* TGA2 */
#define	PCI_PRODUCT_DEC_DEFPA	0x000f		/* DEFPA */
/* product DEC ???		0x0010	??? VME Interface */
#define	PCI_PRODUCT_DEC_21041	0x0014		/* 21041 */
#define	PCI_PRODUCT_DEC_21142	0x0019		/* 21142/3 */
/* Farallon apparently used DEC's vendor ID by mistake */
#define	PCI_PRODUCT_DEC_21052	0x0021		/* 21052 PCI-PCI */
#define	PCI_PRODUCT_DEC_21150	0x0022		/* 21150 PCI-PCI */
#define	PCI_PRODUCT_DEC_21150_BC	0x0023		/* 21150-BC PCI-PCI */
#define	PCI_PRODUCT_DEC_21152	0x0024		/* 21152 PCI-PCI */
#define	PCI_PRODUCT_DEC_21153	0x0025		/* 21153 PCI-PCI */
#define	PCI_PRODUCT_DEC_21154	0x0026		/* 21154 PCI-PCI */
#define	PCI_PRODUCT_DEC_CPQ42XX	0x0046		/* Compaq SMART RAID 42xx */

/* Delta Electronics products */
#define	PCI_PRODUCT_DELTA_8139	0x1360		/* 8139 */
#define	PCI_PRODUCT_DELTA_RHINEII	0x1320		/* RhineII */

//wan+ if
/* Dell Computer products */
#define PCI_PRODUCT_DELL_PERC_2SI   0x0001      /* PERC 2/Si */
#define PCI_PRODUCT_DELL_PERC_3DI   0x0002      /* PERC 3/Di */
#define PCI_PRODUCT_DELL_PERC_3SI   0x0003      /* PERC 3/Si */
#define PCI_PRODUCT_DELL_PERC_3SI_2 0x0004      /* PERC 3/Si */
#define PCI_PRODUCT_DELL_DRAC_3_ADDIN   0x0007      /* DRAC 3 Add-in */
#define PCI_PRODUCT_DELL_DRAC_3_VUART   0x0008      /* DRAC 3 Virtual UART */
#define PCI_PRODUCT_DELL_DRAC_3_EMBD    0x0009      /* DRAC 3 Embedded/Optional */
#define PCI_PRODUCT_DELL_PERC_3DI_3     0x000a      /* PERC 3/Di */
#define PCI_PRODUCT_DELL_DRAC_4_EMBD    0x000c      /* DRAC 4 Embedded/Optional */
#define PCI_PRODUCT_DELL_DRAC_3_OPT 0x000d      /* DRAC 3 Optional */
#define PCI_PRODUCT_DELL_PERC_4DI   0x000e      /* PERC 4/Di i960 */
#define PCI_PRODUCT_DELL_PERC_4DI_2 0x000f      /* PERC 4/Di Verde */
#define PCI_PRODUCT_DELL_DRAC_4     0x0011      /* DRAC 4 */
#define PCI_PRODUCT_DELL_DRAC_4_VUART   0x0012      /* DRAC 4 Virtual UART */
#define PCI_PRODUCT_DELL_PERC_4EDI      0x0013      /* PERC 4e/Di */
#define PCI_PRODUCT_DELL_DRAC_4_SMIC    0x0014      /* DRAC 4 SMIC */
#define PCI_PRODUCT_DELL_PERC_3DI_2_SUB 0x00cf      /* PERC 3/Di */
#define PCI_PRODUCT_DELL_PERC_3SI_2_SUB 0x00d0      /* PERC 3/Si */
#define PCI_PRODUCT_DELL_PERC_3DI_SUB2  0x00d1      /* PERC 3/Di */
#define PCI_PRODUCT_DELL_PERC_3DI_SUB3  0x00d9      /* PERC 3/Di */
#define PCI_PRODUCT_DELL_PERC_3DI_3_SUB 0x0106      /* PERC 3/Di */
#define PCI_PRODUCT_DELL_PERC_3DI_3_SUB2    0x011b      /* PERC 3/Di */
#define PCI_PRODUCT_DELL_PERC_3DI_3_SUB3    0x0121      /* PERC 3/Di */
#define PCI_PRODUCT_DELL_PERC5              0x0015      /* PERC 5 wan+*/
//wan+ end

/* Diamond products */

/* Distributed Processing Technology products */
#define	PCI_PRODUCT_DPT_SC_RAID	0xa400		/* SmartCache/Raid */
#define	PCI_PRODUCT_DPT_I960_PPB	0xa500		/* PCI-PCI bridge */
#define	PCI_PRODUCT_DPT_RAID_I2O	0xa501		/* SmartRAID (I2O) */

/* Emulex products */
#define	PCI_PRODUCT_EMULEX_LPPFC	0x10df		/* Light Pulse FibreChannel */

/* Ensoniq products */
#define	PCI_PRODUCT_ENSONIQ_AUDIOPCI97	0x1371		/* AudioPCI97 */
#define	PCI_PRODUCT_ENSONIQ_AUDIOPCI	0x5000		/* AudioPCI */
#define	PCI_PRODUCT_ENSONIQ_CT5880	0x5880		/* CT5880 */

/* ESS Technology Inc products */
#define	PCI_PRODUCT_ESSTECH_MAESTROII	0x1968		/* Maestro II */
#define	PCI_PRODUCT_ESSTECH_SOLO1	0x1969		/* SOLO-1 AudioDrive */
#define	PCI_PRODUCT_ESSTECH_MAESTRO2E	0x1978		/* Maestro 2E */
#define	PCI_PRODUCT_ESSTECH_ES1989	0x1988		/* ES1989 */
#define	PCI_PRODUCT_ESSTECH_MAESTRO3	0x1998		/* Maestro 3 */
#define	PCI_PRODUCT_ESSTECH_ES1983	0x1999		/* ES1983 Modem */
#define	PCI_PRODUCT_ESSTECH_2898	0x2898		/* ES2898 Modem */

/* Essential Communications products */
#define	PCI_PRODUCT_ESSENTIAL_RR_HIPPI	0x0001		/* RoadRunner HIPPI */
#define	PCI_PRODUCT_ESSENTIAL_RR_GIGE	0x0005		/* RoadRunner Gig-E */

/* Evans & Sutherland products */
#define	PCI_PRODUCT_ES_FREEDOM	0x0001		/* Freedom PCI-GBus */

/* Eumitcom Technology products */
#define	PCI_PRODUCT_EUMITCOM_WL11000P	0x1100		/* WL11000P */

/* FORE products */
#define	PCI_PRODUCT_FORE_PCA200	0x0210		/* ATM PCA-200 */
#define	PCI_PRODUCT_FORE_PCA200E	0x0300		/* ATM PCA-200e */

/* Forte Media products */
#define	PCI_PRODUCT_FORTEMEDIA_FM801	0x0801		/* 801 Sound */

/* Future Domain products */
#define	PCI_PRODUCT_FUTUREDOMAIN_TMC_18C30	0x0000		/* TMC-18C30 (36C70) */

/* Efficient Networks products */
#define	PCI_PRODUCT_EFFICIENTNETS_ENI155PF	0x0000		/* 155P-MF1 ATM (FPGA) */
#define	PCI_PRODUCT_EFFICIENTNETS_ENI155PA	0x0002		/* 155P-MF1 ATM (ASIC) */
#define	PCI_PRODUCT_EFFICIENTNETS_EFSS25	0x0005		/* 25SS-3010 ATM (ASIC) */

/* Global Sun Technology products */
#define	PCI_PRODUCT_GLOBALSUN_GL24110P	0x1101		/* GL24110P */
#define	PCI_PRODUCT_GLOBALSUN_GL24110P02	0x1102		/* GL24110P02 */

/* Hewlett-Packard products */
#define	PCI_PRODUCT_HP_J2585A	0x1030		/* J2585A */
#define	PCI_PRODUCT_HP_J2585B	0x1031		/* J2585B */
#define	PCI_PRODUCT_HP_NETRAID_4M	0x10c2		/* NetRaid-4M */

/* Hifn products */
#define	PCI_PRODUCT_HIFN_7751	0x0005		/* 7751 */
#define	PCI_PRODUCT_HIFN_6500	0x0006		/* 6500 */
#define	PCI_PRODUCT_HIFN_7951	0x0012		/* 7951 */

/* IDT products */
#define	PCI_PRODUCT_IDT_77201	0x0001		/* 77201/77211 ATM (NICStAR) */

/* Industrial Computer Source */
#define	PCI_PRODUCT_INDCOMPSRC_WDT50x	0x22c0		/* WDT 50x Watchdog Timer */

/* Integrated Micro Solutions products */
#define	PCI_PRODUCT_IMS_8849	0x8849		/* 8849 */

/* Intel products */
#define	PCI_PRODUCT_INTEL_GDT_RAID1	0x0600		/* GDT RAID */
#define	PCI_PRODUCT_INTEL_GDT_RAID2	0x061f		/* GDT RAID */
#define	PCI_PRODUCT_INTEL_82452	0x1000		/* PRO 1000 */
#define	PCI_PRODUCT_INTEL_82452_SC	0x1001		/* PRO 1000SC */
#define	PCI_PRODUCT_INTEL_82453	0x1001		/* PRO 1000F */
#define	PCI_PRODUCT_INTEL_82453_CU	0x1004		/* PRO 1000T */
#define	PCI_PRODUCT_INTEL_PRO_100_VE_0	0x1031		/* PRO/100 VE */
#define	PCI_PRODUCT_INTEL_PRO_100_VE_1	0x1032		/* PRO/100 VE */
#define	PCI_PRODUCT_INTEL_PRO_100_VM_0	0x1033		/* PRO/100 VM */
#define	PCI_PRODUCT_INTEL_PRO_100_VM_1	0x1034		/* PRO/100 VM */
#define	PCI_PRODUCT_INTEL_82562EH_HPNA_0	0x1035		/* 82562EH HomePNA */
       /* PRO/1000 MT (82574L) */
#define        PCI_PRODUCT_INTEL_82574L                0x10d3
       /* PRO/1000 (82576) */
#define        PCI_PRODUCT_INTEL_82576                 0x10c9
       /* PRO/1000 FP (82576) */
#define        PCI_PRODUCT_INTEL_82576_FIBER   0x10e6
       /* PRO/1000 FP (82576) */
#define        PCI_PRODUCT_INTEL_82576_SERDES  0x10e7
       /* PRO/1000 QP (82576) */
#define        PCI_PRODUCT_INTEL_82576_QUAD_COPPER     0x10e8
#define	PCI_PRODUCT_INTEL_82562EH_HPNA_1	0x1036		/* 82562EH HomePNA */
#define	PCI_PRODUCT_INTEL_82562EH_HPNA_2	0x1037		/* 82562EH HomePNA */
#define	PCI_PRODUCT_INTEL_PRO_100_VM_2	0x1038		/* PRO/100 VM */
#define	PCI_PRODUCT_INTEL_82815_DC100_HUB	0x1100		/* 82815 Hub */
#define	PCI_PRODUCT_INTEL_82815_DC100_AGP	0x1101		/* 82815 AGP */
#define	PCI_PRODUCT_INTEL_82815_DC100_GRAPH	0x1102		/* 82815 Graphics */
#define	PCI_PRODUCT_INTEL_82815_NOAGP_HUB	0x1110		/* 82815 Hub */
#define	PCI_PRODUCT_INTEL_82815_NOAGP_GRAPH	0x1112		/* 82815 Graphics */
#define	PCI_PRODUCT_INTEL_82815_NOGRAPH_HUB	0x1120		/* 82815 Hub */
#define	PCI_PRODUCT_INTEL_82815_NOGRAPH_AGP	0x1121		/* 82815 AGP */
#define	PCI_PRODUCT_INTEL_82815_FULL_HUB	0x1130		/* 82815 Hub */
#define	PCI_PRODUCT_INTEL_82815_FULL_AGP	0x1131		/* 82815 AGP */
#define	PCI_PRODUCT_INTEL_82815_FULL_GRAPH	0x1132		/* 82815 Graphics */
#define	PCI_PRODUCT_INTEL_82559ER	0x1209		/* 82559ER */
#define	PCI_PRODUCT_INTEL_82092AA	0x1222		/* 82092AA IDE */
#define	PCI_PRODUCT_INTEL_SAA7116	0x1223		/* SAA7116 */
#define	PCI_PRODUCT_INTEL_82596	0x1226		/* EE Pro 10 PCI */
#define	PCI_PRODUCT_INTEL_EEPRO100	0x1227		/* EE Pro 100 */
#define	PCI_PRODUCT_INTEL_EEPRO100S	0x1228		/* EE Pro 100 Smart */
#define	PCI_PRODUCT_INTEL_82557	0x1229		/* 82557 */
#define	PCI_PRODUCT_INTEL_82559	0x1030		/* 82559 */
#define	PCI_PRODUCT_INTEL_82806AA_APIC	0x1161		/* 82806AA PCI64 APIC */
#define	PCI_PRODUCT_INTEL_82437FX	0x122d		/* 82437FX */
#define	PCI_PRODUCT_INTEL_82371FB_ISA	0x122e		/* 82371FB PCI-ISA */
#define	PCI_PRODUCT_INTEL_82371FB_IDE	0x1230		/* 82371FB IDE */
#define	PCI_PRODUCT_INTEL_82371MX	0x1234		/* 82371 PCI-ISA and IDE */
#define	PCI_PRODUCT_INTEL_82437MX	0x1235		/* 82437 PCI/Cache/DRAM */
#define	PCI_PRODUCT_INTEL_82441FX	0x1237		/* 82441FX */
#define	PCI_PRODUCT_INTEL_82380AB	0x123c		/* 82380AB Mobile PCI-ISA */
#define	PCI_PRODUCT_INTEL_82380FB	0x124b		/* 82380FB Mobile PCI-PCI */
#define	PCI_PRODUCT_INTEL_82439HX	0x1250		/* 82439HX */
#define	PCI_PRODUCT_INTEL_82806AA	0x1360		/* 82806AA PCI64 */
#define	PCI_PRODUCT_INTEL_80960RP_ATU	0x1960		/* 80960RP ATU */
#define	PCI_PRODUCT_INTEL_82840_HB	0x1a21		/* 82840 Host */
#define	PCI_PRODUCT_INTEL_82840_AGP	0x1a23		/* 82840 AGP */
#define	PCI_PRODUCT_INTEL_82840_PCI	0x1a24		/* 82840 PCI */
#define	PCI_PRODUCT_INTEL_82801AA_LPC	0x2410		/* 82801AA LPC */
#define	PCI_PRODUCT_INTEL_82801AA_IDE	0x2411		/* 82801AA IDE */
#define	PCI_PRODUCT_INTEL_82801AA_USB	0x2412		/* 82801AA USB */
#define	PCI_PRODUCT_INTEL_82801AA_SMB	0x2413		/* 82801AA SMBus */
#define	PCI_PRODUCT_INTEL_82801AA_ACA	0x2415		/* 82801AA AC-97 Audio */
#define	PCI_PRODUCT_INTEL_82801AA_ACM	0x2416		/* 82801AA AC-97 Modem */
#define	PCI_PRODUCT_INTEL_82801AA_HPB	0x2418		/* 82801AA Hub-to-PCI */
#define	PCI_PRODUCT_INTEL_82801AB_LPC	0x2420		/* 82801AB LPC */
#define	PCI_PRODUCT_INTEL_82801AB_IDE	0x2421		/* 82801AB IDE */
#define	PCI_PRODUCT_INTEL_82801AB_USB	0x2422		/* 82801AB USB */
#define	PCI_PRODUCT_INTEL_82801AB_SMB	0x2423		/* 82801AB SMBus */
#define	PCI_PRODUCT_INTEL_82801AB_ACA	0x2425		/* 82801AB AC-97 Audio */
#define	PCI_PRODUCT_INTEL_82801AB_ACM	0x2426		/* 82801AB AC-97 Modem */
#define	PCI_PRODUCT_INTEL_82801AB_HPB	0x2428		/* 82801AB Hub-to-PCI */
#define	PCI_PRODUCT_INTEL_82801BA_LPC	0x2440		/* 82801BA LPC */
#define	PCI_PRODUCT_INTEL_82801BA_USB	0x2442		/* 82801BA USB */
#define	PCI_PRODUCT_INTEL_82801BA_SMBUS	0x2443		/* 82801BA SMBus */
#define	PCI_PRODUCT_INTEL_82801BA_USB2	0x2444		/* 82801BA USB2 */
#define	PCI_PRODUCT_INTEL_82801BA_ACA	0x2445		/* 82801BA AC-97 Audio */
#define	PCI_PRODUCT_INTEL_82801BA_ACM	0x2446		/* 82801BA AC-97 Modem */
#define	PCI_PRODUCT_INTEL_82801BAM_HPB	0x2448		/* 82801BAM Hub-to-PCI */
#define	PCI_PRODUCT_INTEL_82562	0x2449		/* 82562 */
#define	PCI_PRODUCT_INTEL_82801BAM_IDE	0x244a		/* 82801BAM IDE */
#define	PCI_PRODUCT_INTEL_82801BA_IDE	0x244b		/* 82801BA IDE */
#define	PCI_PRODUCT_INTEL_82801BAM_LPC	0x244c		/* 82801BAM LPC */
#define	PCI_PRODUCT_INTEL_82801BA_AGP	0x244e		/* 82801BA AGP */
#define	PCI_PRODUCT_INTEL_82801CA_CAM	0x2485		/* 82801CA AC-97 Audio */
#define	PCI_PRODUCT_INTEL_82801CAM_IDE	0x248a		/* 82801CAM IDE */
#define	PCI_PRODUCT_INTEL_82801CA_IDE	0x248b		/* 82801CA IDE */
#define	PCI_PRODUCT_INTEL_82820_MCH	0x2501		/* 82820 MCH */
#define	PCI_PRODUCT_INTEL_82820_AGP	0x250f		/* 82820 AGP */
#define	PCI_PRODUCT_INTEL_82850_HB	0x2530		/* 82850 Host */
#define	PCI_PRODUCT_INTEL_82860_HB	0x2531		/* 82860 Host */
#define	PCI_PRODUCT_INTEL_82850_AGP	0x2532		/* 82850/82860 AGP */
#define	PCI_PRODUCT_INTEL_82860_PCI1	0x2533		/* 82860 PCI-PCI */
#define	PCI_PRODUCT_INTEL_82860_PCI2	0x2534		/* 82860 PCI-PCI */
#define	PCI_PRODUCT_INTEL_82860_PCI3	0x2535		/* 82860 PCI-PCI */
#define	PCI_PRODUCT_INTEL_82860_PCI4	0x2536		/* 82860 PCI-PCI */
#define	PCI_PRODUCT_INTEL_82371SB_ISA	0x7000		/* 82371SB PCI-ISA */
#define	PCI_PRODUCT_INTEL_82371SB_IDE	0x7010		/* 82371SB IDE */
#define	PCI_PRODUCT_INTEL_82371USB	0x7020		/* 82371SB USB */
#define	PCI_PRODUCT_INTEL_82437VX	0x7030		/* 82437VX */
#define	PCI_PRODUCT_INTEL_82439TX	0x7100		/* 82439TX System */
#define	PCI_PRODUCT_INTEL_82371AB_ISA	0x7110		/* 82371AB PIIX4 ISA */
#define	PCI_PRODUCT_INTEL_82371AB_IDE	0x7111		/* 82371AB IDE */
#define	PCI_PRODUCT_INTEL_82371AB_USB	0x7112		/* 82371AB USB */
#define	PCI_PRODUCT_INTEL_82371AB_PMC	0x7113		/* 82371AB Power Mgmt */
#define	PCI_PRODUCT_INTEL_82810_MCH	0x7120		/* 82810 */
#define	PCI_PRODUCT_INTEL_82810_GC	0x7121		/* 82810 Graphics */
#define	PCI_PRODUCT_INTEL_82810_DC100_MCH	0x7122		/* 82810-DC100 */
#define	PCI_PRODUCT_INTEL_82810_DC100_GC	0x7123		/* 82810-DC100 Graphics */
#define	PCI_PRODUCT_INTEL_82810E_MCH	0x7124		/* 82810E */
#define	PCI_PRODUCT_INTEL_82810E_GC	0x7125		/* 82810E Graphics */
#define	PCI_PRODUCT_INTEL_82443LX	0x7180		/* 82443LX PCI-AGP */
#define	PCI_PRODUCT_INTEL_82443LX_AGP	0x7181		/* 82443LX AGP */
#define	PCI_PRODUCT_INTEL_82443BX	0x7190		/* 82443BX PCI-AGP */
#define	PCI_PRODUCT_INTEL_82443BX_AGP	0x7191		/* 82443BX AGP */
#define	PCI_PRODUCT_INTEL_82443BX_NOAGP	0x7192		/* 82443BX */
#define	PCI_PRODUCT_INTEL_82440MX	0x7194		/* 82440MX Host */
#define	PCI_PRODUCT_INTEL_82440MX_ACA	0x7195		/* 82440MX AC97 Audio */
#define	PCI_PRODUCT_INTEL_82440MX_ISA	0x7198		/* 82440MX PCI-ISA */
#define	PCI_PRODUCT_INTEL_82440MX_IDE	0x7199		/* 82440MX IDE */
#define	PCI_PRODUCT_INTEL_82440MX_USB	0x719a		/* 82440MX USB */
#define	PCI_PRODUCT_INTEL_82440MX_PM	0x719b		/* 82440MX Power Mgmt */
#define	PCI_PRODUCT_INTEL_82440BX	0x71a0		/* 82440BX PCI-AGP */
#define	PCI_PRODUCT_INTEL_82440BX_AGP	0x71a1		/* 82440BX AGP */
#define	PCI_PRODUCT_INTEL_82443GX	0x71a2		/* 82443GX */
#define	PCI_PRODUCT_INTEL_82740	0x7800		/* 82740 AGP */
#define	PCI_PRODUCT_INTEL_PCI450_PB	0x84c4		/* 82450KX/GX */
#define	PCI_PRODUCT_INTEL_PCI450_MC	0x84c5		/* 82450KX/GX Memory */
#define	PCI_PRODUCT_INTEL_82451NX	0x84ca		/* 82451NX Mem & IO */
#define	PCI_PRODUCT_INTEL_82454NX	0x84cb		/* 82454NX PXB */
#define	PCI_PRODUCT_INTEL_82802AB	0x89ad		/* 82802AB Firmware Hub 4Mbit */
#define	PCI_PRODUCT_INTEL_82802AC	0x89ac		/* 82802AC Firmware Hub 8Mbit */
#define	PCI_PRODUCT_INTEL_S21152	0xb152		/* S21152 PCI-PCI */

/* Macronix */
#define	PCI_PRODUCT_MACRONIX_MX98713	0x0512		/* PMAC 98713 */
#define	PCI_PRODUCT_MACRONIX_MX98715	0x0531		/* PMAC 98715 */
#define	PCI_PRODUCT_MACRONIX_MX98727	0x0532		/* PMAC 98727 */
#define	PCI_PRODUCT_MACRONIX_MX86250	0x8625		/* MX86250 */

/* Madge Networks products */
#define	PCI_PRODUCT_MADGE_COLLAGE25	0x1000		/* Collage 25 ATM adapter */
#define	PCI_PRODUCT_MADGE_COLLAGE155	0x1001		/* Collage 155 ATM adapter */

/* Motorola products */
#define	PCI_PRODUCT_MOT_MPC106	0x0002		/* MPC106 Host-PCI */
/* product MOT MPC107		0x0003	MPC107 Host-PCI */
#define	PCI_PRODUCT_MOT_MPC107	0x0004		/* MPC107 Host-PCI */
#define	PCI_PRODUCT_MOT_SM56	0x5600		/* SM56 */
#define	PCI_PRODUCT_MOT_RAVEN	0x4801		/* Raven Host-PCI */
/* Mylex products */
#define	PCI_PRODUCT_MYLEX_960P	0x0001		/* DAC960P RAID */

/* Mutech products */
#define	PCI_PRODUCT_MUTECH_MV1000	0x0001		/* MV1000 */

/* National Semiconductor products */
#define	PCI_PRODUCT_NS_DP83810	0x0001		/* DP83810 10/100 */
#define	PCI_PRODUCT_NS_DP83815	0x0020		/* DP83815 10/100 */
#define	PCI_PRODUCT_NS_DP83820	0x0022		/* DP83820 1/10/100/1000 */
#define	PCI_PRODUCT_NS_NS87410	0xd001		/* NS87410 */

/* NEC */
#define	PCI_PRODUCT_NEC_USB	0x0035		/* USB */
#define	PCI_PRODUCT_NEC_POWERVR2	0x0046		/* PowerVR PCX2 */
#define	PCI_PRODUCT_NEC_MARTH	0x0074		/* I/O */
#define	PCI_PRODUCT_NEC_PKUG	0x007d		/* I/O */
#define	PCI_PRODUCT_NEC_VERSAMAESTRO	0x8058		/* Versa Maestro */
#define	PCI_PRODUCT_NEC_VERSAPRONXVA26D	0x803c		/* Versa Va26D Maestro */

/* NeoMagic */
#define	PCI_PRODUCT_NEOMAGIC_NM2070	0x0001		/* Magicgraph NM2070 */
#define	PCI_PRODUCT_NEOMAGIC_128V	0x0002		/* Magicgraph 128V */
#define	PCI_PRODUCT_NEOMAGIC_128ZV	0x0003		/* Magicgraph 128ZV */
#define	PCI_PRODUCT_NEOMAGIC_NM2160	0x0004		/* Magicgraph NM2160 */
#define	PCI_PRODUCT_NEOMAGIC_NM2200	0x0005		/* Magicgraph NM2200 */
#define	PCI_PRODUCT_NEOMAGIC_NM2360	0x0006		/* Magicgraph NM2360 */
#define	PCI_PRODUCT_NEOMAGIC_NM2230	0x0025		/* MagicMedia 256AV+ */
#define	PCI_PRODUCT_NEOMAGIC_NM256XLP	0x0016		/* MagicMedia 256XL+ */
#define	PCI_PRODUCT_NEOMAGIC_NM256AV	0x8005		/* MagicMedia 256AV */
#define	PCI_PRODUCT_NEOMAGIC_NM256ZX	0x8006		/* MagicMedia 256ZX */

/* Netgear products */
#define	PCI_PRODUCT_NETGEAR_GA620	0x620a		/* GA620 */
#define	PCI_PRODUCT_NETGEAR_GA620T	0x630a		/* GA620T */

/* Network Security Technologies, Inc. */
#define	PCI_PRODUCT_NETSEC_7751	0x7751		/* 7751 */

/* C-Media Electronics Inc */
#define	PCI_PRODUCT_CMI_CMI8338A	0x0100		/* CMI8338A Audio */
#define	PCI_PRODUCT_CMI_CMI8338B	0x0101		/* CMI8338B Audio */
#define	PCI_PRODUCT_CMI_CMI8738	0x0111		/* CMI8738/C3DX Audio */
#define	PCI_PRODUCT_CMI_CMI8738B	0x0112		/* CMI8738B Audio */
#define	PCI_PRODUCT_CMI_HSP56	0x0211		/* HSP56 AMR */

/* NetVin */
#define	PCI_PRODUCT_NETVIN_NV5000	0x5000		/* NetVin 5000 */

/* Newbridge / Tundra products */
#define	PCI_PRODUCT_NEWBRIDGE_CA91CX42	0x0000		/* Universe VME */

/* NCR/Symbios Logic products */
#define	PCI_PRODUCT_SYMBIOS_810	0x0001		/* 53c810 */
#define	PCI_PRODUCT_SYMBIOS_820	0x0002		/* 53c820 */
#define	PCI_PRODUCT_SYMBIOS_825	0x0003		/* 53c825 */
#define	PCI_PRODUCT_SYMBIOS_815	0x0004		/* 53c815 */
#define	PCI_PRODUCT_SYMBIOS_810AP	0x0005		/* 53c810AP */
#define	PCI_PRODUCT_SYMBIOS_860	0x0006		/* 53c860 */
#define	PCI_PRODUCT_SYMBIOS_1510D	0x000a		/* 53c1510D */
#define	PCI_PRODUCT_SYMBIOS_896	0x000b		/* 53c896 */
#define	PCI_PRODUCT_SYMBIOS_895	0x000c		/* 53c895 */
#define	PCI_PRODUCT_SYMBIOS_885	0x000d		/* 53c885 */
#define	PCI_PRODUCT_SYMBIOS_875	0x000f		/* 53c875 */
#define	PCI_PRODUCT_SYMBIOS_1510	0x0010		/* 53c1510 */
#define	PCI_PRODUCT_SYMBIOS_895A	0x0012		/* 53c895A */
#define	PCI_PRODUCT_SYMBIOS_1010	0x0020		/* 53c1010 */
#define	PCI_PRODUCT_SYMBIOS_1010R	0x0021		/* 53c1010R */
#define	PCI_PRODUCT_SYMBIOS_875J	0x008f		/* 53c875J */
//wan+ if
#define PCI_PRODUCT_SYMBIOS_SAS1064     0x0050      /* SAS1064 */
#define PCI_PRODUCT_SYMBIOS_SAS1068     0x0054      /* SAS1068 */
#define PCI_PRODUCT_SYMBIOS_SAS1068_2   0x0055      /* SAS1068 */
#define PCI_PRODUCT_SYMBIOS_SAS1064E    0x0056      /* SAS1064E */
#define PCI_PRODUCT_SYMBIOS_SAS1064E_2  0x0057      /* SAS1064E */
#define PCI_PRODUCT_SYMBIOS_SAS1068E    0x0058      /* SAS1068E */
#define PCI_PRODUCT_SYMBIOS_SAS1068E_2  0x0059      /* SAS1068E */
#define PCI_PRODUCT_SYMBIOS_SAS1066E    0x005a      /* SAS1066E */
#define PCI_PRODUCT_SYMBIOS_SAS1064A    0x005c      /* SAS1064A */
#define PCI_PRODUCT_SYMBIOS_SAS1066     0x005e      /* SAS1066 */
#define PCI_PRODUCT_SYMBIOS_SAS1078     0x0060      /* SAS1078 wan+*/
#define PCI_PRODUCT_SYMBIOS_SAS1078_PCIE 0x0062     /* SAS1078 */
#define PCI_PRODUCT_SYMBIOS_SAS1078DE   0x007c      /* SAS1078DE wan+*/
#define PCI_PRODUCT_SYMBIOS_875J        0x008f      /* 53c875J */
#define PCI_PRODUCT_SYMBIOS_MEGARAID_320    0x0407      /* MegaRAID 320 */
#define PCI_PRODUCT_SYMBIOS_MEGARAID_3202E  0x0408      /* MegaRAID 320-2E */
#define PCI_PRODUCT_SYMBIOS_MEGARAID_SATA   0x0409      /* MegaRAID SATA 4x/8x */
#define PCI_PRODUCT_SYMBIOS_MEGARAID_SAS    0x0411      /* MegaRAID SAS 1064R wan+*/
#define PCI_PRODUCT_SYMBIOS_MEGARAID_VERDE_ZCR  0x0413  /* MegaRAID Verde ZCR wan+*/
#define PCI_PRODUCT_SYMBIOS_MEGARAID    0x1960      /* MegaRAID */
#define PCI_PRODUCT_SYMBIOS_SAS2004     0x0070      /* SAS2004 */
#define PCI_PRODUCT_SYMBIOS_SAS2008     0x0072      /* SAS2008 */
#define PCI_PRODUCT_SYMBIOS_SAS2108_1   0x0078      /* MegaRAID SAS2108 CRYPTO GEN2 wan+*/
#define PCI_PRODUCT_SYMBIOS_SAS2108_2   0x0079      /* MegaRAID SAS2108 GEN2 wan+*/
#define PCI_PRODUCT_SYMBIOS_TBOLT   	0x005F     /* MegaRAID SAS2108 GEN2 wan+*/
//wan+ end

/* Packet Engines products */
#define	PCI_PRODUCT_SYMBIOS_PE_GNIC	0x0702		/* Packet Engines G-NIC */

/* NexGen products */
#define	PCI_PRODUCT_NEXGEN_NX82C501	0x4e78		/* NX82C501 Host-PCI */

/* NKK products */
#define	PCI_PRODUCT_NKK_NDR4600	0xa001		/* NDR4600 Host-PCI */

/* Nortel Networks products */
#define	PCI_PRODUCT_NORTEL_BS21	0x1211		/* BS21 10/100 */

/* NVidia products */
#define	PCI_PRODUCT_NVIDIA_NV1	0x0008		/* NV1 */
#define	PCI_PRODUCT_NVIDIA_DAC64	0x0009		/* DAC64 */
#define	PCI_PRODUCT_NVIDIA_RIVA_TNT	0x0020		/* Riva TNT */
#define	PCI_PRODUCT_NVIDIA_RIVA_TNT2	0x0028		/* Riva TNT2 */
#define	PCI_PRODUCT_NVIDIA_RIVA_TNT2_ULTRA	0x0029		/* Riva TNT2 Ultra */
#define	PCI_PRODUCT_NVIDIA_VANTA1	0x002c		/* Vanta */
#define	PCI_PRODUCT_NVIDIA_VANTA2	0x002d		/* Vanta */
#define	PCI_PRODUCT_NVIDIA_GEFORCE256	0x0100		/* GeForce256 */
#define	PCI_PRODUCT_NVIDIA_GEFORCE256_DDR	0x0101		/* GeForce256 DDR */
#define	PCI_PRODUCT_NVIDIA_GEFORCE2MX	0x0110		/* GeForce2 MX */
#define	PCI_PRODUCT_NVIDIA_GEFORCE2GO	0x0112		/* GeForce2 Go */
#define	PCI_PRODUCT_NVIDIA_GEFORCE2GTS	0x0150		/* GeForce2 GTS */
#define	PCI_PRODUCT_NVIDIA_GEFORCE2ULTRA	0x0152		/* GeForce2 Ultra */

/* Oak Technologies products */
#define	PCI_PRODUCT_OAKTECH_OTI1007	0x0107		/* OTI107 */

/* Opti products */
#define	PCI_PRODUCT_OPTI_82C557	0xc557		/* 82C557 Host */
#define	PCI_PRODUCT_OPTI_82C558	0xc558		/* 82C558 PCI-ISA */
#define	PCI_PRODUCT_OPTI_82C568	0xc568		/* 82C568 IDE */
#define	PCI_PRODUCT_OPTI_82D568	0xd568		/* 82D568 IDE */
#define	PCI_PRODUCT_OPTI_82C621	0xc621		/* 82C621 IDE */
#define	PCI_PRODUCT_OPTI_82C700	0xc700		/* 82C700 */
#define	PCI_PRODUCT_OPTI_82C701	0xc701		/* 82C701 */
#define	PCI_PRODUCT_OPTI_82C822	0xc822		/* 82C822 */
#define	PCI_PRODUCT_OPTI_RM861HA	0xc861		/* RM861HA */

/* Ross -> Pequr -> ServerWorks -> Broadcom products */
#define	PCI_PRODUCT_RCC_ROSB4	0x0200		/* ROSB4 SouthBridge */
#define	PCI_PRODUCT_RCC_XX5	0x0005		/* PCIHB5 */
#define	PCI_PRODUCT_RCC_CIOB20	0x0006		/* I/O Bridge */
#define	PCI_PRODUCT_RCC_XX7	0x0007		/* PCIHB7 */
#define	PCI_PRODUCT_RCC_CNB20HE	0x0008		/* CNB20HE Host */
#define	PCI_PRODUCT_RCC_CNB20LE	0x0009		/* CNB20LE Host */
#define	PCI_PRODUCT_RCC_CIOB30	0x0010		/* CIOB30 */
#define	PCI_PRODUCT_RCC_CMIC_HE	0x0011		/* CMIC_HE Host */
#define	PCI_PRODUCT_RCC_IDE	0x0211		/* IDE */
#define	PCI_PRODUCT_RCC_USB	0x0220		/* USB */

/* PLX products */
#define	PCI_PRODUCT_PLX_1076	0x1076		/* I/O */
#define	PCI_PRODUCT_PLX_9050	0x9050		/* I/O 9050 */
#define	PCI_PRODUCT_PLX_9080	0x9080		/* I/O 9080 */

/* Promise products */
#define	PCI_PRODUCT_PROMISE_DC5030	0x5300		/* DC5030 */
#define	PCI_PRODUCT_PROMISE_PDC20246	0x4d33		/* PDC20246 */
#define	PCI_PRODUCT_PROMISE_PDC20262	0x4d38		/* PDC20262 */
#define	PCI_PRODUCT_PROMISE_PDC20265	0x0d30		/* PDC20265 */
#define	PCI_PRODUCT_PROMISE_PDC20267	0x4d30		/* PDC20267 */
#define	PCI_PRODUCT_PROMISE_PDC20268	0x4d68		/* PDC20268 */
#define	PCI_PRODUCT_PROMISE_PDC20268R	0x6268		/* PDC20268R */
#define	PCI_PRODUCT_PROMISE_PDC20269	0x4d69		/* PDC20268 */

/* QLogic products */
#define	PCI_PRODUCT_QLOGIC_ISP1020	0x1020		/* ISP1020 */
#define	PCI_PRODUCT_QLOGIC_ISP1022	0x1022		/* ISP1022 */
#define	PCI_PRODUCT_QLOGIC_ISP1080	0x1080		/* ISP1080 */
#define	PCI_PRODUCT_QLOGIC_ISP1240	0x1240		/* ISP1240 */
#define	PCI_PRODUCT_QLOGIC_ISP1280	0x1280		/* ISP1280 */
#define	PCI_PRODUCT_QLOGIC_ISP12160	0x1216		/* ISP12160 */
#define	PCI_PRODUCT_QLOGIC_ISP10160	0x1016		/* ISP12160 */
#define	PCI_PRODUCT_QLOGIC_ISP2100	0x2100		/* ISP2100 */
#define	PCI_PRODUCT_QLOGIC_ISP2200	0x2200		/* ISP2200 */
#define	PCI_PRODUCT_QLOGIC_ISP2300	0x2300		/* ISP2300 */

/* Realtek (Creative Labs?) products */
#define	PCI_PRODUCT_REALTEK_RT8029	0x8029		/* 8029 */
#define	PCI_PRODUCT_REALTEK_RT8129	0x8129		/* 8129 */
#define	PCI_PRODUCT_REALTEK_RT8139	0x8139		/* 8139 */

/* RICOH products */
#define	PCI_PRODUCT_RICOH_RF5C465	0x0465		/* 5C465 PCI-CardBus */
#define	PCI_PRODUCT_RICOH_RF5C466	0x0466		/* 5C466 PCI-CardBus */
#define	PCI_PRODUCT_RICOH_RF5C475	0x0475		/* 5C475 PCI-CardBus */
#define	PCI_PRODUCT_RICOH_RF5C476	0x0476		/* 5C476 PCI-CardBus */
#define	PCI_PRODUCT_RICOH_RF5C477	0x0477		/* 5C477 PCI-CardBus */
#define	PCI_PRODUCT_RICOH_RF5C478	0x0478		/* 5C478 PCI-CardBus */

/* Rockwell products */
#define	PCI_PRODUCT_ROCKWELL_RS56SP_PCI11P1	0x2005		/* RS56/SP-PCI11P1 Modem */

/* S3 products */
#define	PCI_PRODUCT_S3_VIRGE	0x5631		/* ViRGE */
#define	PCI_PRODUCT_S3_TRIO32	0x8810		/* Trio32 */
#define	PCI_PRODUCT_S3_TRIO64	0x8811		/* Trio32/64 */
#define	PCI_PRODUCT_S3_AURORA64P	0x8812		/* Aurora64V+ */
#define	PCI_PRODUCT_S3_TRIO64UVP	0x8814		/* Trio64UV+ */
#define	PCI_PRODUCT_S3_868	0x8880		/* 868 */
#define	PCI_PRODUCT_S3_VIRGE_VX	0x883d		/* ViRGE VX */
#define	PCI_PRODUCT_S3_928	0x88b0		/* 86C928 */
#define	PCI_PRODUCT_S3_864_0	0x88c0		/* 86C864-0 (Vision864) */
#define	PCI_PRODUCT_S3_864_1	0x88c1		/* 86C864-1 (Vision864) */
#define	PCI_PRODUCT_S3_864_2	0x88c2		/* 86C864-2 (Vision864) */
#define	PCI_PRODUCT_S3_864_3	0x88c3		/* 86C864-3 (Vision864) */
#define	PCI_PRODUCT_S3_964_0	0x88d0		/* 86C964-0 (Vision964) */
#define	PCI_PRODUCT_S3_964_1	0x88d1		/* 86C964-1 (Vision964) */
#define	PCI_PRODUCT_S3_964_2	0x88d2		/* 86C964-2 (Vision964) */
#define	PCI_PRODUCT_S3_964_3	0x88d1		/* 86C964-3 (Vision964) */
#define	PCI_PRODUCT_S3_968_0	0x88f0		/* 86C968-0 (Vision968) */
#define	PCI_PRODUCT_S3_968_1	0x88f1		/* 86C968-1 (Vision968) */
#define	PCI_PRODUCT_S3_968_2	0x88f2		/* 86C968-2 (Vision968) */
#define	PCI_PRODUCT_S3_968_3	0x88f3		/* 86C968-3 (Vision968) */
#define	PCI_PRODUCT_S3_TRIO64V2_DX	0x8901		/* Trio64V2/DX */
#define	PCI_PRODUCT_S3_PLATO	0x8902		/* Plato */
#define	PCI_PRODUCT_S3_TRIO3D_AGP	0x8904		/* Trio3D AGP */
#define	PCI_PRODUCT_S3_VIRGE_DX_GX	0x8a01		/* ViRGE DX/GX */
#define	PCI_PRODUCT_S3_VIRGE_GX2	0x8a10		/* ViRGE GX2 */
#define	PCI_PRODUCT_S3_TRIO3_DX2	0x8a13		/* Trio3 DX2 */
#define	PCI_PRODUCT_S3_SAVAGE3D	0x8a20		/* Savage 3D */
#define	PCI_PRODUCT_S3_SAVAGE3D_M	0x8a21		/* Savage 3DM */
#define	PCI_PRODUCT_S3_SAVAGE4	0x8a22		/* Savage 4 */
#define	PCI_PRODUCT_S3_VIRGE_MX	0x8c01		/* ViRGE MX */
#define	PCI_PRODUCT_S3_VIRGE_MXP	0x8c03		/* ViRGE MXP */
#define	PCI_PRODUCT_S3_SAVAGE_MXMV	0x8c10		/* Savage/MX-MV */
#define	PCI_PRODUCT_S3_SAVAGE_MX	0x8c11		/* Savage/MX */
#define	PCI_PRODUCT_S3_SAVAGE_IXMV	0x8c12		/* Savage/IX-MV */
#define	PCI_PRODUCT_S3_SAVAGE_IX	0x8c13		/* Savage/IX */
#define	PCI_PRODUCT_S3_SUPERSAVAGE	0x8c2e		/* SuperSavage */
#define	PCI_PRODUCT_S3_SONICVIBES	0xca00		/* SonicVibes */

/* Sony products */
#define	PCI_PRODUCT_SONY_CXD1947A	0x8009		/* CXD1947A FireWire */
#define	PCI_PRODUCT_SONY_CXD3222	0x8039		/* CXD3222 FireWire */
#define	PCI_PRODUCT_SONY_MEMSTICK_SLOT	0x808a		/* Memory Stick Slot */

/* STB products */
#define	PCI_PRODUCT_STB2_RIVA128	0x0018		/* Velocity128 */

/* Symphony Labs/Winbond products */
#define	PCI_PRODUCT_SYMPHONY_W83C553F	0x0565		/* W83C553F PCI-ISA */

/* Tekram Technology products (1st ID)*/
#define	PCI_PRODUCT_TEKRAM_DC290	0xdc29		/* DC-290(M) */

/* Tekram Technology products(2) */
#define	PCI_PRODUCT_TEKRAM2_DC690C	0x690c		/* DC-690C */

/* Texas Instruments products */
#define	PCI_PRODUCT_TI_TSB12LV22	0x8009		/* TSB12LV22 FireWire */
#define	PCI_PRODUCT_TI_TSB12LV23	0x8019		/* TSB12LV23 FireWire */
#define	PCI_PRODUCT_TI_TSB12LV26	0x8020		/* TSB12LV26 FireWire */
#define	PCI_PRODUCT_TI_PCI4451_FW	0x8027		/* PCI4451 OHCI FireWire */
#define	PCI_PRODUCT_TI_PCI1130	0xac12		/* PCI1130 PCI-CardBus */
#define	PCI_PRODUCT_TI_PCI1031	0xac13		/* PCI1031 PCI-pcmcia */
#define	PCI_PRODUCT_TI_PCI1131	0xac15		/* PCI1131 PCI-CardBus */
#define	PCI_PRODUCT_TI_PCI1250	0xac16		/* PCI1250 PCI-CardBus */
#define	PCI_PRODUCT_TI_PCI1220	0xac17		/* PCI1220 PCI-CardBus */
#define	PCI_PRODUCT_TI_PCI1221	0xac19		/* PCI1221 PCI-CardBus */
#define	PCI_PRODUCT_TI_PCI1450	0xac1b		/* PCI1450 PCI-CardBus */
#define	PCI_PRODUCT_TI_PCI1225	0xac1c		/* PCI1225 PCI-CardBus */
#define	PCI_PRODUCT_TI_PCI1251	0xac1d		/* PCI1251 PCI-CardBus */
#define	PCI_PRODUCT_TI_PCI1211	0xac1e		/* PCI1211 PCI-CardBus */
#define	PCI_PRODUCT_TI_PCI1251B	0xac1f		/* PCI1251B PCI-CardBus */
#define	PCI_PRODUCT_TI_PCI2030	0xac20		/* PCI2030 PCI-PCI */
#define	PCI_PRODUCT_TI_PCI2031	0xac21		/* PCI2031 PCI-PCI */
#define	PCI_PRODUCT_TI_PCI4451_CB	0xac42		/* PCI4451 PCI-CardBus */
#define	PCI_PRODUCT_TI_PCI1410	0xac50		/* PCI1410 PCI-CardBus */
#define	PCI_PRODUCT_TI_PCI1420	0xac51		/* PCI1420 PCI-CardBus */
#define	PCI_PRODUCT_TI_PCI1451	0xac52		/* PCI1451 PCI-CardBus */

/* Toshiba products */
#define	PCI_PRODUCT_TOSHIBA_R4x00	0x0009		/* R4x00 */
#define	PCI_PRODUCT_TOSHIBA_TC35856F	0x0020		/* TC35856F ATM (Meteor) */
#define	PCI_PRODUCT_TOSHIBA_R4X00	0x102f		/* R4x00 Host-PCI */

/* Toshiba(2) products */
#define	PCI_PRODUCT_TOSHIBA2_THB	0x0601		/* Host-PCI */
#define	PCI_PRODUCT_TOSHIBA2_ISA	0x0602		/* PCI-ISA */
#define	PCI_PRODUCT_TOSHIBA2_ToPIC95	0x0603		/* ToPIC95 CardBus-PCI */
#define	PCI_PRODUCT_TOSHIBA2_ToPIC95B	0x060a		/* ToPIC95B PCI-CardBus */
#define	PCI_PRODUCT_TOSHIBA2_ToPIC97	0x060f		/* ToPIC97 PCI-CardBus */
#define	PCI_PRODUCT_TOSHIBA2_ToPIC100	0x0617		/* ToPIC100 PCI-CardBus */
#define	PCI_PRODUCT_TOSHIBA2_TFIRO	0x0701		/* Fast Infrared Type O */

/* Transmeta products */
#define	PCI_PRODUCT_TRANSMETA_NORTHBRIDGE	0x0395		/* Virtual Northbridge */
#define	PCI_PRODUCT_TRANSMETA_MEM1	0x0396		/* Mem1 */
#define	PCI_PRODUCT_TRANSMETA_MEM2	0x0397		/* Mem2 */

/* Trident products */
#define	PCI_PRODUCT_TRIDENT_4DWAVE_DX	0x2000		/* 4DWAVE DX */
#define	PCI_PRODUCT_TRIDENT_4DWAVE_NX	0x2001		/* 4DWAVE NX */
#define	PCI_PRODUCT_TRIDENT_CYBERBLADEI7	0x8400		/* CyberBlade i7 */
#define	PCI_PRODUCT_TRIDENT_CYBERBLADEI7AGP	0x8420		/* CyberBlade i7 AGP */
#define	PCI_PRODUCT_TRIDENT_CYBERBLADEI1	0x8500		/* CyberBlade i1 */
#define	PCI_PRODUCT_TRIDENT_TGUI_9320	0x9320		/* TGUI 9320 */
#define	PCI_PRODUCT_TRIDENT_TGUI_9350	0x9350		/* TGUI 9350 */
#define	PCI_PRODUCT_TRIDENT_TGUI_9360	0x9360		/* TGUI 9360 */
#define	PCI_PRODUCT_TRIDENT_TGUI_9388	0x9388		/* TGUI 9388 */
#define	PCI_PRODUCT_TRIDENT_CYBER_9397	0x9397		/* CYBER 9397 */
#define	PCI_PRODUCT_TRIDENT_CYBER_9397DVD	0x939a		/* CYBER 9397DVD */
#define	PCI_PRODUCT_TRIDENT_TGUI_9420	0x9420		/* TGUI 9420 */
#define	PCI_PRODUCT_TRIDENT_TGUI_9440	0x9440		/* TGUI 9440 */
#define	PCI_PRODUCT_TRIDENT_CYBER_9525	0x9525		/* CYBER 9525 */
#define	PCI_PRODUCT_TRIDENT_TGUI_9660	0x9660		/* TGUI 9660 */
#define	PCI_PRODUCT_TRIDENT_TGUI_9680	0x9680		/* TGUI 9680 */
#define	PCI_PRODUCT_TRIDENT_TGUI_9682	0x9682		/* TGUI 9682 */
#define	PCI_PRODUCT_TRIDENT_3DIMAGE_9750	0x9750		/* 3DImage 9750 */
#define	PCI_PRODUCT_TRIDENT_3DIMAGE_9850	0x9850		/* 3DImage 9850 */
#define	PCI_PRODUCT_TRIDENT_BLADE_3D	0x9880		/* Blade 3D */

/* Tseng Labs products */
#define	PCI_PRODUCT_TSENG_ET4000_W32P_A	0x3202		/* ET4000w32p rev A */
#define	PCI_PRODUCT_TSENG_ET4000_W32P_B	0x3205		/* ET4000w32p rev B */
#define	PCI_PRODUCT_TSENG_ET4000_W32P_C	0x3206		/* ET4000w32p rev C */
#define	PCI_PRODUCT_TSENG_ET4000_W32P_D	0x3207		/* ET4000w32p rev D */
#define	PCI_PRODUCT_TSENG_ET6000	0x3208		/* ET6000/ET6100 */
#define	PCI_PRODUCT_TSENG_ET6300	0x4702		/* ET6300 */

/* US Rebotics */
#define	PCI_PRODUCT_USR_3CP5610	0x1008		/* 3CP5610 */

/* VMware */
#define	PCI_PRODUCT_VMWARE_VIRTUAL	0x0710		/* Virtual */

/* Winbond Electronics products */
#define	PCI_PRODUCT_WINBOND_W83769F	0x0001		/* W83769F */
#define	PCI_PRODUCT_WINBOND_W89C840F	0x0840		/* W89C840F 10/100 */
#define	PCI_PRODUCT_WINBOND_W89C940F	0x0940		/* Linksys EtherPCI II */

/* Winbond Electronics products (PCI products set 2) */
#define	PCI_PRODUCT_WINBOND2_W89C940	0x1980		/* Linksys EtherPCI */

/* Motorola products */
#define	PCI_PRODUCT_MOT_MPC105	0x0001		/* MPC105 PCI bridge */
#define	PCI_PRODUCT_MOT_MPC106	0x0002		/* MPC106 PCI bridge */

/* Xircom products */
#define	PCI_PRODUCT_XIRCOM_X3201_3	0x0002		/* X3201-3 */
#define	PCI_PRODUCT_XIRCOM_X3201_3_21143	0x0003		/* X3201-3 (21143) */
#define	PCI_PRODUCT_XIRCOM_MPCI_MODEM	0x000c		/* MiniPCI Modem */
