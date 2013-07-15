/**
 * @file
 * Ethernet Interface for eLua
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author of sceleton: Adam Dunkels <adam@sics.se>
 *
 * Edited for eLua: Lukas Kacer, 2012 05. 05
 * TODO: replace parts, that are actually using HW with calls platform_eth_*
 */

#include "platform_conf.h"
#ifdef BUILD_LWIP

#include "lwip/opt.h"

#include "elua_lwIP.h"
#include <string.h>
#include "platform/at91sam7x/ethernet/emac.h"

#include "lwip/init.h"
#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include <lwip/stats.h>

#include "lwip/dhcp.h"
#include "lwip/timers.h"


#include "netif/etharp.h"

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

static struct netif Netif;
static u32 periodic_timer; //TODO: myslim ze mozem zmazat periodic_timer


//static const struct eth_addr ethbroadcast = {{0xff,0xff,0xff,0xff,0xff,0xff}};

//static struct emacif Emacif_config;

/* Forward declarations. */
//static void  emacif_input(struct netif *netif);
//static err_t emacif_output(struct netif *netif, struct pbuf *p,
//             struct ip_addr *ipaddr);

static void
low_level_init(struct netif *netif)
{
  //dalsi riadok som tu mal iba pre to, ze som potom mohol spravit: netif->hwaddr[0-5] = emacif->ethaddr.addr[0-5];
  //struct emacif *emacif = netif->state;


  /* set MAC hardware address length */
  netif->hwaddr_len = 6;
  /* set MAC hardware address */ //alebo lepsie povedane, precitaj z HW
  netif->hwaddr[0] = MacAddress[0];
  netif->hwaddr[1] = MacAddress[1];
  netif->hwaddr[2] = MacAddress[2];
  netif->hwaddr[3] = MacAddress[3];
  netif->hwaddr[4] = MacAddress[4];
  netif->hwaddr[5] = MacAddress[5];
  /* maximum transfer unit */
  netif->mtu = 1500;

  /* broadcast capability */
  //netif->flags = NETIF_FLAG_BROADCAST;
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

  /* Do whatever else is needed to initialize interface. */

}


/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
  //struct emacif *emacif = netif->state;
  struct pbuf *q;
  char buf[1514];
  char *bufptr = &buf[0];
  unsigned char rc;

#if ETH_PAD_SIZE
  pbuf_header(p, -ETH_PAD_SIZE);    /* drop the padding word */
#endif

  for(q = p; q != NULL; q = q->next) {
    /* Send the data from the pbuf to the interface, one pbuf at a
       time. The size of the data in each pbuf is kept in the ->len
       variable. */

    /* send data from(q->payload, q->len); */
    memcpy(bufptr, q->payload, q->len);
    bufptr += q->len;
  }

  /* signal that packet should be sent(); */
  rc = platform_eth_send_packet( buf, p->tot_len );
  if (rc == ERR_BUF) { //so if it return nothing, it's ok. --old spec. behaviour
    return ERR_BUF;
  }

#if ETH_PAD_SIZE
  pbuf_header(p, ETH_PAD_SIZE);     /* reclaim the padding word */
#endif

#if LINK_STATS
  lwip_stats.link.xmit++;
#endif /* LINK_STATS */

  return ERR_OK;
}

/*
 * low_level_input():
 *
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 */

static struct pbuf *
low_level_input(struct netif *netif)
{
  //struct emacif *emacif = netif->state;
  struct pbuf *p, *q;
  u16_t len;
  unsigned char buf[1514];
  char *bufptr = &buf[0];

  /* Obtain the size of the packet and put it into the "len"
     variable. */
  len = platform_eth_get_packet_nb(buf, sizeof(buf));
  if( len <= 0 ) {
      return NULL;
  }
  
#if ETH_PAD_SIZE
  len += ETH_PAD_SIZE;      /* allow room for Ethernet padding */
#endif

  /* We allocate a pbuf chain of pbufs from the pool. */
  p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

  if (p != NULL) {

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE);          /* drop the padding word */
#endif

    /* We iterate over the pbuf chain until we have read the entire
     * packet into the pbuf. */
    for(q = p; q != NULL; q = q->next) {
      /* Read enough bytes to fill this pbuf in the chain. The
       * available data in the pbuf is given by the q->len
       * variable. */
      /* read data into(q->payload, q->len); */
      memcpy(q->payload, bufptr, q->len);
      bufptr += q->len;
    }
    /* acknowledge that packet has been read(); */

#if ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE);           /* reclaim the padding word */
#endif

#if LINK_STATS
    lwip_stats.link.recv++;
#endif /* LINK_STATS */
  } else {
    /* drop packet(); */

#if LINK_STATS
    lwip_stats.link.memerr++;
    lwip_stats.link.drop++;
#endif /* LINK_STATS */
  }

  return p;
}

