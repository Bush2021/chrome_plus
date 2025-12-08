#pragma once

#include "../MinDef.h"

EXTERN_C_START

#pragma region Reparse Tag

#define IO_REPARSE_TAG_DRIVE_EXTENDER           (0x80000005L)
#define IO_REPARSE_TAG_FILTER_MANAGER           (0x8000000BL)
#define IO_REPARSE_TAG_IIS_CACHE                (0xA0000010L)
#define IO_REPARSE_TAG_APPXSTRM                 (0xC0000014L)
#define IO_REPARSE_TAG_DFM                      (0x80000016L)
#define IO_REPARSE_TAG_LX_SYMLINK               (0xA000001DL)
#define IO_REPARSE_TAG_LX_FIFO                  (0x80000024L)
#define IO_REPARSE_TAG_LX_CHR                   (0x80000025L)
#define IO_REPARSE_TAG_LX_BLK                   (0x80000026L)

///////////////////////////////////////////////////////////////////////////////
//
// Non-Microsoft tags for reparse points
//
///////////////////////////////////////////////////////////////////////////////

//
// Tag allocated to CONGRUENT, May 2000. Used by IFSTEST
//

#define IO_REPARSE_TAG_IFSTEST_CONGRUENT        (0x00000009L)

//
//  Tag allocated to Moonwalk Universal for HSM
//  GUID: 257ABE42-5A28-4C8C-AC46-8FEA5619F18F
//

#define IO_REPARSE_TAG_MOONWALK_HSM             (0x0000000AL)

//
//  Tag allocated to Tsinghua University for Research purposes
//  No released products should use this tag
//  GUID: b86dff51-a31e-4bac-b3cf-e8cfe75c9fc2
//

#define IO_REPARSE_TAG_TSINGHUA_UNIVERSITY_RESEARCH (0x0000000BL)

//
// Tag allocated to ARKIVIO for HSM
//

#define IO_REPARSE_TAG_ARKIVIO                  (0x0000000CL)

//
//  Tag allocated to SOLUTIONSOFT for name surrogate
//

#define IO_REPARSE_TAG_SOLUTIONSOFT             (0x2000000DL)

//
//  Tag allocated to COMMVAULT for HSM
//

#define IO_REPARSE_TAG_COMMVAULT                (0x0000000EL)

//
//  Tag allocated to Overtone Software for HSM
//

#define IO_REPARSE_TAG_OVERTONE                 (0x0000000FL)

//
//  Tag allocated to Symantec (formerly to KVS Inc) for HSM
//  GUID: A49F7BF6-77CA-493c-A0AA-18DBB28D1098
//

#define IO_REPARSE_TAG_SYMANTEC_HSM2            (0x00000010L)

//
//  Tag allocated to Enigma Data for HSM
//

#define IO_REPARSE_TAG_ENIGMA_HSM               (0x00000011L)

//
//  Tag allocated to Symantec for HSM
//  GUID: B99F4235-CF1C-48dd-9E6C-459FA289F8C7
//

#define IO_REPARSE_TAG_SYMANTEC_HSM             (0x00000012L)

//
//  Tag allocated to INTERCOPE for HSM
//  GUID: F204BE2D-AEEB-4728-A31C-C7F4E9BEA758}
//

#define IO_REPARSE_TAG_INTERCOPE_HSM            (0x00000013L)

//
//  Tag allocated to KOM Networks for HSM
//

#define IO_REPARSE_TAG_KOM_NETWORKS_HSM         (0x00000014L)

//
//  Tag allocated to MEMORY_TECH for HSM
//  GUID: E51BA456-046C-43ea-AEC7-DC1A87E1FD49
//

#define IO_REPARSE_TAG_MEMORY_TECH_HSM          (0x00000015L)

//
//  Tag allocated to BridgeHead Software for HSM
//  GUID: EBAFF6E3-F21D-4496-8342-58144B3D2BD0
//

#define IO_REPARSE_TAG_BRIDGEHEAD_HSM           (0x00000016L)

//
//  Tag allocated to OSR for samples reparse point filter
//  GUID: 3740c860-b19b-11d9-9669-0800200c9a66
//

#define IO_REPARSE_TAG_OSR_SAMPLE               (0x20000017L)

//
//  Tag allocated to Global 360 for HSM
//  GUID: C4B51F66-7F00-4c55-9441-8A1B159F209B
//

#define IO_REPARSE_TAG_GLOBAL360_HSM            (0x00000018L)

//
//  Tag allocated to Altiris for HSM
//  GUID: fc1047eb-fb2d-45f2-a2f4-a71c1032fa2dB
//

#define IO_REPARSE_TAG_ALTIRIS_HSM              (0x00000019L)

//
//  Tag allocated to Hermes for HSM
//  GUID: 437E0FD5-FCB4-42fe-877A-C785DA662AC2
//

#define IO_REPARSE_TAG_HERMES_HSM               (0x0000001AL)

//
//  Tag allocated to PointSoft for HSM
//  GUID: 547BC7FD-9604-4deb-AE07-B6514DF5FBC6
//

#define IO_REPARSE_TAG_POINTSOFT_HSM            (0x0000001BL)

//
//  Tag allocated to GRAU Data Storage for HSM
//  GUID: 6662D310-5653-4D10-8C31-F8E166D1A1BD
//

#define IO_REPARSE_TAG_GRAU_DATASTORAGE_HSM     (0x0000001CL)

//
//  Tag allocated to CommVault for HSM
//  GUID: cc38adf3-c583-4efa-b183-72c1671941de
//

#define IO_REPARSE_TAG_COMMVAULT_HSM            (0x0000001DL)


//
//  Tag allocated to Data Storage Group for single instance storage
//  GUID: C1182673-0562-447a-8E40-4F0549FDF817
//

#define IO_REPARSE_TAG_DATASTOR_SIS             (0x0000001EL)


//
//  Tag allocated to Enterprise Data Solutions, Inc. for HSM
//  GUID: EB63DF9D-8874-41cd-999A-A197542CDAFC
//

#define IO_REPARSE_TAG_EDSI_HSM                 (0x0000001FL)


//
//  Tag allocated to HP StorageWorks Reference Information Manager for Files (HSM)
//  GUID: 3B0F6B23-0C2E-4281-9C19-C6AEEBC88CD8
//

#define IO_REPARSE_TAG_HP_HSM                   (0x00000020L)


//
//  Tag allocated to SER Beteiligung Solutions Deutschland GmbH (HSM)
//  GUID: 55B673F0-978E-41c5-9ADB-AF99640BE90E
//

#define IO_REPARSE_TAG_SER_HSM                  (0x00000021L)


//
//  Tag allocated to Double-Take Software (formerly NSI Software, Inc.) for HSM
//  GUID: f7cb0ce8-453a-4ae1-9c56-db41b55f6ed4
//

#define IO_REPARSE_TAG_DOUBLE_TAKE_HSM          (0x00000022L)


//
//  Tag allocated to Beijing Wisdata Systems CO, LTD for HSM
//  GUID: d546500a-2aeb-45f6-9482-f4b1799c3177
//

#define IO_REPARSE_TAG_WISDATA_HSM              (0x00000023L)


//
//  Tag allocated to Mimosa Systems Inc for HSM
//  GUID: 8ddd4144-1a22-404b-8a5a-fcd91c6ee9f3
//

#define IO_REPARSE_TAG_MIMOSA_HSM               (0x00000024L)


//
//  Tag allocated to H&S Heilig und Schubert Software AG for HSM
//  GUID: 77CA30C0-E5EC-43df-9E44-A4910378E284
//

#define IO_REPARSE_TAG_HSAG_HSM                 (0x00000025L)


//
//  Tag allocated to Atempo Inc. (Atempo Digital Archive)  for HSM
//  GUID: 9B64518A-D6A4-495f-8D01-392F38862F0C
//

#define IO_REPARSE_TAG_ADA_HSM                  (0x00000026L)


//
//  Tag allocated to Autonomy Corporation for HSM
//  GUID: EB112A57-10FC-4b42-B590-A61897FDC432
//

#define IO_REPARSE_TAG_AUTN_HSM                 (0x00000027L)


//
//  Tag allocated to Nexsan for HSM
//  GUID: d35eba9a-e722-445d-865f-dde1120acf16
//

