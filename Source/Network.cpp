#include "Core.h"

#include "Network.h"
#include "Text.h"

#ifdef FO_WINDOWS

# include <WinError.h>

const char* GetLastSocketError()
{
    static THREAD char str[MAX_FOTEXT];
    int                error = WSAGetLastError();

    # define CASE_SOCK_ERROR( code, message ) \
        case code:                            \
            Str::Format( str, # code ", %d, "message, code ); break

    switch( error )
    {
        default:
            Str::Format( str, "%d, unknown error code.", error );
            break;
            CASE_SOCK_ERROR( WSAEINTR, "A blocking operation was interrupted by a call to WSACancelBlockingCall." );
            CASE_SOCK_ERROR( WSAEBADF, "The file handle supplied is not valid." );
            CASE_SOCK_ERROR( WSAEACCES, "An attempt was made to access a socket in a way forbidden by its access permissions." );
            CASE_SOCK_ERROR( WSAEFAULT, "The system detected an invalid pointer address in attempting to use a pointer argument in a call." );
            CASE_SOCK_ERROR( WSAEINVAL, "An invalid argument was supplied." );
            CASE_SOCK_ERROR( WSAEMFILE, "Too many open sockets." );
            CASE_SOCK_ERROR( WSAEWOULDBLOCK, "A non-blocking socket operation could not be completed immediately." );
            CASE_SOCK_ERROR( WSAEINPROGRESS, "A blocking operation is currently executing." );
            CASE_SOCK_ERROR( WSAEALREADY, "An operation was attempted on a non-blocking socket that already had an operation in progress." );
            CASE_SOCK_ERROR( WSAENOTSOCK, "An operation was attempted on something that is not a socket." );
            CASE_SOCK_ERROR( WSAEDESTADDRREQ, "A required address was omitted from an operation on a socket." );
            CASE_SOCK_ERROR( WSAEMSGSIZE, "A message sent on a datagram socket was larger than the internal message buffer or some other network limit, or the buffer used to receive a datagram into was smaller than the datagram itself." );
            CASE_SOCK_ERROR( WSAEPROTOTYPE, "A protocol was specified in the socket function call that does not support the semantics of the socket type requested." );
            CASE_SOCK_ERROR( WSAENOPROTOOPT, "An unknown, invalid, or unsupported option or level was specified in a getsockopt or setsockopt call." );
            CASE_SOCK_ERROR( WSAEPROTONOSUPPORT, "The requested protocol has not been configured into the system, or no implementation for it exists." );
            CASE_SOCK_ERROR( WSAESOCKTNOSUPPORT, "The support for the specified socket type does not exist in this address family." );
            CASE_SOCK_ERROR( WSAEOPNOTSUPP, "The attempted operation is not supported for the type of object referenced." );
            CASE_SOCK_ERROR( WSAEPFNOSUPPORT, "The protocol family has not been configured into the system or no implementation for it exists." );
            CASE_SOCK_ERROR( WSAEAFNOSUPPORT, "An address incompatible with the requested protocol was used." );
            CASE_SOCK_ERROR( WSAEADDRINUSE, "Only one usage of each socket address (protocol/network address/port) is normally permitted." );
            CASE_SOCK_ERROR( WSAEADDRNOTAVAIL, "The requested address is not valid in its context." );
            CASE_SOCK_ERROR( WSAENETDOWN, "A socket operation encountered a dead network." );
            CASE_SOCK_ERROR( WSAENETUNREACH, "A socket operation was attempted to an unreachable network." );
            CASE_SOCK_ERROR( WSAENETRESET, "The connection has been broken due to keep-alive activity detecting a failure while the operation was in progress." );
            CASE_SOCK_ERROR( WSAECONNABORTED, "An established connection was aborted by the software in your host machine." );
            CASE_SOCK_ERROR( WSAECONNRESET, "An existing connection was forcibly closed by the remote host." );
            CASE_SOCK_ERROR( WSAENOBUFS, "An operation on a socket could not be performed because the system lacked sufficient buffer space or because a queue was full." );
            CASE_SOCK_ERROR( WSAEISCONN, "A connect request was made on an already connected socket." );
            CASE_SOCK_ERROR( WSAENOTCONN, "A request to send or receive data was disallowed because the socket is not connected and (when sending on a datagram socket using a sendto call) no address was supplied." );
            CASE_SOCK_ERROR( WSAESHUTDOWN, "A request to send or receive data was disallowed because the socket had already been shut down in that direction with a previous shutdown call." );
            CASE_SOCK_ERROR( WSAETOOMANYREFS, "Too many references to some kernel object." );
            CASE_SOCK_ERROR( WSAETIMEDOUT, "A connection attempt failed because the connected party did not properly respond after a period of time, or established connection failed because connected host has failed to respond." );
            CASE_SOCK_ERROR( WSAECONNREFUSED, "No connection could be made because the target machine actively refused it." );
            CASE_SOCK_ERROR( WSAELOOP, "Cannot translate name." );
            CASE_SOCK_ERROR( WSAENAMETOOLONG, "Name component or name was too long." );
            CASE_SOCK_ERROR( WSAEHOSTDOWN, "A socket operation failed because the destination host was down." );
            CASE_SOCK_ERROR( WSAEHOSTUNREACH, "A socket operation was attempted to an unreachable host." );
            CASE_SOCK_ERROR( WSAENOTEMPTY, "Cannot remove a directory that is not empty." );
            CASE_SOCK_ERROR( WSAEPROCLIM, "A Windows Sockets implementation may have a limit on the number of applications that may use it simultaneously." );
            CASE_SOCK_ERROR( WSAEUSERS, "Ran out of quota." );
            CASE_SOCK_ERROR( WSAEDQUOT, "Ran out of disk quota." );
            CASE_SOCK_ERROR( WSAESTALE, "File handle reference is no longer available." );
            CASE_SOCK_ERROR( WSAEREMOTE, "Item is not available locally." );
            CASE_SOCK_ERROR( WSASYSNOTREADY, "WSAStartup cannot function at this time because the underlying system it uses to provide network services is currently unavailable." );
            CASE_SOCK_ERROR( WSAVERNOTSUPPORTED, "The Windows Sockets version requested is not supported." );
            CASE_SOCK_ERROR( WSANOTINITIALISED, "Either the application has not called WSAStartup, or WSAStartup failed." );
            CASE_SOCK_ERROR( WSAEDISCON, "Returned by WSARecv or WSARecvFrom to indicate the remote party has initiated a graceful shutdown sequence." );
            CASE_SOCK_ERROR( WSAENOMORE, "No more results can be returned by WSALookupServiceNext." );
            CASE_SOCK_ERROR( WSAECANCELLED, "A call to WSALookupServiceEnd was made while this call was still processing. The call has been canceled." );
            CASE_SOCK_ERROR( WSAEINVALIDPROCTABLE, "The procedure call table is invalid." );
            CASE_SOCK_ERROR( WSAEINVALIDPROVIDER, "The requested service provider is invalid." );
            CASE_SOCK_ERROR( WSAEPROVIDERFAILEDINIT, "The requested service provider could not be loaded or initialized." );
            CASE_SOCK_ERROR( WSASYSCALLFAILURE, "A system call that should never fail has failed." );
            CASE_SOCK_ERROR( WSASERVICE_NOT_FOUND, "No such service is known. The service cannot be found in the specified name space." );
            CASE_SOCK_ERROR( WSATYPE_NOT_FOUND, "The specified class was not found." );
            CASE_SOCK_ERROR( WSA_E_NO_MORE, "No more results can be returned by WSALookupServiceNext." );
            CASE_SOCK_ERROR( WSA_E_CANCELLED, "A call to WSALookupServiceEnd was made while this call was still processing. The call has been canceled." );
            CASE_SOCK_ERROR( WSAEREFUSED, "A database query failed because it was actively refused." );
            CASE_SOCK_ERROR( WSAHOST_NOT_FOUND, "No such host is known." );
            CASE_SOCK_ERROR( WSATRY_AGAIN, "This is usually a temporary error during hostname resolution and means that the local server did not receive a response from an authoritative server." );
            CASE_SOCK_ERROR( WSANO_RECOVERY, "A non-recoverable error occurred during a database lookup." );
            CASE_SOCK_ERROR( WSANO_DATA, "The requested name is valid, but no data of the requested type was found." );
            CASE_SOCK_ERROR( WSA_QOS_RECEIVERS, "At least one reserve has arrived." );
            CASE_SOCK_ERROR( WSA_QOS_SENDERS, "At least one path has arrived." );
            CASE_SOCK_ERROR( WSA_QOS_NO_SENDERS, "There are no senders." );
            CASE_SOCK_ERROR( WSA_QOS_NO_RECEIVERS, "There are no receivers." );
            CASE_SOCK_ERROR( WSA_QOS_REQUEST_CONFIRMED, "Reserve has been confirmed." );
            CASE_SOCK_ERROR( WSA_QOS_ADMISSION_FAILURE, "Error due to lack of resources." );
            CASE_SOCK_ERROR( WSA_QOS_POLICY_FAILURE, "Rejected for administrative reasons - bad credentials." );
            CASE_SOCK_ERROR( WSA_QOS_BAD_STYLE, "Unknown or conflicting style." );
            CASE_SOCK_ERROR( WSA_QOS_BAD_OBJECT, "Problem with some part of the filterspec or providerspecific buffer in general." );
            CASE_SOCK_ERROR( WSA_QOS_TRAFFIC_CTRL_ERROR, "Problem with some part of the flowspec." );
            CASE_SOCK_ERROR( WSA_QOS_GENERIC_ERROR, "General QOS error." );
            CASE_SOCK_ERROR( WSA_QOS_ESERVICETYPE, "An invalid or unrecognized service type was found in the flowspec." );
            CASE_SOCK_ERROR( WSA_QOS_EFLOWSPEC, "An invalid or inconsistent flowspec was found in the QOS structure." );
            CASE_SOCK_ERROR( WSA_QOS_EPROVSPECBUF, "Invalid QOS provider-specific buffer." );
            CASE_SOCK_ERROR( WSA_QOS_EFILTERSTYLE, "An invalid QOS filter style was used." );
            CASE_SOCK_ERROR( WSA_QOS_EFILTERTYPE, "An invalid QOS filter type was used." );
            CASE_SOCK_ERROR( WSA_QOS_EFILTERCOUNT, "An incorrect number of QOS FILTERSPECs were specified in the FLOWDESCRIPTOR." );
            CASE_SOCK_ERROR( WSA_QOS_EOBJLENGTH, "An object with an invalid ObjectLength field was specified in the QOS provider-specific buffer." );
            CASE_SOCK_ERROR( WSA_QOS_EFLOWCOUNT, "An incorrect number of flow descriptors was specified in the QOS structure." );
            //	CASE_SOCK_ERROR(WSA_QOS_EUNKOWNPSOBJ,"An unrecognized object was found in the QOS provider-specific buffer.");
            CASE_SOCK_ERROR( WSA_QOS_EPOLICYOBJ, "An invalid policy object was found in the QOS provider-specific buffer." );
            CASE_SOCK_ERROR( WSA_QOS_EFLOWDESC, "An invalid QOS flow descriptor was found in the flow descriptor list." );
            CASE_SOCK_ERROR( WSA_QOS_EPSFLOWSPEC, "An invalid or inconsistent flowspec was found in the QOS provider specific buffer." );
            CASE_SOCK_ERROR( WSA_QOS_EPSFILTERSPEC, "An invalid FILTERSPEC was found in the QOS provider-specific buffer." );
            CASE_SOCK_ERROR( WSA_QOS_ESDMODEOBJ, "An invalid shape discard mode object was found in the QOS provider specific buffer." );
            CASE_SOCK_ERROR( WSA_QOS_ESHAPERATEOBJ, "An invalid shaping rate object was found in the QOS provider-specific buffer." );
            CASE_SOCK_ERROR( WSA_QOS_RESERVED_PETYPE, "A reserved policy element was found in the QOS provider-specific buffer." );
    }
    return str;
}

#else

const char* GetLastSocketError()
{
    static THREAD char str[MAX_FOTEXT];
    Str::Format( str, "%d", errno );
    return str;
}

#endif
