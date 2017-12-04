#include <node.h>
#include <nan.h>
#include <strstream>
#include <iostream>
#include <sstream>
#include <stack>
#include <cvt/wstring>
#include <cvt/utf16>
#include <windows.h>
#include <security.h>
#include <strsafe.h>

#define WINNT_SECURITY_DLL "SECURITY.DLL"
#define DEFAULT_SSPI_PACKAGE "NTLM"
#define MAX_TOKEN_SIZE 1024

typedef struct {
	BOOL supportsSSPI;
	LPSTR defaultPackage;
	HMODULE securityDLL;
	SecurityFunctionTable *functable;
	ULONG numPackages;
	PSecPkgInfo pkgInfo;
	CtxtHandle context;
} sspi_module_rec;

sspi_module_rec sspi_module{ 0, };

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
	char* generate_error_msg(SECURITY_STATUS status)
	{
		switch (status)
		{
		case SEC_E_INCOMPLETE_MESSAGE:
			return "SEC_E_INCOMPLETE_MESSAGE. Data for the whole message was not read from the wire.";
		case SEC_I_COMPLETE_AND_CONTINUE:
			return "SEC_I_COMPLETE_AND_CONTINUE. The client must call CompleteAuthToken and then pass the output to the server.";
		case SEC_I_COMPLETE_NEEDED:
			return "SEC_I_COMPLETE_NEEDED. The client must finish building the message and then call the CompleteAuthToken function.";
		case SEC_I_CONTINUE_NEEDED:
			return "SEC_I_CONTINUE_NEEDED. The client must send the output token to the server and wait for a return token.";
		case SEC_I_INCOMPLETE_CREDENTIALS:
			return "SEC_I_INCOMPLETE_CREDENTIALS. The server has requested client authentication, but either the supplied credentials do not include a certificate, or the certificate was not issued by a certification authority that the server trusts.";
		case SEC_E_INSUFFICIENT_MEMORY:
			return "SEC_E_INSUFFICIENT_MEMORY";
		case SEC_E_INTERNAL_ERROR:
			return "SEC_E_INTERNAL_ERROR. An error occurred that did not map to an SSPI error code";
		case SEC_E_INVALID_HANDLE:
			return "SEC_E_INVALID_HANDLE. The handle passed to the function is not valid.";
		case SEC_E_INVALID_TOKEN:
			return "SEC_E_INVALID_TOKEN. The input token is malformed";
		case SEC_E_LOGON_DENIED:
			return "SEC_E_LOGON_DENIED. The logon failed.";
		case SEC_E_NO_AUTHENTICATING_AUTHORITY:
			return "SEC_E_NO_AUTHENTICATING_AUTHORITY. No authority could be contacted for authentication. The domain name of the authenticating party could be wrong, the domain could be unreachable, or there might have been a trust relationship failure.";
		case SEC_E_NO_CREDENTIALS:
			return "SEC_E_NO_CREDENTIALS. No credentials are available in the security package.";
		case SEC_E_TARGET_UNKNOWN:
			return "SEC_E_TARGET_UNKNOWN. The target was not recognized.";
		case SEC_E_UNSUPPORTED_FUNCTION:
			return "SEC_E_UNSUPPORTED_FUNCTION. The value of the fContextReq parameter is not valid.";
		case SEC_E_WRONG_PRINCIPAL:
			return "SEC_E_WRONG_PRINCIPAL. The principal that received the authentication request is not the same as the one passed into the pszTargetName parameter. This error indicates a failure in mutual authentication.";
		case SEC_E_DELEGATION_POLICY:
			return "SEC_E_DELEGATION_POLICY. The policy does not support delegation of credentials to the target server.";
		case SEC_E_POLICY_NLTM_ONLY:
			return "SEC_E_POLICY_NLTM_ONLY. Delegation of credentials to the target server is not allowed when mutual authentication is not achieved.";
		case SEC_E_MUTUAL_AUTH_FAILED:
			return "SEC_E_MUTUAL_AUTH_FAILED. Server authentication failed when the ISC_REQ_MUTUAL_AUTH flag is specified in the fContextReq parameter.";
		default:
			return "Unknown Error";
		}
	}

	void generate_type1_message(const Nan::FunctionCallbackInfo<v8::Value> &args)
	{
		CredHandle credHandle;
		TimeStamp expiry;
		SECURITY_STATUS result = sspi_module.functable->AcquireCredentialsHandleA(NULL,
			sspi_module.pkgInfo->Name,
			SECPKG_CRED_OUTBOUND, // Outgoing challenge 
			NULL, // LUID not used
			NULL, // package specific info not used
			NULL, // GetKeyFn must be Null
			NULL, // GetKeyArg should be NULL
			&credHandle, // receives credentials
			&expiry);
		if (result != SEC_E_OK)
		{
			stringstream msg;
			msg << "generate_type1_message: AcquireCredentialsHandle: " << generate_error_msg(result);
			Nan::ThrowError(msg.str().c_str());
			return;
		}

		cout << "aquired credentials handle" << endl;
		SecBufferDesc type_1_desc{ 0, };
		SecBuffer type_1_buf{ 0, };
		vector<BYTE> output_token(sspi_module.pkgInfo->cbMaxToken, 0);
		/* Setup the type-1 "output" security buffer */
		type_1_desc.ulVersion = SECBUFFER_VERSION;
		type_1_desc.cBuffers = 1;
		type_1_desc.pBuffers = &type_1_buf;
		type_1_buf.BufferType = SECBUFFER_TOKEN;
		type_1_buf.pvBuffer = &output_token[0];
		type_1_buf.cbBuffer = output_token.size();

		unsigned long attrs;
		result = sspi_module.functable->InitializeSecurityContextA(&credHandle,
			NULL, // first call
			"", // no server information
			0, // no additional flags
			0, // reserved
			SECURITY_NETWORK_DREP, // NETWORK representation
			NULL, // 
			0,
			&sspi_module.context,
			&type_1_desc,
			&attrs,
			&expiry);
		cout << "intialized security context" << endl;
		if (result == SEC_I_COMPLETE_NEEDED || result == SEC_I_COMPLETE_AND_CONTINUE)
		{
			cout << "completing Auth Token" << endl,
			sspi_module.functable->CompleteAuthToken(&sspi_module.context, &type_1_desc);
		}
		else if (result != SEC_E_OK && result != SEC_I_CONTINUE_NEEDED)
		{
			stringstream msg;
			msg << "generate_type1_message: InitializeSecurityContext: " << generate_error_msg(result);
			Nan::ThrowError(msg.str().c_str());
			return;
		}
		auto buffer = Nan::NewBuffer((char*)&output_token[0], type_1_buf.cbBuffer);
		args.GetReturnValue().Set(buffer.ToLocalChecked());
	}


	//
	// generates the type3 message based on the buffer (BUFFER!) from type2_message response
	// from server
	void generate_type3_message(const Nan::FunctionCallbackInfo<v8::Value>& info)
	{
		if (info.Length() != 2)
		{
			Nan::ThrowTypeError("Wrong number of arguments");
			return;
		}

		if (!info[0]->IsObject())
		{
			Nan::ThrowTypeError("1. Argument must be Buffer");
			return;
		}
		if (!info[1]->IsInt32())
		{
			Nan::ThrowTypeError("1. Argument must be Integer (length of buffer");
			return;
		}

		char* type2_response = (char*)node::Buffer::Data(info[0]->ToObject());
		int32_t type2_response_len = info[1]->ToInt32()->Int32Value();
		
		SecBuffer type_2_buf;
		SecBuffer type_3_buf;
		SecBufferDesc type_2_desc;
		SecBufferDesc type_3_desc;
		SECURITY_STATUS status;
		unsigned long attrs;
		TimeStamp expiry; /* For Windows 9x compatibility of SSPI calls */

		/* Setup the type-2 "input" security buffer */
		type_2_desc.ulVersion = SECBUFFER_VERSION;
		type_2_desc.cBuffers = 1;
		type_2_desc.pBuffers = &type_2_buf;
		type_2_buf.BufferType = SECBUFFER_TOKEN;
		type_2_buf.pvBuffer = type2_response;
		type_2_buf.cbBuffer = type2_response_len;

		vector<char> output_token(sspi_module.pkgInfo->cbMaxToken, 0);
		/* Setup the type-3 "output" security buffer */
		type_3_desc.ulVersion = SECBUFFER_VERSION;
		type_3_desc.cBuffers = 1;
		type_3_desc.pBuffers = &type_3_buf;
		type_3_buf.BufferType = SECBUFFER_TOKEN;
		type_3_buf.pvBuffer = &output_token[0];
		type_3_buf.cbBuffer = output_token.size();

		/* Generate our type-3 message */
		status = sspi_module.functable->InitializeSecurityContext(NULL, // current user Context
			&sspi_module.context,
			(TCHAR *)TEXT(""),
			0, 0, SECURITY_NETWORK_DREP,
			&type_2_desc,
			0, &sspi_module.context,
			&type_3_desc,
			&attrs, &expiry);
		if (status != SEC_E_OK)
		{
			stringstream msg;
			msg << "generate_type3_message: InitializeSecurityContext: " << generate_error_msg(status);
			Nan::ThrowError(msg.str().c_str());
			return;
		}
		auto buffer = Nan::NewBuffer((char*)&output_token[0], type_3_buf.cbBuffer);

		info.GetReturnValue().Set(buffer.ToLocalChecked());
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
		exports->Set(Nan::New("generate_type3_message").ToLocalChecked(),
			Nan::New<v8::FunctionTemplate>(generate_type3_message)->GetFunction());
	}

	NODE_MODULE(addon, init);

}  // namespace node_amisxml
