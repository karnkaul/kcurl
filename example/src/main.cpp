import kcurl;

namespace example {
namespace {
namespace fs = std::filesystem;

[[nodiscard]] constexpr auto is_option(std::string_view const arg) -> bool {
	return arg.starts_with('-') || arg.starts_with("--");
}

auto run(int const argc, char const* const* const argv) -> int {
	auto args = std::span{argv, std::size_t(argc)};
	auto const exe_name = [&] {
		if (!args.empty()) {
			auto ret = fs::path{args.front()}.stem().string();
			args = args.subspan(1);
			return ret;
		}
		return std::string{"<exe>"};
	}();

	auto const print_usage = [&exe_name](std::ostream& out) {
		std::println(out, "Usage: {} [URL]", exe_name);
		return &out == &std::cerr ? 1 : 0;
	};

	auto request = kcurl::easy::Request{.url = "https://opensource.org/robots.txt"};
	if (!args.empty()) {
		if (args.size() > 1 || is_option(args.front())) {
			return print_usage(std::cerr);
		}
		request.url = args.front();
	}

	auto const curl = kcurl::Curl{};
	std::println("kcurl {}\nfeatures: {}\n", kcurl::build_version_v, curl.get_features());

	std::println("downloading '{}' ...", request.url);
	auto const result = kcurl::easy::perform(request);

	if (!result) {
		auto const& error = result.error();
		std::println(std::cerr, "error ({}): {}", static_cast<std::int64_t>(error.code), error.text);
		return 1;
	}

	auto const text = result->bytes.as_string_view();
	std::println("success ({})\n{}", result->code, text);

	return 0;
}
} // namespace
} // namespace example

auto main(int argc, char** argv) -> int {
	try {
		return example::run(argc, argv);
	} catch (std::exception const& e) {
		std::println(std::cerr, "PANIC: {}", e.what());
		return 1;
	} catch (...) {
		std::println(std::cerr, "PANIC!");
		return 1;
	}
}
