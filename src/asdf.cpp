#include <cereal/cereal.hpp>
#include <iostream>
#include <array>

struct smaller {
	double sub_double = 123.456;
	bool is_annoying = true;
	std::string_view annoying{"this is a string with \n\nannoying\v\tchars\u1234"};
	cereal::object_type serialize() const {
		cereal::object_type obj;
		cereal_append(obj, sub_double);
		cereal_append(obj, is_annoying);
		cereal_append(obj, annoying);
		return obj;
	}
};

struct object_Object {
	cereal::object_type serialize() const {
		cereal::object_type obj;
		cereal_append(obj, foo);
		cereal_append(obj, bar);
		cereal_append(obj, baz);
		cereal_append(obj, asdf);
		obj.append("arr", std::span(arr));
		return obj;
	}
	int foo = 3;
	int bar = 314;
	float baz = 2324.234;
	struct smaller asdf;
	std::array<uint64_t, 10> arr = {11, 22, 33, 44, 55, 66, 77, 88, 99, 100};
};

int main(void) {
	std::array<object_Object, 10000> object;
	cereal::object_type val;
	val.append("object_thingy", std::span(object));
	std::string res = *cereal::serializeJSON(cereal::any_type(val));

}
