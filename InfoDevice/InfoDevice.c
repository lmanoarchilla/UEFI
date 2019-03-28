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
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Protocol/LoadedImage.h>

#include <Protocol/DevicePath.h>
#include <Protocol/SimpleFileSystem.h>

#include <Guid/FileInfo.h>
#include <Guid/FileSystemInfo.h>
//////////////////////
//EFI_MEMORY_TYPE          PoolAllocationType;

VOID * AllocatePoolSize (IN UINTN Size );
VOID ListFileInfo(EFI_FILE_INFO* FileInfo);
void WaitKey();
EFI_STATUS ListFiles(EFI_FILE_HANDLE NewFileHandle);
EFI_STATUS OpenFolder(EFI_FILE_HANDLE ,CHAR16 *FileName);

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
  Print(L"*****************InfoDevice***************************************************\n");
  gST->ConOut->ClearScreen(gST->ConOut);
  Print(L"Información del dispositivo y de la imagen de la aplicación se está ejecutando\n");

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

    /*
    Print (L"Image FilePath: %X\n",ThisImage->FilePath);
    Print (L"Image handle: %X\n",ThisImage->DeviceHandle);
    Print (L"Image Base: %X\n",ThisImage->ImageBase);
    Print (L"Image Size: %X\n",ThisImage->ImageSize);
    */
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

    Print(L"Acceso a la carpeta raiz de la aplicación y a sus archivos\n");


    //Provee acceso basico para acceder a un dispositivo

    EFI_GUID FileSystemProtocolGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem= NULL;

    efiStatus = gBS->HandleProtocol(ThisImage->DeviceHandle,&FileSystemProtocolGuid,(void *)&FileSystem);
    if (EFI_ERROR (efiStatus)) {
        Print(L"No se ha podido cargar el interfaz para FileSystemProtocol \n");
    }else{
        Print(L"Interfaz del Protocolo FileSystemProtocol de imagen cargado \n");
    }
    //Opens the root directory on a volume.
    //A pointer to the location to return the opened file handle for the root directory
    EFI_FILE_HANDLE ParentHandle;    
    efiStatus = FileSystem->OpenVolume(FileSystem, &ParentHandle);
    if (EFI_ERROR (efiStatus)) {
      Print(L"No se ha podido cargar el el sistema de ficheros donde nos encontramos \n");
    }else{
      Print(L"Sistema de ficheros de la imagen actual en el que nos enconramos cargado  \n");     
    }     


    //Saca información del sistema de ficheros del volumen 
    
    EFI_FILE_SYSTEM_INFO *VolumeInfo;
    UINTN Size = SIZE_OF_EFI_FILE_SYSTEM_INFO + 1024;
    VolumeInfo = (EFI_FILE_SYSTEM_INFO *) AllocatePoolSize (Size);
      

    efiStatus = ParentHandle->GetInfo (ParentHandle, &gEfiFileSystemInfoGuid, &Size, VolumeInfo);
    if (EFI_ERROR (efiStatus)) {
      Print(L"No se ha podido cargar la información del volumen \n");
    }else{
      Print(L"Información del volumen actual  \n");     
    }  
    Print(L"Etiqueta de Volumen:  %s\n",VolumeInfo->VolumeLabel);  
    Print(L"ReadOnly:  ");  
    if (VolumeInfo->ReadOnly) {
      Print (L" (ro)\n");
    } else {
      Print (L" (rw)\n");
    }
    Print(L"Tamaño de volumen:  %x bytes\n",VolumeInfo->VolumeSize);
    Print(L"Espacio libre:  %x bytes\n",VolumeInfo->FreeSpace);
    Print(L"Tamaño de bloques:  %x bytes\n",VolumeInfo->BlockSize);

    WaitKey();

    OpenFolder(ParentHandle,L".");
    ParentHandle->Flush (ParentHandle);
    ParentHandle->Close (ParentHandle);

    //FreePool (VolumeInfo);
    efiStatus = EFI_SUCCESS; 


  return EFI_SUCCESS;
}

/*++
Routine Description:
  Allocates pool memory.
Arguments:
  Size                 - Size in bytes of the pool being requested.
Returns:
  EFI_SUCEESS          - The requested number of bytes were allocated.
  EFI_OUT_OF_RESOURCES - The pool requested could not be allocated.
--*/ 

