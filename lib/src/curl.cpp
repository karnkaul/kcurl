module;

#include <curl/curl.h>

module kcurl.curl;

namespace kcurl {
auto Curl::default_flags() -> long { return long{CURL_GLOBAL_DEFAULT}; }

Curl::Curl(long const flags) { curl_global_init(flags); }

Curl::~Curl() { curl_global_cleanup(); }

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto Curl::get_features() const -> Feature {
	auto ret = Feature{};
	auto const* version_info = curl_version_info(CURLVERSION_NOW);
	auto const set_feature_flag = [&](int const in, Feature const flag) {
		if ((version_info->features & in) == in) {
			ret = Feature(ret | flag);
		}
	};
	set_feature_flag(CURL_VERSION_SSL, TLS);
	set_feature_flag(CURL_VERSION_IPV6, IPv6);
	set_feature_flag(CURL_VERSION_UNICODE, Win32Unicode);
	set_feature_flag(CURL_VERSION_UNIX_SOCKETS, UnixSockets);
	set_feature_flag(CURL_VERSION_HTTP2, Http2);
	set_feature_flag(CURL_VERSION_HTTP3, Http3);
	set_feature_flag(CURL_VERSION_LARGEFILE, LargeFile);
	return ret;
}
} // namespace kcurl
