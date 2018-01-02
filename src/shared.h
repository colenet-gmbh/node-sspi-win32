#if !defined(_SHARED_H__INCLUDED__)
#define _SHARED_H__INCLUDED__

#include <node.h> // include first fixing compile errors with WinSock redefinitions
#include <windows.h>
#include <security.h>

#define WINNT_SECURITY_DLL "SECURITY.DLL"
#define DEFAULT_SSPI_PACKAGE L"NTLM"
#define MAX_TOKEN_SIZE 1024

typedef struct {
	BOOL supportsSSPI;
	SEC_WCHAR* defaultPackage;
	HMODULE securityDLL;
	SecurityFunctionTable *functable;
	ULONG numPackages;
	PSecPkgInfo pkgInfo;
	CtxtHandle context;
	CredHandle credHandle;
	// for testing only server side state
	CtxtHandle server_context;
	CredHandle server_cred_handle;
} sspi_module_rec;

namespace node_sspi_win32
{
	char* generate_error_msg(SECURITY_STATUS status);
	void buffer_free(char *data, void *hint);
	extern sspi_module_rec sspi_module;
}
#endif _SHARED_H__INCLUDED__