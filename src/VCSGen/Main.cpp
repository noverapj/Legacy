#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <array>
#include <stdexcept>

namespace fs = std::filesystem;

static std::string Exec(const std::string& command)
{
    std::array<char, 4096> buffer{};
    std::string result;

#ifdef _WIN32
    FILE* pipe = _popen(command.c_str(), "r");
#else
    FILE* pipe = popen(command.c_str(), "r");
#endif

    if (!pipe)
        return {};

    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe))
        result += buffer.data();

#ifdef _WIN32
    _pclose(pipe);
#else
    pclose(pipe);
#endif

    while (!result.empty() &&
        (result.back() == '\r' || result.back() == '\n'))
    {
        result.pop_back();
    }

    return result;
}

static std::string Quote(const std::string& value)
{
#ifdef _WIN32
    if (!value.empty() && value.back() == '\\')
        return "\"" + value + "\\" + "\"";
    return "\"" + value + "\"";
#else
    return "'" + value + "'";
#endif
}

static std::string GetRevision(const fs::path& repo)
{
    const std::string command =
        "git -C " + Quote(repo.string()) +
        " rev-list --count HEAD";

    const std::string revision = Exec(command);

    if (revision.empty())
    {
        throw std::runtime_error(
            "Unable to get Git revision."
        );
    }

    return revision;
}

static std::string GetDate(const fs::path& repo)
{
    const std::string command =
        "git -C " + Quote(repo.string()) +
        " log -1 --format=%cd " +
        Quote("--date=format:%Y/%m/%d %H:%M:%S");

    const std::string date = Exec(command);

    if (date.empty())
    {
        throw std::runtime_error(
            "Unable to get Git commit date."
        );
    }

    return date;
}

static void ReplaceAll(
    std::string& text,
    const std::string& from,
    const std::string& to)
{
    if (from.empty())
        return;

    size_t pos = 0;

    while ((pos = text.find(from, pos)) != std::string::npos)
    {
        text.replace(pos, from.length(), to);
        pos += to.length();
    }
}

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 4)
        {
            std::cerr
                << "Usage: VersionGen.exe <repo> <template> <output>\n";

            return 1;
        }

        const fs::path repo = fs::absolute(argv[1]);
        const fs::path templateFile = fs::absolute(argv[2]);
        const fs::path outputFile = fs::absolute(argv[3]);

        if (!fs::exists(templateFile))
        {
            throw std::runtime_error(
                "Template file does not exist: " +
                templateFile.string()
            );
        }

        std::ifstream input(templateFile, std::ios::binary);

        if (!input)
        {
            throw std::runtime_error(
                "Unable to open template: " +
                templateFile.string()
            );
        }

        std::stringstream buffer;
        buffer << input.rdbuf();

        std::string content = buffer.str();

        const std::string revision = GetRevision(repo);
        const std::string date = GetDate(repo);

        ReplaceAll(content, "$WCREV$", revision);
        ReplaceAll(content, "$WCDATE$", date);

        std::ofstream output(
            outputFile,
            std::ios::binary | std::ios::trunc
        );

        if (!output)
        {
            throw std::runtime_error(
                "Unable to create output: " +
                outputFile.string()
            );
        }

        output.write(
            content.data(),
            static_cast<std::streamsize>(content.size())
        );

        if (!output)
        {
            throw std::runtime_error(
                "Failed writing output: " +
                outputFile.string()
            );
        }

        std::cout
            << "Revision: " << revision << '\n'
            << "Date: " << date << '\n'
            << "Output: " << outputFile << '\n';

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "VersionGen error: " << e.what() << '\n';
        return 1;
    }
}