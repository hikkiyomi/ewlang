#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

std::vector<std::string> Split(const std::string& str)
{
    std::vector<std::string> result;

    for (auto c : str) {
        if (c == ' ') {
            result.push_back("");
        } else {
            if (result.empty()) {
                result.push_back("");
            }

            result.back().push_back(c);
        }
    }

    return result;
}

std::vector<std::string> Merge(const std::string& filename)
{
    std::ifstream input(filename);
    std::string str;
    std::vector<std::string> result;

    while (getline(input, str)) {
        std::vector<std::string> splitted = Split(str);

        if (splitted.size() == 2 && splitted[0] == "import") {
            std::string withExtension = splitted[1] + ".ew";

            if (!std::filesystem::exists(withExtension)) {
                throw std::runtime_error("module " + splitted[1] + " does not exist");
            }

            std::vector<std::string> temp = Merge(withExtension);
            result.insert(result.end(), std::make_move_iterator(temp.begin()), std::make_move_iterator(temp.end()));

            continue;
        }

        result.push_back(str);
    }

    return result;
}

bool CheckExtension(const std::string& filename)
{
    return std::filesystem::path(filename).extension() == ".ew";
}

void ProcessImports(const std::string& filename)
{
    if (!CheckExtension(filename)) {
        throw std::runtime_error("wrong extension, should be .ew");
    }

    const std::string& outputFile = filename + "_processed";
    std::ofstream output(outputFile);

    for (const auto& line : Merge(filename)) {
        output << line << "\n";
    }
}
