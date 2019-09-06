/*
 * Ã‚Â© Francesco PotortÃƒÂ¬ 2013 - GPLv3 - Revision: 1.13
 *
 * Send an NTP packet and wait for the response, return the Unix time
 *
 * To lower the memory footprint, no buffer1s are allocated for sending
 * and receiving the NTP packets.  Four bytes of memory are allocated
 * for transmision, the rest is random garbage collected from the data
 * memory segment, and the received packet is read one byte at a time.
 * The Unix time is returned, that is, seconds from 1970-01-01T00:00.
 */

#include "ntp.h"

extern "C" {
#include <sntp.h>//NTP client based on lwIP TCP/IP stack of espressif firmware
}

LOCAL os_timer_t sntp_timer;

#define NTP_PACKET_SIZE 48

unsigned long syncInterval;
//char *ntpServer; 
TimeElements tm;

unsigned dst;
unsigned long lastNTPTime = 0;
unsigned long lastNTPMillis = 0;
long adjust = 0;
short timeZone = 0;
short lastSec=0;

unsigned long getUNIXTime() {
  if(lastNTPTime == 0) {
    return 0;
  }
  unsigned long diffms = millis() - lastNTPMillis;
  //return (unsigned long) lastNTPTime + (diffms + adjust)/1000 + timeZone + dst;
  return (unsigned long) lastNTPTime + (diffms + adjust)/1000 + dst;
}

inline unsigned long updateNTP() {
  if(lastNTPMillis == 0 || millis() - lastNTPMillis > 1000 * syncInterval) {
    //unsigned long ntpTime = ntpUnixTime();
	os_timer_disarm(&sntp_timer);
	lastNTPMillis = millis();
	unsigned long ntpTime =	sntp_get_current_timestamp(); 
    if(ntpTime != 0) {
		updateNTP(ntpTime);
    }else{
		os_timer_arm(&sntp_timer, 100, 0);
	}
  }
}

// A UDP instance to let us send and receive packets over UDP
//WiFiUDP udp;

//byte packetBuffer[4]; //buffer to hold incoming and outgoing packets
//char *timeServer = "";
/*
//NTP client based on UDP library
inline unsigned long ntpUnixTime()
{  
  byte packetBuffer[4]; //buffer to hold incoming and outgoing packets
  char *timeServer = ntpServer;  // NTP server
  // set all bytes in the buffer to 0
  // Only the first four bytes of an outgoing NTP packet need to be set
  // appropriately, the rest can be whatever.
  //const long ntpFirstFourBytes = 0xEC0600E3; // NTP request header 236 6 0 227
  //memset(packetBuffer, 0, NTP_PACKET_SIZE); 
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)    
  packetBuffer[0] = 227;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 236;   // Peer Clock Precision
  // Send an NTP request
  udp.beginPacket((const char*)timeServer, "123"); // 123 is the NTP port
  udp.write((const unsigned char*)packetBuffer,4,NTP_PACKET_SIZE);
  udp.endPacket();//(false);//non devo disconnettere perhè aspetto una risposta
  // Wait for response; check every pollIntv ms up to maxPoll times
  const int pollIntv = 50;   // poll every this many ms
  const byte maxPoll = 100;    // poll up to this many times
  int pktLen;       // received packet length
  
  for (byte i=0; i<maxPoll; i++) {
    if ((pktLen = udp.parsePacket(1000)) == NTP_PACKET_SIZE)
      break;
    delay(pollIntv);
  }
  
  if (pktLen != NTP_PACKET_SIZE) {
    return 0;       // no correct packet received
  }
  //ESP8266wifi::getWifi().stopTransparentMode();
  // Read and discard the first useless bytes
  // Set useless to 32 for speed; set to 40 for accuracy.
  const byte useless = 40;
  for (byte i = 0; i < useless; i++)
		udp.read();

  // Read the integer part of sending time
  unsigned long time = (unsigned long) udp.read();  // NTP time
  for (char i = 1; i < 4; i++)
  	time = (time << 8) |  (udp.read() & 0xff);
  // Round to the nearest second if we want accuracy
  // The fractionary part is the next byte divided by 256: if it is
  // greater than 500ms we round to the next second; we also account
  // for an assumed network delay of 50ms, and (0.5-0.05)*256=115;
  // additionally, we account for how much we delayed reading the packet
  // since its arrival, which we assume on average to be pollIntv/2.
  time += (udp.read() > 115 - pollIntv/8);

  // Discard the rest of the packet
  udp.flush();
   
  //return time - 2208988800ul+7200;   // convert NTP time to Unix time (ora legale)
  return time - 2208988800ul+3600;   // convert NTP time to Unix time (ora solare)
}
*/
void sntpInit(){
	sntp_init();
	os_timer_disarm(&sntp_timer);
	os_timer_setfn(&sntp_timer,	(os_timer_func_t *)user_check_sntp_stamp, NULL);
}

