/* ========================================================================= *
 * File: dpong.c
 *
 * Copyright (C) 2005 Nokia. All rights reserved.
 *
 * Author: Leonid Moiseichuk <leonid.moiseichuk@nokia.com>
 *
 * Description:
 *    DBUS pong utility file (= dping server).
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
#include <memory.h>

#include <glib/gmain.h>
#include "utils.h"
#include <dbus/dbus.h>

/* ========================================================================= *
 * Definitions.
 * ========================================================================= */

typedef struct
{
   unsigned report;
   unsigned counter;
   unsigned recv;
   unsigned lost;
   unsigned min_time;
   unsigned max_time;
   unsigned tot_time;
   unsigned initial_ts;

   GMainContext* context;
   GMainLoop*    loop;
} SERVER;

/* ========================================================================= *
 * Local data.
 * ========================================================================= */

static unsigned s_report = 100;
static SERVER*  s_server;
static DBusError s_error;

/* ========================================================================= *
 * Local methods.
 * ========================================================================= */

static SERVER* server_new(void)
{
   SERVER* server = (SERVER*)malloc(sizeof(SERVER));

   if (server)
   {
      memset(server, 0, sizeof(*server));
      server->report   = s_report;
      server->min_time = (unsigned)-1;
      server->max_time = 0;
      server->initial_ts = get_time_us();
   }

   return server;
} /* server_new */

static void server_delete(SERVER* server)
{
   free(server);
}

static DBusHandlerResult ping_object_message_handler_cb(DBusConnection* a_conn, DBusMessage *a_message, void* a_user_data)
{
   /* Make compiler happy */
   a_conn = a_conn;
   a_user_data = a_user_data;

   /* Is it our call?*/
   if (dbus_message_is_method_call(a_message, INTERFACE, METHOD))
   {
      unsigned counter;
      unsigned timestamp;

      if ( dbus_message_get_args(a_message, &s_error, DBUS_TYPE_UINT32, &counter, DBUS_TYPE_UINT32, &timestamp, DBUS_TYPE_INVALID) )
      {
         const unsigned diff = get_time_us() - timestamp;
         SERVER* server = s_server;  /* Probably will be identified for each client */

         /* If we're measuring the roundtrip performance, then just reply
            and be done with it */ 
         if (!dbus_message_get_no_reply(a_message))
         {
            DBusMessage *reply;
            reply = dbus_message_new_method_return(a_message);
            if (NULL == reply)
            {
               fatal("NULL reply message in dpong.c");
            }
            dbus_message_append_args(reply, DBUS_TYPE_UINT32, &counter, DBUS_TYPE_UINT32, &timestamp, DBUS_TYPE_INVALID);
            if (!dbus_connection_send(a_conn, reply, NULL))
            {
               fprintf(stderr, "sending the reply failed\n");
            }
            dbus_message_unref(reply);
            dbus_connection_flush(a_conn);
         }
         else
         {

            if (server->min_time > diff)
               server->min_time = diff;

            if (server->max_time < diff)
               server->max_time = diff;

            server->tot_time += diff;

            /* Check message losing */
            if (counter == server->counter)
            {
               server->recv++;
            }
            else
            {
               /* Number of messages lost */
               server->lost += (server->counter < counter ? counter - server->counter : server->report - server->counter);
            }

            /* Increase the index of message */
            server->counter = counter + 1;
            /* Reporting if it necessary */
            if (0 == (server->counter % server->report))
            {
               fprintf (stdout, "Timestamp: %u microseconds\n", get_time_us());
               fprintf (
                        stdout, "MESSAGES recv %u lost %u LATENCY min %u avg %u max %u THOUGHPUT %.1f m/s\n",
                       server->recv, server->lost, server->min_time,
                       DIV(server->tot_time,server->recv), server->max_time, server->recv/((double)(get_time_us()-server->initial_ts)/1000000)
                       );

               /* Setup values for new test cycle */
               if (server->counter < server->report)
               {
                  /* We lose some messages and receive the first one from
                      the new cycle */
                  server->recv     = 1;
                  server->lost     = counter - 1;
                  server->min_time = diff;
                  server->tot_time = diff;
                  server->max_time = diff;
               }
               else
               {
                  /* Normal flow, all messages received */
                  server->recv     = 0;
                  server->lost     = 0;
                  server->min_time = (unsigned)(-1);
                  server->tot_time = 0;
                  server->max_time = 0;
                  server->initial_ts = get_time_us();
               }
            }
         }
         return 0;
      }
   }

   return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
} /* ping_object_message_handler_cb */

static void ping_object_unregistered_cb(DBusConnection* a_conn, void* a_user_data)
{
   /* Make compiler happy */
   a_conn = a_conn;
   a_user_data = a_user_data;
   fprintf (stderr, "PingObject unregistered") ;
} /* ping_object_unregistered_cb */


/* ========================================================================= *
 * Public methods: main
 * ========================================================================= */

int main(int argc, const char* argv[])
{
   DBusConnection* session;
#if 0
   dbus_uint32_t   result;
#endif
   static const DBusObjectPathVTable vtable = {
      .unregister_function = ping_object_unregistered_cb,
      .message_function = ping_object_message_handler_cb
   } ;

   if (2 == argc)
   {
      s_report = (unsigned)atoi(argv[1]);
   }

   g_type_init();
   s_server = server_new();

   printf ("ping server\n");
   printf ("reporting period is set to %u messages\n", s_report);

   dbus_error_init (&s_error) ;

   /* Setup main loop */
   s_server->context = g_main_context_new();
   s_server->loop = g_main_loop_new(s_server->context, FALSE) ;

   session = dbus_bus_get(DBUS_SERVER_TYPE, &s_error);
   if (check_dbus_error(&s_error) || NULL == session )
   {
      fatal("NULL session in dpong.c main()");
   }

   dbus_connection_setup_with_g_main (session, s_server->context) ;
   dbus_bus_request_name (session, SERVICE, DBUS_NAME_FLAG_ALLOW_REPLACEMENT, &s_error);
   if ( check_dbus_error(&s_error) )
   {
      fatal("Unable to use service name " SERVICE);
   }

#if 0
   /* Create registry */
   registry = bus_registry_new(BusContext*);
   if ( !registry )
   {
      fatal("unable to create registry");
   }

   if ( bus_registry_acquire_service(registry, session, SERVICE, 0, &result, transaction, &s_error) )
   {
      fprintf (stderr, "bus_registry_acquire_service --> %u + %u\n", result, s_error);
      fatal("unable to acquire service");
   }
#endif

   if ( !dbus_connection_register_object_path(session, OBJECT, &vtable, NULL) )
   {
      fatal("failed registering object path");
   }

   fprintf (stderr, "ping server base service is %s\n", dbus_bus_get_unique_name(session));

   fprintf (stderr, "ping server is ready to accept calls\n");
   g_main_loop_run (s_server->loop) ;
   dbus_connection_close (session);
   server_delete(s_server);

   return 0;
} /* main */

/* ========================================================================= *
 *                    No more code in file dpong.c                           *
 * ========================================================================= */
