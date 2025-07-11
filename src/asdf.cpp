#include <cereal/cereal.hpp>
#include <iostream>
#include <array>

struct smaller {
	double sub_double = 123.456;
	bool is_annoying = true;
	std::string annoying{"this is a string with \n\nannoying\v\tchars\u1234"};
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
		return obj;
	}
	int foo = 3;
	int bar = 314;
	float baz = 2324.234;
	struct smaller asdf;
};

int main(void) {
	std::array<int, 5> ints = {11, 22, 33, 44, 55};
	std::array<object_Object, 10000> big;
	object_Object garbage;
	cereal::object_type obj;
	obj.append("ints", std::span(ints));
	obj.append("big", std::span(big));
	cereal_append(obj, garbage);
	std::string res = *cereal::serializeJSON(cereal::any_type(obj));
}
