#include "pch.h"
#include "NetworkListManager.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


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
        //TestNetwork(*pNetwork);//不能再枚举了，否则，递归了。
    }
}


void TestIEnumNetworkConnections(IEnumNetworkConnections & EnumNetworkConnection)
/*

https://learn.microsoft.com/zh-cn/windows/win32/api/netlistmgr/nn-netlistmgr-ienumnetworkconnections
*/
{
    HRESULT hr = S_OK;

    for (;;) {
        INetworkConnection * rgelt{};
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


实现 INetwork 的 COM 对象还实现附加属性的属性包。
若要访问此属性包，可以使用 INetwork 接口和 QueryInterface for IPropertyBag。

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
    //SysFreeString(Description);//文档没说要执行这个，否则出问题。

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

    //INetwork：：SetCategory
    //INetwork：：SetDescription
    //INetwork：：SetName
}


EXTERN_C
__declspec(dllexport)
void WINAPI TestNetworkListManager()
/*


https://learn.microsoft.com/zh-cn/windows/win32/nla/about-the-network-list-manager-api
*/
{
    HRESULT hr = S_OK;

    hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr)) {//COM initialize failed        
        wprintf(L"CoInitialize failed: 0x%08lx\n", hr);
        return;
    }

    INetworkListManager * pNetworkListManager = nullptr;
    hr = CoCreateInstance(CLSID_NetworkListManager, nullptr,
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
    实例化连接或断开连接的网络列表由 IEnumNetworks 缓存。
    当网络的连接状态发生更改时，不会更新此列表。
    建议使用 INetwork 接口来检索网络的当前连接状态。
    */
    for (;;) {
        INetwork * rgelt{};
        ULONG  pceltFetched = 0;
        hr = pEnumNetwork->Next(1, &rgelt, &pceltFetched);
        if (hr != S_OK) {
            break;
        }

        TestNetwork(*rgelt);
    }

    //GetNetwork, GetNetworkConnection.

    //SetSimulatedProfileInfo和ClearSimulatedProfileInfo暂不演示。

    CoUninitialize();
}


//////////////////////////////////////////////////////////////////////////////////////////////////


class MyNetWorkEvent : public INetworkListManagerEvents
{
public:
    MyNetWorkEvent()
    {
        m_ref = 1;
    }

    virtual HRESULT STDMETHODCALLTYPE ConnectivityChanged(NLM_CONNECTIVITY newConnectivity)
    {
        printf("ConnectivityChanged: %04X\n", newConnectivity);
        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppvObject)
    {
        HRESULT Result = S_OK;
        if (IsEqualIID(riid, IID_IUnknown)) {
            *ppvObject = dynamic_cast<IUnknown *>(this);
        } else if (IsEqualIID(riid, IID_INetworkListManagerEvents)) {
            *ppvObject = dynamic_cast<INetworkListManagerEvents *>(this);
        } else {
            Result = E_NOINTERFACE;
        }

        return Result;
    }

    virtual ULONG STDMETHODCALLTYPE AddRef(void)
    {
        return InterlockedIncrement(&m_ref);
    }

    virtual ULONG STDMETHODCALLTYPE Release(void)
    {
        LONG Result = InterlockedDecrement(&m_ref);
        if (Result == 0)
            delete this;
        return Result;
    }

private:
    LONG m_ref;
};


EXTERN_C
__declspec(dllexport)
void WINAPI TestNetworkListManagerEvents()
/*


https://learn.microsoft.com/zh-cn/windows/win32/nla/about-the-network-list-manager-api
https://www.cxyzjd.com/article/chouhuan1877/100808606
*/
{
    (void)CoInitialize(nullptr);

    INetworkListManager * pNetworkListManager = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_NetworkListManager,
                                  nullptr,
                                  CLSCTX_ALL,
                                  IID_INetworkListManager,
                                  (LPVOID *)&pNetworkListManager);

    VARIANT_BOOL bConnected = VARIANT_FALSE;
    hr = pNetworkListManager->get_IsConnected(&bConnected);

    IConnectionPointContainer * pCPContainer = nullptr;
    hr = pNetworkListManager->QueryInterface(IID_IConnectionPointContainer, (void **)&pCPContainer);
    IConnectionPoint * pConnectPoint = nullptr;
    hr = pCPContainer->FindConnectionPoint(IID_INetworkListManagerEvents, &pConnectPoint);

    DWORD Cookie = 0;
    MyNetWorkEvent * pMyEvent = new MyNetWorkEvent();
    pConnectPoint->Advise((IUnknown *)pMyEvent, &Cookie);

    // 必须有下面这个消息循环
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (msg.message == WM_QUIT) {
            break;
        }
    }

    pConnectPoint->Unadvise(Cookie);
    pConnectPoint->Release();
    pCPContainer->Release();
    pNetworkListManager->Release();
    CoUninitialize();
}


