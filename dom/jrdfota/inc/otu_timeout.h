#ifndef _OTU_TIMEOUT_H_
#define _OTU_TIMEOUT_H_

/////////////////////////////////////////////////////////////////////
// TIME OUT DEFINITIONS.
//
// All timeout definitions are given in milli-seconds unless otherwise
// specified.
//////////////////

/**
   This timeout is used to periodically refresh the cache mechanism used
   in some functions of GisusDBImpl class to reduce the load on the DB
   and increase overall system performance.

   NOTE: Time must be given in SECONDS.
 */
#define DB_CACHE_REFRESH_PERIOD   60  /* seconds */


/**
   This timeout is used to start the threads at the very beginning
   in GisusDllImpl class. We will wait this maximum amount of time
   for a thread to start before reporting an error.
*/
#define OTU_THREAD_START_TIMEOUT                   1000   /* milliseconds */


/**
   This time out is used to initiate the connection in non blocking mode
   by the ADConnection thread. In non blocking mode, we need to manage
   the time out by ourselves.
*/
#define OTU_INITIATE_CONNECTION_TIMEOUT           15000 /* milliseconds */


/**
   This time out is used during a connection to insure that the
   connection is not waiting indefinitely in case of poor performant
   network.

   After this time out, the connection is forced to shutdown.
 */
#define OTU_IDLE_TIMEOUT                          15000  /* milliseconds */

/**
   This time out is used to replace OTU_IDLE_TIMEOUT.
   This time out is used during a connection to insure that the
   connection is not waiting indefinitely in case of poor performant
   network.

   After this time out, the connection is forced to shutdown.
 */
#define OTU_CONN_IDLE_TIMEOUT                     60000  /* milliseconds */

/**
   This time out is used by the Master server when forwarding
   authorization request to all Slaves. Each slave is supposed
   to confirm that the authorization has been well recorded.

   If a time out occurs before receiving the answers from all
   the Slaves, then only the one who responded will be sent to
   the client.

 */
#define OTU_AUTH_PROPAGATION_TIMEOUT              10000  /* milliseconds */

/**
   This time out is used when encountering network errors while downloading
   the software. In such condition, a pause is forced during a period
   of time. This period of time is the value given to this macro. After
   this time out, the download is automatically resumed.
*/
#define OTU_ON_DOWNLOAD_FAILURE_PAUSE_TIMEOUT     30000   /* milliseconds */


/**
   This time out is used when the server send the challenge, the client
   must respond the correct challenge result in 10s.
   This time is used by servers. Do not use it in clients.
*/
#define OTU_CHECK_CHALLENGE_TIMEOUT     10000  /* milliseconds */

#endif /* _OTU_TIMEOUT_H_ */