void ICACHE_FLASH_ATTR	user_check_sntp_stamp(void *arg){

	uint32	current_stamp;
	current_stamp =	sntp_get_current_timestamp();
	if(current_stamp ==	0){
		os_timer_arm(&sntp_timer, 100, 0);
	}else{
		os_timer_disarm(&sntp_timer);
		updateNTP(current_stamp);
	}
}

unsigned long updateNTP(unsigned long ntpTime) {
    if(ntpTime != 0) {
      DEBUG_PRINT(F("Time updated, clock skew: "));
      DEBUG_PRINTLN(getUNIXTime() - ntpTime);
      lastNTPTime = ntpTime;
      //lastNTPMillis = millis();
    }
  }

/*
  time.c - low level time and date functions
  Copyright (c) Michael Margolis 2009-2014
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
  1.0  6  Jan 2010 - initial release
  1.1  12 Feb 2010 - fixed leap year calculation error
  1.2  1  Nov 2010 - fixed setTime bug (thanks to Korman for this)
  1.3  24 Mar 2012 - many edits by Paul Stoffregen: fixed timeStatus() to update
                     status, updated examples for Arduino 1.0, fixed ARM
                     compatibility issues, added TimeArduinoDue and TimeTeensy3
                     examples, add error checking and messages to RTC examples,
                     add examples to DS1307RTC library.
  1.4  5  Sep 2014 - compatibility with Arduino 1.5.7
*/

#define LEAP_YEAR(Y) ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )
static const unsigned short monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31}; // API starts months from 1, this array starts from 0