#define IO_REPARSE_TAG_NEXSAN_HSM               (0x00000028L)


//
//  Tag allocated to Double-Take for SIS
//  GUID: BDA506C2-F74D-4495-9A8D-44FD8D5B4F42
//

#define IO_REPARSE_TAG_DOUBLE_TAKE_SIS          (0x00000029L)


//
//  Tag allocated to Sony for HSM
//  GUID: E95032E4-FD81-4e15-A8E2-A1F078061C4E
//

#define IO_REPARSE_TAG_SONY_HSM                 (0x0000002AL)


//
//  Tag allocated to Eltan Comm for HSM
//  GUID: E1596D9F-44D8-43f4-A2D6-E9FE8D3E28FB
//

#define IO_REPARSE_TAG_ELTAN_HSM                (0x0000002BL)


//
//  Tag allocated to Utixo LLC for HSM
//  GUID: 5401F960-2F95-46D0-BBA6-052929FE2C32
//

#define IO_REPARSE_TAG_UTIXO_HSM                (0x0000002CL)


//
//  Tag allocated to Quest Software for HSM
//  GUID: D546500A-2AEB-45F6-9482-F4B1799C3177
//

#define IO_REPARSE_TAG_QUEST_HSM                (0x0000002DL)


//
//  Tag allocated to DataGlobal GmbH for HSM
//  GUID: 7A09CA83-B7B1-4614-ADFD-0BD5F4F989C9
//

#define IO_REPARSE_TAG_DATAGLOBAL_HSM           (0x0000002EL)


//
//  Tag allocated to Qi Tech LLC for HSM
//  GUID: C8110B39-A4CE-432E-B58A-FBEAD296DF03
//

#define IO_REPARSE_TAG_QI_TECH_HSM              (0x2000002FL)

//
//  Tag allocated to DataFirst Corporation for HSM
//  GUID: E0E40591-6434-479f-94AC-DECF6DAEFB5C
//

#define IO_REPARSE_TAG_DATAFIRST_HSM            (0x00000030L)

//
//  Tag allocated to C2C Systems for HSM
//  GUID: 6F2F829C-36AE-4E88-A3B6-E2C24377EA1C
//

#define IO_REPARSE_TAG_C2CSYSTEMS_HSM           (0x00000031L)

//
//  Tag allocated to Waterford Technologies for deduplication
//  GUID: 0AF8B999-B8E8-408b-805F-5448E68F9274
//

#define IO_REPARSE_TAG_WATERFORD                (0x00000032L)

//
//  Tag allocated to Riverbed Technology for HSM
//  GUID: 3336274-255B-4038-9D39-14B0EC3F8256
//

#define IO_REPARSE_TAG_RIVERBED_HSM             (0x00000033L)

//
//  Tag allocated to Caringo, Inc.  for HSM
//  GUID: B92426FA-D35F-48DB-A452-8FD557A23353
//

#define IO_REPARSE_TAG_CARINGO_HSM              (0x00000034L)

//
//  Tag allocated to MaxiScale, Inc. for HSM
//  GUID: 643B4714-BA13-427b-B771-C5BFDE787BB7
//

#define IO_REPARSE_TAG_MAXISCALE_HSM            (0x20000035L)

//
//  Tag allocated to Citrix Systems for profile management
//  GUID: B9150EDE-5845-4818-841B-5BCBB3B848E3
//

#define IO_REPARSE_TAG_CITRIX_PM                (0x00000036L)

//
//  Tag allocated to OpenAFS for DFS
//  GUID: EF21A155-5C92-4470-AB3B-370403D96369
//

#define IO_REPARSE_TAG_OPENAFS_DFS              (0x00000037L)

//
//  Tag allocated to ZL Technologies Inc for HSM
//  GUID: A521FE7A-EB10-4148-BAC7-264359827B7E
//

#define IO_REPARSE_TAG_ZLTI_HSM                 (0x00000038L)

//
//  Tag allocated to EMC Corporation for HSM
//  GUID: 119EA2B9-8979-48b9-B4CE-5082AF2D81E5
//

#define IO_REPARSE_TAG_EMC_HSM                  (0x00000039L)

//
//  Tag allocated to VMware for profile management
//  GUID: 6D020A57-C9BB-4DA4-A43F-49686D8D5E77
//

#define IO_REPARSE_TAG_VMWARE_PM                (0x0000003AL)

//
//  Tag allocated to Arco Computer Products for backup
//  GUID: C933F72B-A64D-44d9-8CD9-F339D12390CC
//

#define IO_REPARSE_TAG_ARCO_BACKUP              (0x0000003BL)

//
//  Tag allocated to Carroll-Net for HSM
//  GUID: 805EB191-564B-415a-A78C-9ED0AF8E02FF
//

#define IO_REPARSE_TAG_CARROLL_HSM              (0x0000003CL)

//
//  Tag allocated to ComTrade for HSM
//  GUID: D546500A-2AEB-45F6-9482-F4B1799C3177
//

#define IO_REPARSE_TAG_COMTRADE_HSM             (0x0000003DL)

//
//  Tag allocated to EaseVault for HSM
//  GUID: BBA65D6F-F8A0-48CC-B748-DBD5FFFCCFB1
//

#define IO_REPARSE_TAG_EASEVAULT_HSM            (0x0000003EL)

//
//  Tag allocated to Hitachi Data Systems for HSM
//  GUID: DC095FD2-AC3C-46BA-9E58-DD182BE86AF4
//

#define IO_REPARSE_TAG_HDS_HSM                  (0x0000003FL)

//
//  Tag allocated to Maginatics for network redirector
//  GUID: 1907AE87-388D-4854-AF63-B585A619DF28
//

#define IO_REPARSE_TAG_MAGINATICS_RDR           (0x00000040L)

//
//  Tag allocated to Google for HSM
//  GUID: 40550C94-68DB-4726-9C62-47BB88577BD0
//

#define IO_REPARSE_TAG_GOOGLE_HSM               (0x00000041L)

//
//  Tag allocated to Quaddra for HSM
//  GUID: 491D6619-20C3-4BFA-AF98-4EA2A7F53F1D
//

#define IO_REPARSE_TAG_QUADDRA_HSM              (0x00000042L)

//
//  Tag allocated to HP for backup
//  GUID: 0c5ed442-4928-473e-970e-5389ff2b4328
//

#define IO_REPARSE_TAG_HP_BACKUP                (0x00000043L)

//
//  Tag allocated to DropBox for HSM
//  GUID: C5BB0F16-68E9-4456-B6B9-5F5EE5F89965
//

#define IO_REPARSE_TAG_DROPBOX_HSM              (0x00000044L)

//
//  Tag allocated to Adobe for HSM
//  GUID: 48F5DB1C-ED3C-44F7-8EBA-3623C09D6179
//

#define IO_REPARSE_TAG_ADOBE_HSM                (0x00000045L)

//
//  Tag allocated to HP for HPE Data Protector HSM
//  GUID: 3370DFC4-DCED-4DEB-AD0C-9A2416B3198B
//

#define IO_REPARSE_TAG_HP_DATA_PROTECT          (0x00000046L)

//
//  Tag allocated to Activision for HSM
//  GUID: 18CC35B3-5DFF-408E-B42E-9FA6731BC506
//

#define IO_REPARSE_TAG_ACTIVISION_HSM           (0x00000047L)

//
//  Tag allocated to Hitachi for HSM
//  GUID: DFFD30B6-6CC0-46A8-94C2-AAE32C9B16D1
//

#define IO_REPARSE_TAG_HDS_HCP_HSM              (0x00000048L)

//
//  Tag allocated to AuriStor File System
//  GUID: AFBD291F-3DF8-46BA-BE87-47A632714BB7
//

#define IO_REPARSE_TAG_AURISTOR_FS              (0x00000049L)

//
//  Tag allocated to ItStation for restore
//  GUID: C33A7A21-6142-4860-9AAA-0008F1064616
//

#define IO_REPARSE_TAG_ITSTATION                (0x0000004AL)

//
//  Tag allocated to Spharsoft for dedup
//  GUID: 2AB7C758-E6AA-43FB-9194-80A436E22F26
//

#define IO_REPARSE_TAG_SPHARSOFT                (0x0000004BL)

//
//  Tag allocated to AlertBoot for encryption
//  GUID: 80050DB9-52D3-4742-B28B-3918177242D5
//

