module kcurl.curl;

namespace kcurl {
auto Curl::default_flags() -> long { return {}; }

Curl::Curl(long const /*flags*/) {}

Curl::~Curl() = default; // NOLINT(performance-trivially-destructible)

auto Curl::get_features() const -> Feature { return {}; } // NOLINT(readability-convert-member-functions-to-static)
} // namespace kcurl
