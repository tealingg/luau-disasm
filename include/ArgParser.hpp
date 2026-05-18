#include <string>
#include <vector>

class ArgParser {
private:
    std::vector<std::string> arguments;
public:
    ArgParser(int argc, char *argv[]);
    bool doesArgExist(const std::string& argument);
    const std::string& getArgValue(const std::string& argument);
};