#pragma once
#include <variant>
#include <cstdint>
#include <string>
#include <vector>
#include <span>
#include <cassert>
#include <optional>
#include <type_traits>

#define CEREAL_STRINGIFY(ARG) #ARG
#define cereal_append(PATTERN, NAME)\
	(PATTERN).append(CEREAL_STRINGIFY(NAME), (NAME))

namespace cereal {

using base_type = std::variant<
#define NUMERIC_TYPE(VAL) VAL,
#define STRING_TYPE(VAL) VAL,
#define BOOLEAN_TYPE(VAL) VAL
#include "types.def" // stole this little trick from Clang, it's evil and i love it
>;

struct object_type;
struct array_type;
struct any_type;

template <typename T>
constexpr bool is_base_type() {
#define NUMERIC_TYPE(VAL) std::is_same<std::remove_cvref_t<T>, VAL>::value ||
#define STRING_TYPE(VAL) std::is_same<std::remove_cvref_t<T>, VAL>::value ||
#define BOOLEAN_TYPE(VAL) std::is_same<std::remove_cvref_t<T>, VAL>::value
	return
#include "types.def"
;
}

template <typename T>
concept Serializable = is_base_type<T>() || requires (const T &t) {
	{t.serialize()} -> std::same_as<object_type>;
};

// Type signature for serialize for some arbitrary type T:
//
// object_type T::serialize() const;
//

template <typename T>
concept HasSerializeFn = requires (const T &t) {
	{t.serialize()} -> std::same_as<object_type>;
};

template <typename T>
concept BaseType = is_base_type<T>();

struct array_type {
	array_type() = delete;

	template <Serializable T, size_t Len>
	explicit array_type(const std::span<T, Len> &arr);

	std::vector<any_type> val;
};


// This is the type you're actually supposed to use.
//
// Just default-construct one of these, and call append on it a bunch of times
// to fill it with fields.
//
// So long as all the types you add are base types (integers, floats, strings,
// bools), or types with a .serialize() method, it'll all work magically.
//
// Any strings you pass this should either be statically allocated
// (pass in string literals) or they should be valid for the entire time the
// `object_type` exists.
struct object_type {
	template <Serializable T>
	void append(const std::string_view &name, const T &val);

	template <Serializable T, size_t Len>
	void append(const std::string_view &name, const std::span<T, Len> &arr);

	std::vector<std::pair<std::string_view, any_type>> val;
};

// You can also use this type if you don't want to serialize a proper object;
// maybe you just want to serialize a single value or an array.
//
// A ctor is provided to effectively "typecast" an object_type to this type;
// it's recommended you make serializers take an any_type.
struct any_type {
	any_type() = delete;
	template <BaseType T>
	explicit any_type(const T &val) : val(base_type(val)) {}

	template <HasSerializeFn T>
	explicit any_type(const T &val) : val(val.serialize()) {}

	explicit any_type(const object_type &val) : val(val) {}
	explicit any_type(const array_type &val) : val(val) {}
	explicit any_type(const base_type &val) : val(val) {}

	template <Serializable T, size_t Len>
	explicit any_type(const std::span<T, Len> &arr) : val(array_type(arr)) {}


	std::variant<base_type, array_type, object_type> val;
};

template <Serializable T, size_t Len>
array_type::array_type(const std::span<T, Len> &arr) {
	val.reserve(arr.size());
	for (const auto &elem : arr) {
		val.push_back(any_type(elem));
	}
}

template <Serializable T>
void object_type::append(const std::string_view &name, const T &arg) {
	val.push_back(std::pair<std::string_view, any_type>(name, any_type(arg)));
}

template <Serializable T, size_t Len>
void object_type::append(const std::string_view &name, const std::span<T, Len> &arr) {
	val.push_back(std::pair<std::string_view, any_type>(name, any_type(arr)));
}

std::optional<std::string> serializeJSON(const any_type &parent);

}
