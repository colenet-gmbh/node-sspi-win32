#if !defined(_TYPE_3_H__INCLUDED__)
#define _TYPE_3_H__INCLUDED__

#include <node.h>
#include <nan.h>

namespace node_sspi_win32
{
	void generate_type3_message(const Nan::FunctionCallbackInfo<v8::Value>& info);
}

#endif // _TYPE_3_H__INCLUDED__