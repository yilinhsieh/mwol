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


#ifndef	_MQTT_CLIENT_H_
#define	_MQTT_CLIENT_H_

#define GETWAY_MQTT_TOPIC_SUB  "/%s/S"
#define GETWAY_MQTT_TOPIC_PUB  "/%s/C"
#define GETWAY_MQTT_CLIENT_ID  "mwol|%s"
#include <stdbool.h>
/*************************************************************************************************/
extern void init_mqtt(void);
extern void mqtt_arp_list(void);
extern void mqtt_wake_response(int sn, bool success);
extern void mqtt_arp_list_response(int);
extern void  mqtt_error_response(int, int);

/*************************************************************************************************/

#endif
