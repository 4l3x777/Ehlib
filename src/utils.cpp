#include "utils.h"

#include <random>
#include <algorithm>

std::string generate_random_boundary()
{
	std::string boundary_numbers = "0123456789";
	thread_local std::default_random_engine random_generator(std::random_device{}());
	thread_local auto distributor = std::uniform_int_distribution{{}, boundary_numbers.size() - 1};
	auto result = std::string(28, 0x0);
	std::generate_n(
		result.begin(),
		28,
		[&]()
		{
			return boundary_numbers[distributor(random_generator)];
		}
	);
	return result;
}

std::string hex_encode(char const c)
{
	char s[3];
	if (c & 0x80) snprintf(&s[0], 3, "%02X", static_cast<unsigned int>(c & 0xff));
	else snprintf(&s[0], 3, "%02X", static_cast<unsigned int>(c));
	return std::string(s);
}

std::string url_encode(const std::string& str)
{
	std::string res;
	res.reserve(str.size());
	bool form = true;
	for (auto const& e : str)
	{
		if (e == ' ' && form) res += '+';
		else if (
            isalnum(static_cast<unsigned char>(e)) ||
			e == '-' ||
            e == '_' ||
            e == '.' ||
            e == '~'
        ) res += e;
		else res += '%' + hex_encode(e);
	}
	return res;
}

std::string base64_encode(const std::string& in)
{
	std::string out;
	std::string Alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	int val = 0, valb = -6;
	for (unsigned char c : in)
    {
		val = (val << 8) + c;
		valb += 8;
		while (valb >= 0)
        {
			out.push_back(Alphabet[(val >> valb) & 0x3F]);
			valb -= 6;
		}
	}
	if (valb > -6) out.push_back(Alphabet[((val << 8) >> (valb + 8)) & 0x3F]);
	while (out.size() % 4) out.push_back('=');
	return out;
}