#define IO_REPARSE_TAG_ALERTBOOT                (0x2000004CL)

//
//  Tag allocated to mTalos for monitoring
//  GUID: C51BB90B-77D2-43B9-8E88-CC5745E977FF
//

#define IO_REPARSE_TAG_MTALOS                   (0x0000004DL)

//
//  Tag allocated to CTERA for HSM
//  GUID: A0431A3B-7789-485C-8638-FCA288FDBA71
//

#define IO_REPARSE_TAG_CTERA_HSM                (0x0000004EL)

//
//  Tag allocated to Nippon Techno Lab for HSM
//  GUID: 8E5C2A8D-7344-4923-A741-2E9EFF128126
//

#define IO_REPARSE_TAG_NIPPON_HSM               (0x0000004FL)

//
//  Tag allocated to Redstor for HSM
//  GUID: E6636830-4534-4FA2-9D4F-DDD22818EDCC
//

#define IO_REPARSE_TAG_REDSTOR_HSM              (0x00000050L)

//
//  Tag allocated to NeuShield for monitoring
//  GUID: 585414E1-6E0E-46B6-8838-ED837173F590
//

#define IO_REPARSE_TAG_NEUSHIELD                (0x00000051L)

//
//  Tag allocated to DOR for HSM
//  GUID: 51A0CF2A-5221-4093-9025-06D80C2AA122
//

#define IO_REPARSE_TAG_DOR_HSM                  (0x00000052L)

//
//  Tag allocated to StorageCraft Technology for backup
//  GUID: 5E2913DA-EAEE-47CB-B866-EDDCAEB3EBC3
//

#define IO_REPARSE_TAG_SHX_BACKUP               (0x00000053L)

//
//  Tag allocated to NVIDIA Corporation for union file system
//  GUID: E2700D02-2ABA-4629-805D-956A381244A5
//

#define IO_REPARSE_TAG_NVIDIA_UNIONFS           (0x20000054L)

//
//  Tag allocated to HubStor for HSM
//  GUID: 9492F18B-303F-4394-9804-8ED21710CA8C
//

#define IO_REPARSE_TAG_HUBSTOR_HSM              (0x00000055L)

//
//  Tag allocated to iManage for HSM
//  GUID: 7C334C3C-0C03-4659-93D8-EBCF627BC3F1
//

#define IO_REPARSE_TAG_IMANAGE_HSM              (0x20000056L)

//
//  Tag allocated to EaseFilter Technologies Limited for HSM
//  GUID: 8015B12A-A78F-4645-B999-7ED7978B51F1
//

#define IO_REPARSE_TAG_EASEFILTER_HSM           (0x00000057L)

//
//  Tag allocated to Peer Software Inc. Limited for HSM
//  GUID: A9DFF133-08B0-4005-9E6B-D9C8876942F4
//

#define IO_REPARSE_TAG_PEER_GFS                 (0x00000058L)

//
//  Tag allocated to Amazon Web Services Inc. This is for use
//  in the AWS AppStream Product.
//  GUID: 0485DEC0-0A06-4352-892A-FDEFA63BFDD8
//

#define IO_REPARSE_TAG_AMZN_APPSTREAM           (0x00000059L)


//
//  Tag allocated to Acronis for HSM
//
//  GUID: A8EA81D5-7F25-42AC-B20D-F0EEB15B1F82}
//
#define IO_REPARSE_TAG_ACRONIS_HSM_0            (0x00000060L)

//
//  Tag allocated to Acronis for HSM
//
//  GUID: 8FCBC190-0ACE-477A-B08C-577548E9BB22
//
#define IO_REPARSE_TAG_ACRONIS_HSM_1            (0x00000061L)

//
//  Tag allocated to Acronis for HSM
//
//  GUID: 1ABBD355-7E88-4D0B-AFBD-6BF3FF3F65E5
//
#define IO_REPARSE_TAG_ACRONIS_HSM_2            (0x00000062L)

//
//  Tag allocated to Acronis for HSM
//
//  GUID: EE4F962D-25D6-4F69-BEE1-DC952D853305
//
#define IO_REPARSE_TAG_ACRONIS_HSM_3            (0x00000063L)

//
//  Tag allocated to Acronis for HSM
//
//  GUID: 4AAB412F-BF9E-460C-9292-31DE5A3F9D08
//
#define IO_REPARSE_TAG_ACRONIS_HSM_4            (0x00000064L)

//
//  Tag allocated to Acronis for HSM
//
//  GUID: 67D29104-E971-477D-8BA5-E077E2D17472
//
#define IO_REPARSE_TAG_ACRONIS_HSM_5            (0x00000065L)

//
//  Tag allocated to Acronis for HSM
//
//  GUID: 6EFAE00E-9C04-4767-B30A-C9DDE120E7DE
//
#define IO_REPARSE_TAG_ACRONIS_HSM_6            (0x00000066L)

//
//  Tag allocated to Acronis for HSM
//
//  GUID: 7D66C79F-7E5C-4265-BEC8-E7F916A31C97
//
#define IO_REPARSE_TAG_ACRONIS_HSM_7            (0x00000067L)

//
//  Tag allocated to Acronis for HSM
//
//  GUID: 43055DA8-8B12-436C-B8CC-01505809247A
//
#define IO_REPARSE_TAG_ACRONIS_HSM_8            (0x00000068L)

//
//  Tag allocated to Acronis for HSM
//
//  GUID: 8DCA3C54-234F-41CA-9BB7-0886C1F1AC94
//
#define IO_REPARSE_TAG_ACRONIS_HSM_9            (0x00000069L)

//
//  Tag allocated to Acronis for HSM
//
//  GUID: 76B16F29-AA91-4442-B36F-301C57037B5D
//
#define IO_REPARSE_TAG_ACRONIS_HSM_A            (0x0000006AL)

//
//  Tag allocated to Acronis for HSM
//
//  GUID: CE206456-FC1A-4841-8527-7BDADBDD8A0B
//
#define IO_REPARSE_TAG_ACRONIS_HSM_B            (0x0000006BL)

//
//  Tag allocated to Acronis for HSM
//
//  GUID: 9CEC8C4B-DAD0-4C2A-BC7B-D1C818A3E9E8
//
#define IO_REPARSE_TAG_ACRONIS_HSM_C            (0x0000006CL)

//
//  Tag allocated to Acronis for HSM
//
//  GUID: C25CB784-3C8B-48DF-B14B-D2A87C441183
//
#define IO_REPARSE_TAG_ACRONIS_HSM_D            (0x0000006DL)

//
//  Tag allocated to Acronis for HSM
//
//  GUID: C74393B8-3025-467E-BF75-1E9771CC2BDA
//
#define IO_REPARSE_TAG_ACRONIS_HSM_E            (0x0000006EL)

//
//  Tag allocated to Acronis for HSM
//
//  GUID: 8B72233F-3B15-4496-B1AA-B3E7C5FD3485
//
#define IO_REPARSE_TAG_ACRONIS_HSM_F            (0x0000006FL)

#pragma endregion ntifs.h

#pragma region Link Tracking Information

//
// The following three FSCTLs are placed in this file to facilitate sharing
// between the redirector and the IO subsystem
//
// This FSCTL is used to garner the link tracking information for a file.
// The data structures used for retrieving the information are
// LINK_TRACKING_INFORMATION defined further down in this file.
//

#define FSCTL_LMR_GET_LINK_TRACKING_INFORMATION   CTL_CODE(FILE_DEVICE_NETWORK_FILE_SYSTEM,58,METHOD_BUFFERED,FILE_ANY_ACCESS)

//
// This FSCTL is used to update the link tracking information on a server for
// an intra machine/ inter volume move on that server
//

#define FSCTL_LMR_SET_LINK_TRACKING_INFORMATION   CTL_CODE(FILE_DEVICE_NETWORK_FILE_SYSTEM,59,METHOD_BUFFERED,FILE_ANY_ACCESS)

//
// The following IOCTL is used in link tracking implementation. It determines if the
// two file objects passed in are on the same server. This IOCTL is available in
// kernel mode only since it accepts FILE_OBJECT as parameters
//

#define IOCTL_LMR_ARE_FILE_OBJECTS_ON_SAME_SERVER CTL_CODE(FILE_DEVICE_NETWORK_FILE_SYSTEM,60,METHOD_BUFFERED,FILE_ANY_ACCESS)

