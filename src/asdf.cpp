#include <cereal/cereal.hpp>
#include <iostream>
#include <array>

struct smaller {
	double sub_double = 123.456;
	bool is_annoying = true;
	void serialize(cereal::Pattern &pat) const {
		cereal_append(pat, sub_double);
		cereal_append(pat, is_annoying);
	}
};

struct object_Object {
	void serialize(cereal::Pattern &pat) const {
		cereal_append(pat, foo);
		cereal_append(pat, bar);
		cereal_append(pat, baz);
		cereal_append(pat, asdf);
	}
	int foo = 3;
	int bar = 314;
	float baz = 2324.234;
	struct smaller asdf;
};

int main(void) {
	std::array<int, 5> ints = {11, 22, 33, 44, 55};
	object_Object garbage;
	cereal::Pattern pat;
	pat.append("ints", std::span(ints));
	cereal_append(pat, garbage);
	std::string res = *serializeJSON(pat);
	std::cout << res;
}
