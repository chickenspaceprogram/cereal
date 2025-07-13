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

int serialize_any_type(std::string &outstr, const any_type &arr);

int serialize_array_type(std::string &outstr, const array_type &arr) {
	outstr += '[';
	for (const auto &elem : arr.val) {
		if (serialize_any_type(outstr, elem) != 0) {
			return -1;
		}
		outstr += ',';
	}
	outstr.back() = ']';
	return 0;
}

int serialize_object_type(std::string &outstr, const object_type &obj) {
	outstr += '{';
	for (const auto &[name, fields] : obj.val) {
		if (serialize_base_type(outstr, name) != 0)
			return -1;
		outstr += ':';
		if (serialize_any_type(outstr, fields) != 0)
			return -1;
		outstr += ',';
	}
	outstr.back() = '}';
	return 0;
}

int serialize_any_type(std::string &outstr, const any_type &val) {
	size_t index = val.val.index();
	if (index == 0) {
		auto lambda = [&outstr](const auto &asdf) -> int {return serialize_base_type(outstr, asdf);};
		return std::visit(lambda, std::get<0>(val.val));
	}
	else if (index == 1) {
		return serialize_array_type(outstr, std::get<1>(val.val));
	}
	else {
		return serialize_object_type(outstr, std::get<2>(val.val));
	}
}

}

std::optional<std::string> serializeJSON(const object_type &parent) {
	std::optional<std::string> out(std::string{});
	if (serialize_object_type(*out, parent) != 0) {
		out.reset();
	}
	return out;
}

std::optional<std::string> serializeJSON(const any_type &parent) {
	
	std::optional<std::string> out(std::string{});
	if (serialize_any_type(*out, parent) != 0) {
		out.reset();
	}
	return out;
}

}