///*
// * emacif_output():
// *
// * This function is called by the TCP/IP stack when an IP packet
// * should be sent. It calls the function called low_level_output() to
// * do the actual transmission of the packet.
// *
// */
//
//static err_t
//emacif_output(struct netif *netif, struct pbuf *p,
//      struct ip_addr *ipaddr)
//{
//
// /* resolve hardware address, then send (or queue) packet */
//  return etharp_output(netif, ipaddr, p);
//
//}

/*
 * emacif_input():
 *
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface.
 *
 */

static void
emacif_input(struct netif *netif)
{
  //struct emacif *emacif;
  //struct eth_hdr *ethhdr;
  struct pbuf *p;

  //emacif = netif->state;

  /* move received packet into a new pbuf */
  p = low_level_input(netif);
  /* no packet could be read, silently ignore this */
  if (p == NULL) {
    //TODO: LWIP_DEBUGF(TAPIF_DEBUG, ("tapif_input: low_level_input returned NULL\n"));
    return;
  }

#if LINK_STATS
  lwip_stats.link.recv++;
#endif /* LINK_STATS */

  ethernet_input(p, netif);

//  /* points to packet payload, which starts with an Ethernet header */
//  ethhdr = p->payload;
//
//  switch (htons(ethhdr->type)) {
//  /* IP packet? */
//  case ETHTYPE_IP:
////#if 0
/////* CSi disabled ARP table update on ingress IP packets.
////   This seems to work but needs thorough testing. */
////    /* update ARP table */
////    etharp_ip_input(netif, p);
////#endif
//    /* skip Ethernet header */
//    pbuf_header(p, -sizeof(struct eth_hdr));
//#if PPPOE_SUPPORT
//  /* PPPoE packet? */
//  case ETHTYPE_PPPOEDISC:
//  case ETHTYPE_PPPOE:
//#endif /* PPPOE_SUPPORT */
//    /* pass to network layer */
//    /* full packet send to tcpip_thread to process */
//    if (netif->input(p, netif) != ERR_OK) {
//      LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
//      pbuf_free(p);
//      //TODO: check with dbg if next line(p = NULL) is done inside pbuf_free()
//      p = NULL;
//    }
//    break;
//  case ETHTYPE_ARP:
//    /* pass p to ARP module  */
////      etharp_arp_input(netif, &emacif->ethaddr, p);
//    etharp_arp_input(netif, netif->hwaddr, p);
//    break;
//  default:
//    pbuf_free(p);
//    //TODO: check with dbg if next line(p = NULL) is done inside pbuf_free()
//    p = NULL;
//    break;
//  }
}

/*
static void
arp_timer(void *arg)
{
  etharp_tmr();
  sys_timeout(ARP_TMR_INTERVAL, arp_timer, NULL);
}
*/

/*
 * emacif_setmac():
 *
 * Set the MAC address of the system.
 * Should only be called before emacif_init is called.
 * The stack calls emacif_init after the user calls netif_add
 *
 */

//static void
//emacif_setmac(u8_t *addr)
//{
//    Emacif_config.ethaddr.addr[0] = addr[0];
//    Emacif_config.ethaddr.addr[1] = addr[1];
//    Emacif_config.ethaddr.addr[2] = addr[2];
//    Emacif_config.ethaddr.addr[3] = addr[3];
//    Emacif_config.ethaddr.addr[4] = addr[4];
//    Emacif_config.ethaddr.addr[5] = addr[5];
//}

/*
 * emacif_init():
 *
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 */

err_t
emacif_init(struct netif *netif)
{
  //struct emacif *emacif;

  //emacif = &Emacif_config;
  //
  //if (emacif == NULL)
  //{
  //      LWIP_DEBUGF(NETIF_DEBUG, ("emacif_init: out of memory\n"));
  //      return ERR_MEM;
  //}
  //netif->state = emacif;

#if LWIP_SNMP
  /* ifType ethernetCsmacd(6) @see RFC1213 */
  netif->link_type = 6;
  /* your link speed here */
  netif->link_speed = ;
  netif->ts = 0;
  netif->ifinoctets = 0;
  netif->ifinucastpkts = 0;
  netif->ifinnucastpkts = 0;
  netif->ifindiscards = 0;
  netif->ifoutoctets = 0;
  netif->ifoutucastpkts = 0;
  netif->ifoutnucastpkts = 0;
  netif->ifoutdiscards = 0;
#endif

  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  netif->output = etharp_output;
  netif->linkoutput = low_level_output;

  low_level_init(netif);

  //etharp_init(); v etharp.h je: #define etharp_init() /* Compatibility define, not init needed. */


  //sys_timeout(ARP_TMR_INTERVAL, arp_timer, NULL);

  return ERR_OK;
}

/*
 * emacif_poll():
 *
 * Polling task
 * Should be called periodically
 * Note: variable netif is global.
 */
