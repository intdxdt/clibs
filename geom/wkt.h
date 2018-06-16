//
//16/06/18.
//

#include <regex>
#include <unordered_map>
#include "util.h"

#ifndef GEOM_WKT_H
#define GEOM_WKT_H
namespace geom {

    std::regex re_typeStr(R"(\s*(\w+)\s*\(\s*(.*)\s*\)\s*)",
                          std::regex_constants::ECMAScript | std::regex_constants::icase);

    std::regex re_emptyTypeStr(R"(\s*(\w+)\s*EMPTY\s*)",
                               std::regex_constants::ECMAScript | std::regex_constants::icase);

    std::regex re_spaces(R"(\s+)",
                         std::regex_constants::ECMAScript | std::regex_constants::icase);

    std::regex re_parenComma(R"(\)\s*,\s*\()",
                             std::regex_constants::ECMAScript | std::regex_constants::icase);

    std::regex re_doubleParenComma(R"(\)\s*\)\s*,\s*\(\s*\()",
                                   std::regex_constants::ECMAScript | std::regex_constants::icase);

    std::regex re_trimParens(R"(\s*\(?(.*?)\)?\s*)",
                             std::regex_constants::ECMAScript | std::regex_constants::icase);

    std::vector<std::string> FindStringSubmatch(const std::regex&, const std::string&);

    bool is_empty_wkt(const std::string&);


//wkt type and coordiantes
    std::unordered_map<std::string, std::string> wkt_type_const(const std::string& input_wkt) {
        std::regex wktreg;
        auto wkt = trim_copy(input_wkt);
        std::unordered_map<std::string, std::string> captures{};
        captures["wkt"] = "";
        captures["type"] = "";
        captures["coords"] = "";


        if (is_empty_wkt(wkt)) {
            wktreg = re_emptyTypeStr;
        }
        auto match = FindStringSubmatch(wktreg, wkt);
        if (!match.empty()) {
            for (size_t i = 0; i < match.size(); ++i) {
                auto name = match[i];
                if (i == 0 || name.empty()) {
                    if (i == 0) {
                        captures["wkt"] = match[i];
                    }
                    continue;
                }
                auto val = match[i];
                if (name == "type") {
                    val = str_tolower(val);
                }
                captures[name] = val;
            }
        }
        return captures;
    }


    std::vector<std::string> FindStringSubmatch(const std::regex& wktreg, const std::string& wkt) {
        std::vector<std::string> tokens;
        std::smatch matches;
        if (std::regex_match(wkt, matches, wktreg)) {
            for (const auto& m : matches) {
                tokens.emplace_back(m.str());
            }
        }
        return std::move(tokens);
    }

    ///checks for the emptines of wkt string
    bool is_empty_wkt(const std::string& wkt) {
        return wkt.find("EMPTY") != std::string::npos;
    }
}
#endif //GEOM_WKT_H
