/** @file
  This is a simple shell application

  Copyright (c) 2008, Intel Corporation                                                         
  All rights reserved. This program and the accompanying materials                          
  are licensed and made available under the terms and conditions of the BSD License         
  which accompanies this distribution.  The full text of the license may be found at        
  http://opensource.org/licenses/bsd-license.php                                            

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

**/
//Librerias UEFI Básicas
#include <Uefi.h>
#include <PiPei.h>

#include <Library/UefiApplicationEntryPoint.h>

#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>


#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
//#include <Library/FileExplorerLib.h>
#include <Library/GenericBdsLib.h>
#include <Library/BaseLib.h>

#include <Protocol/LoadedImage.h>
#include <Protocol/DevicePath.h>


#include <Protocol/SimpleFileSystem.h>
#include <Protocol/BlockIo.h>

//Protocolos


//IN EFI_FILE_HANDLE   *RootHandle = NULL;
/**
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
static CHAR16 *OsLoaderMemoryTypeDesc[EfiMaxMemoryType] = {
	L"reserved  ",
	L"LoaderCode",
	L"LoaderData",
	L"BS_code   ",
	L"BS_data   ",
	L"RT_code   ",
	L"RT_data   ",
	L"available ",
	L"Unusable  ",
	L"ACPI_recl ",
	L"ACPI_NVS  ",
	L"MemMapIO  ",
	L"MemPortIO ",
	L"PAL_code  "
};
CHAR16 *banner = L"Aplicación UEFI- Version %d.%d.%d\n";
VOID * AllocatePoolS(IN UINTN Size );
BOOLEAN GrowBuffer(IN OUT EFI_STATUS *Status,IN OUT VOID **Buffer,IN UINTN BufferSize);
EFI_MEMORY_DESCRIPTOR* GetMemoryMap(UINTN entries, UINTN mapKey, UINTN descriptorSize, UINT32 descriptorVersion);
EFI_STATUS LibGetSystemConfigurationTable	(	IN EFI_GUID * 	TableGuid, IN OUT VOID ** 	Table);
VOID * LibGetVariable (IN CHAR16  *Name,IN EFI_GUID *VendorGuid);
VOID * LibGetVariableAndSize (IN CHAR16 *Name,IN EFI_GUID *VendorGuid,OUT UINTN *VarSize);
BOOLEAN JumpToImage (
    IN UINT8                 *ImageStart,
    IN UINTN                 NoEntries,
    IN EFI_MEMORY_DESCRIPTOR *MemoryMap,
    IN UINTN                 MapKey,
    IN UINTN                 DescriptorSize,
    IN UINT32                DescriptorVersion
    );
BOOLEAN CheckPEHeader (IN  UINT8  *Base);
typedef  
BOOLEAN (* KERNEL_IMAGE_ENTRY_POINT) (
    IN UINTN                 NoEntries,
    IN EFI_MEMORY_DESCRIPTOR *MemoryMap,
    IN UINTN                 MapKey
	);




EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
	Print(banner); // Print the welcome information.
	EFI_STATUS efiStatus;
	//EFI_CONFIGURATION_TABLE *ConfigurationTable = gST->ConfigurationTable;
	UINTN NumberOfTableEntries = gST->NumberOfTableEntries;


	//CHAR16	FirmwareVendor = *SystemTable->FirmwareVendor;

	DEBUG ((0xffffffff, "\n\nUEFI Base Training DEBUG DEMO:SampleApp2\n") );	
	BOOLEAN salir=FALSE;
	EFI_INPUT_KEY key;
	UINTN EventIndex;
	//gBS->WaitForEvent(1,&gST->ConIn->WaitForKey,&EventIndex);
	ZeroMem(&key, sizeof(EFI_INPUT_KEY));


	do{
		gST->ConOut->ClearScreen(gST->ConOut);
		gST->ConIn->Reset(gST->ConIn,FALSE);
		gST->ConOut->EnableCursor(gST->ConOut, FALSE);
		Print(L"******************************************\n");
		Print(L"*   Menu de opciones de la aplicación    *\n");
		Print(L"******************************************\n");
		Print(L"F1.Información de la tabla del sistema UEFI\n");
		Print(L"F2.Información del dispositivo y de la imagen de la aplicación se está ejecutando\n");
		Print(L"F3.Acceso a la carpeta raiz de la apliación y a sus archivos\n");
		Print(L"F4.Dumpeo de la memoria\n");
		Print(L"F5.Muestra las variables del entorno\n");
		Print(L"F6.Cargar OSKERNEL.BIN\n");
		Print(L"\n Press any key continue.\n\n\n");		
		gBS->WaitForEvent(1,&gST->ConIn->WaitForKey,&EventIndex);
		//Print(L"Opcion: %c\n",key.ScanCode);
		gST->ConIn->ReadKeyStroke(gST->ConIn, &key);				
		switch(key.ScanCode){			
			case SCAN_F1 :
			    gST->ConOut->ClearScreen(gST->ConOut);
			    Print(L"F1.Información de la tabla del sistema UEFI\n");

			    Print(L"Tabla del sistema: 0x%p08x\n", *SystemTable);
				Print(L"Handle Imagen:0x%p08x\n",ImageHandle);
				Print(L"NumberOfTableEntries:%d\n",NumberOfTableEntries);

				EFI_GUID AcpiTableGuid = gEfiAcpiTableGuid; 
				EFI_GUID SMBIOSTableGuid = gEfiSmbiosTableGuid;
				EFI_GUID SalSystemTableGuid = gEfiSalSystemTableGuid;
				EFI_GUID MpsTableGuid = gEfiMpsTableGuid;

				VOID** AcpiTable,**SMBIOSTable,**SalSystemTable,**MpsTable;

				LibGetSystemConfigurationTable((void *)&AcpiTableGuid,(void *)&AcpiTable);
				LibGetSystemConfigurationTable((void *)&SMBIOSTableGuid,(void *)&SMBIOSTable);
				LibGetSystemConfigurationTable((void *)&SalSystemTableGuid,(void *)&SalSystemTable);
				LibGetSystemConfigurationTable((void *)&MpsTableGuid,(void *)&MpsTable);   
				Print(L" ACPI Table is at address%X\n",AcpiTable);
				Print(L" SMBIOS Table is at address%X\n",SMBIOSTable);
				Print(L" Sal System Table is at address%X\n",SalSystemTable);
				Print(L" MPS Table is at address%X\n",MpsTable);
				Print(L"\n Press Enter continue.\n");			
				gBS->WaitForEvent(1,&gST->ConIn->WaitForKey,&EventIndex);				
				///Informacion de la tabla del sistema UEfi

			break;
		   	case SCAN_F2  :
		   	gST->ConOut->ClearScreen(gST->ConOut);
		   	Print(L"F2.Información del dispositivo y de la imagen de la aplicación se está ejecutando\n");

		    EFI_HANDLE GlobalImage = NULL; // EFI_HANDLE is a typedef to a VOID pointer.
			GlobalImage = ImageHandle;
			EFI_LOADED_IMAGE *LoadedImageProtocol ;
			EFI_GUID LoadedImageProtocolGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;

			efiStatus = gBS->LocateProtocol(&LoadedImageProtocolGuid, NULL, (void *)&LoadedImageProtocol);
			if (EFI_ERROR (efiStatus)) {
		    	Print(L"Error: No se ha podido localizar el Protocolo LoadedImageProtocolGuid que nos permite obtener información de la imagen  \n");
		    	gBS->Stall(3 * 1000 * 1000);
		    	return EFI_LOAD_ERROR;
		    }else{
		    	Print(L"Protocolo LoadedImageProtocolGuid localizado\n");
		    }

		    EFI_LOADED_IMAGE *ThisImage = NULL;
			efiStatus = gBS->HandleProtocol (ImageHandle, &LoadedImageProtocolGuid,(void *)&ThisImage);
			if (EFI_ERROR (efiStatus)) {
		    	Print(L"No se ha podido cargar el interfaz para LoadedImageProtocolGuid \n");
		    }else{
		    	Print(L"Interfaz del Protocolo LoadedImageProtocolGuid de imagen cargado\n");
		    }

		    EFI_GUID DevicePathProtocol = EFI_DEVICE_PATH_PROTOCOL_GUID;
		    EFI_DEVICE_PATH_PROTOCOL *DevicePath =NULL;
		    efiStatus = gBS->HandleProtocol(ThisImage->DeviceHandle,&DevicePathProtocol,(void *)&DevicePath);
			if (EFI_ERROR (efiStatus)) {
		    	Print(L"No se ha podido cargar el interfaz para DevicePathProtocol \n");
		    }else{
		    	Print(L"Interfaz del Protocolo DevicePathProtocol de imagen cargado \n");
		    }
		    Print (L"Image device : %s\n",DevicePathToStr(DevicePath));
		    Print (L"Image file: %s\n",DevicePathToStr(ThisImage->FilePath));
		    Print (L"Image FilePath: %X\n",ThisImage->FilePath);
		    Print (L"Image handle: %X\n",ThisImage->DeviceHandle);
			Print (L"Image Base: %X\n",ThisImage->ImageBase);
			Print (L"Image Size: %X\n",ThisImage->ImageSize);    
			////Informacion sobre el dispositivo donde se aloja esta imagen EFi
			Print(L"\n Press Enter continue.\n");
			gBS->WaitForEvent(1,&gST->ConIn->WaitForKey,&EventIndex);	
		      break; //optional
		      
		  	case SCAN_F3  :
		  		gST->ConOut->ClearScreen(gST->ConOut);
		  		Print(L"F3.Acceso a la carpeta raiz de la aplicación y a sus archivos\n");
		      	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *Vol= NULL;
				EFI_FILE_PROTOCOL *RootFs=0;
				EFI_GUID FileSystemProtocol = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

				efiStatus = gBS->HandleProtocol(ThisImage->DeviceHandle,&FileSystemProtocol,(void *)&Vol);
				if (EFI_ERROR (efiStatus)) {
			    	Print(L"No se ha podido cargar el interfaz para FileSystemProtocol \n");
			    }else{
			    	Print(L"Interfaz del Protocolo FileSystemProtocol de imagen cargado \n");
			    }
			    
			    efiStatus = Vol->OpenVolume(Vol,&RootFs);
			   	if (EFI_ERROR (efiStatus)) {
			    	Print(L"No se ha podido cargar el el sistema de ficheros donde nos encontramos \n");
			    }else{
			    	Print(L"Sistema de ficheros de la imagen actual en el que nos enconramos cargado  \n");    	
			    }       
			   
			    EFI_FILE_HANDLE CurDir = RootFs;
			    
			    //CHAR16 *FileName=DevicePathToStr(ThisImage->FilePath);
			    // alternativa hace lo mismo pero además hay que inicializar FileName y darle un tamaño con lo que no sería dinámico        
			    //StrCpy(FileName,DevicePathToStr(ThisImage->FilePath)); 

			    /*UINTN i= 0;
			    for(i=StrLen(FileName);i>=0 && FileName[i]!='\\';i--); FileName[i]	=0;
				StrCat(FileName,L"\\OSKERNEL.BIN");*/
				
				//CHAR16 *FileName=L"\\OSKERNEL.BIN";
				CHAR16 *FileName=L"\\helloworld.exe";
				EFI_FILE_PROTOCOL *FileHandle=NULL;
				efiStatus = CurDir->Open(CurDir,&FileHandle, FileName, EFI_FILE_MODE_READ, 0);
				if (EFI_ERROR (efiStatus)) {
			    	Print(L"No se ha podido abrir el fichero \n");
			    }else{
			    	Print(L"Se ha podido abrir el fichero \n");
			    }
				UINTN Size = 0x00100000;
				//EFI_MEMORY_TYPE EfiLoaderData = NULL;
				void *OsKernelBuffer =NULL;
				efiStatus = gBS->AllocatePool(EfiLoaderData, Size, (VOID *)&OsKernelBuffer);
				if (EFI_ERROR (efiStatus)) {
			    	Print(L"No se ha podido establecer el buffer para el acceso al archivo\n");
			    }else{
			    	Print(L"hemos establecido el buffer para el acceso al archivo\n");
			    }
				efiStatus = FileHandle->Read(FileHandle, &Size, OsKernelBuffer);
				if (EFI_ERROR (efiStatus)) {
			    	Print(L"No se ha podido leer el archivo \n");
			    }else{
			    	Print(L"hemos leido el archivo\n");
			    	Print(L"%X bytes of OSKERNEL.BIN read into memory at %X\n",Size,OsKernelBuffer);
			    }
				efiStatus = FileHandle->Close(FileHandle);
				if (EFI_ERROR (efiStatus)) {
			    	Print(L"No se ha podido cerrar el archivo \n");
			    }else{
			    	Print(L"hemos cerrado el archivo\n");
			    }
			    //gBS->FreePool(OsKernelBuffer);
			    gBS->WaitForEvent(1,&gST->ConIn->WaitForKey,&EventIndex);	

	///Acceso a la carpeta donde nos encontramos y acceso a archivos
			    Print(L"\n Press Enter continue.\n");
		      break; //optional

		    case SCAN_F4  :
		    gST->ConOut->ClearScreen(gST->ConOut);
		    Print(L"F4.Dumpeo de la memoria\n");
		       UINTN NoHandles = 0;
			    EFI_HANDLE *HandleBuffer = NULL;
			    EFI_GUID BlockIoProtocol = EFI_BLOCK_IO_PROTOCOL_GUID;
			    EFI_BLOCK_IO_PROTOCOL *BlkIo = NULL;
			    EFI_BLOCK_IO_MEDIA *Media = NULL;

			    efiStatus = gBS->LocateHandleBuffer(ByProtocol,&BlockIoProtocol,NULL,&NoHandles,(VOID *)&HandleBuffer);   
			    EFI_HANDLE *Block=NULL;
			    UINT32	MediaId=0;
			    Print(L"Dispositivos de bloque de arranque\n");
			    Print(L" =============================================\n");

			    for(UINTN i=0;i<NoHandles;i++) {
			    	gBS->HandleProtocol ((VOID *)HandleBuffer[i],&DevicePathProtocol,(void *)&DevicePath);    	
					gBS->HandleProtocol ((VOID *)HandleBuffer[i],&BlockIoProtocol,(void *)&BlkIo);	
					Print(L"\nBlock #%d of device %s\n",i,DevicePathToStr(DevicePath));
					Media = BlkIo->Media;
					efiStatus = gBS->AllocatePool(EfiBootServicesData,Media->BlockSize,(void *)&Block);		
					Print(L"\nBlock #%d of device %d\n",i,Media->MediaId);
					MediaId = Media->MediaId;
					efiStatus = BlkIo->ReadBlocks(BlkIo,MediaId,(EFI_LBA)0,Media->BlockSize,Block);
					 gBS->FreePool(Block);
					//DumpHex(0,0,Media->BlockSize,Block);
				}	
				gBS->FreePool(HandleBuffer);

				EFI_MEMORY_DESCRIPTOR   *Buffer = NULL;
			    UINTN                   BufferSize =  0x00100000;
			    UINTN               NoEntries = 0;
			    UINTN               MapKey= 0;
			    UINTN               DescriptorSize = 0;

			    UINT32              DescriptorVersion = 0;

			    BOOLEAN         TryAgain = TRUE;

			    while (TryAgain) {
				    if (!Buffer && BufferSize) {
				        efiStatus = EFI_BUFFER_TOO_SMALL;
				    }
				    TryAgain = FALSE;
				    if (efiStatus == EFI_BUFFER_TOO_SMALL) {
				        if (Buffer) {
				            gBS->FreePool (Buffer);
				        }
				        efiStatus = gBS->AllocatePool(EfiLoaderData, BufferSize, (VOID *)&Buffer);

				        if (&Buffer) {
				            TryAgain = TRUE;
				        } else {    
				            efiStatus = EFI_OUT_OF_RESOURCES;
				        } 
				    }

				     if (!TryAgain && EFI_ERROR(efiStatus) && Buffer) {
				        gBS->FreePool (Buffer);
				        Buffer = NULL;
				    }
				    efiStatus= gBS->GetMemoryMap(&BufferSize, Buffer, &MapKey, &DescriptorSize, &DescriptorVersion);
				}
				
				if (!EFI_ERROR(efiStatus)) {
			        //NoEntries = BufferSize / DescriptorSize;
			        /*
			        DEBUG ((0xffffffff, "\n\nUEFI Base Training DEBUG DEMO\n") );
					DEBUG ((0xffffffff, "0xffffffff USING DEBUG ALL Mask Bits Set\n") );
			        DEBUG ((EFI_D_ERROR,    " 0x%08x USING DEBUG EFI_D_ERROR\n", (UINTN)(EFI_D_ERROR))  );*/
			        Print(L" efiStatus:%d\n",efiStatus);
			    }
			    NoEntries = BufferSize / DescriptorSize;
				
			    //Print(L"&DescriptorSize:%d\t*DescriptorSize value:%d\n",&DescriptorSize,*DescriptorSize);
			    Print(L"Memory Descriptor List:\n\n");
				Print(L" Type  Start Address  End Address  Attributes \n");
				Print(L" =============================================\n");
				EventIndex =0;
				EFI_MEMORY_DESCRIPTOR  *MemoryMapEntry = Buffer;
				for(UINTN i=0; i < NoEntries; i++) {						
				    Print(L"%s %lX %lX %lX\n",
					OsLoaderMemoryTypeDesc[MemoryMapEntry->Type],
					MemoryMapEntry->PhysicalStart,
					MemoryMapEntry->PhysicalStart + LShiftU64(MemoryMapEntry->NumberOfPages,EFI_PAGE_SHIFT)-1,
					MemoryMapEntry->Attribute);
				    MemoryMapEntry = NEXT_MEMORY_DESCRIPTOR(MemoryMapEntry,DescriptorSize);
				}
			 ///dumpeo de la memoria
			////Probar la siguiente funcion : ShellCommandRunMemMap (
				EventIndex =0;
				gBS->FreePool(Buffer);
				Print(L"\n Press Enter continue.\n");
				gBS->WaitForEvent(1,&gST->ConIn->WaitForKey,&EventIndex);					
		      break; //optional
		    case SCAN_F5  :
		    gST->ConOut->ClearScreen(gST->ConOut);
		    Print(L"F5.Muestra las variables del entorno\n");
			CHAR16 VariableName[50];
			VariableName[0]=0x0000;
			//VariableName[0] = 0x0000;
			EFI_GUID VendorGuid = { 0,0,0,{0,0,0,0,0,0,0,0} };
			UINTN VariableNameSize;
			VOID * VariableValue = 0;
			Print(L"	GUID		Variable Name			Value\n");
			Print(L"===================================================\n");
			efiStatus = EFI_SUCCESS;
			do {
				VariableNameSize = 256;
				efiStatus = gRT->GetNextVariableName(&VariableNameSize,VariableName,&VendorGuid);
				
				switch(efiStatus){
				   case EFI_NOT_FOUND  :
				      Print(L"EFI_NOT_FOUND : %d\n",efiStatus);
				      break; //optional
				   case EFI_BUFFER_TOO_SMALL  :
				      Print(L"EFI_BUFFER_TOO_SMALL : %d\n",efiStatus);
				      break; //optional
				   case EFI_INVALID_PARAMETER  :
				      Print(L"EFI_INVALID_PARAMETER : %d\n",efiStatus);
				      break; //optional
				    case EFI_DEVICE_ERROR  :
				      Print(L"EFI_DEVICE_ERROR : %d\n",efiStatus);
				      break; //optional
				   // you can have any number of case statements.
				   default : //Optional
				    VariableValue = LibGetVariable(VariableName,&VendorGuid);
					Print(L"%.-35g %.-20s %X\n",&VendorGuid,VariableName,VariableValue);
				}//fin del case 2
				/*
				if (efiStatus == EFI_SUCCESS) {
					VariableValue = LibGetVariable(VariableName,&VendorGuid);
				Print(L"%.-35g %.-20s %X\n",&VendorGuid,VariableName,VariableValue);

				}*/
				
			} while (efiStatus == EFI_SUCCESS);
			//Busqueda y acceso a las variables del entorno EUfi
			Print(L"\n Press Enter continue.\n");
			gBS->WaitForEvent(1,&gST->ConIn->WaitForKey,&EventIndex);	
		    break;


		   
		    case SCAN_F6  :



		    Print(L"\n Llamada a la salida de servicios de boot.\n");
		    
		    EFI_MEMORY_DESCRIPTOR* MemoryMap = GetMemoryMap(BufferSize, MapKey, DescriptorSize,  DescriptorVersion);
		    if(CheckPEHeader (OsKernelBuffer)){
	    		efiStatus = gBS->ExitBootServices(ImageHandle,MapKey);
	    		if (efiStatus){
	    			MemoryMapEntry = MemoryMap;
	    		for (UINTN i = 0; i < NoEntries; ++i)
	    		{
	    			if (MemoryMapEntry->Attribute & EFI_MEMORY_RUNTIME)
	    			{
	    				MemoryMapEntry->VirtualStart = MemoryMapEntry->PhysicalStart;
	    			}
	    			MemoryMapEntry = NEXT_MEMORY_DESCRIPTOR(MemoryMapEntry,DescriptorSize);
	    		}
	    		efiStatus = gRT->SetVirtualAddressMap(NoEntries * DescriptorSize, DescriptorSize,EFI_MEMORY_DESCRIPTOR_VERSION, MemoryMap);
	    		
		    	switch(efiStatus){
				   case EFI_NOT_FOUND  :
				      Print(L"EFI_NOT_FOUND : %d\n",efiStatus);
				      gBS->WaitForEvent(1,&gST->ConIn->WaitForKey,&EventIndex);
				      break; //optional
				   case EFI_NO_MAPPING  :
				      Print(L"EFI_NO_MAPPING : %d\n",efiStatus);gBS->WaitForEvent(1,&gST->ConIn->WaitForKey,&EventIndex);
				      break; //optional
				   case EFI_INVALID_PARAMETER  :
				      Print(L"EFI_INVALID_PARAMETER : %d\n",efiStatus);gBS->WaitForEvent(1,&gST->ConIn->WaitForKey,&EventIndex);
				      break; //optional
				    case EFI_UNSUPPORTED  :
				      Print(L"EFI_UNSUPPORTED : %d\n",efiStatus);gBS->WaitForEvent(1,&gST->ConIn->WaitForKey,&EventIndex);
				      break; //optional
				   // you can have any number of case statements.
				   default : //Optional
				    Print(L" SetVirtualAddressMap \n" );
		    		gBS->WaitForEvent(1,&gST->ConIn->WaitForKey,&EventIndex);
				    BOOLEAN okjump = JumpToImage (OsKernelBuffer,  5, MemoryMap, MapKey, DescriptorSize, DescriptorVersion);
		    		Print(L" EntryPoint = (KERNEL_IMAGE_ENTRY_POINT): %d \n", okjump);
		    		gBS->WaitForEvent(1,&gST->ConIn->WaitForKey,&EventIndex);
				}//fin del case 3			    
	    		}
	    				
		    }else{
		    	Print(L"Cabecera incorrecta");

		    }
		    Print(L"\n Press Enter continue.\n");
		    gBS->WaitForEvent(1,&gST->ConIn->WaitForKey,&EventIndex);	
	///Acceso a la carpeta donde nos encontramos y acceso a archivos
			
			
		    break; //optional
		   // you can have any number of case statements.
		   default : //Optional
		   
		   salir=TRUE;
		}//fin del case 1


	/*if (getchar()==".") {
		// Reboot the system.
		err = gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
		
		// Should never get here unless there's an error.
		Print(L"Error calling ResetSystem: %r", err);
		gBS->Stall( 3 * 1000 * 1000);
	}*/
	//DEBUG ((0xffffffff, "0xffffffff USING DEBUG ALL Mask Bits Set\n") );ASSERT_EFI_ERROR(0x80000000);
	//Busqueda y acceso a las variables del entorno EUfi
	//gST->ConOut->ClearScreen(gST->ConOut);
	}while(!salir);
	//err = gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
	//gBS->Stall( 3 * 1000 * 1000);
   return EFI_SUCCESS;
}


