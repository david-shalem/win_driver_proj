/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    user and kernel

--*/

//
// Define an Interface Guid so that app can find the device and talk to it.
//

DEFINE_GUID (GUID_DEVINTERFACE_printerdriver,
    0x9178cfdc,0x69df,0x4503,0x86,0x96,0x93,0x2c,0x25,0xf6,0x9d,0x84);
// {9178cfdc-69df-4503-8696-932c25f69d84}