//
// Control structure for FSCTL_LMR_GET_LINK_TRACKING_INFORMATION
//

//
// For links on DFS volumes the volume id and machine id are returned for
// link tracking
//
typedef enum _LINK_TRACKING_INFORMATION_TYPE
{
    NtfsLinkTrackingInformation,
    DfsLinkTrackingInformation
} LINK_TRACKING_INFORMATION_TYPE, *PLINK_TRACKING_INFORMATION_TYPE;

typedef struct _LINK_TRACKING_INFORMATION
{
    LINK_TRACKING_INFORMATION_TYPE Type;
    UCHAR   VolumeId[16];
} LINK_TRACKING_INFORMATION, *PLINK_TRACKING_INFORMATION;

//
// Control structure for FSCTL_LMR_SET_LINK_TRACKING_INFORMATION
//
typedef struct _REMOTE_LINK_TRACKING_INFORMATION_
{
    PVOID   TargetFileObject;
    ULONG   TargetLinkTrackingInformationLength;
    UCHAR   TargetLinkTrackingInformationBuffer[1];
} REMOTE_LINK_TRACKING_INFORMATION, *PREMOTE_LINK_TRACKING_INFORMATION;

#pragma endregion ntifs.h

#pragma region FSCTLs

/* FSCTL_QUERY_EXTENT_READ_CACHE_INFO ntifs.h */

#if (_WIN32_WINNT >= _WIN32_WINNT_THRESHOLD)

typedef struct _VCN_RANGE_INPUT_BUFFER
{
    LARGE_INTEGER StartingVcn;
    LARGE_INTEGER ClusterCount;
} VCN_RANGE_INPUT_BUFFER, *PVCN_RANGE_INPUT_BUFFER;

typedef struct _EXTENT_READ_CACHE_INFO_BUFFER
{
    LARGE_INTEGER AllocatedCache;
    LARGE_INTEGER PopulatedCache;
    LARGE_INTEGER InErrorCache;
} EXTENT_READ_CACHE_INFO_BUFFER, *PEXTENT_READ_CACHE_INFO_BUFFER;

#endif /* _WIN32_WINNT >= _WIN32_WINNT_THRESHOLD */

/* FSCTL_QUERY_REFS_VOLUME_COUNTER_INFO ntifs.h */

#if (_WIN32_WINNT >= _WIN32_WINNT_THRESHOLD)

typedef struct _REFS_VOLUME_COUNTER_INFO_INPUT_BUFFER
{
    BOOLEAN ResetCounters;
} REFS_VOLUME_COUNTER_INFO_INPUT_BUFFER, *PREFS_VOLUME_COUNTER_INFO_INPUT_BUFFER;

typedef struct _VOLUME_REFS_INFO_BUFFER
{
    //
    //  These values will not be affected at reset
    //
    LARGE_INTEGER CacheSizeInBytes;
    LARGE_INTEGER AllocatedCacheInBytes;
    LARGE_INTEGER PopulatedCacheInBytes;
    LARGE_INTEGER InErrorCacheInBytes;
    LARGE_INTEGER MemoryUsedForCacheMetadata;
    ULONG CacheLineSize;
    LONG CacheTransactionsOutstanding;
    LONG CacheLinesFree;
    LONG CacheLinesInError;

    //
    //  These values will be affected at reset
    //
    LARGE_INTEGER CacheHitsInBytes;
    LARGE_INTEGER CacheMissesInBytes;
    LARGE_INTEGER CachePopulationUpdatesInBytes;
    LARGE_INTEGER CacheWriteThroughUpdatesInBytes;
    LARGE_INTEGER CacheInvalidationsInBytes;
    LARGE_INTEGER CacheOverReadsInBytes;
    LARGE_INTEGER MetadataWrittenBytes;

    LONG CacheHitCounter;
    LONG CacheMissCounter;
    LONG CacheLineAllocationCounter;
    LONG CacheInvalidationsCounter;
    LONG CachePopulationUpdatesCounter;
    LONG CacheWriteThroughUpdatesCounter;
    LONG MaxCacheTransactionsOutstanding;

//  LONG TimeRequiredForCacheHitIn100us;
//  LONG TimeRequiredForCacheMissIn100us;

    LONGLONG DataWritesReallocationCount;
    LONGLONG DataInPlaceWriteCount;
    LONGLONG MetadataAllocationsFastTierCount;
    LONGLONG MetadataAllocationsSlowTierCount;
    LONGLONG DataAllocationsFastTierCount;
    LONGLONG DataAllocationsSlowTierCount;

    LONGLONG DestagesSlowTierToFastTier;
    LONGLONG DestagesFastTierToSlowTier;
    LONG SlowTierDataFillRatio;
    LONG FastTierDataFillRatio;
    LONG SlowTierMetadataFillRatio;
    LONG FastTierMetadataFillRatio;

    LONGLONG SlowToFastDestageReadLatency;
    LONG SlowToFastDestageReadLatencyBase;

    LONGLONG SlowToFastDestageWriteLatency;
    LONG SlowToFastDestageWriteLatencyBase;

    LONGLONG FastToSlowDestageReadLatency;
    LONG FastToSlowDestageReadLatencyBase;

    LONGLONG FastToSlowDestageWriteLatency;
    LONG FastToSlowDestageWriteLatencyBase;

    LONGLONG SlowTierContainerFillRatio;
    LONG SlowTierContainerFillRatioBase;

    LONGLONG FastTierContainerFillRatio;
    LONG FastTierContainerFillRatioBase;

    LONG Unused1;
    LONG Unused2;
    LONG Unused3;
    LONG Unused4;

    LONGLONG TreeUpdateCount;
    LONGLONG CheckpointCount;
    LONGLONG LogWriteCount;
    LONG LogFillRatio;

    LONG ReadCacheInvalidationsForOverwrite;
    LONG ReadCacheInvalidationsForReuse;
    LONG ReadCacheInvalidationsGeneral;

    LONG ReadCacheChecksOnMount;
    LONG ReadCacheIssuesOnMount;

    LONGLONG TrimLatency;
    LONG TrimLatencyBase;

    LONGLONG DataCompactionCount;

    LONGLONG CompactionReadLatency;
    LONG CompactionReadLatencyBase;

    LONGLONG CompactionWriteLatency;
    LONG CompactionWriteLatencyBase;

    LARGE_INTEGER DataInPlaceWriteClusterCount;

    LONG CompactionFailedDueToIneligibleContainer;
    LONG CompactionFailedDueToMaxFragmentation;

    LONGLONG CompactedContainerFillRatio;
    LONG CompactedContainerFillRatioBase;

    LONG ContainerMoveRetryCount;
    LONG ContainerMoveFailedDueToIneligibleContainer;

    LONG CompactionFailureCount;
    LONG ContainerMoveFailureCount;

    LARGE_INTEGER NumberOfDirtyMetadataPages;
    LONG NumberOfDirtyTableListEntries;
    LONG NumberOfDeleteQueueEntries;

    LONG MAAFilteredViewSize;
    LONG MAAFilteredViewInsertions;
    LONG MAAFilteredViewDeletions;
    LONG MAAFilteredViewCollisions;
    LONG MAAFilteredViewPurges;
    LONGLONG MAARegionsVisitedPerAllocationSum;
    LONG MAARegionsVisitedPerAllocationBase;
    LONG MAAMaxRegionsVisitedPerAllocation;

    LONGLONG TreeUpdateLatencyExclusive;
    LONGLONG TreeUpdateLatencyTotal;
    LONG TreeUpdateLatencyBase;

    LONGLONG CheckpointLatencyTreeUpdateExclusive;
    LONGLONG CheckpointLatencyTreeUpdateTotal;
    LONG CheckpointLatencyTreeUpdateBase;
    LONGLONG CheckpointLatencyTotal;
    LONG CheckpointLatencyTotalBase;

} VOLUME_REFS_INFO_BUFFER, *PVOLUME_REFS_INFO_BUFFER;

#endif /* _WIN32_WINNT >= _WIN32_WINNT_THRESHOLD */

/* FSCTL_QUERY_DIRECT_ACCESS_EXTENTS ntifs.h */

#if (NTDDI_VERSION >= NTDDI_WIN10_TH2)