VOID * AllocatePoolS(IN UINTN Size ){
    EFI_STATUS              Status;
    VOID                    *p;

    Status = gBS->AllocatePool(EfiBootServicesData, Size, &p);
    if (EFI_ERROR(Status)) {
        DEBUG((EFI_D_ERROR, "AllocatePool: out of pool  %x\n", Status));
        p = NULL;
    }
    return p;
}
BOOLEAN GrowBuffer(IN OUT EFI_STATUS *Status,IN OUT VOID **Buffer,IN UINTN BufferSize){
	/*++
Routine Description:
    Helper function called as part of the code needed
    to allocate the proper sized buffer for various 
    EFI interfaces.
Arguments:
    Status      - Current status
    Buffer      - Current allocated buffer, or NULL
    BufferSize  - Current buffer size needed
    
Returns:
    
    TRUE - if the buffer was reallocated and the caller 
    should try the API again.
--*/
{
    BOOLEAN         TryAgain;

    //
    // If this is an initial request, buffer will be null with a new buffer size
    //

    if (!*Buffer && BufferSize) {
        *Status = EFI_BUFFER_TOO_SMALL;
    }

    //
    // If the status code is "buffer too small", resize the buffer
    //
        
    TryAgain = FALSE;
    if (*Status == EFI_BUFFER_TOO_SMALL) {

        if (*Buffer) {
            gBS->FreePool (*Buffer);
        }

        *Buffer = AllocatePoolS (BufferSize);

        if (*Buffer) {
            TryAgain = TRUE;
        } else {    
            *Status = EFI_OUT_OF_RESOURCES;
        } 
    }

    //
    // If there's an error, free the buffer
    //

    if (!TryAgain && EFI_ERROR(*Status) && *Buffer) {
        gBS->FreePool (*Buffer);
        *Buffer = NULL;
    }

    return TryAgain;
}
}
EFI_STATUS LibGetSystemConfigurationTable	(	IN EFI_GUID * 	TableGuid, IN OUT VOID ** 	Table){
    UINTN Index;
 	Print(L"LibGet  NumberOfTableEntries:%d\n",gST->NumberOfTableEntries);
    for(Index=0;Index < gST->NumberOfTableEntries;Index++) {
        if (CompareGuid(TableGuid,&(gST->ConfigurationTable[Index].VendorGuid))==1) {
            *Table = gST->ConfigurationTable[Index].VendorTable;
            //Print(L" compare:%x\n",CompareGuid(TableGuid,&(gST->ConfigurationTable[Index].VendorGuid)));
            return EFI_SUCCESS;
        }
    }
return EFI_NOT_FOUND;
}	

