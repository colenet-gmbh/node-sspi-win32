#include <node.h>
#include <nan.h>

#include "shared.h"
#include "type_1.h"
#include "type_2.h"
#include "type_3.h"
#include "verify_type3.h"

namespace node_sspi_win32
{
	using v8::FunctionCallbackInfo;
	using v8::Isolate;
	using v8::Local;
	using v8::Object;
	using v8::String;
	using v8::Value;
	using v8::ArrayBuffer;

	using namespace std;


	void sspi_cleanup()
	{
		FreeCredentialsHandle(&sspi_module.credHandle);
		FreeCredentialsHandle(&sspi_module.server_cred_handle);
		FreeContextBuffer(&sspi_module.server_context);
		FreeContextBuffer(&sspi_module.context);

		if (sspi_module.securityDLL != NULL) {
			if (sspi_module.functable != NULL) {
				sspi_module.functable->FreeContextBuffer(sspi_module.pkgInfo);
			}
			FreeLibrary(sspi_module.securityDLL);
		}
	}

	void init_sspi()
	{
		LPCTSTR lpDllName = WINNT_SECURITY_DLL;
		INIT_SECURITY_INTERFACE pInit;
		SECURITY_STATUS ss = SEC_E_INTERNAL_ERROR;

		sspi_module.defaultPackage = DEFAULT_SSPI_PACKAGE;
		try {
			sspi_module.securityDLL = LoadLibrary(lpDllName);
			pInit = (INIT_SECURITY_INTERFACE)GetProcAddress(sspi_module.securityDLL, SECURITY_ENTRYPOINT);
			sspi_module.functable = pInit();
			ss = sspi_module.functable->EnumerateSecurityPackages(&sspi_module.numPackages, &sspi_module.pkgInfo);
			if (ss != SEC_E_OK) {
				throw std::exception("Error loading SSPI module.");
			}
			sspi_module.supportsSSPI = TRUE;
		}
		catch (...) {
			sspi_cleanup();
			throw;
		}
	}

	// Call upon shutting down node.js
	void exit(void*)
	{
		sspi_cleanup();
	}

	void init(Local<Object> exports)
	{
		node::AtExit(exit);
		init_sspi();

		exports->Set(Nan::New("generate_type1_message").ToLocalChecked(),
			Nan::New<v8::FunctionTemplate>(generate_type1_message)->GetFunction());

		exports->Set(Nan::New("generate_type2_message").ToLocalChecked(),
			Nan::New<v8::FunctionTemplate>(generate_type2_message)->GetFunction());

		exports->Set(Nan::New("generate_type3_message").ToLocalChecked(),
			Nan::New<v8::FunctionTemplate>(generate_type3_message)->GetFunction());

		exports->Set(Nan::New("verify_type3_message").ToLocalChecked(),
			Nan::New<v8::FunctionTemplate>(verify_type_3)->GetFunction());
	}

	NODE_MODULE(addon, init);

}  // namespace node_amisxml