VOID * AllocatePoolSize (IN UINTN Size){
  EFI_STATUS  Status;
  VOID        *p;

  Status = gBS->AllocatePool (EfiBootServicesCode, Size, &p);
  if (EFI_ERROR (Status)) {
    Print (L"AllocatePool: out of pool  \n");
    p = NULL;
  }
  return p;
}

VOID ListFileInfo(EFI_FILE_INFO* FileInfo){
    Print(L"%d\t %d\t %d\t",  FileInfo->Size, FileInfo->FileSize, FileInfo->PhysicalSize);
    Print(L"%s \n", FileInfo->FileName);
}

void WaitKey(){
    EFI_STATUS   Status = 0;
    UINTN        Index=0;
    EFI_INPUT_KEY  Key;

    Status = gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &Index);
    if (EFI_ERROR(Status)) {
        Print(L"WaitKey: WaitForEvent Error!\n");
    }
    Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
    if (EFI_ERROR(Status)) {
        Print(L"WaitKey: ReadKeyStroke Error!\n");
    }
}

EFI_STATUS ListFiles(EFI_FILE_HANDLE NewFileHandle){

    EFI_STATUS efiStatus;
    EFI_FILE_INFO *FileInfo;
    UINTN FileInfoSize;
    UINTN i=0,j=0;
    FileInfoSize = SIZE_OF_EFI_FILE_INFO + 1024;
    FileInfo = (EFI_FILE_INFO *) AllocatePoolSize (FileInfoSize);

    gST->ConOut->ClearScreen(gST->ConOut);
    Print(L"Size \tFileSize \tPhysical Size \tName \t\n");
      

    efiStatus = NewFileHandle->SetPosition (NewFileHandle, 0);
    while(!(FileInfoSize == 0)) {
      FileInfoSize = SIZE_OF_EFI_FILE_INFO + 1024;
      efiStatus  = NewFileHandle->Read (NewFileHandle, &FileInfoSize, FileInfo);
      i++;      
      if ((i%15)==0){        
        WaitKey();
        i=0;
      }

      if (FileInfo->Attribute & EFI_FILE_DIRECTORY) {          
          Print(L"d \t");
          j++;
      }
      
      ListFileInfo(FileInfo);     
    }
    Print(L"Numero de ficheros: %d\n",i);
    Print(L"Numero de directorios: %d\n",j);
    return EFI_SUCCESS;
  }
EFI_STATUS OpenFolder(EFI_FILE_HANDLE ParentHandle,CHAR16 *FileName){

  //Examinamos el archivo root
    EFI_STATUS efiStatus;

    EFI_FILE_INFO *FileInfo;
    UINTN FileInfoSize;
    FileInfoSize = SIZE_OF_EFI_FILE_INFO + 1024;
    FileInfo = (EFI_FILE_INFO *) AllocatePoolSize (FileInfoSize);

    EFI_FILE_HANDLE NewFileHandle;    
     
    efiStatus = ParentHandle->Open(ParentHandle,&NewFileHandle, L".", EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR (efiStatus)) {
      Print(L"No se ha podido abrir el archivo root \n");
    }else{
      Print(L"Se ha podido abrir el archivo root \n");
    }

    if (NewFileHandle == NULL) {
      return EFI_SUCCESS;
    }

    efiStatus = ParentHandle->GetInfo (NewFileHandle, &gEfiFileInfoGuid, &FileInfoSize, FileInfo);
    //efiStatus = NewFileHandle->GetInfo (NewFileHandle, &gEfiFileInfoGuid, &FileInfoSize, FileInfo);
    if (EFI_ERROR (efiStatus)) {
      Print(L"No se ha podido cargar la información del archivo root \n");
    }else{
      Print(L"Información del archivo root  \n");     
    } 
    if (FileInfo==NULL){
      Print(L"fileinfo ==NULL");
    }

    gST->ConOut->ClearScreen(gST->ConOut);
    Print(L"Size \tFileSize \tPhysical Size \tName \t\n");    
    

    efiStatus = NewFileHandle->SetPosition (NewFileHandle, 0);

    ListFiles(NewFileHandle);
    return EFI_SUCCESS;
}