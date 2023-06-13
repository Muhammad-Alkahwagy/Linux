/*
 *
 *     add-client.c: client program, takes two numbers as input,
 *                   sends to server for addition,
 *                   gets result from server,
 *                   prints the result on the screen
 *
 */

#include <stdio.h>     //provides input/output operations such as printf and scanf.
#include <stdlib.h>    //provides general utility functions such as memory allocation and random number generation
#include <unistd.h>    //provides access to various POSIX operating system API functions such as fork, exec, and sleep.
#include <string.h>    //provides string manipulation functions such as strlen, strcpy, and strcat
#include <stdbool.h>   //provides the Boolean data type and related macros such as true and false.
#include <ctype.h>     //provides functions for testing and converting characters such as isalpha, isdigit, and toupper.

#include <dbus/dbus.h> //header file for the D-Bus library, 


const char *const INTERFACE_NAME = "in.softprayog.dbus_example";
const char *const SERVER_BUS_NAME = "in.softprayog.add_server";
const char *const CLIENT_BUS_NAME = "in.softprayog.add_client";
const char *const SERVER_OBJECT_PATH_NAME = "/in/softprayog/adder";
const char *const CLIENT_OBJECT_PATH_NAME = "/in/softprayog/add_client";
const char *const METHOD_NAME = "add_numbers";

/*
DBusError: 
const char * 	name      : npublic error name 
const char * 	message   : public error message
unsigned int 	dummy1    : 1 : placeholder 
unsigned int 	dummy2    : 1 : placeholder 
unsigned int 	dummy3    : 1 : placeholder 
unsigned int 	dummy4    : 1 : placeholder 
unsigned int 	dummy5    : 1 : placeholder 
void * 	        padding1  : 1 : placeholder 
*/
DBusError dbus_error;
void print_dbus_error (char *str);

