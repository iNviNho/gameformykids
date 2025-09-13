#ifndef MYGAME_SVGDELEMENTPATHPARSER_H
#define MYGAME_SVGDELEMENTPATHPARSER_H

#include <iostream>
#include <vector>
#include "glm/vec2.hpp"


class SvgDElementPathParser {
public:
    static std::vector<glm::vec2> parsePath(const std::string& d) {
        std::vector<glm::vec2> points;
        size_t i = 0;
        glm::vec2 last{0,0};

        while(i < d.size()) {
            // Skip spaces
            while(i < d.size() && std::isspace(d[i])) i++;
            if(i >= d.size()) break;

            char cmd = d[i++];
            // Skip spaces after command
            while(i < d.size() && std::isspace(d[i])) i++;

            auto readFloat = [&](float &val) {
                size_t start = i;
                while(i < d.size() && (std::isdigit(d[i]) || d[i]=='.' || d[i]=='-' || d[i]=='e' || d[i]=='E')) i++;
                val = std::stof(d.substr(start, i-start));
                while(i < d.size() && std::isspace(d[i])) i++;
            };

            if(cmd == 'M' || cmd == 'L') {
                float x, y;
                readFloat(x);
                readFloat(y);
                last = {x, y};
                points.push_back(last);
            } else if(cmd == 'C') {
                float x1, y1, x2, y2, x, y;
                readFloat(x1); readFloat(y1);
                readFloat(x2); readFloat(y2);
                readFloat(x); readFloat(y);
                const int steps = 20;
                glm::vec2 p0 = last;
                glm::vec2 p1{x1, y1}, p2{x2, y2}, p3{x, y};
                for(int s=1;s<=steps;s++){
                    float t = s/float(steps);
                    float u = 1-t;
                    float t2 = t*t, u2 = u*u;
                    float t3 = t2*t, u3 = u2*u;
                    points.push_back({ u3*p0.x + 3*u2*t*p1.x + 3*u*t2*p2.x + t3*p3.x,
                                       u3*p0.y + 3*u2*t*p1.y + 3*u*t2*p2.y + t3*p3.y });
                }
                last = p3;
            } else {
                std::cerr << "Unknown command: " << cmd << "\n";
            }
        }
        return points;
    }
};

#endif //MYGAME_SVGDELEMENTPATHPARSER_H