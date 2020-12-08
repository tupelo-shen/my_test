/*	$OpenBSD: mii.h,v 1.1 1998/09/10 17:17:33 jason Exp $	*/
/*	$NetBSD: mii.h,v 1.1 1998/08/10 23:55:17 thorpej Exp $	*/
 
/*
 * Copyright (c) 1997 Manuel Bouyer.  All rights reserved.
 *
 * Modification to match BSD/OS 3.0 MII interface by Jason R. Thorpe,
 * Numerical Aerospace Simulation Facility, NASA Ames Research Center.
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
 *	This product includes software developed by Manuel Bouyer.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
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

#ifndef _DEV_MII_MII_H_
#define	_DEV_MII_MII_H_

/*
 * Registers common to all PHYs.
 */

#define	MII_NPHY	32	/* max # of PHYs per MII */

/*
 * MII commands, used if a device must drive the MII lines
 * manually.
 */
#define	MII_COMMAND_START	0x01
#define	MII_COMMAND_READ	0x02
#define	MII_COMMAND_WRITE	0x01
#define	MII_COMMAND_ACK		0x02

#define MII_BMCR            0x00        /* Basic mode control register */
#define MII_BMSR            0x01        /* Basic mode status register  */
#define MII_PHYSID1         0x02        /* PHYS ID 1                   */
#define MII_PHYSID2         0x03        /* PHYS ID 2                   */
#define MII_ADVERTISE       0x04        /* Advertisement control reg   */
#define MII_LPA             0x05        /* Link partner ability reg    */
#define MII_EXPANSION       0x06        /* Expansion register          */
#define MII_CTRL1000        0x09        /* 1000BASE-T control          */
#define MII_STAT1000        0x0a        /* 1000BASE-T status           */
#define MII_ESTATUS	    0x0f	/* Extended Status */
#define MII_DCOUNTER        0x12        /* Disconnect counter          */
#define MII_FCSCOUNTER      0x13        /* False carrier counter       */
#define MII_NWAYTEST        0x14        /* N-way auto-neg test reg     */
#define MII_RERRCOUNTER     0x15        /* Receive error counter       */
#define MII_SREVISION       0x16        /* Silicon revision            */
#define MII_RESV1           0x17        /* Reserved...                 */
#define MII_LBRERROR        0x18        /* Lpback, rx, bypass error    */
#define MII_PHYADDR         0x19        /* PHY address                 */
#define MII_RESV2           0x1a        /* Reserved...                 */
#define MII_TPISTATUS       0x1b        /* TPI status for 10mbps       */
#define MII_NCONFIG         0x1c        /* Network interface config    */

//wan #if 0
#define BMSR_EXTSTAT   0x0100  /* Extended status in register 15 */
#define BMCR_SPEED0 0x2000     /* speed selection (LSB) */
#define BMCR_SPEED1 0x0040     /* speed selection (MSB) */
#define BMCR_S10 0x0000                /* 10 Mb/s */
#define BMCR_S100      BMCR_SPEED0     /* 100 Mb/s */
#define BMCR_S1000     BMCR_SPEED1     /* 1000 Mb/s */
               /* This is also the 1000baseT control register */
#define MII_100T2CR 0x09       /* 100base-T2 control register */
#define GTCR_TEST_MASK 0xe000  /* see 802.3ab ss. 40.6.1.1.2 */
#define GTCR_MAN_MS 0x1000     /* enable manual master/slave control */
#define GTCR_ADV_MS 0x0800     /* 1 = adv. master, 0 = adv. slave */
#define GTCR_PORT_TYPE 0x0400  /* 1 = DCE, 0 = DTE (NIC) */
#define GTCR_ADV_1000TFDX 0x0200 /* adv. 1000baseT FDX */
#define GTCR_ADV_1000THDX 0x0100 /* adv. 1000baseT HDX */

           /* This is also the 1000baseT status register */