typedef struct _QUERY_DIRECT_ACCESS_EXTENTS
{
    LONGLONG FileOffset;
    LONGLONG Length;
    ULONG Flags;
    ULONG Reserved;
} QUERY_DIRECT_ACCESS_EXTENTS, *PQUERY_DIRECT_ACCESS_EXTENTS;

#define QUERY_DIRECT_ACCESS_IMAGE_EXTENTS   0x00000001
#define QUERY_DIRECT_ACCESS_DATA_EXTENTS    0x00000002

#endif /* NTDDI_VERSION >= NTDDI_WIN10_TH2 */

/* FSCTL_GHOST_FILE_EXTENTS ntifs.h */

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN10_TH2)
typedef struct _FSCTL_GHOST_FILE_EXTENTS_INPUT_BUFFER
{
    LARGE_INTEGER FileOffset;
    LARGE_INTEGER ByteCount;

    GUID RecallOwnerGuid;

    ULONG RecallMetadataBufferSize;
    UCHAR RecallMetadataBuffer[ANYSIZE_ARRAY];
} FSCTL_GHOST_FILE_EXTENTS_INPUT_BUFFER, *PFSCTL_GHOST_FILE_EXTENTS_INPUT_BUFFER;
#endif // #if (_WIN32_WINNT >= _WIN32_WINNT_WIN10_TH2)

/* FSCTL_QUERY_GHOSTED_FILE_EXTENTS ntifs.h */

typedef struct _FSCTL_QUERY_GHOSTED_FILE_EXTENTS_INPUT_RANGE
{
    LARGE_INTEGER FileOffset;
    LARGE_INTEGER ByteCount;
} FSCTL_QUERY_GHOSTED_FILE_EXTENTS_INPUT_RANGE, *PFSCTL_QUERY_GHOSTED_FILE_EXTENTS_INPUT_RANGE;

typedef struct _GHOSTED_FILE_EXTENT
{
    LARGE_INTEGER FileOffset;
    LARGE_INTEGER ByteCount;

    GUID RecallOwnerGuid;
    ULONG NextEntryOffset;
    ULONG RecallMetadataBufferSize;
    UCHAR RecallMetadataBuffer[ANYSIZE_ARRAY];
} GHOSTED_FILE_EXTENT, *PGHOSTED_FILE_EXTENT;

typedef struct _FSCTL_QUERY_GHOSTED_FILE_EXTENTS_OUTPUT
{
    ULONG ExtentCount;
    ULONG TotalExtentCount;
    UCHAR Extents[ANYSIZE_ARRAY];
} FSCTL_QUERY_GHOSTED_FILE_EXTENTS_OUTPUT, *PFSCTL_QUERY_GHOSTED_FILE_EXTENTS_OUTPUT;

#define GHOSTED_FILE_EXTENTS_OUTPUT_SIZE(Count, MaxRecallMetadataBufferSize)   \
  FIELD_OFFSET(FSCTL_QUERY_GHOSTED_FILE_EXTENTS_OUTPUT, Extents) +             \
      ALIGN_UP((FIELD_OFFSET(GHOSTED_FILE_EXTENT, RecallMetadataBuffer) +      \
                MaxRecallMetadataBufferSize),                                  \
               8) *                                                            \
          (Count)

/* FSCTL_UNMAP_SPACE ntifs.h */

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN10_TH2)

typedef struct _FSCTL_UNMAP_SPACE_INPUT_BUFFER
{
    LARGE_INTEGER BytesToUnmap;
} FSCTL_UNMAP_SPACE_INPUT_BUFFER, *PFSCTL_UNMAP_SPACE_INPUT_BUFFER;

typedef struct _FSCTL_UNMAP_SPACE_OUTPUT
{
    LARGE_INTEGER BytesUnmapped;
} FSCTL_UNMAP_SPACE_OUTPUT, *PFSCTL_UNMAP_SPACE_OUTPUT;

#endif // #if (_WIN32_WINNT >= _WIN32_WINNT_WIN10_TH2)

/* FSCTL_QUERY_VOLUME_NUMA_INFO ntifs.h */

#if (NTDDI_VERSION >= NTDDI_WIN10_RS2)

typedef struct _FSCTL_QUERY_VOLUME_NUMA_INFO_OUTPUT
{
    ULONG NumaNode;
} FSCTL_QUERY_VOLUME_NUMA_INFO_OUTPUT, *PFSCTL_QUERY_VOLUME_NUMA_INFO_OUTPUT;

#endif // #if (NTDDI_VERSION >= NTDDI_WIN10_RS2)

/* FSCTL_REFS_DEALLOCATE_RANGES ntifs.h */

#if (NTDDI_VERSION >= NTDDI_WIN10_RS2)

typedef struct _REFS_DEALLOCATE_RANGES_RANGE
{
    ULONGLONG StartOfRange;
    ULONGLONG CountOfRange;
} REFS_DEALLOCATE_RANGES_RANGE, *PREFS_DEALLOCATE_RANGES_RANGE;

typedef struct _REFS_DEALLOCATE_RANGES_INPUT_BUFFER
{
    ULONG RangeCount;
    REFS_DEALLOCATE_RANGES_RANGE Ranges[ANYSIZE_ARRAY];
} REFS_DEALLOCATE_RANGES_INPUT_BUFFER, *PREFS_DEALLOCATE_RANGES_INPUT_BUFFER;

#endif // #if (NTDDI_VERSION >= NTDDI_WIN10_RS2)

/* FSCTL_REFS_DEALLOCATE_RANGES_EX ntifs.h */

#if (NTDDI_VERSION >= NTDDI_WIN10_FE)

typedef enum _REFS_DEALLOCATE_RANGES_ALLOCATOR
{
    REFS_DEALLOCATE_RANGES_ALLOCATOR_NONE = 0,
    REFS_DEALLOCATE_RANGES_ALLOCATOR_SAA = 1,
    REFS_DEALLOCATE_RANGES_ALLOCATOR_CAA = 2,
    REFS_DEALLOCATE_RANGES_ALLOCATOR_MAA = 3,
} REFS_DEALLOCATE_RANGES_ALLOCATOR, *PREFS_DEALLOCATE_RANGES_ALLOCATOR;

typedef struct _REFS_DEALLOCATE_RANGES_INPUT_BUFFER_EX
{
    ULONG RangeCount;
    REFS_DEALLOCATE_RANGES_ALLOCATOR Allocator;
    LONGLONG StreamReserveUpdateCount;
    ULONG OffsetToRanges;
    ULONG OffsetToLeakCounts;
    ULONGLONG Reserved[2];
} REFS_DEALLOCATE_RANGES_INPUT_BUFFER_EX, *PREFS_DEALLOCATE_RANGES_INPUT_BUFFER_EX;

#endif // #if (NTDDI_VERSION >= NTDDI_WIN10_FE)

/* FSCTL_REFS_REMOVE_HARDLINK_BACKPOINTER ntifs.h */

#if (NTDDI_VERSION >= NTDDI_WIN10_MN)

typedef struct _REFS_REMOVE_HARDLINK_BACKPOINTER
{
    ULONGLONG ParentDirectory;
    ULONGLONG Reserved;
    WCHAR FileName[1];
} REFS_REMOVE_HARDLINK_BACKPOINTER, *PREFS_REMOVE_HARDLINK_BACKPOINTER;

#endif // #if (NTDDI_VERSION >= NTDDI_WIN10_MN)

/* FSCTL_REFS_STREAM_SNAPSHOT_MANAGEMENT ntifs.h */

#if (NTDDI_VERSION >= NTDDI_WIN10_FE)

enum _REFS_STREAM_EXTENT_PROPERTIES
{
    REFS_STREAM_EXTENT_PROPERTY_VALID = 0x0010,
    REFS_STREAM_EXTENT_PROPERTY_STREAM_RESERVED = 0x0020,
    REFS_STREAM_EXTENT_PROPERTY_CRC32 = 0x0080,
    REFS_STREAM_EXTENT_PROPERTY_CRC64 = 0x0100,
    REFS_STREAM_EXTENT_PROPERTY_GHOSTED = 0x0200,
    REFS_STREAM_EXTENT_PROPERTY_READONLY = 0x0400,
    REFS_STREAM_EXTENT_PROPERTY_SPARSE = 0x0008,
};
typedef USHORT REFS_STREAM_EXTENT_PROPERTIES;
typedef REFS_STREAM_EXTENT_PROPERTIES* PREFS_STREAM_EXTENT_PROPERTIES;