//////////////////////////////////////////////////////////////////////////////////////////////////


namespace Utility
{
    enum class CostGuidance
    {
        Normal,
        OptIn,
        Conservative
    };

    CostGuidance GetNetworkCostGuidance(NLM_CONNECTION_COST cost)
    {
        if (WI_IsAnyFlagSet(cost, NLM_CONNECTION_COST_ROAMING | NLM_CONNECTION_COST_OVERDATALIMIT)) {
            if (WI_IsFlagSet(cost, NLM_CONNECTION_COST_ROAMING)) {
                std::wcout << "Connection is roaming; using the connection may result in additional charge." << std::endl;
            } else {
                std::wcout << "Connection has exceeded the usage cap limit." << std::endl;
            }
            return CostGuidance::OptIn;
        } else if (WI_IsAnyFlagSet(cost, NLM_CONNECTION_COST_FIXED | NLM_CONNECTION_COST_VARIABLE)) {
            if (WI_IsFlagSet(cost, NLM_CONNECTION_COST_FIXED)) {
                std::wcout << "Connection has limited allowed usage." << std::endl;
            } else {
                std::wcout << "Connection is charged based on usage." << std::endl;
            }
            return CostGuidance::Conservative;
        } else {
            if (WI_IsFlagSet(cost, NLM_CONNECTION_COST_UNRESTRICTED)) {
                std::wcout << "Connection cost is unrestricted." << std::endl;
            } else {
                std::wcout << "Connection cost is unknown." << std::endl;
            }
            return CostGuidance::Normal;
        }
    }

    bool ShouldAttemptToConnectToInternet(NLM_CONNECTIVITY connectivity, INetworkListManager * networkListManager)
    {
        // check internet connectivity
        if (WI_IsAnyFlagSet(connectivity, NLM_CONNECTIVITY_IPV4_INTERNET | NLM_CONNECTIVITY_IPV6_INTERNET)) {
            std::wcout << "Machine has internet connectivity." << std::endl;
            return true;
        } else if (WI_IsAnyFlagSet(connectivity, NLM_CONNECTIVITY_IPV4_LOCALNETWORK | NLM_CONNECTIVITY_IPV6_LOCALNETWORK)) {
            // we are local connected, check if we're behind a captive portal before attempting to connect to the Internet.
            //
            // note: being behind a captive portal means connectivity is local and there is at least one interface(network)
            // behind a captive portal.

            bool localConnectedBehindCaptivePortal = false;
            wil::com_ptr<IEnumNetworks> enumConnectedNetworks;
            THROW_IF_FAILED(networkListManager->GetNetworks(NLM_ENUM_NETWORK_CONNECTED, enumConnectedNetworks.put()));

            // Enumeration returns S_FALSE when there are no more items.
            wil::com_ptr<INetwork> networkConnection;
            while (THROW_IF_FAILED(enumConnectedNetworks->Next(1, networkConnection.put(), nullptr)) == S_OK) {
                wil::com_ptr<IPropertyBag> networkProperties = networkConnection.query<IPropertyBag>();

                // these might fail if there's no value
                wil::unique_variant variantInternetConnectivityV4;
                networkProperties->Read(NA_InternetConnectivityV4, variantInternetConnectivityV4.addressof(), nullptr);
                wil::unique_variant variantInternetConnectivityV6;
                networkProperties->Read(NA_InternetConnectivityV6, variantInternetConnectivityV6.addressof(), nullptr);

                // read the VT_UI4 from the VARIANT and cast it to a NLM_INTERNET_CONNECTIVITY
                // If there is no value, then assume no special treatment.
                NLM_INTERNET_CONNECTIVITY v4Connectivity = static_cast<NLM_INTERNET_CONNECTIVITY>(variantInternetConnectivityV6.vt == VT_UI4 ? variantInternetConnectivityV4.ulVal : 0);
                NLM_INTERNET_CONNECTIVITY v6Connectivity = static_cast<NLM_INTERNET_CONNECTIVITY>(variantInternetConnectivityV6.vt == VT_UI4 ? variantInternetConnectivityV6.ulVal : 0);

                if (WI_IsFlagSet(v4Connectivity, NLM_INTERNET_CONNECTIVITY_WEBHIJACK) || WI_IsFlagSet(v6Connectivity, NLM_INTERNET_CONNECTIVITY_WEBHIJACK)) {
                    // at least one connected interface is behind a captive portal
                    // we should assume that the device is behind it
                    localConnectedBehindCaptivePortal = true;
                }
            }

            if (!localConnectedBehindCaptivePortal) {
                std::wcout << "Machine has local connectivity and not behind a captive portal." << std::endl;
                return true;
            } else {
                std::wcout << "Machine is behind a captive portal." << std::endl;
            }
        } else {
            std::wcout << "Machine is not connected." << std::endl;
        }
        return false;
    }

