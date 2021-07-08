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

#include "config.h"
#include "common.h"
#include "json.h"
#include "mqtt_client.h"

static struct wol_json_op {
	char	*action;
	void	(*process_wol_json_op)(cJSON *);
} wol_op[] = {

	{"arp", process_arp_list_op},
	{"wake", process_wake_op},


	{NULL, NULL}

};

/**********************************************************************/

/*
{
	"header": {
		"action": "arp"
	},
	"payload": {}
}
*/
void process_arp_list_op(cJSON *root)
{
	int seq = 0;
	seq = cJSON_GetObjectItem(root, "seq")->valueint;

	mqtt_arp_list_response(seq);

}

/*
{ 
    "action": "wake",
    "targets": [ 
                    {"mac": "00:03:7f:11:23:1f"},
                    {"mac": "12:34:56:11:23:45"}
               ]
    "seq":  32767
}
*/

void process_wake_op(cJSON *root)
{
	cJSON *targets = NULL;
	char *mac = NULL;
	int seq = 0;
	int wol_result = 0;
	int target_num = 0;

	seq = cJSON_GetObjectItem(root, "seq")->valueint;
	targets = cJSON_GetObjectItem(root, "targets");
	target_num = cJSON_GetArraySize(targets);
	MSG_DEBUG("seq:%d, target num = %d\n", seq, target_num);
	
	for(int i = 0; i < target_num; i ++) 
	{
		cJSON *target = NULL;
		target = cJSON_GetArrayItem(targets, i);
		mac = cJSON_GetObjectItem(target, "mac")->valuestring;
		if(mac != NULL){
			MSG_DEBUG("wol mac:%s\n", mac);
			wol_result += wake_on_lan('b', true, mac);
		}
	}
	if(wol_result == 0)
	{
		MSG_DEBUG("send wake %d  Success. \n", seq);
		mqtt_wake_response(seq, true);
	} else {
		MSG_DEBUG("send wol  %d fail. \n", seq);
		mqtt_wake_response(seq, false);
	}
}


/**********************************************************************/

static void
json_parse_token(char *action, cJSON *root)
{
	int i = 0;
	bool match = false;
	for (; wol_op[i].action != NULL; i++) {
		if(strcmp(action, wol_op[i].action) == 0 && wol_op[i].process_wol_json_op){
			
			wol_op[i].process_wol_json_op(root);
			match = true;
			break;
		}
	}
	
	if(match != true) // no match
	{
		int seq = 0;
		seq = cJSON_GetObjectItem(root, "seq")->valueint;
		mqtt_error_response(seq, ERROR_ACTION_NAME_NOTVAILD);
	}
}



static int validate_json_Object(cJSON *item)
{
	cJSON *found_action = NULL;

	found_action = cJSON_GetObjectItem(item,"action");
	if(NULL == found_action)
	{
		return JSON_ERROR;
	}

	return JSON_SUCCESS;

}

/*
{ 
    "action": "wake",
    "targets": [ 
                    {"mac": "00:03:7f:11:23:1f"},
                    {"mac": "12:34:56:11:23:45"}
               ]
    "seq":  32767
}
*/
int process_json_object(char *msg)
{
	cJSON *item = NULL;
	
	int ret = JSON_ERROR;
	char *action;

	MSG_DEBUG("Process json object string=%s\n", msg);

	item = cJSON_Parse(msg);

	if (!item)
	{
		MSG_DEBUG("Error before: [%s]\n", cJSON_GetErrorPtr());
		cJSON_Delete(item);
		return JSON_ERROR;
	}
	ret = validate_json_Object(item);
	if(JSON_SUCCESS != ret)
	{
		MSG_DEBUG("not command Json Object.\n");
		cJSON_Delete(item);
		return JSON_ERROR;
	}
	else{

		action = cJSON_GetObjectItem(item, "action")->valuestring;
		json_parse_token(action, item);
	}
	cJSON_Delete(item);
	return JSON_SUCCESS;
}

