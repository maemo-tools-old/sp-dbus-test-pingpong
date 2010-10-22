/* ========================================================================= *
 * File: utils.c
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
