/* ========================================================================= *
 * File: dping.c
 *
 * Copyright (C) 2005 Nokia. All rights reserved.
 *
 * Author: Leonid Moiseichuk <leonid.moiseichuk@nokia.com>
 *
 * Description:
 *    DBUS ping utility file.
 *
 * History:
 *
 * 14-Oct-2005 Leonid Moiseichuk
 * - initial version created after discussion with Eero.
 * ========================================================================= */

/* ========================================================================= *
 * Includes
 * ========================================================================= */

#include "utils.h"

/* ========================================================================= *
 * Definitions.
 * ========================================================================= */


/* ========================================================================= *
 * Local data.
 * ========================================================================= */

/* ========================================================================= *
 * Local methods.
 * ========================================================================= */

/* ========================================================================= *
 * Public methods: main
 * ========================================================================= */

int main(void)
{
   DBusError       error;
   DBusConnection* session;
   DBusMessage*    message;
   unsigned        counter = 0;
   unsigned        timestamp;

   g_type_init();
   dbus_error_init (&error) ;
   session = dbus_bus_get(DBUS_SERVER_TYPE, &error);
   if (check_dbus_error(&error) || NULL == session )
   {
      fatal("NULL session in dping.c main()");
   }

   /* Flooding */
   while (1)
   {
      /* Make a ping to the server */
      message = dbus_message_new_method_call(SERVICE, OBJECT, INTERFACE, METHOD);
      if ( NULL == message )
      {
         fatal("NULL message in dping.c main()");
      }

      dbus_message_set_no_reply (message, TRUE);
      timestamp = get_time_us();
      dbus_message_append_args(message, DBUS_TYPE_UINT32, &counter, DBUS_TYPE_UINT32, &timestamp, DBUS_TYPE_INVALID);
      counter++;
      dbus_connection_send(session, message, NULL);
      dbus_message_unref(message);
      dbus_connection_flush(session);
   }

   return 0;
} /* main */

/* ========================================================================= *
 *                    No more code in file dping.c                           *
 * ========================================================================= */
