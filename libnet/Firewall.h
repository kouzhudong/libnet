#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////


#define NET_FW_IP_PROTOCOL_TCP_NAME L"TCP"
#define NET_FW_IP_PROTOCOL_UDP_NAME L"UDP"

#define NET_FW_RULE_DIR_IN_NAME L"In"
#define NET_FW_RULE_DIR_OUT_NAME L"Out"

#define NET_FW_RULE_ACTION_BLOCK_NAME L"Block"
#define NET_FW_RULE_ACTION_ALLOW_NAME L"Allow"

#define NET_FW_RULE_ENABLE_IN_NAME L"TRUE"
#define NET_FW_RULE_DISABLE_IN_NAME L"FALSE"


//////////////////////////////////////////////////////////////////////////////////////////////////


int __cdecl DynamicEdgeTraversal();

int __cdecl EnumeratingFirewallRules();

int __cdecl DisablingFirewallPerInterface();

int __cdecl DisablingFirewall();

int __cdecl EnablingGroup();

int __cdecl GettingFirewallSettings();

int __cdecl RestrictingService();

int __cdecl AddGreRule();

int __cdecl AddIcmpRule();

int __cdecl AddLanRule();

int __cdecl AddOutboundRule();

int __cdecl AddPerInterfaceRule();

int __cdecl AddServiceRule();

void __cdecl RWFTOFPM();

void __cdecl RWFWTOFPM();

int __cdecl MultipleProfiles();

int __cdecl Exercising(int argc, wchar_t * argv[]);


//////////////////////////////////////////////////////////////////////////////////////////////////
