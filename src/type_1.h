#if !defined(_TYPE_1_H__INCLUDED__)
#define _TYPE_1_H__INCLUDED__

#include <node.h>
#include <nan.h>

namespace node_sspi_win32
{
	void generate_type1_message(const Nan::FunctionCallbackInfo<v8::Value> &args);
}

#endif // _TYPE_1_H__INCLUDED__