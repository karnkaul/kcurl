export module kcurl.http;

export import kcurl.easy;

export namespace kcurl::http {
/// \brief Wrapper over a response status code.
class Status {
  public:
	/// \brief Response Status Code.
	// NOLINTNEXTLINE(performance-enum-size)
	enum struct Code : std::int64_t {
		Ok = 200,
	};

	/// \brief Category of Response status code.
	enum class Category : std::int8_t {
		Informational,
		Successful,
		Redirection,
		ClientError,
		ServerError,
		Other,
	};

	Status() = default;

	/// \param code: Pass an easy::Response::code here.
	explicit constexpr Status(std::int64_t const code) : m_code(Code{code}), m_category(get_status_category(m_code)) {}

	[[nodiscard]] constexpr auto get_code() const -> Code { return m_code; }
	[[nodiscard]] constexpr auto get_category() const -> Category { return m_category; }

	[[nodiscard]] constexpr auto is_success() const -> bool { return m_category == Category::Successful; }
	[[nodiscard]] constexpr auto is_error() const -> bool {
		return m_category == Category::ClientError || m_category == Category::ServerError;
	}

	explicit(false) constexpr operator Code() const { return get_code(); }

  private:
	struct CategoryRange {
		static constexpr std::int64_t range_v{100};

		[[nodiscard]] constexpr auto in_range(Status::Code const in) const -> bool {
			return in >= lo && in < Status::Code(std::int64_t(lo) + range_v);
		}

		Status::Category category{};
		Status::Code lo{};
	};

	static constexpr auto category_ranges_v = std::array{
		CategoryRange{.category = Status::Category::Informational, .lo = Status::Code{100}},
		CategoryRange{.category = Status::Category::Successful, .lo = Status::Code{200}},
		CategoryRange{.category = Status::Category::Redirection, .lo = Status::Code{300}},
		CategoryRange{.category = Status::Category::ClientError, .lo = Status::Code{400}},
		CategoryRange{.category = Status::Category::ServerError, .lo = Status::Code{500}},
	};

	static constexpr auto get_status_category(Status::Code const status_code) -> Status::Category {
		for (auto const& range : category_ranges_v) {
			if (range.in_range(status_code)) {
				return range.category;
			}
		}
		return Status::Category::Other;
	}

	Code m_code{};
	Category m_category{};
};

enum class Verb : std::int8_t { Get, Post };

struct Query {
	std::string key{};
	std::string value{};
};

/// \brief Error of a fetch operation.
struct Error {
	/// \param status http Response Status.
	/// \param error_text Desired error text.
	/// \returns Formatted Error.
	[[nodiscard]] static auto from_response(Status status, std::string_view error_text) -> Error;

	CurlCode curl_code{};
	Status status{};
	std::string text{};
};

/// \brief Input parameter for fetch().
struct Request {
	using Flag = easy::Request::Flag;

	/// \brief URL to fetch. Must be a valid URL.
	std::string base_url{};

	/// \brief User agent to use, if any.
	std::string user_agent{};
	/// \brief List of HTTP queries, if any.
	/// Appended to base_url if verb == Verb::Get, else added as post fields.
	std::vector<Query> queries{};
	/// \brief List of HTTP header queries, if any.
	/// Suffix a key with ':' to remove that default header.
	std::vector<Query> headers{};
	/// \brief Request method.
	Verb verb{Verb::Get};
	/// \brief Request flags.
	Flag flags{};
};

/// \brief Successful response of a fetch operation.
/// Response is a class template to enable user-side
/// reuse for custom payloads (eg JSON).
template <typename Type>
struct Response;

template <>
struct Response<void> {
	/// \returns Error with formatted error_text and this response's status.
	[[nodiscard]] auto rewrap_as_error(std::string_view const error_text) const -> Error {
		return Error::from_response(status, error_text);
	}

	Status status{};
};

template <typename Type>
struct Response {
	/// \returns Rewrapped payload with this response's status.
	template <typename T>
	[[nodiscard]] auto rewrap_as(T payload) const -> Response<T> {
		return Response<T>{.payload = std::move(payload), .status = status};
	}

	/// \returns status wrapped in Response<void>.
	[[nodiscard]] auto rewrap_as_void() const -> Response<void> { return Response<void>{.status = status}; }

	/// \returns Error with formatted error_text and this response's status.
	[[nodiscard]] auto rewrap_as_error(std::string_view const error_text) const -> Error {
		return Error::from_response(status, error_text);
	}

	Type payload{};
	Status status{};
};

/// \brief Result of a fetch operation.
/// Result is a class template to enable user-side
/// reuse for custom Reponse payloads (eg JSON).
template <typename Type>
using Result = std::expected<Response<Type>, Error>;

/// \brief Replace special characters to be URL-friendly.
/// \param text Input (unescaped) text.
/// \returns Escaped string.
[[nodiscard]] auto escape(std::string_view text) -> std::string;
/// \brief Replace URL escapes with their source characters.
/// \param escaped Escaped string.
/// \returns Unescaped string.
[[nodiscard]] auto unescape(std::string_view escaped) -> std::string;

/// \param request http Request to convert.
/// \returns Corresponding easy::Request.
[[nodiscard]] auto to_easy_request(Request request) -> easy::Request;

/// \brief Perform easy::Request and interpret the easy::Result as an http::Result.
/// This lower-level function is exposed primarily for users to customize
/// some behavior after the conversion of an http::Request to an easy::Request
/// in the process of fetching the Response.
/// If that is not required, use fetch() directly.
/// \param request easy::Request to perform.
/// \returns http::Response as a ByteArray on success, else http::Error.
[[nodiscard]] auto perform(easy::Request const& request) -> Result<ByteArray>;

/// \brief Primary http API.
/// \param request http::Request to fetch.
/// \returns http::Response as a ByteArray on success, else http::Error.
[[nodiscard]] auto fetch(Request request) -> Result<ByteArray> {
	return http::perform(to_easy_request(std::move(request)));
}
} // namespace kcurl::http