//void emacif_poll(struct netif *netif)
void emacif_poll()
{
//  periodic_timer++;
//  if( periodic_timer >= 5 ) {
//      periodic_timer = 0;
      sys_check_timeouts();
      emacif_input(&Netif);
//  }

//  u32 elapsed_time = platform_eth_get_elapsed_time();
//  if( elapsed_time == 0 ) { //na zaklade casovaca
//    periodic_timer += elapsed_time;
//    if( periodic_timer >= 349 ) {
//      periodic_timer = 0;
//      sys_check_timeouts();
//      emacif_input(&Netif);
//    }
//  } else { // naozajstny interupt
//      emacif_input(&Netif);
//  }
}


/***********************************************************************/
/***** sys_arch.c *****************************************************/
/***********************************************************************/
#define UMAX(a, b)      ((a) > (b) ? (a) : (b))


/* @return (platform) time time in ms. faster increments (like microseconds)
 *         will harm lwIP timers (fast timeouts)
 */
u32
sys_now(void)
{
//  return platform_timer_read_sys()/1000;
  return platform_timer_read_sys()/1000;
}





/// The IP address
//unsigned char IpAddress[4];
unsigned char IpAddress[4] = {192, 168, 100, 129};

/// Set the default router's IP address.
unsigned char GateWay[4] = {192, 168, 100, 1};

// The NetMask address
unsigned char NetMask[4] = {255, 255, 255, 0 };

/* Init Ethernet and lwIP */
void elua_lwip_init( u8_t *addr ) {
    struct ip_addr ipaddr, netmask, gw;
    //struct netif *netif;

//    unsigned char new_IpAddress[4] = {192, 168, 1, 222};
//    unsigned char new_GateWay[4] = {192, 168, 1, 1};
//    unsigned char new_netmask[4] = {255, 255, 255, 0};

    periodic_timer = 0;

    // Initialize lwIP modules
    lwip_init();

    // Initialize net interface for lwIP

    //emacif_setmac((u8_t*)MacAddress);

#if !LWIP_DHCP
    IP4_ADDR(&gw, GateWay[0], GateWay[1], GateWay[2], GateWay[3]);
    IP4_ADDR(&ipaddr, IpAddress[0], IpAddress[1], IpAddress[2], IpAddress[3]);
    IP4_ADDR(&netmask, NetMask[0], NetMask[1], NetMask[2], NetMask[3]);
#else
    IP4_ADDR(&gw, 0, 0, 0, 0);
    IP4_ADDR(&ipaddr, 0, 0, 0, 0);
    IP4_ADDR(&netmask, 0, 0, 0, 0);
#endif

    netif_add(&Netif, &ipaddr, &netmask, &gw, NULL, emacif_init, ip_input);
    netif_set_default(&Netif);
    netif_set_up(&Netif);

  #if LWIP_DHCP
    dhcp_start(&Netif);
  #endif



//TODO: pre vsetkych daemonov ktory budu v nejakom zozname
//    // Initialize http server application
//    if (ERR_OK != httpd_init()) {
//        return -1;
//    }

    //sntp_init();

    httpd_init();


}


//TODO: presunut do noveho suboru. napr. elua_socket.c, alebo elua_net.c
/* this part is re-writed code from elua_uip.c */

int elua_net_socket( int type )
{
  int i;
  struct uip_conn* pconn;
  int old_status;

  // [TODO] add UDP support at some point.
  if( type == ELUA_NET_SOCK_DGRAM )
    return -1;

  old_status = platform_cpu_set_global_interrupts( PLATFORM_CPU_DISABLE );
  // Iterate through the list of connections, looking for a free one
/* TODO: nieco taketo, ale treba presne prist na to, ako sa vysporiadat z
  MEMP_NUM_TCP_PCB_LISTEN a MEMP_NUM_TCP_PCB. vid. http://lwip.wikia.com/wiki/Raw/TCP
  */
//  for( i = 0; i < UIP_CONNS; i ++ )
//  {
//    pconn = uip_conns + i;
//    if( pconn->tcpstateflags == UIP_CLOSED )
//    {
//      // Found a free connection, reserve it for later use
//      uip_conn_reserve( i );
//      break;
//    }
//  }
  platform_cpu_set_global_interrupts( old_status );
  return i == UIP_CONNS ? -1 : i;
}

// Send data
elua_net_size elua_net_send( int s, const void* buf, elua_net_size len )
{
//  volatile struct elua_uip_state *pstate = ( volatile struct elua_uip_state* )&( uip_conns[ s ].appstate );
//
//  if( !ELUA_UIP_IS_SOCK_OK( s ) || !uip_conn_active( s ) )
//    return -1;
//  if( len == 0 )
//    return 0;
//  elua_prep_socket_state( pstate, ( void* )buf, len, ELUA_NET_NO_LASTCHAR, ELUA_NET_ERR_OK, ELUA_UIP_STATE_SEND );
//TODO"preco som sem chcel dat force interupt??  platform_eth_force_interrupt();
//  while( pstate->state != ELUA_UIP_STATE_IDLE );
//  return len - pstate->len;
}

//elua_net_recvbuf asi moze ostat blokujuce, ale pre elua_net_recv ktore nie je urcene pre lua na to nevidim dovod.
//elua_accept moze byt tiez blokujuce pre lua.



#endif //BUILD_LWIP