    void EvaluateAndReportConnectivity(bool optedIn, NLM_CONNECTIVITY connectivity, INetworkListManager * networkListManager)
    {
        if (Utility::ShouldAttemptToConnectToInternet(connectivity, networkListManager)) {
            //Utility::EvaluateCostAndConnect(optedIn, networkListManager);
            UNREFERENCED_PARAMETER(optedIn);
        } else {
            std::wcout << "Not attempting to connect to the Internet." << std::endl;
        }
    }

    void EvaluateAndReportConnectionCost(NLM_CONNECTION_COST connectionCost)
    {
        const auto costGuidance = Utility::GetNetworkCostGuidance(connectionCost);
        switch (costGuidance) {
        case Utility::CostGuidance::OptIn:
            // In opt-in scenarios, apps handle cases where the network access cost is significantly higher than the plan cost.
            // For example, when a user is roaming, a mobile carrier may charge a higher rate data usage.
            std::wcout << "Apps should implement opt-in behavior." << std::endl;
            break;
        case Utility::CostGuidance::Conservative:
            // In conservative scenarios, apps implement restrictions for optimizing network usage to handle transfers over metered networks.
            std::wcout << "Apps should implement conservative behavior." << std::endl;
            break;
        case Utility::CostGuidance::Normal:
        default:
            // In normal scenarios, apps do not implement restrictions. Apps treat the connection as unlimited in cost.
            std::wcout << "Apps should implement normal behavior." << std::endl;
            break;
        }
    }
}


#pragma region COM helpers
using unique_connectionpoint_token = wil::unique_com_token<IConnectionPoint,
    DWORD,
    decltype(&IConnectionPoint::Unadvise),
    &IConnectionPoint::Unadvise>;

unique_connectionpoint_token FindConnectionPointAndAdvise(REFIID itf, IUnknown * source, IUnknown * sink)
{
    wil::com_ptr<IConnectionPointContainer> container = wil::com_query<IConnectionPointContainer>(source);
    wil::com_ptr<IConnectionPoint> connectionPoint;
    THROW_IF_FAILED(container->FindConnectionPoint(itf, connectionPoint.put()));

    unique_connectionpoint_token token{connectionPoint.get()};
    THROW_IF_FAILED(connectionPoint->Advise(sink, token.put()));
    return token;
}

// typename T is the connection point interface we are connecting to.
template <typename T>
unique_connectionpoint_token FindConnectionPointAndAdvise(IUnknown * source, IUnknown * sink)
{
    return FindConnectionPointAndAdvise(__uuidof(T), source, sink);
}
#pragma endregion


#pragma region ListenToNetworkConnectivityChangesSample
class NetworkConnectivityListener final
    : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, INetworkListManagerEvents>
{
public:
    NetworkConnectivityListener(bool optedIn, INetworkListManager * networkListManager)
        : m_optedIn(optedIn), m_networkListManager(networkListManager)
    {
    }

    NetworkConnectivityListener(const NetworkConnectivityListener &) = delete;
    NetworkConnectivityListener & operator=(const NetworkConnectivityListener &) = delete;

    IFACEMETHODIMP ConnectivityChanged(NLM_CONNECTIVITY connectivity) noexcept override try {
        std::wcout << L"INetworkListManagerEvents::ConnectivityChanged" << std::endl;
        Utility::EvaluateAndReportConnectivity(m_optedIn, connectivity, m_networkListManager.get());
        return S_OK;
    }
    CATCH_RETURN();

private:
    wil::com_ptr<INetworkListManager> m_networkListManager;
    bool m_optedIn;
};


EXTERN_C
__declspec(dllexport)
void WINAPI ListenToNetworkConnectivityChangesSample(BOOL optedIn)
/*

有待改进。
*/
{
    (void)CoInitialize(nullptr);

    std::wcout << "Listening to network connectivity changes." << std::endl;
    if (optedIn) {
        std::wcout << L"User has opted into possible network usage charges." << std::endl;
    } else {
        std::wcout << L"User has not opted into possible network usage charges." << std::endl;
    }

    const auto networkListManager = wil::CoCreateInstance<NetworkListManager, INetworkListManager>();

    auto token = FindConnectionPointAndAdvise<INetworkListManagerEvents>(
        networkListManager.get(),
        Microsoft::WRL::Make<NetworkConnectivityListener>(optedIn, networkListManager.get()).Get());

    std::wcout << "Press Enter to stop." << std::endl;
    static_cast<void>(getchar());

    CoUninitialize();
}
#pragma endregion


//////////////////////////////////////////////////////////////////////////////////////////////////