VOID * LibGetVariableAndSize (IN CHAR16 *Name,IN EFI_GUID *VendorGuid,OUT UINTN *VarSize){
    EFI_STATUS              Status;
    VOID                    *Buffer;
    UINTN                   BufferSize;

    //
    // Initialize for GrowBuffer loop
    //

    Buffer = NULL;
    BufferSize = 100;

    //
    // Call the real function
    //

    while (GrowBuffer (&Status, &Buffer, BufferSize)) {
        Status = gRT->GetVariable(Name,VendorGuid,NULL,&BufferSize,Buffer);
    }
    if (Buffer) {
        *VarSize = BufferSize;
    } else {
        *VarSize = 0;
    }
    return Buffer;
}
VOID * LibGetVariable (IN CHAR16  *Name,IN EFI_GUID *VendorGuid){

    UINTN   VarSize;
    return LibGetVariableAndSize (Name, VendorGuid, &VarSize);
}
EFI_MEMORY_DESCRIPTOR* GetMemoryMap(UINTN entries, UINTN mapKey, UINTN descriptorSize, UINT32 descriptorVersion)
{
	UINTN totalSize = 0;
	EFI_MEMORY_DESCRIPTOR* descriptor;
	gBS->GetMemoryMap(&totalSize, NULL, &mapKey, &descriptorSize, &descriptorVersion);
	descriptor = (EFI_MEMORY_DESCRIPTOR*)AllocatePoolS(totalSize);
	gBS->GetMemoryMap(&totalSize, descriptor, &mapKey, &descriptorSize, &descriptorVersion);
	entries = totalSize / descriptorSize;
	return descriptor;
}

