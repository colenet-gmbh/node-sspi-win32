#include <node.h>
#include <strstream>
#include <iostream>
#include <sstream>
#include <stack>
#include <cvt/wstring>
#include <cvt/utf16>
#include <windows.h>
#include <security.h>
#include <strsafe.h>


namespace node_sspi_win32
{
	using v8::FunctionCallbackInfo;
	using v8::Isolate;
	using v8::Local;
	using v8::Object;
	using v8::String;
	using v8::Value;



	// Call upon shutting down node.js
	void exit(void*)
	{
	}

	void init(Local<Object> exports)
	{
		node::AtExit(exit);
		//NODE_SET_METHOD(exports, "get_service_object", get_service_object::get_service_object);
		//NODE_SET_METHOD(exports, "put_service_object", put_service_object::put_service_object);
		//NODE_SET_METHOD(exports, "proxy_for_url", proxy::proxy_for_url);
		//NODE_SET_METHOD(exports, "pac_url", proxy::pac_url);
		//NODE_SET_METHOD(exports, "machine_name", dpn::machine_name);
	}


	NODE_MODULE(addon, init)

}  // namespace node_amisxml
