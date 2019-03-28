//misc.c
EFI_STATUS LibGetSystemConfigurationTable	(	IN EFI_GUID * 	TableGuid, IN OUT VOID ** 	Table){
    UINTN Index;
 	Print(L"LibGet  NumberOfTableEntries:%d\n",gST->NumberOfTableEntries);
    for(Index=0;Index < gST->NumberOfTableEntries;Index++) {
        if (CompareGuid(TableGuid,&(gST->ConfigurationTable[Index].VendorGuid))==1) {
            *Table = gST->ConfigurationTable[Index].VendorTable;
            Print(L" compare:%x\n",CompareGuid(TableGuid,&(gST->ConfigurationTable[Index].VendorGuid)));
            return EFI_SUCCESS;
        }
    }
return EFI_NOT_FOUND;
}

VOID *
AllocatePool (
    IN UINTN                Size
    )
{
    EFI_STATUS              Status;
    VOID                    *p;

    Status = uefi_call_wrapper(BS->AllocatePool, 3, PoolAllocationType, Size, &p);
    if (EFI_ERROR(Status)) {
        DEBUG((D_ERROR, "AllocatePool: out of pool  %x\n", Status));
        p = NULL;
    }
    return p;
}

BOOLEAN
GrowBuffer(
    IN OUT EFI_STATUS   *Status,
    IN OUT VOID         **Buffer,
    IN UINTN            BufferSize
    )
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
            FreePool (*Buffer);
        }

        *Buffer = AllocatePool (BufferSize);

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
        FreePool (*Buffer);
        *Buffer = NULL;
    }

    return TryAgain;
}
EFI_MEMORY_DESCRIPTOR *
LibMemoryMap (
    OUT UINTN               *NoEntries,
    OUT UINTN               *MapKey,
    OUT UINTN               *DescriptorSize,
    OUT UINT32              *DescriptorVersion
    )
{
    EFI_STATUS              Status;
    EFI_MEMORY_DESCRIPTOR   *Buffer;
    UINTN                   BufferSize;

    //
    // Initialize for GrowBuffer loop
    //

    Status = EFI_SUCCESS;
    Buffer = NULL;
    BufferSize = sizeof(EFI_MEMORY_DESCRIPTOR);

    //
    // Call the real function
    //

    while (GrowBuffer (&Status, (VOID **) &Buffer, BufferSize)) {
        Status = uefi_call_wrapper(BS->GetMemoryMap, 5, &BufferSize, Buffer, MapKey, DescriptorSize, DescriptorVersion);
    }

    //
    // Convert buffer size to NoEntries
    //

    if (!EFI_ERROR(Status)) {
        *NoEntries = BufferSize / *DescriptorSize;
    }

    return Buffer;
}