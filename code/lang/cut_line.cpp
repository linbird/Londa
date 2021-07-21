#include <iostream>
#include <locale>
#include <sstream>

struct csv_whitespace : std::ctype<wchar_t>
{
    bool do_is(mask m, char_type c) const
    {
        if ((m & space) && c == L' ') {
            return false; // 空格将不被分类为空白符
        }
        if ((m & space) && c == L',') {
            return true; // 逗号将被分类为空白符
        }
        return ctype::do_is(m, c); // 将剩下的留给亲类
    }
};

int main()
{
    std::wstring in = L"Column 1,Column 2,Column 3\n123,456,789";
    std::wstring token;

    std::wcout << "default locale:\n";
    std::wistringstream s1(in);
    while (s1 >> token) {
        std::wcout << "  " << token << '\n';
    }

    std::wcout << "locale with modified ctype:\n";
    std::wistringstream s2(in);
    csv_whitespace* my_ws = new csv_whitespace; // 注意：此分配不泄露
    s2.imbue(std::locale(s2.getloc(), my_ws));
    while (s2 >> token) {
        std::wcout << "  " << token<< '\n';
    }
}
