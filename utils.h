/* ========================================================================= *
 * File: utils.h
 *
 * Description:
 *    DBUS ping-pong utility file.
 *
 * Copyright (C) 2005-2011 Nokia Corporation.
 *
 * Author: Leonid Moiseichuk <leonid.moiseichuk@nokia.com>
 * Contact: Eero Tamminen <eero.tamminen@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License 
 * version 2 as published by the Free Software Foundation. 
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * ========================================================================= */

#ifndef UTILS_H_USED
#define UTILS_H_USED

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= *
 * Includes
 * ========================================================================= */

#include <sys/time.h>
#include <unistd.h>

#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h>

/* ========================================================================= *
 * Definitions.
 * ========================================================================= */

extern time_t base_time;

/* Moving to base seconds */
#define BASE_US(s)    ((unsigned)((s) - base_time) * 1000000u)

/* Read struct timeval in microseconds with adjusting system start moment */
#define TV_TO_US(tv)    (BASE_US(tv.tv_sec) + (unsigned)tv.tv_usec)

/* Type of DBUS server */
/* #define DBUS_SERVER_TYPE   DBUS_BUS_SYSTEM */
#define DBUS_SERVER_TYPE   DBUS_BUS_SESSION

/* Definitions for proper addressing for ping-pong */
#define SERVICE   "com.nokia.PingTest"
#define OBJECT    "/com/nokia/PingTest"
#define INTERFACE "com.nokia.PingTest"
#define METHOD    "ping"

/* ========================================================================= *
 * Methods.
 * ========================================================================= */

/* ------------------------------------------------------------------------- *
 * set_base_time -- set test base time.
 * parameters: message
 * returns: nothing
 * ------------------------------------------------------------------------- */
void set_base_time(void);

/* ------------------------------------------------------------------------- *
 * get_time_us -- returns current time in microseconds.
 * parameters: nothing
 * returns: time in microseconds
 * ------------------------------------------------------------------------- */
static inline unsigned get_time_us(void)
{
   struct timeval tv;
   return (0 == gettimeofday(&tv, NULL) ? TV_TO_US(tv) : BASE_US(time(NULL)));
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
