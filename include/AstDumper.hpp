#include <string>

class AstDumper {
   private:
    std::string inputString;
    std::string astJson;

   public:
    AstDumper(const std::string& input) : inputString(input) {}
    bool dump();
    const std::string& getAstJson();
};