typedef struct _REFS_STREAM_EXTENT
{
    LONGLONG Vcn;
    LONGLONG Lcn;
    LONGLONG Length;
    REFS_STREAM_EXTENT_PROPERTIES Properties; // enum _REFS_STREAM_EXTENT_PROPERTIES
} REFS_STREAM_EXTENT, *PREFS_STREAM_EXTENT;

typedef enum _REFS_STREAM_SNAPSHOT_OPERATION
{
    REFS_STREAM_SNAPSHOT_OPERATION_INVALID = 0,
    REFS_STREAM_SNAPSHOT_OPERATION_CREATE = 1,
    REFS_STREAM_SNAPSHOT_OPERATION_LIST = 2,
    REFS_STREAM_SNAPSHOT_OPERATION_QUERY_DELTAS = 3,
    REFS_STREAM_SNAPSHOT_OPERATION_REVERT = 4,
    REFS_STREAM_SNAPSHOT_OPERATION_SET_SHADOW_BTREE = 5,
    REFS_STREAM_SNAPSHOT_OPERATION_CLEAR_SHADOW_BTREE = 6,
    REFS_STREAM_SNAPSHOT_OPERATION_MAX = REFS_STREAM_SNAPSHOT_OPERATION_CLEAR_SHADOW_BTREE,
} REFS_STREAM_SNAPSHOT_OPERATION, *PREFS_STREAM_SNAPSHOT_OPERATION;

typedef struct _REFS_STREAM_SNAPSHOT_MANAGEMENT_INPUT_BUFFER
{
    REFS_STREAM_SNAPSHOT_OPERATION Operation;
    USHORT SnapshotNameLength;
    USHORT OperationInputBufferLength;
    ULONGLONG Reserved[2];
    USHORT NameAndInputBuffer[ANYSIZE_ARRAY];
} REFS_STREAM_SNAPSHOT_MANAGEMENT_INPUT_BUFFER, *PREFS_STREAM_SNAPSHOT_MANAGEMENT_INPUT_BUFFER;

typedef struct _REFS_STREAM_SNAPSHOT_LIST_OUTPUT_BUFFER_ENTRY
{
    ULONG NextEntryOffset;
    USHORT SnapshotNameLength;
    ULONGLONG SnapshotCreationTime;
    ULONGLONG StreamSize;
    ULONGLONG StreamAllocationSize;
    ULONGLONG Reserved[2];
    WCHAR SnapshotName[ANYSIZE_ARRAY];
} REFS_STREAM_SNAPSHOT_LIST_OUTPUT_BUFFER_ENTRY, *PREFS_STREAM_SNAPSHOT_LIST_OUTPUT_BUFFER_ENTRY;

typedef struct _REFS_STREAM_SNAPSHOT_LIST_OUTPUT_BUFFER
{
    ULONG EntryCount;
    ULONG BufferSizeRequiredForQuery;
    ULONG Reserved[2];
    REFS_STREAM_SNAPSHOT_LIST_OUTPUT_BUFFER_ENTRY Entries[ANYSIZE_ARRAY];
} REFS_STREAM_SNAPSHOT_LIST_OUTPUT_BUFFER, *PREFS_STREAM_SNAPSHOT_LIST_OUTPUT_BUFFER;

typedef struct _REFS_STREAM_SNAPSHOT_QUERY_DELTAS_INPUT_BUFFER
{
    LONGLONG StartingVcn;
    ULONG Flags;
    ULONG Reserved;
} REFS_STREAM_SNAPSHOT_QUERY_DELTAS_INPUT_BUFFER, *PREFS_STREAM_SNAPSHOT_QUERY_DELTAS_INPUT_BUFFER;

typedef struct _REFS_STREAM_SNAPSHOT_QUERY_DELTAS_OUTPUT_BUFFER
{
    ULONG ExtentCount;
    ULONG Reserved[2];
    REFS_STREAM_EXTENT Extents[ANYSIZE_ARRAY];
} REFS_STREAM_SNAPSHOT_QUERY_DELTAS_OUTPUT_BUFFER, *PREFS_STREAM_SNAPSHOT_QUERY_DELTAS_OUTPUT_BUFFER;

#endif // #if (NTDDI_VERSION >= NTDDI_WIN10_FE)

/* FSCTL_DUPLICATE_CLUSTER ntifs.h */
#if (NTDDI_VERSION >= NTDDI_WIN10_NI)
typedef struct _DUPLICATE_CLUSTER_DATA
{

    LONGLONG SourceLcn;
    LONGLONG TargetLcn;
    LARGE_INTEGER TargetFileOffset;
    ULONG DuplicationLimit;
    ULONG Reserved;

} DUPLICATE_CLUSTER_DATA, *PDUPLICATE_CLUSTER_DATA;
#endif (NTDDI_VERSION >= NTDDI_WIN10_NI)

/* FSCTL_QUERY_LCN_WEAK_REFERENCE_STATE ntifs.h */

#if (NTDDI_VERSION >= NTDDI_WIN10_NI)

enum _LCN_WEAK_REFERENCE_STATE
{

    LCN_WEAK_REFERENCE_VALID = 0x00000001,
    LCN_WEAK_REFERENCE_BROKEN = 0x00000002,
    LCN_CHECKSUM_VALID = 0x00000004,
    LCN_IS_VALID = 0x00000008,
    LCN_IS_STREAM_RESERVED = 0x00000010,
    LCN_IS_READ_ONLY = 0x00000020
};
typedef ULONG LCN_WEAK_REFERENCE_STATE;
typedef LCN_WEAK_REFERENCE_STATE *PLCN_WEAK_REFERENCE_STATE;

typedef struct _LCN_WEAK_REFERENCE_BUFFER
{
    //
    // The LCN here acts as a token to reference the weak reference on a given
    // file system cluster. It doesn't mean much else to an application.
    //
    LONGLONG Lcn;
    LONGLONG LengthInClusters;
    ULONG ReferenceCount;
    LCN_WEAK_REFERENCE_STATE State; // enum _LCN_WEAK_REFERENCE_STATE
} LCN_WEAK_REFERENCE_BUFFER, *PLCN_WEAK_REFERENCE_BUFFER;

#endif (NTDDI_VERSION >= NTDDI_WIN10_NI)

/* FSCTL_CREATE_LCN_WEAK_REFERENCE ntifs.h */

#if (NTDDI_VERSION >= NTDDI_WIN10_NI)

typedef ULONG LCN_WEAK_REFERENCE_CREATE_FLAGS;
typedef LCN_WEAK_REFERENCE_CREATE_FLAGS *PLCN_WEAK_REFERENCE_CREATE_FLAGS;

typedef struct _LCN_WEAK_REFERENCE_CREATE_INPUT_BUFFER
{
    LONGLONG Offset;
    LONGLONG Length;
    ULONG Flags;
    ULONG Reserved;
} LCN_WEAK_REFERENCE_CREATE_INPUT_BUFFER, *PLCN_WEAK_REFERENCE_CREATE_INPUT_BUFFER;

typedef struct _LCN_WEAK_REFERENCE_VCN_MAPPING
{
    LONGLONG Vcn;
    LONGLONG Lcn;
    LONGLONG CountOfRange;
} LCN_WEAK_REFERENCE_VCN_MAPPING, *PLCN_WEAK_REFERENCE_VCN_MAPPING;

typedef struct _LCN_WEAK_REFERENCE_CREATE_OUTPUT_BUFFER
{
    ULONG MappingCount;
    LCN_WEAK_REFERENCE_VCN_MAPPING VcnLcnMappings[ANYSIZE_ARRAY];
} LCN_WEAK_REFERENCE_CREATE_OUTPUT_BUFFER, *PLCN_WEAK_REFERENCE_CREATE_OUTPUT_BUFFER;

#endif (NTDDI_VERSION >= NTDDI_WIN10_NI)

/* FSCTL_CLEAR_LCN_WEAK_REFERENCE ntifs.h */

#if (NTDDI_VERSION >= NTDDI_WIN10_NI)

typedef struct _LCN_WEAK_REFERENCE_RANGE
{
    LONGLONG StartOfRange;
    LONGLONG CountOfRange;
} LCN_WEAK_REFERENCE_RANGE, *PLCN_WEAK_REFERENCE_RANGE;

