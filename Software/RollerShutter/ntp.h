#ifndef __NTP_h
#define __NTP_h

#include "common.h"

//convenience macros to convert to and from tm years 
#define  tmYearToCalendar(Y) ((Y) + 1970)  // full four digit year 
#define  CalendarYrToTm(Y)   ((Y) - 1970)
//#define  tmYearToY2k(Y)      ((Y) - 30)    // offset is from 2000
//#define  y2kYearToTm(Y)      ((Y) + 30)   
/*
/* Useful Constants */
#define SECS_PER_MIN  ((time_t)(60UL))
#define SECS_PER_HOUR ((time_t)(3600UL))
#define SECS_PER_DAY  ((time_t)(SECS_PER_HOUR * 24UL))
#define DAYS_PER_WEEK ((time_t)(7UL))
#define SECS_PER_WEEK ((time_t)(SECS_PER_DAY * DAYS_PER_WEEK))
#define SECS_PER_YEAR ((time_t)(SECS_PER_DAY * 365UL)) // TODO: ought to handle leap years
#define SECS_YR_2000  ((time_t)(946684800UL)) // the time at the start of y2k
/* Useful Macros for getting elapsed time *//*
#define numberOfSeconds(_time_) ((_time_) % SECS_PER_MIN)  
#define numberOfMinutes(_time_) (((_time_) / SECS_PER_MIN) % SECS_PER_MIN) 
#define numberOfHours(_time_) (((_time_) % SECS_PER_DAY) / SECS_PER_HOUR)
#define dayOfWeek(_time_) ((((_time_) / SECS_PER_DAY + 4)  % DAYS_PER_WEEK)+1) // 1 = Sunday
#define elapsedDays(_time_) ((_time_) / SECS_PER_DAY)  // this is number of days since Jan 1 1970
#define elapsedSecsToday(_time_) ((_time_) % SECS_PER_DAY)   // the number of seconds since last midnight 
// The following macros are used in calculating alarms and assume the clock is set to a date later than Jan 1 1971
// Always set the correct time before settting alarms
#define previousMidnight(_time_) (((_time_) / SECS_PER_DAY) * SECS_PER_DAY)  // time at the start of the given day
#define nextMidnight(_time_) (previousMidnight(_time_)  + SECS_PER_DAY)   // time at the end of the given day 
#define elapsedSecsThisWeek(_time_) (elapsedSecsToday(_time_) +  ((dayOfWeek(_time_)-1) * SECS_PER_DAY))   // note that week starts on day 1
#define previousSunday(_time_) ((_time_) - elapsedSecsThisWeek(_time_))      // time at the start of the week for the given time
#define nextSunday(_time_) (previousSunday(_time_)+SECS_PER_WEEK)          // time at the end of the week for the given time
*/

/* Useful Macros for converting elapsed time to a unsigned long *//*
#define minutesTounsigned long ((M)) ( (M) * SECS_PER_MIN)  
#define hoursTounsigned long   ((H)) ( (H) * SECS_PER_HOUR)  
#define daysTounsigned long    ((D)) ( (D) * SECS_PER_DAY) // fixed on Jul 22 2011
#define weeksTounsigned long   ((W)) ( (W) * SECS_PER_WEEK) 
*/
typedef struct  { 
  unsigned short Second; 
  unsigned short Minute; 
  unsigned short Hour; 
  unsigned short Wday;   // day of week, sunday is day 1
  unsigned short Day;
  unsigned short Month; 
  unsigned short Year;   // offset from 1970; 
} 	TimeElements;

unsigned long getUNIXTime();
//unsigned long ntpUnixTime();
//unsigned long updateNTP();
void setNtpServer(byte, const char*);
unsigned long updateNTP(unsigned long);
void ICACHE_FLASH_ATTR	user_check_sntp_stamp(void *);
void sntpInit();
//void sampleCurrTime(unsigned long timeInput);
bool sampleCurrTime(void);
int hour();
int hourFormat12();
unsigned short isAM();
unsigned short isPM();
int minute();
int second();
int day();
int weekday();
int month();
int year();
void adjustTime(long);
void setTimeZone(short);
void setSyncInterval(unsigned long);
void setNtpServer(char *);
void setSDT(bool);
TimeElements DSTToTimeEl(char *);
char *printUNIXTime(char*);
char *printUNIXTimeMin(char*);
char dow(int, char, char);
char NthDate(int, char, char, char);
TimeElements fromStrToTimeEl(char *);
unsigned long makeTime(const TimeElements&);
#endif //__NTP_h


