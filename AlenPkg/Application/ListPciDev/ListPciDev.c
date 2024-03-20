/** @file
*
*  A basic application displays the infomation of the device.
*
*  Copyright (c) 2008 - 2024, Intel Corporation. All rights reserved.<BR>
*
**/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <IndustryStandard/Pci.h>

#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciIo.h>

EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *gPciRootBridgeIo;

EFI_STATUS LocatePCIRootBridgeIO();

EFI_STATUS ListPciDevice();

EFI_STATUS PciDeviceCheck(
	IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRootBridgeIo,
	OUT PCI_TYPE00 *Pci,
	IN UINT8 Bus,
	IN UINT8 Device,
	IN UINT8 Func
);

/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
	EFI_STATUS Status;
	PCI_TYPE00 Pci;

	EFI_TIME *Time;
	UINT16 Year;
	UINT8 Month;
	UINT8 Day;
	UINT8 Hour;
	UINT8 Minute;
	UINT8 Second;
	
    // Locate the PciRootBridge protocol
	Status = LocatePCIRootBridgeIO();
	if(EFI_ERROR(Status)){
		Print(L"Locate Pci RootBridge Io failed.\n");
	}
	else{
		Print(L"Locate Pci RootBridge Io successed.\n");
	}

    // List Pci Device information
	ListPciDevice();

    // Print the current time
	Status = gRT->GetTime(
        Time,
        NULL
        );
	if(EFI_ERROR(Status)){
        Print(L"Get Time failed.\n");
    }
    else{
        Print(L"\nGet Time successed.\n");

        Year	= Time->Year;
        Month	= Time->Month;
        Day		= Time->Day;
        Hour	= Time->Hour;
        Minute	= Time->Minute;
        Second	= Time->Second;
        Print(L"Current Time: %d / %d / %d - %d: %d: %d \n", Year, Month, Day, Hour, Minute, Second);
    }
	
	// Press ESC to exit
	EFI_INPUT_KEY Key;
	Print(L"\nPress ESC to Exit...\n");
	while(1){
		Status = gST->ConIn->ReadKeyStroke(
			gST->ConIn,
			&Key
		);
		if(!EFI_ERROR(Status) && Key.ScanCode == SCAN_ESC){
			break;
		}
	}
	
	return Status;
}

EFI_STATUS
LocatePCIRootBridgeIO()
{
	EFI_STATUS Status;
	EFI_HANDLE *PciHandleBuffer;
	UINTN HandleCount = 0;
	UINTN HandleIndex = 0;
	
	Status = gBS->LocateHandleBuffer(
		ByProtocol,
		&gEfiPciRootBridgeIoProtocolGuid,
		NULL,
		&HandleCount,
		&PciHandleBuffer
	);
	if(EFI_ERROR(Status)) return Status;

	for(HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++){
		Status = gBS->HandleProtocol(
			PciHandleBuffer[HandleIndex],
			&gEfiPciRootBridgeIoProtocolGuid,
			(VOID **)&gPciRootBridgeIo
		);
		if(EFI_ERROR(Status))	continue;
		else	return Status;
	}
	return Status;
}

EFI_STATUS
ListPciDevice()
{
	EFI_STATUS Status = EFI_SUCCESS;
	PCI_TYPE00 Pci;
	UINT16 Bus, Dev, Func, PciDevCount = 0;

	Print(L"Pci No   Bus      Dev      Func  |  Vendor      Device      ClassCode | BAR\n");
	for(Bus = 0; Bus < 256; Bus++){
		for(Dev = 0; Dev <= PCI_MAX_DEVICE; Dev++){
			for(Func = 0; Func <= PCI_MAX_FUNC; Func++){
				Status = PciDeviceCheck(gPciRootBridgeIo, &Pci, (UINT8)Bus, (UINT8)Dev, (UINT8)Func);
				if(Status == EFI_SUCCESS){
					PciDevCount++;
					Print(L"%d        %x        %x        %x        ", PciDevCount, (UINT8)Bus, (UINT8)Dev, (UINT8)Func);
					Print(L"%x        %x        %x        %x\n", Pci.Hdr.VendorId, Pci.Hdr.DeviceId, Pci.Hdr.ClassCode[0], Pci.Device.Bar[0]);
				}
			}
		}
	}
	return EFI_SUCCESS;
}

EFI_STATUS
PciDeviceCheck (
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *PciRootBridgeIo,
  OUT PCI_TYPE00                       *Pci,
  IN  UINT8                            Bus,
  IN  UINT8                            Device,
  IN  UINT8                            Func
  )
{
  UINT64      DevAddress;
  EFI_STATUS  Status;

  DevAddress = EFI_PCI_ADDRESS (Bus, Device, Func, 0);

  Status = PciRootBridgeIo->Pci.Read (
        PciRootBridgeIo,
        EfiPciWidthUint32,
        DevAddress,
        1,
        Pci
        );

  if (!EFI_ERROR (Status) && ((Pci->Hdr).VendorId != 0xffff)) {
    Status = PciRootBridgeIo->Pci.Read (
        PciRootBridgeIo,
        EfiPciWidthUint32,
        DevAddress,
        sizeof (PCI_TYPE00) / sizeof (UINT32),
        Pci
        );

    return EFI_SUCCESS;
  }

  return EFI_NOT_FOUND;
}


