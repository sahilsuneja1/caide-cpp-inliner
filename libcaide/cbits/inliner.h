#include <vector>
#include <string>
#include <set>

class Inliner {
public:
    Inliner(const std::vector<std::string>& systemHeadersDirectories,
            const std::vector<std::string>& userHeadersDirectories);

    std::string doInline(const std::string& cppFile);

private:
    std::vector<std::string> systemHeadersDirectories, userHeadersDirectories;
    std::set<std::string> includedHeaders;
    std::vector<std::string> inlineResults;
};
