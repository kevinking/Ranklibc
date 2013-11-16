#include "Util.h"

void splitString(const string& str, vector<string> &tokens, const string& delimiters)
{
    // 找到第一个不是分割符号的字符位置
    string::size_type start = str.find_first_not_of(delimiters);
    // 找到之后的第一个分割符号
    string::size_type end = str.find_first_of(delimiters, start);
    while(string::npos != start || string::npos != end)
    {
        tokens.push_back(str.substr(start, end - start));
        start = str.find_first_not_of(delimiters, end);
        end = str.find_first_of(delimiters, start);
    }
}

