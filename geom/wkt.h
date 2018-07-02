
#include <regex>
#include <unordered_map>
#include "util.h"

#ifndef GEOM_WKT_H
#define GEOM_WKT_H
namespace geom {

    using Shell = std::vector<std::vector<float>>;
    using Holes = std::vector<std::shared_ptr<Shell>>;
    struct WKTParserObj {
        std::shared_ptr<Shell> shell;
        std::shared_ptr<Holes> holes;
        GeoType gtype;
    };

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
std::unordered_map<std::string, std::string> wkt_type_coords(const std::regex& wktreg,  const std::string& input_wkt) ;
    //wkt string
    std::string wkt_string(std::string& wkt ) {
        std::ostringstream  buffer;
        auto tokens = split_str(wkt, "\n");
        for (const auto& token : tokens) {
            buffer << trim_copy(token).c_str();
        }
        return buffer.str();
    }
//Read wkt string
    const WKTParserObj& ReadWKT(std::string wkt) const {
        wkt = wkt_string(wkt);
        std::function<double(const std::string&, const WKTParserObj&)>  parser;
        auto matches = wkt_type_coords(re_typeStr, wkt);
        auto obj = WKTParserObj{nullptr, nullptr, GeoType::UnkownType};

        auto mtype = matches["type"] ;
        auto coords = matches["coords"];

        if (mtype == "polygon") {
            obj.gtype= GeoType::PolygonType;
            parser = wkt_polygon_parser;
        }
        else if (mtype == "linestring") {
            obj.gtype =  GeoType::LineStringType;
            parser = wkt_linestring_parser;
        }
        else if (mtype == "point") {
            obj.gtype =  GeoType::PointType;
            parser = wkt_point_parser;
        }

        if (coords != nullptr && obj.gtype != GeoType_Unkown) {
            parser(coords, obj);
        }
        return obj;
    }

    //wkt type and coordiantes
    std::unordered_map<std::string, std::string> wkt_type_coords(const std::regex& wktreg,  const std::string& input_wkt) {
        auto wkt = trim_copy(input_wkt);
        std::unordered_map<std::string, std::string> captures{
                {"wkt",    ""},
                {"type",   ""},
                {"coords", ""},
        };

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
                    val = lower_str(val);
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
