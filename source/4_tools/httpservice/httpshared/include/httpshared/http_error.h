#pragma once

namespace http
{
  enum class Error {
    Success = 0,
    Unknown,
    Connection,
    BindIPAddress,
    Read,
    Write,
    ExceedRedirectCount,
    Canceled,
    SSLConnection,
    SSLLoadingCerts,
    SSLServerVerification,
    SSLServerHostnameVerification,
    UnsupportedMultipartBoundaryChars,
    Compression,
    ConnectionTimeout,
    ProxyConnection,

    // For internal use only
    SSLPeerCouldBeClosed_,
  };

  std::string to_string(Error error);
}