int main (int argc, char **argv)
{
    DBusConnection *conn;
    int ret;
    /*
    Array of char to receive input 
    */
    char input [80];

 /*
 Initializes a DBusError structure
*/
    dbus_error_init (&dbus_error);
/*
Func : dbus_bus_get
Desc : Connects to a bus daemon and registers the client with it.

Parameters
type	bus type
error	address where an error can be returned.

Returns
a DBusConnection with new ref or NULL on error
----------------------------------------------------------
bus type:
DBUS_BUS_SESSION : The login session bus.
DBUS_BUS_SYSTEM  : The systemwide bus.
DBUS_BUS_STARTER : The bus that started us, if any
*/
    conn = dbus_bus_get (DBUS_BUS_SESSION, &dbus_error);
/*
Checks whether an error occurred (the error is set).
*/
    if (dbus_error_is_set (&dbus_error))
        print_dbus_error ("dbus_bus_get");

    if (!conn) 
        exit (1);

    printf ("Please type two numbers: ");

/*
char *fgets(char *str, int n, FILE *stream);

str :  is a pointer to an array of characters where the line read is stored.
n   : is the maximum number of characters to be read (including the null character at the end).
stream  : is a pointer to the input stream from which to read the line.


*/
    while (fgets (input, 78, stdin) != NULL) {

        // Get a well known name
        while (1) {
/*
dbus_bus_request_name (DBusConnection *connection, const char *name, unsigned int flags, DBusError *error)
 	Asks the bus to assign the given name to this connection by invoking the RequestName method on the bus
*/

            ret = dbus_bus_request_name (conn, CLIENT_BUS_NAME, 0, &dbus_error);
/*Check if name had no existing owner, and the caller is now the primary owner; or that the name had an owner, and the caller specified*/
            if (ret == DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) 
               break;
/*
DBUS_REQUEST_NAME_REPLY_IN_QUEUE : Service could not become the primary owner and has been placed in the queue
*/
            if (ret == DBUS_REQUEST_NAME_REPLY_IN_QUEUE) {
               fprintf (stderr, "Waiting for the bus ... \n");
               sleep (1);
               continue;
            }
 //ecks whether an error occurred            
            if (dbus_error_is_set (&dbus_error))
               print_dbus_error ("dbus_bus_get");
        }
        
        DBusMessage *request;

/*
dbus_message_new_method_call (const char *destination, const char *path, const char *iface,
 const char *method)
Constructs a new message to invoke a method on a remote object

Parameters: 
destination	name that the message should be sent to or NULL
path	object path the message should be sent to
iface	interface to invoke method on, or NULL
method	method to invoke
Returns
a new DBusMessage, free with dbus_message_unref()

*/

        if ((request = dbus_message_new_method_call (SERVER_BUS_NAME, SERVER_OBJECT_PATH_NAME, 
                           INTERFACE_NAME, METHOD_NAME)) == NULL) {
            fprintf (stderr, "Error in dbus_message_new_method_call\n");
            exit (1);
        }

        DBusMessageIter iter;
        /*	Initializes a DBusMessageIter for appending arguments to the end of a message*/
        dbus_message_iter_init_append (request, &iter);
        char *ptr = input;
        /*Appends a basic-typed value to the message
         Parameters
            iter	the append iterator
            type	the type of the value
            value	the address of the value
            Returns
            FALSE if not enough memory
        */
        if (!dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &ptr)) {
            fprintf (stderr, "Error in dbus_message_iter_append_basic\n");
            exit (1);
        }
        DBusPendingCall *pending_return;

        /*
        Queues a message to send, as with dbus_connection_send(), 
        but also returns a DBusPendingCall used to receive a reply to the message.
        Parameters
            connection	the connection
            message	the message to send
            pending_return	return location for a DBusPendingCall object, or NULL if connection is disconnected or when you try to send Unix file descriptors on a connection that does not support them.
            timeout_milliseconds	timeout in milliseconds, -1 (or DBUS_TIMEOUT_USE_DEFAULT) for default or DBUS_TIMEOUT_INFINITE for no timeout
            Returns
            FALSE if no memory, TRUE otherwise.
        
        */
        if (!dbus_connection_send_with_reply (conn, request, &pending_return, -1)) {
            fprintf (stderr, "Error in dbus_connection_send_with_reply\n");
            exit (1);
        }

        if (pending_return == NULL) {
            fprintf (stderr, "pending return is NULL");
            exit (1);
        }
    /*Blocks until the outgoing message queue is empty*/
        dbus_connection_flush (conn);
    /*Decrements the reference count of a DBusMessage, freeing the message if the count reaches 0.*/            
        dbus_message_unref (request);	
    /*Block until the pending call is completed*/
        dbus_pending_call_block (pending_return);

        DBusMessage *reply;
        /*Gets the reply, or returns NULL if none has been received yet*/
        if ((reply = dbus_pending_call_steal_reply (pending_return)) == NULL) {
            fprintf (stderr, "Error in dbus_pending_call_steal_reply");
            exit (1);
        }
     /*Decrements the reference count on a pending call, freeing it if the count reaches 0.*/

        dbus_pending_call_unref	(pending_return);

        char *s;
        /*Gets arguments from a message given a variable argument list.
        
        Parameters
            message	the message
            error	error to be filled in on failure
            first_arg_type	the first argument type
            ...	location for first argument value, then list of type-location pairs
            Returns FALSE if the error was set
        */
        if (dbus_message_get_args (reply, &dbus_error, DBUS_TYPE_STRING, &s, DBUS_TYPE_INVALID)) {
            printf ("%s\n", s);
        }
        else
        {
             fprintf (stderr, "Did not get arguments in reply\n");
             exit (1);
        }
        dbus_message_unref (reply);	

        if (dbus_bus_release_name (conn, CLIENT_BUS_NAME, &dbus_error) == -1) {
             fprintf (stderr, "Error in dbus_bus_release_name\n");
             exit (1);
        }

        printf ("Please type two numbers: ");
    }

    return 0;
}

void print_dbus_error (char *str) 
{
    fprintf (stderr, "%s: %s\n", str, dbus_error.message);
    dbus_error_free (&dbus_error);
}