typedef struct _LCN_WEAK_REFERENCE_CLEAR_INPUT_BUFFER
{
    ULONG RangeCount;
    LCN_WEAK_REFERENCE_RANGE Ranges[ANYSIZE_ARRAY];
} LCN_WEAK_REFERENCE_CLEAR_INPUT_BUFFER, *PLCN_WEAK_REFERENCE_CLEAR_INPUT_BUFFER;

#endif // (NTDDI_VERSION >= NTDDI_WIN10_NI)

/* FSCTL_REFS_SET_VOLUME_DEDUP_INFO ntifs.h */

#if (NTDDI_VERSION >= NTDDI_WIN10_NI)

#define REFS_VOLUME_DEDUP_INFO_INPUT_BUFFER_VERSION_V1 1
#define REFS_VOLUME_DEDUP_INFO_INPUT_BUFFER_VERSION    2

typedef struct _REFS_VOLUME_DEDUP_INFO_INPUT_BUFFER
{
    ULONG Version;

    BOOLEAN SetDedupState;
    BOOLEAN Enable;

    BOOLEAN SetWeakRefState;
    BOOLEAN EnableWeakRef;

    BOOLEAN SetDirtyRangeTrackingState;
    BOOLEAN EnableDirtyRangeTracking;

    BOOLEAN SetWeakRefInconsistentState;
    BOOLEAN SetWeakRefInconsistent;
} REFS_VOLUME_DEDUP_INFO_INPUT_BUFFER, *PREFS_VOLUME_DEDUP_INFO_INPUT_BUFFER;

#endif // (NTDDI_VERSION >= NTDDI_WIN10_NI)

/* FSCTL_REFS_QUERY_VOLUME_DEDUP_INFO */

#if (NTDDI_VERSION >= NTDDI_WIN10_NI)

#define REFS_VOLUME_DEDUP_INFO_OUTPUT_BUFFER_VERSION_V1 1
#define REFS_VOLUME_DEDUP_INFO_OUTPUT_BUFFER_VERSION    2

typedef struct _REFS_VOLUME_DEDUP_INFO_OUTPUT_BUFFER
{
    ULONG Version;
    BOOLEAN Enabled;
    BOOLEAN EnabledWeakRef;
    BOOLEAN EnabledDirtyRangeTracking;
    BOOLEAN WeakRefInconsistent;
    BOOLEAN IsClustered;
    ULONG VolumeIdHash;
    GUID VolumeGuid;
    GUID VolumeUniqueGuid;
} REFS_VOLUME_DEDUP_INFO_OUTPUT_BUFFER, *PREFS_VOLUME_DEDUP_INFO_OUTPUT_BUFFER;

#endif // (NTDDI_VERSION >= NTDDI_WIN10_NI)

/* FSCTL_REFS_QUERY_VOLUME_TOTAL_SHARED_LCNS ntifs.h */

#if (NTDDI_VERSION >= NTDDI_WIN10_NI)

#define REFS_QUERY_VOLUME_TOTAL_SHARED_LCNS_OUTPUT_BUFFER_VERSION 1

typedef struct _REFS_QUERY_VOLUME_TOTAL_SHARED_LCNS_OUTPUT_BUFFER
{
    ULONG Version;
    ULONGLONG TotalSharedLcns;
} REFS_QUERY_VOLUME_TOTAL_SHARED_LCNS_OUTPUT_BUFFER, *PREFS_QUERY_VOLUME_TOTAL_SHARED_LCNS_OUTPUT_BUFFER;

#endif // (NTDDI_VERSION >= NTDDI_WIN10_NI)

#if (NTDDI_VERSION >= NTDDI_WIN10_FE)
typedef struct _SET_CACHED_RUNS_STATE_INPUT_BUFFER
{
    BOOLEAN Enable;
} SET_CACHED_RUNS_STATE_INPUT_BUFFER, *PSET_CACHED_RUNS_STATE_INPUT_BUFFER;
#endif

/* FSCTL_REFS_SET_VOLUME_COMPRESSION_INFO ntifs.h */

#if (NTDDI_VERSION >= NTDDI_WIN10_NI)

typedef enum _REFS_COMPRESSION_FORMATS
{
    REFS_COMPRESSION_FORMAT_UNCHANGED = 0,
    REFS_COMPRESSION_FORMAT_UNKNOWN = 1,
    REFS_COMPRESSION_FORMAT_UNCOMPRESSED = 2,
    REFS_COMPRESSION_FORMAT_LZ4 = 3,
    REFS_COMPRESSION_FORMAT_ZSTD = 4,
} REFS_COMPRESSION_FORMATS, *PREFS_COMPRESSION_FORMATS;

typedef enum _REFS_SET_VOLUME_COMPRESSION_INFO_FLAGS
{
    REFS_SET_VOLUME_COMPRESSION_INFO_FLAG_START_COMPRESSION = 0x00000001,
    REFS_SET_VOLUME_COMPRESSION_INFO_FLAG_STOP_COMPRESSION = 0x00000002,
    REFS_SET_VOLUME_COMPRESSION_INFO_FLAG_GC_ONLY = 0x00000004,
#if (NTDDI_VERSION >= NTDDI_WIN10_GE)
    REFS_SET_VOLUME_COMPRESSION_INFO_FLAG_ENABLE_COMPRESSION = 0x00000008,
    REFS_SET_VOLUME_COMPRESSION_INFO_FLAG_DISABLE_COMPRESSION = 0x00000010,
#endif
    REFS_SET_VOLUME_COMPRESSION_INFO_FLAG_MAX = 0x00000010,
} REFS_SET_VOLUME_COMPRESSION_INFO_FLAGS, *PREFS_SET_VOLUME_COMPRESSION_INFO_FLAGS;

#define REFS_SET_VOLUME_COMPRESSION_INFO_INPUT_BUFFER_VERSION 1

typedef struct _REFS_SET_VOLUME_COMPRESSION_INFO_INPUT_BUFFER
{
    ULONG Version;

    //
    //  The compression format for the volume. If this value is
    //  REFS_COMPRESSION_FORMAT_UNCHANGED then no compression
    //  parameters on the volume are changed.
    //
    REFS_COMPRESSION_FORMATS CompressionFormat;

    //
    //  The compression level to use for the selected compression
    //  format. This is compression format dependant.
    //
    SHORT CompressionLevel;

    //
    //  The compression chunk size in bytes. Multiple of the volumes
    //  cluster size and power of two.
    //
    ULONG CompressionChunkSizeBytes;

    //
    //  REFS_SET_VOLUME_COMPRESSION_INFO_FLAGS
    //
    ULONG Flags;

    //
    //  Tuning parameters for compression, recompression, and decompression.
    //
    ULONG CompressionTuning;
    ULONG RecompressionTuning;
    ULONG DecompressionTuning;

    //
    //  Reserved for future implementations. Zeroed.
    //
    ULONG Reserved[6];
} REFS_SET_VOLUME_COMPRESSION_INFO_INPUT_BUFFER, *PREFS_SET_VOLUME_COMPRESSION_INFO_INPUT_BUFFER;

#endif // (NTDDI_VERSION >= NTDDI_WIN10_NI)

/* FSCTL_REFS_QUERY_VOLUME_COMPRESSION_INFO ntifs.h */

#if (NTDDI_VERSION >= NTDDI_WIN10_NI)

typedef enum _REFS_QUERY_VOLUME_COMPRESSION_INFO_FLAGS
{
    REFS_QUERY_VOLUME_COMPRESSION_INFO_FLAGS_RUNNING = 0x00000001,
    REFS_QUERY_VOLUME_COMPRESSION_INFO_FLAGS_STOPPED = 0x00000002
} REFS_QUERY_VOLUME_COMPRESSION_INFO_FLAGS, *PREFS_QUERY_VOLUME_COMPRESSION_INFO_FLAGS;

#define REFS_QUERY_VOLUME_COMPRESSION_INFO_OUTPUT_BUFFER_VERSION 1

