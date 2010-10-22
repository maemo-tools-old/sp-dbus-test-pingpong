/* ========================================================================= *
 * File: dping.c
 * vim: et:ts=3:sw=3:
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

/* ========================================================================= *
 * Definitions.
 * ========================================================================= */

typedef struct
{
   unsigned report;
   unsigned counter;
   unsigned recv;
   unsigned lost;
   unsigned dpong_min_time;
   unsigned dpong_max_time;
   unsigned dpong_tot_time;
   unsigned dping_min_time;
   unsigned dping_max_time;
   unsigned dping_tot_time;
   unsigned total_min_time;
   unsigned total_max_time;
   unsigned total_tot_time;
   unsigned initial_ts;
   unsigned tot_pending;

   GMainContext *context;
   GMainLoop *loop;

} STATS;


/* ========================================================================= *
 * Local data.
 * ========================================================================= */

static unsigned s_report = 1000;
static DBusError s_error;
static STATS*  s_stats;

/* ========================================================================= *
 * Local methods.
 * ========================================================================= */

static STATS *
stats_new(void)
{
   STATS* stats = (STATS*)malloc(sizeof(STATS));

   if (stats)
   {
      memset(stats, 0, sizeof(*stats));
      stats->report     = s_report; /** FIXME ** */
      stats->initial_ts = get_time_us();
   }

   return stats;
} /* stats_new */

static void
stats_delete(STATS* stats)
{
   free(stats);
}

static void
notify_func(DBusPendingCall *pending, void *data)
{
   unsigned counter, orig_timestamp, timestamp;
   DBusMessage *reply = dbus_pending_call_steal_reply(pending);
   int type = dbus_message_get_type(reply);

  /* Make the compiler happy */
   data = data;

   switch (type)
   {
      case DBUS_MESSAGE_TYPE_METHOD_RETURN:
         if ( dbus_message_get_args(reply, &s_error,
                  DBUS_TYPE_UINT32, &counter,
                  DBUS_TYPE_UINT32, &orig_timestamp,
                  DBUS_TYPE_UINT32, &timestamp,
                  DBUS_TYPE_INVALID))
          {
             STATS* stats = s_stats;
             const unsigned now = get_time_us();
             const unsigned diff = (now > orig_timestamp ? now - orig_timestamp : 0);
             const unsigned dping_dpong_diff = timestamp-orig_timestamp;

             if (diff > 0)
             {
               if (0 == stats->dping_min_time || stats->dping_min_time > diff)
                 stats->dping_min_time = diff;

               if (0 == stats->dpong_min_time || stats->dpong_min_time > dping_dpong_diff)
                 stats->dpong_min_time = dping_dpong_diff;

               if (0 == stats->dping_max_time || stats->dping_max_time < diff)
                 stats->dping_max_time = diff;

               if (0 == stats->dpong_max_time || stats->dpong_max_time < dping_dpong_diff)
                 stats->dpong_max_time = dping_dpong_diff;

               if (0 == stats->total_min_time || stats->total_min_time > (stats->dpong_min_time + stats->dping_min_time))
                 stats->total_min_time = (stats->dpong_min_time + stats->dping_min_time);

               if (0 == stats->total_max_time || stats->total_max_time < (stats->dpong_max_time + stats->dping_max_time))
                 stats->total_max_time = (stats->dpong_max_time + stats->dping_max_time);

               stats->dping_tot_time += diff;
               stats->dpong_tot_time += dping_dpong_diff;
               stats->total_tot_time += (diff + dping_dpong_diff);
             }

            /* Check message losing */
            if (counter == stats->counter)
            {
               stats->recv++;
            }
            else
            {
               /* Number of messages lost */
               stats->lost += (stats->counter < counter ? counter - stats->counter : stats->report - stats->counter);
            }
                        /* Increase the index of message */
            stats->counter = counter + 1;


                        /* Reporting if it necessary */
            if (0 == (stats->counter % stats->report))
            {
               fprintf (stdout, "dping timestamp: %u microseconds\n", now);
               fprintf (
                        stdout, "Dping->dpong statistics: LATENCY min %u avg %u max %u\n",
                       stats->dpong_min_time,
                       (stats->dpong_tot_time / stats->recv), stats->dpong_max_time);

               fprintf (stdout, "dping<-dpong statistics: LATENCY min %u avg %u max %u\n",
                       stats->dping_min_time, (stats->dping_tot_time / stats->recv), stats->dping_max_time);
               fprintf (stdout, "Statistics for the whole roundtrip:\n");
               fprintf (stdout, "MESSAGES recv %u lost %u LATENCY min %u avg %u max %u THROUGHPUT %.1f m/s\n\n",
                       stats->recv, stats->lost,
                       stats->total_min_time, (stats->total_tot_time / stats->recv), stats->total_max_time,
                       stats->recv/((double)(now - stats->initial_ts)/1000000));


                             /* Setup values for new test cycle */
               if (stats->counter < stats->report)
               {
                  /* We lose some messages and receive the first one from
                      the new cycle */
                  stats->recv     = 1;
                  stats->lost     = counter - 1;
                  stats->dping_min_time = diff;
                  stats->dping_tot_time = diff;
                  stats->dping_max_time = diff;
                  stats->dpong_min_time = dping_dpong_diff;
                  stats->dpong_tot_time = dping_dpong_diff;
                  stats->dpong_max_time = dping_dpong_diff;
                  stats->total_min_time = diff + dping_dpong_diff;
                  stats->total_tot_time = diff + dping_dpong_diff;
                  stats->total_max_time = diff + dping_dpong_diff;
               }
               else
               {
                  /* Normal flow, all messages received */
                  stats->recv     = 0;
                  stats->lost     = 0;
                  stats->dping_min_time = 0;
                  stats->dping_tot_time = 0;
                  stats->dping_max_time = 0;
                  stats->dpong_min_time = 0;
                  stats->dpong_tot_time = 0;
                  stats->dpong_max_time = 0;
                  stats->total_min_time = 0;
                  stats->total_tot_time = 0;
                  stats->total_max_time = 0;
                  stats->initial_ts = get_time_us();
               }

            }

          }
         break;
      case DBUS_MESSAGE_TYPE_ERROR:
         fprintf(stdout, "Got an error!\n");
         break;
      default:
         fprintf(stdout, "Got an unexpected reply!\n");
         break;
   }
   dbus_pending_call_unref(pending);
   dbus_message_unref(reply);
}

