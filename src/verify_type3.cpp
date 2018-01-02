#include <node.h> // include first fixing compile errors with WinSock redefinitions
#include <nan.h>

#include <windows.h>
#include <security.h>
#include <strsafe.h>
#include <iostream>
#include <sstream>

#include "shared.h"
#include "verify_type3.h"

using namespace std;

namespace node_sspi_win32
{
	//
	// generates the type2 message based on the buffer (BUFFER!) from type1_message response
	// from server
	void verify_type_3(const Nan::FunctionCallbackInfo<v8::Value>& info)
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

		int64_t type1_request_len = info[1]->ToInt32()->IntegerValue();
		void* type1_request = node::Buffer::Data(info[0]->ToObject());

		SecBuffer type_3_buf;
		SecBuffer type_4_buf;
		SecBufferDesc type_3_desc;
		SecBufferDesc type_4_desc;
		SECURITY_STATUS status;
		unsigned long attrs;

		/* Setup the type-3 "input" security buffer */
		type_3_desc.ulVersion = SECBUFFER_VERSION;
		type_3_desc.cBuffers = 1;
		type_3_desc.pBuffers = &type_3_buf;
		type_3_buf.BufferType = SECBUFFER_TOKEN;
		type_3_buf.pvBuffer = type1_request;
		type_3_buf.cbBuffer = type1_request_len;

		/* Setup the type-4 "output" security buffer */
		type_4_desc.ulVersion = SECBUFFER_VERSION;
		type_4_desc.cBuffers = 1;
		type_4_desc.pBuffers = &type_4_buf;
		type_4_buf.BufferType = SECBUFFER_TOKEN;
		type_4_buf.pvBuffer = malloc(sspi_module.pkgInfo->cbMaxToken);
		type_4_buf.cbBuffer = sspi_module.pkgInfo->cbMaxToken;

		/* Generate our type-4 message */
		status = sspi_module.functable->AcceptSecurityContext(&sspi_module.server_cred_handle,
			&sspi_module.server_context,
			&type_3_desc,
			0,
			SECURITY_NETWORK_DREP,
			&sspi_module.server_context,
			&type_4_desc,
			&attrs,
			nullptr
		);
		if (status != SEC_E_OK && status != SEC_I_CONTINUE_NEEDED)
		{
			stringstream msg;
			msg << "verify_type3_message: Accept: " << generate_error_msg(status);
			Nan::ThrowError(msg.str().c_str());
			return;
		}
		auto buffer = Nan::NewBuffer((char*)type_4_buf.pvBuffer, type_4_buf.cbBuffer, buffer_free, nullptr);

		info.GetReturnValue().Set(buffer.ToLocalChecked());
	}

}