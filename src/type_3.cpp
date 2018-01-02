#include <node.h> // include first fixing compile errors with WinSock redefinitions
#include <nan.h>

#include <windows.h>
#include <security.h>
#include <strsafe.h>
#include <iostream>
#include <sstream>

#include "shared.h"
#include "type_3.h"

using namespace std; 

namespace node_sspi_win32
{
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

		/* Setup the type-3 "output" security buffer */
		type_3_desc.ulVersion = SECBUFFER_VERSION;
		type_3_desc.cBuffers = 1;
		type_3_desc.pBuffers = &type_3_buf;
		type_3_buf.BufferType = SECBUFFER_TOKEN;
		type_3_buf.pvBuffer = malloc(sspi_module.pkgInfo->cbMaxToken);
		type_3_buf.cbBuffer = sspi_module.pkgInfo->cbMaxToken;

		/* Generate our type-3 message */
		status = sspi_module.functable->InitializeSecurityContext(NULL, // current user Context
			&sspi_module.context,
			(TCHAR *)TEXT(""),
			0, 0, SECURITY_NETWORK_DREP,
			&type_2_desc,
			0, &sspi_module.context,
			&type_3_desc,
			&attrs, &expiry);
		if (status != SEC_E_OK && status != SEC_I_CONTINUE_NEEDED)
		{
			stringstream msg;
			msg << "generate_type3_message: InitializeSecurityContext: " << generate_error_msg(status);
			Nan::ThrowError(msg.str().c_str());
			return;
		}
		auto buffer = Nan::NewBuffer((char*)type_3_buf.pvBuffer, type_3_buf.cbBuffer, buffer_free, nullptr);

		info.GetReturnValue().Set(buffer.ToLocalChecked());
	}
}