/*
{
	"header": {
		"action": "arpList"
	},
	"payload": {
		"deviceId": "06:f0:21:34:5f:1d",
		"note": [{
				"mac": "00:03:7f:11:23:1f",
				"ip": "10.13.35.31",
				"hostName": "* *"
			},
			{
				"mac": "00:25:22:42:b6:93",
				"ip": "192.168.10.235",
				"hostName": "DESKTOP-VVNKNR8.lan"
			},
			{
				"mac": "00:03:7f:11:23:1f",
				"ip": "192.168.10.113",
				"hostName": "* *"
			}
		]
	}
}
*/
void json_arp_list(char **msg, int sn)
{
	FILE *fp;
	unsigned int arp_flags;
	unsigned int hw_type;
	char buffer[256], arp_mac[32], arp_if[32];
	char hostName[64] = {0};
	int ret = -1;

	char *out=NULL;
	
	cJSON *dir2=NULL;
	cJSON *dataArry=NULL;
	cJSON *dir3=NULL;
#if defined (USE_IPV6)
		char s_addr1[INET6_ADDRSTRLEN];
#else
		char s_addr1[INET_ADDRSTRLEN];
#endif
	s_config *board_config = config_get();

	cJSON *root = cJSON_CreateObject();
	dataArry = cJSON_CreateArray();
	
	

	cJSON_AddStringToObject(root, "action", "response_arp");
	cJSON_AddNumberToObject(root, "seq", sn);
	cJSON_AddItemToObject(root, "note", dir2=cJSON_CreateObject());
	cJSON_AddStringToObject(dir2, "deviceId", board_config->gw_id);

	dir3=cJSON_CreateObject();
	cJSON_AddStringToObject(dir3, "mac", board_config->presetmac);
	cJSON_AddStringToObject(dir3, "ip", "* *");
	cJSON_AddStringToObject(dir3, "hostName", "preset mac");
	cJSON_AddItemToArray(dataArry, dir3);
	
	fp = fopen("/proc/net/arp", "r");
	if (fp) {
		// skip first line
		fgets(buffer, sizeof(buffer), fp);
		while (fgets(buffer, sizeof(buffer), fp)) {
			//MSG_DEBUG("arp: %s ", buffer);
			arp_flags = 0;
			if (sscanf(buffer, "%s 0x%x 0x%x %s ", s_addr1, &hw_type, &arp_flags, arp_mac) ) {
				if((arp_flags & 0x02) && (strcmp(arp_mac, "00:00:00:00:00:00"))){
					//printf("s_addr1:%s, arp_mac:%s\n",s_addr1,arp_mac);
					//printf("arp_flags:0x%02x\n",arp_flags);
					
					dir3=cJSON_CreateObject();
					memset(hostName, 0, sizeof(hostName));
					ret = get_name_info(s_addr1, hostName);
					cJSON_AddStringToObject(dir3, "mac", arp_mac);
					cJSON_AddStringToObject(dir3, "ip", s_addr1);
					if(ret == 0){
						cJSON_AddStringToObject(dir3, "hostName", hostName);
					}else{
						cJSON_AddStringToObject(dir3, "hostName", "* *");
					}
					cJSON_AddItemToArray(dataArry, dir3);
				}
			}
		}
		fclose(fp);
	}
	
	cJSON_AddItemToObject(dir2, "data", dataArry);

	out = cJSON_PrintUnformatted(root);

	*msg = (char *)safe_malloc(strlen(out)+1);

	strncpy(*msg, out, strlen(out));

	//MSG_DEBUG("%s\n",out);
	cJSON_Delete(root);
	if(out)
	{
		free(out);
	}
}


/*
* {
    "response":"wake",
    "seq":  32767            
    "status":"success"
}
*/
void json_wake_response(char **msg, int seq, bool success)
{
	char *out;
	
	cJSON *root = cJSON_CreateObject();
	
	cJSON_AddStringToObject(root, "response", "wake");
	cJSON_AddNumberToObject(root, "seq", seq);
	if(success){
		cJSON_AddStringToObject(root, "status", "success");
	} else {
		cJSON_AddStringToObject(root, "status", "fail");
	}
	out = cJSON_PrintUnformatted(root);
	*msg = (char *)safe_malloc(strlen(out)+1);

	strncpy(*msg, out, strlen(out));

	cJSON_Delete(root);
	if(out)
	{
		free(out);
	}
}



/*
* {
    "response":"error",
    "seq":  sn            
    "error_num": errornum
}
*/

void json_Error_response(char **msg, int seq, int  error_num)
{
	char *out;
	
	cJSON *root = cJSON_CreateObject();
	
	cJSON_AddStringToObject(root, "response", "error");
	cJSON_AddNumberToObject(root, "seq", seq);
	cJSON_AddNumberToObject(root, "error_num", seq);
	out = cJSON_PrintUnformatted(root);
	*msg = (char *)safe_malloc(strlen(out)+1);

	strncpy(*msg, out, strlen(out));

	cJSON_Delete(root);
	if(out)
	{
		free(out);
	}
}


