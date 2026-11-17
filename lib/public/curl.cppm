export module kcurl.curl;

export import std;

export namespace kcurl {
/// \brief Top-level RAII wrapper for curl.
/// Use if libcurl initialization/shutdown is not already handled.
class Curl {
  public:
	enum Feature : std::uint8_t { // NOLINT(cppcoreguidelines-use-enum-class)
		None = 0,
		TLS = 1 << 0,
		IPv6 = 1 << 1,
		Win32Unicode = 1 << 2,
		UnixSockets = 1 << 3,
		Http2 = 1 << 4,
		Http3 = 1 << 5,
		LargeFile = 1 << 6,
	};

	Curl(Curl const&) = delete;
	Curl(Curl&&) = delete;
	Curl& operator=(Curl const&) = delete;
	Curl& operator=(Curl&&) = delete;

	[[nodiscard]] static auto default_flags() -> long;

	explicit Curl(long flags = default_flags());
	~Curl();

	[[nodiscard]] auto get_features() const -> Feature;
};
} // namespace kcurl

template <>
struct std::formatter<kcurl::Curl::Feature> {
	template <class ParseContext>
	constexpr auto parse(ParseContext& ctx) {
		return ctx.begin();
	}

	static auto format(kcurl::Curl::Feature const flags, format_context& fc) -> format_context::iterator {
		auto first = true;
		auto const append = [&](std::string_view const text) {
			if (!first) {
				format_to(fc.out(), "|{}", text);
			} else {
				format_to(fc.out(), "{}", text);
			}
			first = false;
		};

		using Feature = kcurl::Curl::Feature;
		if ((flags & Feature::TLS) == Feature::TLS) {
			append("TLS");
		}
		if ((flags & Feature::IPv6) == Feature::IPv6) {
			append("IPv6");
		}
		if ((flags & Feature::Win32Unicode) == Feature::Win32Unicode) {
			append("Win32Unicode");
		}
		if ((flags & Feature::UnixSockets) == Feature::UnixSockets) {
			append("UnixSockets");
		}
		if ((flags & Feature::Http2) == Feature::Http2) {
			append("HTTP2");
		}
		if ((flags & Feature::Http3) == Feature::Http3) {
			append("HTTP3");
		}
		if ((flags & Feature::LargeFile) == Feature::LargeFile) {
			append("LargeFile");
		}
		return fc.out();
	}
};
