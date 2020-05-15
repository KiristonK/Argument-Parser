#include <iostream>
#include <regex>
#include "Functions.cpp"
using namespace std;

string find_arg(const std::vector<string>& lines, const std::vector<string>& tags)
{
	if (!is_child(lines, tags)) return "Not Found!";

	const auto& tag = tags.back();
	if (tag.find('~') != std::string::npos)
	{
		const auto tag_tilde = tag.find('~');
		const auto tag_val = tag.substr(tag_tilde + 1);
		const auto tag_sym = tag.substr(0, tag_tilde);
		for (const auto& line : lines)
		{
			if (starts_with(line, "<" + tag_sym))
			{
				auto line_copy = line.substr(line.find(' ') + 1);
				const auto arg_pos = line_copy.find(tag_val);
				if (arg_pos == std::string::npos) return "Not Found!";
				if (arg_pos != 0)
					if (line_copy[arg_pos - 1] != ' ')return "Not Found!";

				line_copy = line_copy.substr(arg_pos);
				auto arg_val = line_copy.substr(line_copy.find('"') + 1);
				arg_val = arg_val.substr(0, arg_val.find('"'));
				return arg_val;
			}
		}
	}

	return "Not Found!";
}


int main()
{
	int n, q;
	std::vector<string> lines;
	std::vector<string> queries;
	std::cin >> n >> q;
	for (auto i = 0; i <= n; i++)
	{
		string line;
		std::getline(std::cin, line);
		if (!line.empty())
			lines.push_back(trim(line));
	}
	for (auto i = 0; i < q; i++)
	{
		string query;
		std::getline(std::cin, query);
		if (!query.empty())
			queries.push_back(trim(query));
	}

	for (const auto& elem : queries)
	{
		auto r = split(elem, '.');
		cout << trim(find_arg(lines, r)) << endl;
	}
	return 0;
}
