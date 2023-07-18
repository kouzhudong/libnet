#include "pch.h"
#include "NetworkListManager.h"


void TestINetworkConnection(INetworkConnection & NetworkConnection)
/*

https://learn.microsoft.com/zh-cn/windows/win32/api/netlistmgr/nn-netlistmgr-inetworkconnection
*/
{
    HRESULT hr = S_OK;
    VARIANT_BOOL IsConnected;
    hr = NetworkConnection.get_IsConnected(&IsConnected);
    //printf("IsConnected:%d.\r\n", IsConnected);

    hr = NetworkConnection.get_IsConnectedToInternet(&IsConnected);
    //printf("IsConnectedToInternet:%d.\r\n", IsConnected);

    GUID AdapterId;
    hr = NetworkConnection.GetAdapterId(&AdapterId);
    //printf("AdapterId:%s.\r\n", AdapterId);

    GUID  ConnectionId;
    hr = NetworkConnection.GetConnectionId(&ConnectionId);

    NLM_CONNECTIVITY Connectivity;
    hr = NetworkConnection.GetConnectivity(&Connectivity);

    NLM_DOMAIN_TYPE DomainType;
    hr = NetworkConnection.GetDomainType(&DomainType);

    INetwork * pNetwork;
    hr = NetworkConnection.GetNetwork(&pNetwork);
    if (hr == S_OK && pNetwork) {
        //TestNetwork(*pNetwork);//������ö���ˣ����򣬵ݹ��ˡ�
    }
}


void TestIEnumNetworkConnections(IEnumNetworkConnections & EnumNetworkConnection)
/*

https://learn.microsoft.com/zh-cn/windows/win32/api/netlistmgr/nn-netlistmgr-ienumnetworkconnections
*/
{
    HRESULT hr = S_OK;

    for (;;) {
        INetworkConnection * rgelt;
        ULONG  pceltFetched = 0;
        hr = EnumNetworkConnection.Next(1, &rgelt, &pceltFetched);
        if (hr != S_OK) {
            break;
        }

        TestINetworkConnection(*rgelt);
    }
}


void TestNetwork(INetwork & Network)
/*


ʵ�� INetwork �� COM ����ʵ�ָ������Ե����԰���
��Ҫ���ʴ����԰�������ʹ�� INetwork �ӿں� QueryInterface for IPropertyBag��

https://learn.microsoft.com/zh-cn/windows/win32/api/netlistmgr/nn-netlistmgr-inetwork
*/
{
    HRESULT hr = S_OK;
    VARIANT_BOOL IsConnected;
    hr = Network.get_IsConnected(&IsConnected);

    hr = Network.get_IsConnectedToInternet(&IsConnected);

    NLM_NETWORK_CATEGORY Category;
    hr = Network.GetCategory(&Category);

    NLM_CONNECTIVITY Connectivity;
    hr = Network.GetConnectivity(&Connectivity);
    
    BSTR Description = nullptr;
    hr = Network.GetDescription(&Description);
    SysFreeString(Description);

    NLM_DOMAIN_TYPE  NetworkType;
    hr = Network.GetDomainType(&NetworkType);

    BSTR  NetworkName = nullptr;
    hr = Network.GetName(&NetworkName);
    //SysFreeString(Description);//�ĵ�û˵Ҫִ���������������⡣

    IEnumNetworkConnections * pEnumNetworkConnection = nullptr;
    hr = Network.GetNetworkConnections(&pEnumNetworkConnection);
    if (hr == S_OK && pEnumNetworkConnection) {
        TestIEnumNetworkConnections(*pEnumNetworkConnection);
    }    

    GUID  GuidNetworkId;
    hr = Network.GetNetworkId(&GuidNetworkId);

    DWORD LowDateTimeCreated;
    DWORD HighDateTimeCreated;
    DWORD LowDateTimeConnected;
    DWORD HighDateTimeConnected;
    hr = Network.GetTimeCreatedAndConnected(&LowDateTimeCreated,
                                            &HighDateTimeCreated,
                                            &LowDateTimeConnected,
                                            &HighDateTimeConnected);

    //INetwork����SetCategory
    //INetwork����SetDescription
    //INetwork����SetName
}


EXTERN_C
__declspec(dllexport)
void WINAPI TestNetworkListManager()
/*


https://learn.microsoft.com/zh-cn/windows/win32/nla/about-the-network-list-manager-api
*/
{
    HRESULT hr = S_OK;

    hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr)) {//COM initialize failed        
        wprintf(L"CoInitialize failed: 0x%08lx\n", hr);
        return;
    }

    INetworkListManager *pNetworkListManager = NULL; 
    hr = CoCreateInstance(CLSID_NetworkListManager, NULL, 
                          CLSCTX_ALL, IID_INetworkListManager,
                          (LPVOID *)&pNetworkListManager);

    VARIANT_BOOL IsConnected = 0;
    hr = pNetworkListManager->get_IsConnected(&IsConnected);
    //IsConnected == -1

    hr = pNetworkListManager->get_IsConnectedToInternet(&IsConnected);
    //IsConnected == -1

    NLM_CONNECTIVITY Connectivity;
    hr = pNetworkListManager->GetConnectivity(&Connectivity);
    //Connectivity ==	NLM_CONNECTIVITY_IPV4_INTERNET | NLM_CONNECTIVITY_IPV6_INTERNET (1088)	NLM_CONNECTIVITY

    IEnumNetworkConnections * pEnum;
    hr = pNetworkListManager->GetNetworkConnections(&pEnum);
    if (hr == S_OK && pEnum) {
        TestIEnumNetworkConnections(*pEnum);
    }

    IEnumNetworks * pEnumNetwork;
    hr = pNetworkListManager->GetNetworks(NLM_ENUM_NETWORK_ALL, &pEnumNetwork);
    /*
    https://chromium.googlesource.com/chromium/src.git/+/72.0.3626.80/chrome/browser/ssl/captive_portal_helper_win.cc
    https://hg.mozilla.org/mozilla-central/file/tip/netwerk/system/win32/nsNotifyAddrListener.cpp
    https://learn.microsoft.com/zh-cn/windows/win32/api/netlistmgr/nf-netlistmgr-ienumnetworks-next
    \Windows-classic-samples\Samples\NetworkListManager\Utility.cpp
    */

    /*
    ʵ�������ӻ�Ͽ����ӵ������б��� IEnumNetworks ���档 
    �����������״̬��������ʱ��������´��б� 
    ����ʹ�� INetwork �ӿ�����������ĵ�ǰ����״̬��
    */
    for (;;) {
        INetwork * rgelt;
        ULONG  pceltFetched = 0;
        hr = pEnumNetwork->Next(1, &rgelt, &pceltFetched);
        if (hr != S_OK) {
            break;
        }

        TestNetwork(*rgelt);
    }

    //GetNetwork, GetNetworkConnection.

    //SetSimulatedProfileInfo��ClearSimulatedProfileInfo�ݲ���ʾ��

    CoUninitialize();
}