#include "shared.h"

namespace node_sspi_win32
{

	sspi_module_rec sspi_module{ 0, };

	// frees allocated buffers with NewBuffer - avoid exceptions during process shutdown
	void buffer_free(char *data, void *hint)
	{
		free(data);
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
		case SEC_E_BUFFER_TOO_SMALL:
			return "SEC_E_BUFFER_TOO_SMALL: The message buffer is too small. Used with the Digest SSP.";
		case SEC_E_CRYPTO_SYSTEM_INVALID:
			return "SEC_E_CRYPTO_SYSTEM_INVALID: The cipher chosen for the security context is not supported. Used with the Digest SSP.";
		case SEC_E_MESSAGE_ALTERED:
			return "SEC_E_MESSAGE_ALTERED: The message has been altered. Used with the Digest and Schannel SSPs.";
		case SEC_E_OUT_OF_SEQUENCE:
			return "SEC_E_OUT_OF_SEQUENCE: The message was not received in the correct sequence.";
		case SEC_E_QOP_NOT_SUPPORTED:
			return "SEC_E_QOP_NOT_SUPPORTED: Neither confidentiality nor integrity are supported by the security context. Used with the Digest SSP.";
		case SEC_I_CONTEXT_EXPIRED:
			return "SEC_I_CONTEXT_EXPIRED: The message sender has finished using the connection and has initiated a shutdown.For information about initiating or recognizing a shutdown, see Shutting Down an Schannel Connection.Used with the Schannel SSP.";
		case SEC_I_RENEGOTIATE:
			return "SEC_I_RENEGOTIATE: The remote party requires a new handshake sequence or the application has just initiated a shutdown.Return to the negotiation loop and call AcceptSecurityContext(General) or InitializeSecurityContext(General), passing empty input buffers.";
		default:
			return "Unknown Error";
		}
	}
} // namespace node_sspi_win32