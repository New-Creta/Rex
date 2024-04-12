#ifndef _DOKANFS_MC_
#define _DOKANFS_MC_

//
//  Status values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-------------------------+-------------------------------+
//  |Sev|C|       Facility          |               Code            |
//  +---+-+-------------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//

// Typical drivers have a separate message defined here for every logging call
// site. However, that approach would cause forced truncation when injecting
// variable-length parameters into messages, and it would also tightly couple
// us to the Event Viewer, which we are not sure we want to use long term. So
// having a message ID per log level with no static content allows us to use
// printf-style logging that's portable to other mechanisms.
//
//  Values are 32 bit values laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//
#define FACILITY_RPC_RUNTIME        0x2
#define FACILITY_RPC_STUBS          0x3
#define FACILITY_IO_ERROR_CODE      0x4
#define FACILITY_DOKANFS_ERROR_CODE 0x6

//
// Define the severity codes
//
#define STATUS_SEVERITY_SUCCESS       0x0
#define STATUS_SEVERITY_INFORMATIONAL 0x1
#define STATUS_SEVERITY_WARNING       0x2
#define STATUS_SEVERITY_ERROR         0x3

//
// MessageId: DOKANFS_INFO_MSG
//
// MessageText:
//
// %2
//
#define DOKANFS_INFO_MSG ((NTSTATUS)0x40060001L)

//
// MessageId: DOKANFS_ERROR_MSG
//
// MessageText:
//
// %2
//
#define DOKANFS_ERROR_MSG ((NTSTATUS)0xC0060002L)

#endif