#define MII_100T2SR 0x0a       /* 100base-T2 status register */
#define GTSR_MAN_MS_FLT 0x8000 /* master/slave config fault */
#define GTSR_MS_RES 0x4000     /* result: 1 = master, 0 = slave */
#define GTSR_LRS    0x2000     /* local rx status, 1 = ok */
#define GTSR_RRS    0x1000     /* remove rx status, 1 = ok */
#define GTSR_LP_1000TFDX 0x0800        /* link partner 1000baseT FDX capable */
#define GTSR_LP_1000THDX 0x0400        /* link partner 1000baseT HDX capable */
#define GTSR_LP_ASM_DIR 0x0200 /* link partner asym. pause dir. capable */
#define GTSR_IDLE_ERR   0x00ff /* IDLE error count */

#define MII_EXTSR     0x0f     /* Extended status register */
#define EXTSR_1000XFDX  0x8000 /* 1000X full-duplex capable */
#define EXTSR_1000XHDX  0x4000 /* 1000X half-duplex capable */
#define EXTSR_1000TFDX  0x2000 /* 1000T full-duplex capable */
#define EXTSR_1000THDX  0x1000 /* 1000T half-duplex capable */

#define EXTSR_MEDIAMASK (EXTSR_1000XFDX|EXTSR_1000XHDX| \
                        EXTSR_1000TFDX|EXTSR_1000THDX)
//wan #endif

#define	MII_BMCR	0x00 	/* Basic mode control register (rw) */
#define	BMCR_RESET	0x8000	/* reset */
#define	BMCR_LOOP	0x4000	/* loopback */
//wan #define	BMCR_S100	0x2000	/* speed (10/100) select */
#define	BMCR_AUTOEN	0x1000	/* autonegotiation enable */
#define	BMCR_PDOWN	0x0800	/* power down */
#define	BMCR_ISO	0x0400	/* isolate */
#define	BMCR_STARTNEG	0x0200	/* restart autonegotiation */
#define	BMCR_FDX	0x0100	/* Set duplex mode */
#define	BMCR_CTEST	0x0080	/* collision test */

#define	MII_BMSR	0x01	/* Basic mode status register (ro) */
#define	BMSR_100T4	0x8000	/* 100 base T4 capable */
#define	BMSR_100TXFDX	0x4000	/* 100 base Tx full duplex capable */
#define	BMSR_100TXHDX	0x2000	/* 100 base Tx half duplex capable */
#define	BMSR_10TFDX	0x1000	/* 10 base T full duplex capable */
#define	BMSR_10THDX	0x0800	/* 10 base T half duplex capable */
#define	BMSR_ACOMP	0x0020	/* Autonegotiation complete */
#define	BMSR_RFAULT	0x0010	/* Link partner fault */
#define	BMSR_ANEG	0x0008	/* Autonegotiation capable */
#define	BMSR_LINK	0x0004	/* Link status */
#define	BMSR_JABBER	0x0002	/* Jabber detected */
#define	BMSR_EXT	0x0001	/* Extended capability */

#define	BMSR_MEDIAMASK	(BMSR_100T4|BMSR_100TXFDX|BMSR_100TXHDX|BMSR_10TFDX| \
			 BMSR_10THDX|BMSR_ANEG)

/*
 * Convert BMSR media capabilities to ANAR bits for autonegotiation.
 * Note the shift chopps off the BMSR_ANEG bit.
 */
#define	BMSR_MEDIA_TO_ANAR(x)	(((x) & BMSR_MEDIAMASK) >> 6)

#define	MII_PHYIDR1	0x02	/* ID register 1 (ro) */

#define	MII_PHYIDR2	0x03	/* ID register 2 (ro) */
#define	IDR2_OUILSB	0xfc00	/* OUI LSB */
#define	IDR2_MODEL	0x03f0	/* vendor model */
#define	IDR2_REV	0x000f	/* vendor revision */

