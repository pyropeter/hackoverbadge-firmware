#include "aether.h"

#include "ui/event.h"
#include "ui/font.h"

#include <funk/nrf24l01p.h>

// make reassembly buffer slightly larger than 2048 bytes
#define AETHER_SLICES 67

/*

Packet size: 32 bytes (fixed)

Packet format:
First byte is type-byte. Followed by 31 bytes payload.

Type-Bytes:
0-n:  Save 31-byte-slice $typebyte into reassembly-buffer
      n is AETHER_SLICES-1
0xff: announcement
      Use reassembly-buffer as LCD contents and gain user attention.
      The first byte of the payload is a flags field and must be zero.
      If flags field is not zero, the badge must ignore the packet.
      (This allows for future extensions, e.g. run-length-encoding or checksumming)

*/

// type bytes
#define PKGTYPE_ANNOUNCE 0xff

static void debug(char *msg)
{
	badge_framebuffer fb = { { { 0 } } };
	badge_framebuffer_render_text(&fb, 0, 0, msg);
	badge_framebuffer_flush(&fb);
}

static void handle_packet(uint8_t *reassembly_buffer, uint8_t *pkt)
{
	if (pkt[0] < AETHER_SLICES) {
		memcpy(reassembly_buffer + 31 * pkt[0], pkt + 1, 31);
	} else if (pkt[0] == PKGTYPE_ANNOUNCE) {
		//debug("Announce");
		//for (int volatile i=0; i<10000000; i++);
		badge_framebuffer_flush((badge_framebuffer*)reassembly_buffer);
		//for (int volatile i=0; i<10000000; i++);
		//debug("Announce 2");
	}
}

void aether_main(void)
{
	uint8_t reassembly_buffer[AETHER_SLICES*31];
	uint8_t pkt[32];
	struct NRF_CFG nrfcfg;
	int pktlen;

	// set up radio foo
	nrf_init();
	nrfcfg.channel = 89;
	nrfcfg.nrmacs = 1;
	nrfcfg.maclen[0] = 32; // this is actually length of recved pkts
	memcpy(nrfcfg.mac0, "\xfa\xfd\xb8\x25\xbe", 5);
	nrf_config_set(&nrfcfg);

	// start receiving packets
	// use crc, use two-byte crc
	nrf_rcv_pkt_start(R_CONFIG_EN_CRC | R_CONFIG_CRCO);

	// initialize display
	debug("Waiting...");

	// listen for packets
	while (badge_input_raw() == 0) {
		pktlen = nrf_rcv_pkt_poll(32, pkt);
		if (pktlen != 32) {
			// nothing was received, sleep
			badge_event_wait();
		} else {
			handle_packet(reassembly_buffer, pkt);
		}
	}

	// nRF shutdown
	nrf_rcv_pkt_end();
	nrf_off();
}