BOOLEAN
JumpToImage (
    IN UINT8                 *ImageStart,
    IN UINTN                 NoEntries,
    IN EFI_MEMORY_DESCRIPTOR *MemoryMap,
    IN UINTN                 MapKey,
    IN UINTN                 DescriptorSize,
    IN UINT32                DescriptorVersion
    )

//
//
// This code jumps to the Entry point of a PE32+ image. It assumes the image
//  is valid and has been relocated to it's loaded address.
//
// The Entry point in the image is realy a pointer to a plabel. The plabel 
//  contains the entry address of the rountine and it's gp. Any call via a 
//  pointer to a function in C for IA-64 is really an indirect procedure call
//  via a plabel. The C compiler will load the gp for you. I don't know how 
//  to call to an address out side your linked image in C with out going 
//  through the plabel.
//
// It is possible to use linker flags to make a PE32+ Binary == PE32+ image.
//  the key is aligning the image on a 32 byte boundry.
//
{
	Print(L"Hemos entrado en JumpToImage\n");
    EFI_IMAGE_DOS_HEADER			*DosHdr;
    EFI_IMAGE_NT_HEADERS32			*PeHdr;
    KERNEL_IMAGE_ENTRY_POINT	EntryPoint;

	Print(L"despues de las declaraciones\n");
    //
    // Assume CheckPEHeader() has already passed on this Image 
    //

    DosHdr = (EFI_IMAGE_DOS_HEADER *)ImageStart;

	Print(L"DosHdr = (EFI_IMAGE_DOS_HEADER *)ImageStart;\n");
    
    PeHdr = (EFI_IMAGE_NT_HEADERS32 *)(ImageStart + DosHdr->e_lfanew);
    Print(L"PeHdr = (EFI_IMAGE_NT_HEADERS32 *)(ImageStart + DosHdr->e_lfanew);\n");

    Print(L" PeHdr->OptionalHeader.ImageBase: %x \n", PeHdr->OptionalHeader.ImageBase);
    Print(L" PeHdr->OptionalHeader.AddressOfEntryPoint: %x \n", PeHdr->OptionalHeader.AddressOfEntryPoint);
    Print(L" PeHdr->OptionalHeader.ImageBase + PeHdr->OptionalHeader.AddressOfEntryPoint: %x \n", PeHdr->OptionalHeader.ImageBase + PeHdr->OptionalHeader.AddressOfEntryPoint);

    EntryPoint = (KERNEL_IMAGE_ENTRY_POINT) (PeHdr->OptionalHeader.ImageBase + PeHdr->OptionalHeader.AddressOfEntryPoint);              
    return EntryPoint(NoEntries, MemoryMap, MapKey);
}
BOOLEAN
CheckPEHeader (IN  UINT8   *Base){
    EFI_IMAGE_DOS_HEADER			*DosHdr;
    EFI_IMAGE_NT_HEADERS32			*PeHdr;

    Print(L"Hemos entrado en CheckPEHeader\n");
    DosHdr = (EFI_IMAGE_DOS_HEADER *)Base;
    Print(L"despues de DosHdr = (EFI_IMAGE_DOS_HEADER *)Base\n");


    if (DosHdr->e_magic != EFI_IMAGE_DOS_SIGNATURE ) {
    	Print(L"DosHdr->e_magic != EFI_IMAGE_DOS_SIGNATURE :0x%x\n", DosHdr->e_magic);
    	Print(L"EFI_IMAGE_DOS_SIGNATURE :0x%x\n",EFI_IMAGE_DOS_SIGNATURE);
    	Print(L"SIGNATURE_16('M', 'Z') :0x%x\n",SIGNATURE_16('M', 'Z'));
    	
        return FALSE;
    }
    Print(L"Hemos salido de EFI_IMAGE_DOS_SIGNATURE\n");
    PeHdr = (EFI_IMAGE_NT_HEADERS32 *)(Base + DosHdr->e_lfanew);
    Print(L"Despues de EFI_IMAGE_NT_HEADERS32 *)(Base + DosHdr->e_lfanew\n");
    if (PeHdr->Signature != EFI_IMAGE_NT_SIGNATURE ) {
    	 Print(L"PeHdr->Signature != EFI_IMAGE_NT_SIGNATURE\n");
        return FALSE;
    }
    Print(L"despues de PeHdr->Signature != EFI_IMAGE_NT_SIGNATURE\n");
	#define       APP_OSLDR_PEHDR_MACHINE     IMAGE_FILE_MACHINE_I386
    Print(L"#define       APP_OSLDR_PEHDR_MACHINE     EFI_IMAGE_MACHINE_IA32\n");
    if (PeHdr->FileHeader.Machine != APP_OSLDR_PEHDR_MACHINE) {
    	Print(L"PeHdr->FileHeader.Machine != APP_OSLDR_PEHDR_MACHINE\n");
        return FALSE;
    }
    Print(L"Despues de PeHdr->FileHeader.Machine != APP_OSLDR_PEHDR_MACHINE\n");
    return TRUE;
}