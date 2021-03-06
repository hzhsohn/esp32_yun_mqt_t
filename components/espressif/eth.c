
#include "esp_system.h"
#include "esp_err.h"
#include "esp_event_loop.h"
#include "esp_event.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_eth.h"

#include "rom/ets_sys.h"
#include "rom/gpio.h"

#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_sig_map.h"

#include "nvs_flash.h"
#include "driver/gpio.h"

#include "eth_phy/phy_lan8720.h"
#include "eth.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "esp_log.h"
#include "esp_event_loop.h"


#define DEFAULT_ETHERNET_PHY_CONFIG phy_lan8720_default_ethernet_config

#define PIN_SMI_MDC			GPIO_NUM_23
#define PIN_SMI_MDIO		GPIO_NUM_18
#define PIN_CLOCK_MODE		2	//PHY_CLOCK_GPIO16_OUT

static void eth_gpio_config_rmii(void)
{
    // RMII data pins are fixed:
    // TXD0 = GPIO19
    // TXD1 = GPIO22
    // TX_EN = GPIO21
    // RXD0 = GPIO25
    // RXD1 = GPIO26
    // CLK == GPIO0
    phy_rmii_configure_data_interface_pins();
    // MDC is GPIO 23, MDIO is GPIO 18
    phy_rmii_smi_configure_pins(PIN_SMI_MDC, PIN_SMI_MDIO);
}

void app_eth()
{

	//------------------------------------------------
		esp_err_t ret = ESP_OK;

		eth_config_t config = DEFAULT_ETHERNET_PHY_CONFIG;
		/* Set the PHY address in the example configuration */
		config.phy_addr = PHY0;
		config.gpio_config = eth_gpio_config_rmii;
		config.tcpip_input = tcpip_adapter_eth_input;
		//初始化GPIO16.这个50M叼信号线-------------------------
		config.clock_mode = PIN_CLOCK_MODE;
		//config.phy_power_enable = phy_device_power_enable_via_gpio;

		ret = esp_eth_init(&config);

		if(ret == ESP_OK) {
			esp_eth_enable();

		}
		else{
			ESP_LOGI(TAG_ETH, "ETH init fail.");
		}
}

int ethGetIP(char* dstIP,char* dstMask,char* dstGW)
{	
		dstIP[0]=0;
		dstMask[0]=0;
		dstGW[0]=0;

		tcpip_adapter_ip_info_t ip;
		memset(&ip, 0, sizeof(tcpip_adapter_ip_info_t));		
		if (tcpip_adapter_get_ip_info(ESP_IF_ETH, &ip) == 0) {
			sprintf(dstIP,IPSTR, IP2STR(&ip.ip));
			sprintf(dstMask,IPSTR, IP2STR(&ip.netmask));
			sprintf(dstGW,IPSTR, IP2STR(&ip.gw));
			return 0;
		}
		return 1;
}