typedef struct _REFS_QUERY_VOLUME_COMPRESSION_INFO_OUTPUT_BUFFER
{
    ULONG Version;

    //
    //  The default compression format for the volume.
    //
    REFS_COMPRESSION_FORMATS DefaultCompressionFormat;

    //
    //  The default compression level for the volume.
    //
    SHORT DefaultCompressionLevel;

    //
    //  The default compression chunk size in bytes for the volume.
    //
    ULONG DefaultCompressionChunkSizeBytes;

    //
    //  The volume cluster size in bytes.
    //
    ULONG VolumeClusterSizeBytes;

    //
    //  The total number of clusters on the volume.
    //
    ULONGLONG TotalVolumeClusters;

    //
    //  The total number of committed allocated clusters on the volume.
    //
    ULONGLONG TotalAllocatedClusters;

    //
    //  The total number of allocated clusters on the volume that
    //  are compressible.
    //
    ULONGLONG TotalCompressibleClustersAllocated;

    //
    //  The total number of compressible clusters that are still
    //  referenced in the namespace. This is always less or equal than
    //  TotalCompressibleClusters, and the delta of the two yields
    //  the total number of clusters that can be reclaimed by
    //  recompressing the volume.
    //
    ULONGLONG TotalCompressibleClustersInUse;

    //
    //  The total number of compressed clusters allocated on the volume.
    //
    ULONGLONG TotalCompressedClusters;

    //
    //  REFS_QUERY_VOLUME_COMPRESSION_INFO_FLAGS
    //
    ULONG Flags;

    //
    //  The tuning parameters as stored on the volume.
    //
    ULONG CompressionTuning;
    ULONG RecompressionTuning;
    ULONG DecompressionTuning;

    //
    //  The last status code reported by volume compression.
    //
    NTSTATUS LastCompressionStatus;

    //
    //  Reserved for future implementations. Filled with zeroes.
    //
    ULONG Reserved[8];
} REFS_QUERY_VOLUME_COMPRESSION_INFO_OUTPUT_BUFFER, *PREFS_QUERY_VOLUME_COMPRESSION_INFO_OUTPUT_BUFFER;

#endif // #if (NTDDI_VERSION >= NTDDI_WIN10_NI)

/* FSCTL_REFS_SET_VOLUME_IO_METRICS_INFO ntifs.h */

#if (NTDDI_VERSION >= NTDDI_WIN11_ZN)

#define REFS_SET_VOLUME_IO_METRICS_INFO_INPUT_BUFFER_VERSION 1

typedef struct _REFS_SET_VOLUME_IO_METRICS_INFO_INPUT_BUFFER
{
    ULONG Version;

    ULONG GlobalSecondsToTrack;
    ULONG MetricsPeriodicitySeconds;
    ULONG MetricsGenerationsPerContainer;

    ULONG Reserved[8];
} REFS_SET_VOLUME_IO_METRICS_INFO_INPUT_BUFFER, *PREFS_SET_VOLUME_IO_METRICS_INFO_INPUT_BUFFER;

#endif

/* FSCTL_REFS_QUERY_VOLUME_IO_METRICS_INFO ntifs.h */

#if (NTDDI_VERSION >= NTDDI_WIN11_ZN)

#define REFS_QUERY_VOLUME_IO_METRICS_INFO_INPUT_BUFFER_VERSION 1
#define REFS_QUERY_VOLUME_IO_METRICS_INFO_OUTPUT_BUFFER_VERSION 1

typedef enum _REFS_QUERY_VOLUME_IO_METRICS_INFO_QUERY_TYPE
{
    REFS_QUERY_VOLUME_IO_METRICS_INFO_QUERY_TYPE_PARAMETERS = 1,
    REFS_QUERY_VOLUME_IO_METRICS_INFO_QUERY_TYPE_METRICS_DATA = 2,
} REFS_QUERY_VOLUME_IO_METRICS_INFO_QUERY_TYPE, *PREFS_QUERY_VOLUME_IO_METRICS_INFO_QUERY_TYPE;

typedef struct _REFS_QUERY_VOLUME_IO_METRICS_INFO_INPUT_BUFFER
{
    ULONG Version;
    REFS_QUERY_VOLUME_IO_METRICS_INFO_QUERY_TYPE QueryType;
    ULONG Reserved[6];
    union
    {
        ULONGLONG UnusedAlign;
        struct
        {
            ULONG Reserved[6];
        } Parameters;
        struct
        {
            ULONGLONG ResumeKeyBlob[2];
            ULONG Reserved[6];
        } MetricsData;
    };
} REFS_QUERY_VOLUME_IO_METRICS_INFO_INPUT_BUFFER, *PREFS_QUERY_VOLUME_IO_METRICS_INFO_INPUT_BUFFER;

typedef struct _REFS_QUERY_VOLUME_IO_METRICS_METRICS_DATA
{
    ULONGLONG PlaceHolder;
} REFS_QUERY_VOLUME_IO_METRICS_METRICS_DATA, *PREFS_QUERY_VOLUME_IO_METRICS_METRICS_DATA;

typedef struct _REFS_QUERY_VOLUME_IO_METRICS_INFO_OUTPUT_BUFFER
{
    ULONG Version;
    REFS_QUERY_VOLUME_IO_METRICS_INFO_QUERY_TYPE QueryType;
    ULONG Reserved[6];
    union
    {
        ULONGLONG UnusedAlign;
        struct
        {
            ULONG GlobalSecondsToTrack;
            ULONG MetricsPeriodicitySeconds;
            ULONG MetricsGenerationsPerContainer;
            ULONG Reserved[6];
        } Parameters;
        struct
        {
            ULONG EntryCount;
            ULONGLONG ResumeKeyBlob[2];
            ULONG Reserved[6];
            REFS_QUERY_VOLUME_IO_METRICS_METRICS_DATA Metrics[ANYSIZE_ARRAY];
        } MetricsData;
    };
} REFS_QUERY_VOLUME_IO_METRICS_INFO_OUTPUT_BUFFER, *PREFS_QUERY_VOLUME_IO_METRICS_INFO_OUTPUT_BUFFER;

#endif // #if (NTDDI_VERSION >= NTDDI_WIN11_ZN)

/* FSCTL_REFS_SET_ROLLBACK_PROTECTION_INFO ntifs.h */

#if (NTDDI_VERSION >= NTDDI_WIN11_GE)

#define REFS_SET_ROLLBACK_PROTECTION_INFO_INPUT_BUFFER_VERSION 1

typedef struct _REFS_SET_ROLLBACK_PROTECTION_INFO_INPUT_BUFFER
{
    ULONG Version;
    BOOLEAN FailMountOnMismatch;
    ULONG CustomPayloadLength;
    ULONG CustomPayloadOffset;
} REFS_SET_ROLLBACK_PROTECTION_INFO_INPUT_BUFFER, *PREFS_SET_ROLLBACK_PROTECTION_INFO_INPUT_BUFFER;

#endif // #if (NTDDI_VERSION >= NTDDI_WIN11_GE)

/* FSCTL_REFS_QUERY_ROLLBACK_PROTECTION_INFO ntifs.h */

#if (NTDDI_VERSION >= NTDDI_WIN11_GE)

#define REFS_QUERY_ROLLBACK_PROTECTION_INFO_OUTPUT_BUFFER_VERSION 1

typedef struct _REFS_QUERY_ROLLBACK_PROTECTION_INFO_OUTPUT_BUFFER
{
    ULONG Version;
    GUID VolumeGuid;
    GUID RollbackProtectionGuid;
    BOOLEAN FailMountOnMismatch;
    ULONGLONG FrozenVirtualClock;
    ULONGLONG CurrentVirtualClock;
    USHORT ChecksumType;
    ULONG ChecksumLength;
    ULONG ChecksumOffset;
    ULONG CustomPayloadLength;
    ULONG CustomPayloadOffset;
} REFS_QUERY_ROLLBACK_PROTECTION_INFO_OUTPUT_BUFFER, *PREFS_QUERY_ROLLBACK_PROTECTION_INFO_OUTPUT_BUFFER;

#endif // #if (NTDDI_VERSION >= NTDDI_WIN11_GE)

/* IOCTL_VOLSNAP_FLUSH_AND_HOLD_WRITES ntifs.h */

#define VOLSNAPCONTROLTYPE                              0x00000053 // 'S'
#define IOCTL_VOLSNAP_FLUSH_AND_HOLD_WRITES             CTL_CODE(VOLSNAPCONTROLTYPE, 0, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS) 

#pragma endregion

EXTERN_C_END
