#include <cereal/cereal.hpp>
#include <sstream>
#include <cmath>
#include <iostream>

namespace cereal {

namespace {

template <BaseType T>
int serialize_base_type(std::string &outstr, const T &val) {
	outstr += std::to_string(val);
	return 0;
}

template <>
int serialize_base_type<std::string_view>(std::string &outstr, const std::string_view &str) {
	outstr += '\"';
	for (auto chr : str) {
		if (chr == '\\') {
			outstr += "\\\\";
		}
		else if (chr == '\"') {
			outstr += "\\\"";
		}
		else if (chr <= 0x1f) {
			if (chr <= 0x09) {
				outstr += "\\u000";
				outstr += '0' + chr;
			}
			else if (chr < 0x10) {
				outstr += "\\u000";
				outstr += 'A' + chr - 0x0A;
			}
			else if (chr <= 0x19) {
				outstr += "\\u001";
				outstr += '0' + chr;
			}
			else {
				outstr += "\\u001";
				outstr += 'A' + chr - 0x0A;
			}
		}
		else {
			outstr += chr;
		}
	}
	outstr += '\"';
	return 0;
}


template <>
int serialize_base_type<std::string>(std::string &outstr, const std::string &str) {
	return serialize_base_type(outstr, std::string_view(str));
}

template <>
int serialize_base_type<char *>(std::string &outstr, char * const&str) {
	return serialize_base_type(outstr, std::string_view(str));
}


template <>
int serialize_base_type<bool>(std::string &outstr, const bool &val) {
	if (val) {
		outstr += "true";
	}
	else {
		outstr += "false";
	}
	return 0;
}

template <>
int serialize_base_type<float>(std::string &outstr, const float &val) {
	if (std::isnan(val) || std::isinf(val)) {
		return -1;
	}
	outstr += std::to_string(val);
	return 0;
}

template <>
int serialize_base_type<double>(std::string &outstr, const double &val) {
	if (std::isnan(val) || std::isinf(val)) {
		return -1;
	}
	outstr += std::to_string(val);
	return 0;
}

int serialize_complex_type(std::string &outstr, const any_type &val) {
	size_t index = val.index();
	if (index == 0) {
		auto lambda =  [&outstr](const auto &asdf) -> int {return serialize_base_type(outstr, asdf);};
		return std::visit(lambda, std::get<0>(val));
	}
	else if (index == 1) {
		const auto &res = std::get<1>(val);
		outstr += '[';
		for (const auto &elem : res) {
			if (serialize_complex_type(outstr, elem.get_tree()) != 0) {
				return -1;
			}
			outstr += ',';
		}
		outstr.back() = ']';
	}
	else {
		const auto &res = std::get<2>(val);
		outstr += '{';
		for (const auto &[name, fields] : res) {
			if (serialize_base_type(outstr, name) != 0)
				return -1;
			outstr += ':';
			if (serialize_complex_type(outstr, fields.get_tree()) != 0)
				return -1;
			outstr += ',';
		}
		outstr.back() = '}';
	}
	return 0;
}

}

std::optional<std::string> serializeJSON(const Pattern &pattern) {
	const any_type &type = pattern.get_tree();
	std::optional<std::string> out(std::string{});
	if (serialize_complex_type(*out, type) != 0) {
		out.reset();
	}
	return out;
}

}

