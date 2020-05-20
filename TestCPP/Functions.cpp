#include <string>
#include <vector>
#include <map>

/**
 * \brief Function to clear string from whitespace
 * \param arg string that need to be cleared from empty symbols
 * \return input string without empty symbols on start or at the end
 */
std::string trim(std::string arg)
{
	int start = 0, count = arg.size();
	while (arg[start] == ' ')
		start++;
	while (arg[count] == ' ')
		count--;
	return arg.substr(start, count);
}

/**
 * \brief Function to split string to separate parts when splitter was found
 * \param arg Input string
 * \param splitter Symbol on which string need to be cut
 * \return Vector with string, that occur between splitter symbols
 */
std::vector<std::string> split(std::string arg, const char splitter)
{
	auto start = -1;
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


/**
 * \brief Function to get tag names from entire code
 * \param lines Source code
 * \return Vector with tag names
 */
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

/**
 * \brief Functions that creates map with tags as key, and vector with it's nested tags as value
 * \param lines Source code
 * \return Map of tags and theirs "child"
 */
std::multimap<std::string, std::vector<std::string>> tag_space(std::vector<std::string> lines)
{
	auto i = 0;
	std::multimap<std::string, std::vector<std::string>> res;

	for (const auto& tag : get_tags(lines)) //Iteration through tag names
	{
		i = 0; //Line with tag number
		auto cnt = lines[i].find(' ') != std::string::npos ? lines[i].find(' ') : tag.size() + 2; //If any arguments were find cnt = position of first argument,
																												//else cnt = tag name size + 2 (2 symbols: <>)
		while (lines[i].substr(0, cnt).find(tag) == std::string::npos) //Loop to find line with current tag
		{
			i++;
			cnt = lines[i].find(' ') != std::string::npos ? lines[i].find(' ') : tag.size() + 2;
		}

		std::vector<std::string> v; //Vector of nested tags
		while (lines[i].find("</" + tag + ">") == std::string::npos) //Iteration through lines until current closed tag not find
		{
			if (lines[i][1] != '/') //Push every tag  name to vector of nested tags
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

/**
 * \brief function that check if arg is any of elements in list
 * \param list List of items to search
 * \param arg Value to compare with items
 * \return True if arg was find in items \return False if not
 */
bool is_any(const std::vector<std::string>& list, const std::string& arg)
{
	for (const auto& element : list)
	{
		if (element == arg)
			return true;
	}
	return false;
}

/**
 * \brief Function to check if str starts with arg
 * \param str Input string
 * \param arg Probable suffix
 * \return True if arg is on start of str \return False id not
 */
bool starts_with(const std::string& str, const std::string& arg)
{
	for (auto i = 0; i < arg.size(); i++)
		if (str[i] != arg[i]) return false;
	return true;
}

/**
 * \brief Function to check if tag command is right ordered through nested tags
 * \param map Map with tags and theirs child
 * \param args Vector with tag names
 * \return True if tags is in correct order (Argument path was built correct) \return False if not
 */
bool is_right_order(std::multimap<std::string, std::vector<std::string>>& map, std::vector<std::string> args)
{
	std::vector<std::string> correct;
	correct.push_back(args[0]); //By default, think that first element is correct ordered
	if (args.size() > 1)
	{
		for (auto i = 0; i < args.size() - 1; i++) //Iteration through tags
		{
			const auto& m_elem = map.find(args[i]);
			if (!m_elem->second.empty())
				if (!is_any(m_elem->second, args[i + 1])) return false; //If next tag is not child of current return false
			for (const auto& elem : map)
				if (!is_any(correct, elem.first) && is_any(elem.second, args[i + 1])) return false; //If tag is nested and not in correct collection return false
			correct.push_back(args[i + 1]);
		}
	}
	return true;
}

/**
 * \brief Function that check if args is child
 * \param lines Source code
 * \param args Tag names
 * \return True if tags is in correct order (Argument path was built correct) \return False if not
 */
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