
 * FunctionName : user_set_softap_config
 * Description  : set SSID and password of ESP8266 softAP
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
user_set_softap_config(void)
{
   struct softap_config config;

   wifi_softap_get_config(&config); // Get config first.
   
   os_memset(config.ssid, 0, 32);
   os_memset(config.password, 0, 64);
   os_memcpy(config.ssid, "ESP8266", 7);
   os_memcpy(config.password, "12345678", 8);
   config.authmode = AUTH_WPA_WPA2_PSK;
   config.ssid_len = 0;// or its actual length
   config.beacon_interval = 100;
   config.max_connection = 4; // how many stations can connect to ESP8266 softAP at most.

   wifi_softap_set_config(&config);// Set ESP8266 softap config .
   
}

void dhcps_lease_test(void)
{
   struct dhcps_lease dhcp_lease;
   IP4_ADDR(&dhcp_lease.start_ip, 192, 168, 5, 100);
   IP4_ADDR(&dhcp_lease.end_ip, 192, 168, 5, 105);
   wifi_softap_set_dhcps_lease(&dhcp_lease);
}
void user_init(void)
{
   struct ip_info info;
   wifi_set_opmode(STATIONAP_MODE); //Set softAP + station mode
   wifi_softap_dhcps_stop();
   IP4_ADDR(&info.ip, 192, 168, 5, 1);
   IP4_ADDR(&info.gw, 192, 168, 5, 1);
   IP4_ADDR(&info.netmask, 255, 255, 255, 0);
   wifi_set_ip_info(SOFTAP_IF, &info);
   dhcps_lease_test();
   wifi_softap_dhcps_start();


/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
    os_printf("SDK version:%s\n", system_get_sdk_version());
        
    wifi_set_opmode(SOFTAP_MODE);

    // ESP8266 softAP set config.
    user_set_softap_config();

}