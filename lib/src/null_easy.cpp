module kcurl.easy;

namespace kcurl {
auto easy::perform(Request const& /*request*/) -> Result {
	return std::unexpected{Error{.code = CurlCode{-1}, .text = "kcurl not built with curl enabled"}};
}
} // namespace kcurl