#define	MII_OUI(id1, id2)	(((id1) << 6) | ((id2) >> 10))
#define	MII_MODEL(id2)		(((id2) & IDR2_MODEL) >> 4)
#define	MII_REV(id2)		((id2) & IDR2_REV)

#define	MII_ANAR	0x04	/* Autonegotiation advertisement (rw) */
#define ANAR_NP		0x8000	/* Next page (ro) */
#define	ANAR_ACK	0x4000	/* link partner abilities acknowledged (ro) */
#define ANAR_RF		0x2000	/* remote fault (ro) */
#define ANAR_T4		0x0200	/* local device supports 100bT4 */
#define ANAR_TX_FD	0x0100	/* local device supports 100bTx FD */
#define ANAR_TX		0x0080	/* local device supports 100bTx */
#define ANAR_10_FD	0x0040	/* local device supports 10bT FD */
#define ANAR_10		0x0020	/* local device supports 10bT */
#define	ANAR_CSMA	0x0001	/* protocol selector CSMA/CD */

#define	MII_ANLPAR	0x05	/* Autonegotiation lnk partner abilities (rw) */
#define ANLPAR_NP	0x8000	/* Next page (ro) */
#define	ANLPAR_ACK	0x4000	/* link partner accepted ACK (ro) */
#define ANLPAR_RF	0x2000	/* remote fault (ro) */
#define ANLPAR_T4	0x0200	/* link partner supports 100bT4 */
#define ANLPAR_TX_FD	0x0100	/* link partner supports 100bTx FD */
#define ANLPAR_TX	0x0080	/* link partner supports 100bTx */
#define ANLPAR_10_FD	0x0040	/* link partner supports 10bT FD */
#define ANLPAR_10	0x0020	/* link partner supports 10bT */
#define	ANLPAR_CSMA	0x0001	/* protocol selector CSMA/CD */

//wan #if 0
#define ANLPAR_PAUSE_SYM       (1 << 10)
#define ANLPAR_PAUSE_TOWARDS   (3 << 10)
#define ANLPAR_PAUSE_NONE      (0 << 10)
#define ANLPAR_PAUSE_ASYM      (2 << 10)

#define ANAR_FC     0x0400     /* local device supports PAUSE */

#define ANAR_PAUSE_NONE     (0 << 10)
#define ANAR_PAUSE_SYM      (1 << 10)
#define ANAR_PAUSE_ASYM     (2 << 10)
#define ANAR_PAUSE_TOWARDS  (3 << 10)

#define ANAR_X_FD   0x0020     /* local device supports 1000BASE-X FD */
#define ANAR_X_HD   0x0040     /* local device supports 1000BASE-X HD */
#define ANAR_X_PAUSE_NONE      (0 << 7)
#define ANAR_X_PAUSE_SYM       (1 << 7)
#define ANAR_X_PAUSE_ASYM      (2 << 7)
#define ANAR_X_PAUSE_TOWARDS   (3 << 7)
//wan #endif

#define	MII_ANER	0x06	/* Autonegotiation expansion (ro) */
#define ANER_MLF	0x0010	/* multiple link detection fault */
#define ANER_LPNP	0x0008	/* link parter next page-able */
#define ANER_NP		0x0004	/* next page-able */
#define ANER_PAGE_RX	0x0002	/* Page received */
#define ANER_LPAN	0x0001	/* link parter autoneg-able */

#define        MII_ANNP        0x07    /* Autonegotiation next page */
               /* section 28.2.4.1 and 37.2.6.1 */

#define        MII_ANLPRNP     0x08    /* Autonegotiation link partner rx next page */
               /* section 32.5.1 and 37.2.6.1 */