inline void sampleCurrTime(unsigned long timeInput){
// break the given unsigned long into time components
//and cache them in the tm struct
// this is a more compact version of the C library localtime function
// note that year is offset from 1970 !!!

  unsigned short year;
  unsigned short month, monthLength;
  unsigned long time;
  unsigned long days;

  time = (unsigned long) timeInput;
  tm.Second = time % 60;
  time /= 60; // now it is minutes
  tm.Minute = time % 60;
  time /= 60; // now it is hours
  tm.Hour = time % 24;
  time /= 24; // now it is days
  tm.Wday = ((time + 4) % 7) + 1;  // Sunday is day 1 
  
  year = 0;  
  days = 0;
  while((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= time) {
    year++;
  }
  tm.Year = year; // year is offset from 1970 
  
  days -= LEAP_YEAR(year) ? 366 : 365;
  time  -= days; // now it is days in this year, starting at 0
  
  days=0;
  month=0;
  monthLength=0;
  for (month=0; month<12; month++) {
    if (month==1) { // february
      if (LEAP_YEAR(year)) {
        monthLength=29;
      } else {
        monthLength=28;
      }
    } else {
      monthLength = monthDays[month];
    }
    
    if (time >= monthLength) {
      time -= monthLength;
    } else {
        break;
    }
  }
  tm.Month = month + 1;  // jan is month 1  
  tm.Day = time + 1;     // day of month
}

bool sampleCurrTime(void){
	//update NTP time every prefixed time
	updateNTP();
	//sample the actual time, break it in is components and cache them in a structure
	sampleCurrTime(getUNIXTime());
	DEBUG2_PRINT(F("Local time is "));
	char buf[50];
	DEBUG2_PRINTLN(printUNIXTime(buf));
	bool hit = (lastSec > tm.Second);
	lastSec = tm.Second;
	return hit;
}

int hour() { // the hour now 
  return tm.Hour;  
}

int hourFormat12() { // the hour for the given time in 12 hour format
  if( tm.Hour == 0 )
    return 12; // 12 midnight
  else if( tm.Hour  > 12)
    return tm.Hour - 12 ;
  else
    return tm.Hour ;
}

unsigned short isAM() { // returns true if time now is AM
  return !isPM(); 
}

unsigned short isPM() { // returns true if PM
  return (hour() >= 12); 
}

int minute() { // the minute for the given time
  return tm.Minute;  
}

int second() {
  return tm.Second;
}

int day(){
  return tm.Day;
}

int weekday() {   // Sunday is day 1
  return tm.Wday;
}
   
int month(){
  return tm.Month;
}

int year() {  // as in Processing, the full four digit year: (2009, 2010 etc) 
  return tmYearToCalendar(tm.Year);
}

void adjustTime(long adjustment) {// set the msec of correction of the measure with sign  
  adjust= adjustment;
}

void setSyncInterval(unsigned long interval){// set the number of seconds between re-sync
  syncInterval = interval;
}

void setTimeZone(short tmz){// set the number of hours of delay
  //timeZone = tmz*3600;
  sntp_stop();
  sntp_set_timezone(tmz);      // UTC time
  //sntp_init();
}

void setSDT(bool yes){// set the number of hours of delay
	if(yes)
		dst = 3600;
	else
		dst = 0;
}

void setNtpServer(byte id, const char* serv){// set the number of seconds between re-sync
  //ntpServer = serv;
  sntp_setservername(id, (char*) serv);
  //sntp_stop();
} 

char *printUNIXTime(char *buf){
  memset(buf, 0, DATEBUFLEN);
  //snprintf(buf, 50, "%04i-%02i-%02i %02i:%02i:%02i UTC", tm.Year+1970, tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second);
  snprintf(buf, DATEBUFLEN, "%02i/%02i/%04i %02i:%02i:%02i", tm.Day, tm.Month, tm.Year+1970, tm.Hour, tm.Minute, tm.Second);
  return buf;
}

char *printUNIXTimeMin(char *buf){
  memset(buf, 0, DATEBUFLEN);
  //snprintf(buf, 50, "%04i-%02i-%02i %02i:%02i:%02i UTC", tm.Year+1970, tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second);
  snprintf(buf, DATEBUFLEN, "%02i/%02i/%04i-%02i:%02i", tm.Day, tm.Month, tm.Year+1970, tm.Hour, tm.Minute);
  return buf;
}
	
TimeElements fromStrToTimeEl(char *str){ 
	TimeElements ts;
	const char s[4] = ":-/";
	char *token;
    unsigned short dateTime[7] = {0,0,0,0,0,0,0};
	//2019:07:30/03:57:30
	/* get the first token */
	token = strtok(str, s);
	dateTime[0] = (unsigned short) (*token == '*')?255:atoi(token);
	/* walk through other tokens */
	int i = 1;
	while( token != NULL && i < 8 ) {
	  token = strtok(NULL, s);
	  dateTime[i] = atoi(token);
	  ++i;
	}
	ts.Year = (dateTime[0] == 255)?year():dateTime[0];
	ts.Month = (dateTime[1] == 255)?month():dateTime[1];
	ts.Day = (dateTime[2] == 255)?day():dateTime[2];
	ts.Wday = (dateTime[3] == 255)?weekday():dateTime[3];
	ts.Hour = (dateTime[4] == 255)?hour():dateTime[4];
	ts.Minute = (dateTime[5] == 255)?minute():dateTime[5];
	ts.Second = (dateTime[6] == 255)?second():dateTime[6];
	return ts;
}

TimeElements DSTToTimeEl(char *str){ 
	TimeElements ts;
	const char s[4] = ".:/";
	char *token;
    unsigned short dateTime[6] = {0,0,0,0,0,0};
	//4.5.0/02:00:00
	/* get the first token */
	token = strtok(str, s);
	dateTime[0] = (unsigned short) atoi(token);
	/* walk through other tokens */
	int i = 1;
	while( token != NULL && i < 7 ) {
	  token = strtok(NULL, s);
	  dateTime[i] = (unsigned short) atoi(token);
	  ++i;
	}
		ts.Year = year();
	ts.Month = (dateTime[0] == 255)?month():dateTime[0];
	ts.Wday = (dateTime[2] == 255)?weekday():dateTime[2];
	ts.Day = NthDate(ts.Year, ts.Month, ts.Wday, dateTime[1]);
	ts.Hour = (dateTime[3] == 255)?hour():dateTime[3];
	ts.Minute = (dateTime[4] == 255)?minute():dateTime[4];
	ts.Second = (dateTime[5] == 255)?second():dateTime[5];
	return ts;
}
	
unsigned long makeTime(const TimeElements &tm){   	
// assemble time elements into time_t 
// note year argument is offset from 1970 (see macros in time.h to convert to other formats)
// previous version used full four digit year (or digits since 2000),i.e. 2009 was 2009 or 9
  
  int i;
  unsigned long seconds;
  static unsigned short monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31};

  // seconds from 1970 till 1 jan 00:00:00 of the given year
  seconds= tm.Year*(SECS_PER_DAY * 365);
  for (i = 0; i < tm.Year; i++) {
    if (LEAP_YEAR(i)) {
      seconds +=  SECS_PER_DAY;   // add extra days for leap years
    }
  }
  /*
  // add days for this year, months start from 1
  for (i = 1; i < tm.Month; i++) {
    if ( (i == 2) && LEAP_YEAR(tm.Year)) { 
      seconds += SECS_PER_DAY * 29;
    } else {
      seconds += SECS_PER_DAY * monthDays[i-1];  //monthDay array starts from 0
    }
  }
  */
  //from Simplified routines to get DST adjusted
  // Replacement for broken mktime() 
  for (i = 0; i < tm.Month; i++) {
    if ( (i == 1) && LEAP_YEAR(tm.Year)) { 
      seconds += SECS_PER_DAY * 29;
    } else {
      seconds += SECS_PER_DAY * monthDays[i];  //monthDay array starts from 0
    }
  }
  seconds+= (tm.Day-1) * SECS_PER_DAY;
  seconds+= tm.Hour * SECS_PER_HOUR;
  seconds+= tm.Minute * SECS_PER_MIN;
  seconds+= tm.Second;
  return (unsigned long) seconds; 
}

