#include <string>
#include <vector>
#include <map>

std::string trim(std::string arg)
{
	int start = 0, count = arg.size();
	while (arg[start] == ' ')
		start++;
	while (arg[count] == ' ')
		count--;
	return arg.substr(start, count);
}

std::vector<std::string> split(std::string arg, const char splitter, int start = -1)
{
	std::vector<std::string> res;
	for (auto i = 0; i < arg.size(); i++)
	{
		if (arg[i] == splitter)
		{
			res.push_back(arg.substr(start + 1, (i - start) - 1));
			start = i;
		}
	}
	arg = arg.substr(start + 1);
	res.push_back(arg);
	return res;
}


std::vector<std::string> get_tags(std::vector<std::string> &lines)
{
	std::vector<std::string> tags;
	for (const auto& line : lines)
	{
		if (line[0] == '<' && line[1] != '/')
		{
			const auto cnt = line.find(' ') == std::string::npos ? line.size() - 1 : line.find(' ');
			tags.push_back(line.substr(1, cnt - 1));
		}
	}
	return tags;
}

std::multimap<std::string, std::vector<std::string>> tag_space(std::vector<std::string> lines, int i = 0)
{
	std::multimap<std::string, std::vector<std::string>> res;

	for (const auto& tag : get_tags(lines))
	{
		i = 0;
		auto cnt = lines[i].find(' ') != std::string::npos ? lines[i].find(' ') : tag.size() + 2;
		while (lines[i].substr(0, cnt).find(tag) == std::string::npos)
		{
			i++;
			cnt = lines[i].find(' ') != std::string::npos ? lines[i].find(' ') : tag.size() + 2;
		}

		std::vector<std::string> v;
		while (lines[i].find("</" + tag + ">") == std::string::npos)
		{
			if (lines[i][1] != '/')
			{
				cnt = tag.size();
				if (lines[i].find(' ') != std::string::npos)
					cnt = lines[i].find(' ') - 1;
				auto tmp = lines[i].substr(1, cnt);
				if (tmp != tag)
					v.push_back(tmp);
			}
			i++;
		}
		res.insert(std::make_pair(tag, v));
	}
	return res;
}

bool is_any(const std::vector<std::string>& list, const std::string& arg)
{
	for (auto& element : list)
	{
		if (element == arg)
			return true;
	}
	return false;
}

bool starts_with(const std::string& str, const std::string& arg)
{
	for (auto i = 0; i < arg.size(); i++)
		if (str[i] != arg[i]) return false;
	return true;
}

bool is_right_order(std::multimap<std::string, std::vector<std::string>>& map, std::vector<std::string> args)
{
	std::vector<std::string> correct;
	correct.push_back(args[0]);
	if (args.size() > 1)
	{
		for (auto i = 0; i < args.size() - 1; i++)
		{
			const auto& m_elem = map.find(args[i]);
			if (!m_elem->second.empty())
				if (!is_any(m_elem->second, args[i + 1])) return false;
			for (const auto& elem : map)
				if (!is_any(correct, elem.first) && is_any(elem.second, args[i + 1])) return false;
			correct.push_back(args[i + 1]);
		}
	}
	return true;
}

bool is_child(const std::vector<std::string>& lines, std::vector<std::string> args)
{
	auto map = tag_space(lines);
	const auto l_arg = args.back().substr(0, args.back().find('~'));
	args.pop_back();
	args.push_back(l_arg);

	if (args.size() > 1)
	{
		for (auto i = 0; i < args.size() - 1; i++)
		{
			const auto& el = map.find(args[i]);
			if (el == map.end()) return "Not Found!";
			if (!is_any(el->second, args[i + 1])) { return false; }
		}
	}
	else
		for (const auto& m_elem : map)
			if (m_elem.first != l_arg && is_any(m_elem.second, l_arg)) return false;

	return is_right_order(map, args);
}