/* Basic mode control register. */
#define BMCR_RESV               0x003f  /* Unused...                   */
#define BMCR_SPEED1000		0x0040  /* MSB of Speed (1000)         */
#define BMCR_CTST               0x0080  /* Collision test              */
#define BMCR_FULLDPLX           0x0100  /* Full duplex                 */
#define BMCR_ANRESTART          0x0200  /* Auto negotiation restart    */
#define BMCR_ISOLATE            0x0400  /* Disconnect DP83840 from MII */
#define BMCR_PDOWN              0x0800  /* Powerdown the DP83840       */
#define BMCR_ANENABLE           0x1000  /* Enable auto negotiation     */
#define BMCR_SPEED100           0x2000  /* Select 100Mbps              */
#define BMCR_LOOPBACK           0x4000  /* TXD loopback bits           */
#define BMCR_RESET              0x8000  /* Reset the DP83840           */

/* Basic mode status register. */
#define BMSR_ERCAP              0x0001  /* Ext-reg capability          */
#define BMSR_JCD                0x0002  /* Jabber detected             */
#define BMSR_LSTATUS            0x0004  /* Link status                 */
#define BMSR_ANEGCAPABLE        0x0008  /* Able to do auto-negotiation */
#define BMSR_RFAULT             0x0010  /* Remote fault detected       */
#define BMSR_ANEGCOMPLETE       0x0020  /* Auto-negotiation complete   */
#define BMSR_RESV               0x00c0  /* Unused...                   */
#define BMSR_ESTATEN		0x0100	/* Extended Status in R15 */
#define BMSR_100HALF2           0x0200  /* Can do 100BASE-T2 HDX */
#define BMSR_100FULL2           0x0400  /* Can do 100BASE-T2 FDX */
#define BMSR_10HALF             0x0800  /* Can do 10mbps, half-duplex  */
#define BMSR_10FULL             0x1000  /* Can do 10mbps, full-duplex  */
#define BMSR_100HALF            0x2000  /* Can do 100mbps, half-duplex */
#define BMSR_100FULL            0x4000  /* Can do 100mbps, full-duplex */
#define BMSR_100BASE4           0x8000  /* Can do 100mbps, 4k packets  */

/* Advertisement control register. */
#define ADVERTISE_SLCT          0x001f  /* Selector bits               */
#define ADVERTISE_CSMA          0x0001  /* Only selector supported     */
#define ADVERTISE_10HALF        0x0020  /* Try for 10mbps half-duplex  */
#define ADVERTISE_1000XFULL     0x0020  /* Try for 1000BASE-X full-duplex */
#define ADVERTISE_10FULL        0x0040  /* Try for 10mbps full-duplex  */
#define ADVERTISE_1000XHALF     0x0040  /* Try for 1000BASE-X half-duplex */
#define ADVERTISE_100HALF       0x0080  /* Try for 100mbps half-duplex */
#define ADVERTISE_1000XPAUSE    0x0080  /* Try for 1000BASE-X pause    */
#define ADVERTISE_100FULL       0x0100  /* Try for 100mbps full-duplex */
#define ADVERTISE_1000XPSE_ASYM 0x0100  /* Try for 1000BASE-X asym pause */
#define ADVERTISE_100BASE4      0x0200  /* Try for 100mbps 4k packets  */
#define ADVERTISE_PAUSE_CAP     0x0400  /* Try for pause               */
#define ADVERTISE_PAUSE_ASYM    0x0800  /* Try for asymetric pause     */
#define ADVERTISE_RESV          0x1000  /* Unused...                   */
#define ADVERTISE_RFAULT        0x2000  /* Say we can detect faults    */
#define ADVERTISE_LPACK         0x4000  /* Ack link partners response  */
#define ADVERTISE_NPAGE         0x8000  /* Next page bit               */

/* 1000BASE-T Control register */
#define ADVERTISE_1000FULL      0x0200  /* Advertise 1000BASE-T full duplex */
#define ADVERTISE_1000HALF      0x0100  /* Advertise 1000BASE-T half duplex */

#endif /* _DEV_MII_MII_H_ */
