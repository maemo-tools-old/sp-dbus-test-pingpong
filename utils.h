/* ========================================================================= *
 * File: utils.h
 *
 * Copyright (C) 2005 Nokia. All rights reserved.
 *
 * Author: Leonid Moiseichuk <leonid.moiseichuk@nokia.com>
 *
 * Description:
 *    DBUS ping-pong utility file.
 *
 * History:
 *
 * 14-Oct-2005 Leonid Moiseichuk
 * - initial version created after discussion with Eero.
 * ========================================================================= */

#ifndef UTILS_H_USED
#define UTILS_H_USED

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= *
 * Includes
 * ========================================================================= */

#include <time.h>
#include <unistd.h>

#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h>

/* ========================================================================= *
 * Definitions.
 * ========================================================================= */

/* Division with rounding up */
#define DIV(val,div)    (((val) + ((div) >> 1)) / (div))

/* Clock source */
#define TIME_SOURCE     CLOCK_REALTIME

/* Read struct timespec in microseconds, limit is about 4000 seconds from by system start */
#define TS_TO_US(ts)    ((ts).tv_sec * 1000000u + DIV((ts).tv_nsec,1000u))

/* Type of DBUS server */
#define DBUS_SERVER_TYPE   DBUS_BUS_SYSTEM
/* #define DBUS_SERVER_TYPE   DBUS_BUS_SESSION */

/* Definitions for proper addressing for ping-pong */
#define SERVICE   "com.nokia.PingTest"
#define OBJECT    "/com/nokia/PingTest"
#define INTERFACE "com.nokia.PingTest"
#define METHOD    "ping"

/* ========================================================================= *
 * Methods.
 * ========================================================================= */

/* ------------------------------------------------------------------------- *
 * get_time_us -- returns current time in microseconds.
 * parameters: nothing
 * returns: time in microseconds, 0-4K
 * ------------------------------------------------------------------------- */
static inline unsigned get_time_us(void)
{
   struct timespec ts;
   return (0 == clock_gettime(TIME_SOURCE, &ts) ? TS_TO_US(ts) : 0);
} /* get_time_us */

/* ------------------------------------------------------------------------- *
 * fatal -- print the fatal message and die.
 * parameters: message
 * returns: nothing
 * ------------------------------------------------------------------------- */
void fatal(const char* msg);

/* ------------------------------------------------------------------------- *
 * check_dbus_error -- checks and prints the error message and die.
 * parameters: dbus fatal to check
 * returns: 0 or die
 * ------------------------------------------------------------------------- */
int check_dbus_error(DBusError* error);


#ifdef __cplusplus
}
#endif

#endif /* UTILS_H_USED */