//from https://github.com/szczys/Automatic-Daylight-Savings-Time
/*--------------------------------------------------------------------------
  FUNC: 6/11/11 - Returns day of week for any given date
  PARAMS: year, month, date
  RETURNS: day of week (0-7 is Sun-Sat)
  NOTES: Sakamoto's Algorithm
    http://en.wikipedia.org/wiki/Calculating_the_day_of_the_week#Sakamoto.27s_algorithm
    Altered to use char when possible to save microcontroller ram
--------------------------------------------------------------------------*/
char dow(int y, char m, char d)
   {
       static char t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
       y -= m < 3;
       return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
   }

/*--------------------------------------------------------------------------
  FUNC: 6/11/11 - Returns the date for Nth day of month. For instance,
    it will return the numeric date for the 2nd Sunday of April
  PARAMS: year, month, day of week, Nth occurence of that day in that month
  RETURNS: date
  NOTES: There is no error checking for invalid inputs.
--------------------------------------------------------------------------*/
char NthDate(int year, char month, char DOW, char NthWeek){
  char targetDate = 1;
  char firstDOW = dow(year,month,targetDate);
  while (firstDOW != DOW){ 
    firstDOW = (firstDOW+1)%7;
    targetDate++;
  }
  //Adjust for weeks
  targetDate += (NthWeek-1)*7;
  return targetDate; //day of month
}



