#ifndef UNICODE
#define UNICODE
#endif

#include <stdio.h>
#include <Windows.h>
#include <Iphlpapi.h>
#include <Assert.h>

#include <winnetwk.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "mpr.lib")

void getMAC();
BOOL WINAPI EnumerateFunc(LPNETRESOURCE lpnr);
void DisplayStruct(int i, LPNETRESOURCE lpnrLocal);

int main() {
	getMAC();
	LPNETRESOURCE lpnr = NULL;

	if (EnumerateFunc(lpnr) == FALSE) {
		printf("Call to EnumerateFunc failed\n");
		system("pause");
		return 1;
	}
	else
	{
		system("pause");
		return 0;
	}
	system("pause");
}
void getMAC() {
	PIP_ADAPTER_INFO AdapterInfo;
	DWORD dwBufLen = sizeof(AdapterInfo);
	char *mac_addr = (char*)malloc(17);

	AdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW) 
	{
		AdapterInfo = (IP_ADAPTER_INFO *)malloc(dwBufLen);
		if (AdapterInfo == NULL) 
		{
			printf("Error allocating memory needed to call GetAdaptersinfo\n");
		}
	}

	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR)
	{
		PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
		do 
		{
			sprintf(mac_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
				pAdapterInfo->Address[0], pAdapterInfo->Address[1],
				pAdapterInfo->Address[2], pAdapterInfo->Address[3],
				pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
			printf("Address: %s, mac: %s\n\n", pAdapterInfo->IpAddressList.IpAddress.String, mac_addr);
			pAdapterInfo = pAdapterInfo->Next;
		} 
		while (pAdapterInfo);
	}
	free(AdapterInfo);
}

BOOL WINAPI EnumerateFunc(LPNETRESOURCE lpnr)
{
	DWORD dwResult, dwResultEnum;
	HANDLE hEnum;
	DWORD cbBuffer = 16384;     // 16K is a good size
	DWORD cEntries = -1;        // enumerate all possible entries
	LPNETRESOURCE lpnrLocal;    // pointer to enumerated structures
	DWORD i;
	//
	// Call the WNetOpenEnum function to begin the enumeration.
	//
	dwResult = WNetOpenEnum(RESOURCE_GLOBALNET, // all network resources
		RESOURCETYPE_ANY,   // all resources
		0,  // enumerate all resources
		lpnr,       // NULL first time the function is called
		&hEnum);    // handle to the resource

	if (dwResult != NO_ERROR) 
	{
		printf("WnetOpenEnum failed with error %d\n", dwResult);
		return FALSE;
	}

	lpnrLocal = (LPNETRESOURCE)GlobalAlloc(GPTR, cbBuffer);
	if (lpnrLocal == NULL) 
	{
		printf("WnetOpenEnum failed with error %d\n", dwResult);
		return FALSE;
	}

	do 
	{
		ZeroMemory(lpnrLocal, cbBuffer);
		dwResultEnum = WNetEnumResource(hEnum,  // resource handle
			&cEntries,      // defined locally as -1
			lpnrLocal,      // LPNETRESOURCE
			&cbBuffer);     // buffer size
							//
							// If the call succeeds, loop through the structures.
							//
		if (dwResultEnum == NO_ERROR) {
			for (i = 0; i < cEntries; i++) 
			{
				DisplayStruct(i, &lpnrLocal[i]);

				if (RESOURCEUSAGE_CONTAINER == (lpnrLocal[i].dwUsage
					& RESOURCEUSAGE_CONTAINER))
					if (!EnumerateFunc(&lpnrLocal[i]))
						printf("EnumerateFunc returned FALSE\n");
			}
		}
		// Process errors.
		//
		else if (dwResultEnum != ERROR_NO_MORE_ITEMS) {
			printf("WNetEnumResource failed with error %d\n", dwResultEnum);
			break;
		}
	}
	//
	// End do.
	//
	while (dwResultEnum != ERROR_NO_MORE_ITEMS);
	GlobalFree((HGLOBAL)lpnrLocal);
	dwResult = WNetCloseEnum(hEnum);

	if (dwResult != NO_ERROR) 
	{
		printf("WNetCloseEnum failed with error %d\n", dwResult);
		return FALSE;
	}

	return TRUE;
}

void DisplayStruct(int i, LPNETRESOURCE lpnrLocal)
{
	printf("NETRESOURCE[%d] Scope: ", i);
	switch (lpnrLocal->dwScope) {
	case (RESOURCE_CONNECTED):
		printf("connected\n");
		break;
	case (RESOURCE_GLOBALNET):
		printf("all resources\n");
		break;
	case (RESOURCE_REMEMBERED):
		printf("remembered\n");
		break;
	default:
		printf("unknown scope %d\n", lpnrLocal->dwScope);
		break;
	}

	printf("NETRESOURCE[%d] Type: ", i);
	switch (lpnrLocal->dwType) {
	case (RESOURCETYPE_ANY):
		printf("any\n");
		break;
	case (RESOURCETYPE_DISK):
		printf("disk\n");
		break;
	case (RESOURCETYPE_PRINT):
		printf("print\n");
		break;
	default:
		printf("unknown type %d\n", lpnrLocal->dwType);
		break;
	}

	printf("NETRESOURCE[%d] DisplayType: ", i);
	switch (lpnrLocal->dwDisplayType) {
	case (RESOURCEDISPLAYTYPE_GENERIC):
		printf("generic\n");
		break;
	case (RESOURCEDISPLAYTYPE_DOMAIN):
		printf("domain\n");
		break;
	case (RESOURCEDISPLAYTYPE_SERVER):
		printf("server\n");
		break;
	case (RESOURCEDISPLAYTYPE_SHARE):
		printf("share\n");
		break;
	case (RESOURCEDISPLAYTYPE_FILE):
		printf("file\n");
		break;
	case (RESOURCEDISPLAYTYPE_GROUP):
		printf("group\n");
		break;
	case (RESOURCEDISPLAYTYPE_NETWORK):
		printf("network\n");
		break;
	default:
		printf("unknown display type %d\n", lpnrLocal->dwDisplayType);
		break;
	}

	printf("NETRESOURCE[%d] Usage: 0x%x = ", i, lpnrLocal->dwUsage);
	if (lpnrLocal->dwUsage & RESOURCEUSAGE_CONNECTABLE)
		printf("connectable ");
	if (lpnrLocal->dwUsage & RESOURCEUSAGE_CONTAINER)
		printf("container ");
	printf("\n");

	printf("NETRESOURCE[%d] Localname: %S\n", i, lpnrLocal->lpLocalName);
	printf("NETRESOURCE[%d] Remotename: %S\n", i, lpnrLocal->lpRemoteName);
	printf("NETRESOURCE[%d] Comment: %S\n", i, lpnrLocal->lpComment);
	printf("NETRESOURCE[%d] Provider: %S\n", i, lpnrLocal->lpProvider);
	printf("\n");
}
