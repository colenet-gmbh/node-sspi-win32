#include <node.h> // include first fixing compile errors with WinSock redefinitions
#include <nan.h>

#include <windows.h>
#include <security.h>
#include <strsafe.h>
#include <iostream>
#include <sstream>

#include "shared.h"
#include "type_2.h"

using namespace std;

namespace node_sspi_win32
{

	//
	// generates the type2 message based on the buffer (BUFFER!) from type1_message response
	// from server
	void generate_type2_message(const Nan::FunctionCallbackInfo<v8::Value>& info)
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

		TimeStamp expiry;
		int64_t type1_request_len = info[1]->ToInt32()->IntegerValue();
		void* type1_request = (void*)node::Buffer::Data(info[0]->ToObject());
		SecBuffer type_1_buf;
		SecBuffer type_2_buf;
		SecBufferDesc type_1_desc;
		SecBufferDesc type_2_desc;
		SECURITY_STATUS status;
		unsigned long attrs;

		SECURITY_STATUS result = sspi_module.functable->AcquireCredentialsHandle(NULL,
			sspi_module.pkgInfo->Name,
			SECPKG_CRED_INBOUND | SECPKG_CRED_BOTH, // Outgoing challenge 
			NULL, // LUID not used
			NULL, // package specific info not used
			NULL, // GetKeyFn must be Null
			NULL, // GetKeyArg should be NULL
			&sspi_module.server_cred_handle, // receives credentials
			&expiry);
		if (result != SEC_E_OK)
		{
			stringstream msg;
			msg << "generate_type2_message: AcquireCredentialsHandle: " << generate_error_msg(result);
			Nan::ThrowError(msg.str().c_str());
			return;
		}

		/* Setup the type-1 "input" security buffer */
		type_1_desc.ulVersion = SECBUFFER_VERSION;
		type_1_desc.cBuffers = 1;
		type_1_desc.pBuffers = &type_1_buf;
		type_1_buf.BufferType = SECBUFFER_TOKEN;
		type_1_buf.pvBuffer = type1_request;
		type_1_buf.cbBuffer = type1_request_len;

		/* Setup the type-2 "output" security buffer */
		type_2_desc.ulVersion = SECBUFFER_VERSION;
		type_2_desc.cBuffers = 1;
		type_2_desc.pBuffers = &type_2_buf;
		type_2_buf.BufferType = SECBUFFER_TOKEN;
		type_2_buf.pvBuffer = malloc(sspi_module.pkgInfo->cbMaxToken);
		type_2_buf.cbBuffer = sspi_module.pkgInfo->cbMaxToken;

		/* Generate our type-2 message */
		status = sspi_module.functable->AcceptSecurityContext(&sspi_module.server_cred_handle,
			nullptr,
			&type_1_desc,
			0,
			SECURITY_NETWORK_DREP,
			&sspi_module.server_context,
			&type_2_desc,
			&attrs,
			nullptr
		);
		if (status != SEC_E_OK && status != SEC_I_CONTINUE_NEEDED)
		{
			stringstream msg;
			msg << "generate_type2_message: Accept: " << generate_error_msg(status);
			Nan::ThrowError(msg.str().c_str());
			return;
		}
		auto buffer = Nan::NewBuffer((char*)type_2_buf.pvBuffer, type_2_buf.cbBuffer, buffer_free, nullptr);

		info.GetReturnValue().Set(buffer.ToLocalChecked());
	}

}