/* ========================================================================= *
 * Public methods: main
 * ========================================================================= */

int main(int argc, char *argv[])
{
   DBusError       error;
   DBusConnection* session;
   DBusMessage*    message;
   unsigned        counter = 0;
   unsigned        timestamp;
   dbus_bool_t     oneway = TRUE;

   set_base_time();
   g_type_init();
   s_stats = stats_new();

   dbus_error_init (&error) ;
   session = dbus_bus_get(DBUS_SERVER_TYPE, &error);
   if (check_dbus_error(&error) || NULL == session )
   {
      fatal("NULL session in dping.c main()");
   }

   s_stats->context = g_main_context_new();
   s_stats->loop = g_main_loop_new(s_stats->context, TRUE) ;

   dbus_connection_setup_with_g_main (session, s_stats->context) ;

   /* Check if user wants to use the roundtrip mode */

   if ( argc == 2 && !strcmp(argv[1], "-r") )
   {
      oneway = FALSE;
   }
   else if ( argc == 3 && !strcmp(argv[1], "-r") )
   {
     unsigned report = (unsigned)atoi(argv[2]);
     if ( report != 0 )
     {
        s_stats->report = report;
     }
     oneway = FALSE;
   }

   if (oneway == FALSE)
   {
     fprintf (stdout, "Measuring the roundtrip performance instead of one-way performance.\n");
     fprintf (stdout, "Calculating the statistics for batches of %u messages\n", s_stats->report);
   }

   /* Flooding */
   while (1)
   {
      DBusPendingCall *pending;
      /* Make a ping to the server */
      message = dbus_message_new_method_call(SERVICE, OBJECT, INTERFACE, METHOD);
      if ( NULL == message )
      {
         fatal("NULL message in dping.c main()");
      }

      dbus_message_set_no_reply (message, oneway);
      timestamp = get_time_us();
      dbus_message_append_args(message,
            DBUS_TYPE_UINT32, &counter,
            DBUS_TYPE_UINT32, &timestamp,
            DBUS_TYPE_INVALID);
      counter++;
      if (!oneway)
      {
         if (dbus_connection_send_with_reply(session, message, &pending, -1) )
         {
            if (!dbus_pending_call_set_notify(pending, notify_func, NULL, NULL) || pending == NULL)
            {
               fatal("Error while setting notify!\n");
            }
         }
      }
      else
      {
         dbus_connection_send(session, message, NULL);
      }
      dbus_message_unref(message);
      dbus_connection_flush(session);

      if (!oneway)
      {
         /* Visit the main loop to get any potential replies.
            Perhaps we should have the sender added as a g_source instead? */
         g_main_context_iteration(s_stats->context, FALSE);
      }
   }

   dbus_connection_close (session);
   stats_delete(s_stats);

   return 0;
} /* main */

/* ========================================================================= *
 *                    No more code in file dping.c                           *
 * ========================================================================= */
