#include <node.h> // include first fixing compile errors with WinSock redefinitions
#include <nan.h>
#include <windows.h>
#include <security.h>
#include <strsafe.h>
#include <iostream>
#include <sstream>

#include "shared.h"
#include "type_1.h"

using namespace std;

namespace node_sspi_win32
{
	void generate_type1_message(const Nan::FunctionCallbackInfo<v8::Value> &args)
	{
		TimeStamp expiry;
		SECURITY_STATUS result = sspi_module.functable->AcquireCredentialsHandle(NULL,
			sspi_module.pkgInfo->Name,
			SECPKG_CRED_OUTBOUND, // Outgoing challenge 
			NULL, // LUID not used
			NULL, // package specific info not used
			NULL, // GetKeyFn must be Null
			NULL, // GetKeyArg should be NULL
			&sspi_module.credHandle, // receives credentials
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
		/* Setup the type-1 "output" security buffer */
		type_1_desc.ulVersion = SECBUFFER_VERSION;
		type_1_desc.cBuffers = 1;
		type_1_desc.pBuffers = &type_1_buf;

		type_1_buf.BufferType = SECBUFFER_TOKEN;
		type_1_buf.pvBuffer = malloc(sspi_module.pkgInfo->cbMaxToken);
		type_1_buf.cbBuffer = sspi_module.pkgInfo->cbMaxToken;

		unsigned long attrs;
		result = sspi_module.functable->InitializeSecurityContext(&sspi_module.credHandle,
			NULL, // first call
			"", // no server information
			ISC_REQ_DELEGATE | ISC_REQ_MUTUAL_AUTH,
			0, // reserved
			SECURITY_NATIVE_DREP, // NETWORK representation
			NULL, // 
			0,
			&sspi_module.context,
			&type_1_desc,
			&attrs,
			nullptr);
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
		auto buffer = Nan::NewBuffer((char*)type_1_buf.pvBuffer, type_1_buf.cbBuffer, buffer_free, nullptr);
		args.GetReturnValue().Set(buffer.ToLocalChecked());
	}

}