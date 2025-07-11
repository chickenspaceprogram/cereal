#pragma once
#include <variant>
#include <cstdint>
#include <string>
#include <vector>
#include <span>
#include <optional>
#include <type_traits>

#define CEREAL_STRINGIFY(ARG) #ARG
#define cereal_append(PATTERN, NAME)\
	(PATTERN).append(CEREAL_STRINGIFY(NAME), (NAME))

namespace cereal {

class Pattern;

using base_type = std::variant<
#define NUMERIC_TYPE(VAL) VAL,
#define STRING_TYPE(VAL) VAL,
#define BOOLEAN_TYPE(VAL) VAL
#include "types.def" // stole this little trick from Clang, it's evil and i love it
>;

using array_type = std::vector<Pattern>;
using object_type = std::vector<std::pair<std::string, Pattern>>;

using any_type = std::variant<base_type, array_type, object_type>;

template <typename T>
constexpr bool is_base_type() {
#define NUMERIC_TYPE(VAL) std::is_same<std::remove_cvref_t<T>, VAL>::value ||
#define STRING_TYPE(VAL) std::is_same<std::remove_cvref_t<T>, VAL>::value ||
#define BOOLEAN_TYPE(VAL) std::is_same<std::remove_cvref_t<T>, VAL>::value
	return
#include "types.def"
;
}

class Pattern;

template <typename T>
concept Serializable = is_base_type<T>() || requires (const T &t, Pattern &pat) {
	t.serialize(pat);
};

template <typename T>
concept HasSerializeFn = requires (const T &t, Pattern &pat) {
	t.serialize(pat);
};

template <typename F>
concept Functor = requires (F &&f) {
	f();
};

template <typename T>
concept BaseType = is_base_type<T>();


class Pattern {
	public:
	Pattern() : data(object_type()) {}
	template <HasSerializeFn T>
	explicit Pattern(const T &val) : data(object_type()) {
		val.serialize(*this);
	}
	template <BaseType T>
	explicit Pattern(const T &val) : data(val) {}

	template <Serializable T>
	Pattern(const std::string &str, const T &val) : 
		data(object_type(std::pair(str, Pattern(val)))) {}
	template <Serializable T, std::size_t SpanLen>
	explicit Pattern(const std::span<T, SpanLen> &arr) : 
		data(std::vector<Pattern>()) {
		for (const auto &i : arr) {
			std::get<1>(data).emplace_back(Pattern(i));
		}
	}

	template <Serializable T>
	void append(const std::string &str, const T &val) {
		std::get<2>(data).emplace_back(std::pair(str, Pattern(val)));
	}

	template <Serializable T, std::size_t SpanLen>
	void append(const std::string &str, const std::span<T, SpanLen> &arr) {
		std::get<2>(data).emplace_back(std::pair(str, Pattern(arr)));
	}



	const any_type &get_tree() const { return data; }
	private:
	any_type data;
};

std::optional<std::string> serializeJSON(const Pattern &pattern);

}
