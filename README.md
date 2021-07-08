
Server publish topic: /${MAC}/S
Client publish topic: /${MAC}/C 

EX: if client eth1 mac:  C6:FB:4C:F5:26:23
Server publish topic: /C6:FB:4C:F5:26:23/S
Client publish topic: /C6:FB:4C:F5:26:23/C 

Command struct {
Action string `json:"action"`
Targets []string `json:"targets,omitempty"`
Seq int `json:"seq"`
Note string `json:"note,omitempty"`
}

response struct {
Response string `json:"response"`
Seq int `json:"seq"`
Note string `json:"note,omitempty"`
}

targets struct{
Mac  string `json:"mac"`
Ip  string `json:"ip",omitempty`
}


{ 
    "action": "wake",
    "targets": [ 
                    {"mac": "00:03:7f:11:23:1f"},
                    {"mac": "12:34:56:11:23:45"}
               ]
    "seq":  32767
}


{
    "response":"wake",
    "seq":  32767            
    "status":"success"
}

{
    "response":"wake",
    "seq":  32767            
    "status":"fail"
    "note": "unknown"
}


test with mosquitto client:
 mosquitto_sub -p 8884 -h test.mosquitto.org  --cafile /usr/share/mwol/ssl/mosquitto.org.crt  --key /usr/share/mwol/ssl/client.key  --cert /usr/share/mwol/ssl/client.crt -t "/mwol/18:31:bf:52:1a:a5" 

 mosquitto_pub -p 8884 -h test.mosquitto.org  --cafile /usr/share/mwol/ssl/mosquitto.org.crt  --key /usr/share/mwol/ssl/client.key  --cert /usr/share/mwol/ssl/client.crt -t "/mwol/18:31:bf:52:1a:a5" -m "{\"action\": \"wake\",\"targets\": [ {\"mac\": \"00:03:7f:11:23:1f\"}, {\"mac\": \"12:34:56:11:23:45\"}], \"seq\":  32767}"




~

