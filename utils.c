/* ========================================================================= *
 * File: utils.c
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

/* ========================================================================= *
 * Includes
 * ========================================================================= */

#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

/* ========================================================================= *
 * Definitions.
 * ========================================================================= */


/* ========================================================================= *
 * Local data.
 * ========================================================================= */

time_t base_time;

/* ========================================================================= *
 * Local methods.
 * ========================================================================= */

/* ========================================================================= *
 * Public methods.
 * ========================================================================= */

/* ------------------------------------------------------------------------- *
 * set_base_time -- set test base time.
 * parameters: message
 * returns: nothing
 * ------------------------------------------------------------------------- */
void set_base_time(void)
{
  const time_t t = time(NULL);
  base_time = t - (t % (30 * 60));  /* test shall start in 30 minutes boundary */
} /* set_base_time */


/* ------------------------------------------------------------------------- *
 * fatal -- prints the error message and die.
 * parameters: message
 * returns: nothing
 * ------------------------------------------------------------------------- */
void fatal(const char* msg)
{
   fprintf (stderr, "%s\n", msg);
   exit(-1);
} /* fatal */

/* ------------------------------------------------------------------------- *
 * check_dbus_error -- print the error message and die.
 * parameters: dbus error to check
 * returns: 1 or 0
 * ------------------------------------------------------------------------- */
int check_dbus_error(DBusError* error)
{
   if ( dbus_error_is_set (error) )
   {
      fprintf (stderr, "dbus error %s %s\n", error->name, error->message);
      exit(-1);
   }

   return 0;
} /* check_dbus_error */




/* ========================================================================= *
 *                    No more code in file utils.c                           *
 * ========================================================================= */
