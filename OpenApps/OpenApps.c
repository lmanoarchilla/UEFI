/** @file
  This is a simple shell application  de Acceso a la carpeta raiz de la aplicación y a sus archivos

  Copyright (c) 2008, Intel Corporation                                                         
  All rights reserved. This program and the accompanying materials                          
  are licensed and made available under the terms and conditions of the BSD License         
  which accompanies this distribution.  The full text of the license may be found at        
  http://opensource.org/licenses/bsd-license.php                                            

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

**/

#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>

/////
#include <Library/UefiBootServicesTableLib.h>

/* Every EFI driver and application is passed an image handle when it is loaded.
  This image handle will contain a Loaded Image Protocol.
*/

#include <Protocol/LoadedImage.h>

#include <Protocol/DevicePath.h>
#include <Library/DevicePathLib.h>
//Provides library functions to construct and parse UEFI Device Paths
#include <Protocol/SimpleFileSystem.h>

#include <Guid/FileInfo.h>
/**
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
UefiMain (IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable ){  
  EFI_STATUS efiStatus;

  DEBUG ((0xffffffff, "\n\nUEFI Base Training DEBUG DEMO:Info device\n") );
  Print(L"InfoDevice\n"); 
  Print(L"Información del dispositivo y de la imagen de la aplicación se está ejecutando\n");

  gST->ConOut->ClearScreen(gST->ConOut);
  Print(L"Información del dispositivo y de la imagen de la aplicación se está ejecutando\n");

  //EFI_HANDLE GlobalImage = ImageHandle;
  /*
  // Todo esto lo usamos si queremos usar la instancia de un protocolo 
  EFI_GUID LoadedImageProtocolGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
  
  EFI_LOADED_IMAGE *LoadedImageProtocol = NULL;
  
  //Returns the first protocol instance that matches the given protocol
  //LoadedImageProtocolGuid: Protocol to search for 
  //Registration -> Null
  //LoadedImageProtocol: Interface a pointer to the first interface that matches Protocol and Registration
  efiStatus = gBS->LocateProtocol(&LoadedImageProtocolGuid, NULL, (void *)&LoadedImageProtocol);
  if (EFI_ERROR (efiStatus)) {

      Print(L"Error: No se ha podido localizar el Protocolo LoadedImageProtocolGuid que nos permite obtener información de la imagen  \n");

      //Induces a fine-grained stall
      gBS->Stall(3 * 1000 * 1000);
      return EFI_LOAD_ERROR;
    }else{
      Print(L"Protocolo LoadedImageProtocolGuid localizado\n");
    }

  Print (L"Image FilePath: %X\n",LoadedImageProtocol->FilePath);
  Print (L"Image handle: %X\n",LoadedImageProtocol->DeviceHandle);
  Print (L"Image Base: %X\n",LoadedImageProtocol->ImageBase);
  Print (L"Image Size: %X\n",LoadedImageProtocol->ImageSize);
*/
  // Lo usamos para preguntar al handle por un protocolo especifico, en este caso el handle con el que obtenemos informacion de la imagen UEFI en la que estamos
  EFI_GUID LoadedImageProtocolGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID; // The GUID of device path protocol
  EFI_LOADED_IMAGE *ThisImage = NULL;

  // Queries a handle to determine if it supports a specified protocol.
  // ImageHandle: The handle being queried
  // LoadedImageProtocolGuid:  GUID of the protocol
  // ThisImage: Supplies the address where a pointer to the corresponding Protocol Interface is returned
  efiStatus = gBS->HandleProtocol (ImageHandle, &LoadedImageProtocolGuid,(void *)&ThisImage);
  if (EFI_ERROR (efiStatus)) {
      Print(L"No se ha podido cargar el interfaz para LoadedImageProtocolGuid \n");
    }else{
      Print(L"Interfaz del Protocolo LoadedImageProtocolGuid de imagen cargado\n");
    }
    Print (L"Image FilePath: %X\n",ThisImage->FilePath);
    Print (L"Image handle: %X\n",ThisImage->DeviceHandle);
    Print (L"Image Base: %X\n",ThisImage->ImageBase);
    Print (L"Image Size: %X\n",ThisImage->ImageSize);


    EFI_GUID DevicePathProtocolGuid = EFI_DEVICE_PATH_PROTOCOL_GUID;// The GUID of device path protocol
    EFI_DEVICE_PATH_PROTOCOL *DevicePath =NULL;

    
    
          
       
    ////Informacion sobre el dispositivo donde se aloja esta imagen EFi
    // Queries a handle to determine if it supports a specified protocol.
    // ImageHandle: The handle being queried
    // LoadedImageProtocolGuid:  GUID of the protocol
    // ThisImage: Supplies the address where a pointer to the corresponding Protocol Interface is returned
    efiStatus = gBS->HandleProtocol(ThisImage->DeviceHandle,&DevicePathProtocolGuid,(void *)&DevicePath);
    if (EFI_ERROR (efiStatus)) {
        Print(L"No se ha podido cargar el interfaz para DevicePathProtocol \n");
    }else{
      Print(L"Interfaz del Protocolo DevicePathProtocol de imagen cargado \n");
    }

    Print (L"Image device : %d\n",DevicePath->Type);
    Print (L"Image device : %s\n",ConvertDevicePathToText(DevicePath,FALSE,FALSE));
    //Print (L"Image file: %s\n",DevicePathToStr(ThisImage->FilePath));

    Print(L"F3.Acceso a la carpeta raiz de la aplicación y a sus archivos\n");


    //Provee acceso basico para acceder a un dispositivo

    EFI_GUID FileSystemProtocolGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem= NULL;   
        

    efiStatus = gBS->HandleProtocol(ThisImage->DeviceHandle,&FileSystemProtocolGuid,(void *)&FileSystem);
    if (EFI_ERROR (efiStatus)) {
        Print(L"No se ha podido cargar el interfaz para FileSystemProtocol \n");
      }else{
        Print(L"Interfaz del Protocolo FileSystemProtocol de imagen cargado \n");
      }
      
      EFI_FILE_PROTOCOL *RootFs=0;
      //Opens the root directory on a volume.
      //A pointer to the location to return the opened file handle for the root directory
      efiStatus = FileSystem->OpenVolume(FileSystem,&RootFs);

      if (EFI_ERROR (efiStatus)) {
        Print(L"No se ha podido cargar el el sistema de ficheros donde nos encontramos \n");
      }else{
        Print(L"Sistema de ficheros de la imagen actual en el que nos enconramos cargado  \n");     
      }       
     
      EFI_FILE_HANDLE CurrentFileHandle = RootFs;
      EFI_FILE_PROTOCOL *NewFileHandle=NULL;
      //CHAR16 *FileName=L"\\Efi";
      CHAR16 *FileName=L"\\Agenda.efi";
      //CHAR16 *FileName=L"\\helloworld.txt";
      efiStatus = CurrentFileHandle->Open(CurrentFileHandle,&NewFileHandle, FileName, EFI_FILE_MODE_READ, 0);
      if (EFI_ERROR (efiStatus)) {
          Print(L"No se ha podido abrir el Volumen \n");
      }else{
        Print(L"Se ha podido abrir el Volumen \n");
      }

      UINTN Size = 0x0001000;
      void *Buffer =NULL;

      while (Size == 0x0001000){
        
        efiStatus = gBS->AllocatePool(EfiLoaderData, Size, (VOID *)&Buffer);
        if (EFI_ERROR (efiStatus)) {
            Print(L"No se ha podido establecer el buffer para el acceso al archivo\n");
          }else{
            Print(L"hemos establecido el buffer para el acceso al archivo\n");
          }

        efiStatus = NewFileHandle->Read(NewFileHandle, &Size, Buffer);
        if (EFI_ERROR (efiStatus)) {
            Print(L"No se ha podido leer el archivo \n");
        }else{
          Print(L"hemos leido el archivo\n");
          Print(L"%X bytes of %s read into memory at %X\n",Size, FileName, Buffer);
        }
      }

   if (CurrentFileHandle == NULL) {
    return EFI_SUCCESS;
  }
  EFI_FILE_INFO   *FileInfo = NULL;
  UINTN           FileInfoSize = 0; 
  efiStatus = CurrentFileHandle->GetInfo(CurrentFileHandle,&gEfiFileInfoGuid,&FileInfoSize,NULL);
  if (efiStatus == EFI_BUFFER_TOO_SMALL){    
    efiStatus = gBS->AllocatePool(EfiLoaderData, FileInfoSize, (VOID *)&FileInfo);
    if (EFI_ERROR (efiStatus)) {
      Print(L"No se ha podido establecer el buffer para el acceso al archivo\n");
    }else{
      Print(L"hemos establecido el buffer para el acceso al archivo\n");
    }
    efiStatus = CurrentFileHandle->GetInfo(CurrentFileHandle,&gEfiFileInfoGuid,&FileInfoSize,FileInfo);    
    if (EFI_ERROR(efiStatus) && (FileInfo != NULL)) {      
      FileInfo = NULL;
    }
  }
  Print(L"%X bytes of %S read into memory at %X\n",FileInfoSize, FileInfo->FileName, FileInfo);
   Print(L"EFI_FILE_INFO structure: %d\n", FileInfo->Size);
   Print(L"Tamaño del archivo: %d\n", FileInfo->FileSize);
   Print(L"Tamaño del archivo en el volumen: %d\n", FileInfo->PhysicalSize);
   Print(L"Atributos: %d\n", FileInfo->Attribute);
   
   EFI_DEVICE_PATH *appPath;
   appPath = FileDevicePath(ThisImage->DeviceHandle, FileName);
   EFI_HANDLE MainImageHandle = ImageHandle;
   EFI_HANDLE AppHandle;
   efiStatus = gBS->LoadImage(FALSE,MainImageHandle, appPath, NULL, 0,&AppHandle);
   if (EFI_ERROR(efiStatus) && (AppHandle != NULL)) {      
      AppHandle = NULL;
      Print(L"LoadINFO fail\n");
    }
     Print(L"LoadINFO\n");

  efiStatus = gBS->StartImage(AppHandle, NULL, NULL);
  if (EFI_ERROR(efiStatus)) {
    Print(L"Unable to start App");
  
  }
   Print(L"StartImage\n");
  return EFI_SUCCESS;
}