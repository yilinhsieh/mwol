/**
 * mwol - MQTT Wake On Lan
 * Copyright (C) 2019 Mleaf <mleaf90@gmail.com, 350983773@qq.com>
 * 微信公众号 《WiFi物联网》
 *
 * mwol is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * mwol is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _CONFIG_H
#define _CONFIG_H
#include <stdbool.h>        /* bool type */

#define DEFAULT_MQTT_SERVER		"www.mleaf.org"
#define DEFAULT_MQTT_PORT       1883
#define DEFAULT_ENCRYPTION      false
#define DEFAULT_SSL      		false
#define DEFAULT_IFNAME          "eth0"
#define DEFAULT_MQTT_USERNAME       "wol"
#define DEFAULT_MQTT_PASSWORD       "wolpass"
#define DEFAULT_PRESET_MAC       "01:02:03:04:05:06"	
#define	DEFAULT_CA_FILE		"/usr/share/mwol/ssl/ca.crt"
#define	DEFAULT_CRT_FILE	"/usr/share/mwol/ssl/client.crt"
#define	DEFAULT_KEY_FILE	"/usr/share/mwol/ssl/client.key"
			
#define DEFAULT_CONFIGFILE  "/etc/mwol.json"

/*
{
        "ssl": true,
        "port": 8884,
        "keyfile": "/usr/share/mwol/ssl/client.key",
        "encryption": true,
        "id": "18:31:bf:52:1a:a5",
        "cafile": "/usr/share/mwol/ssl/mosquitto.org.crt",
        "crtfile": "/usr/share/mwol/ssl/client.crt",
        "hostname": "test.mosquitto.org",
        "wol_ifname": "eno1"
}
*/
typedef struct _mqtt_server_t {
	char 	*hostname;
	short	port;
	bool 	ssl;
	char 	*cafile;
	char 	*crtfile;
	char 	*keyfile;
	bool 	encryption;
	char 	*username;
	char 	*password;
	char 	*id;
	
}t_mqtt_server;

typedef struct{
	char *configfile;
	char *gw_id;
	char *presetmac;
	char *ifname;
	t_mqtt_server	*mqtt_server;
}s_config;

extern int config_read_from_json(char *cfg_path);
extern int config_init(void);
extern s_config *config_get(void);

#endif

