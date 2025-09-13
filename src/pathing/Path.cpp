#include "Path.h"

#include "data_dir.h"
#include "lunasvg.h"
#include "SvgDElementPathParser.h"
#include "../terrain/Terrain.h"
#include "../utils/Log.h"

void Path::generatePath(
    const std::string& pathFile,
    const float& pathFoundationSize,
    const float& offsetX,
    const float& offsetZ,
    const int& terrainSize
    ) {
    auto svgFile = lunasvg::Document::loadFromFile(pathFile);
    if(svgFile == nullptr) {
        throw std::runtime_error("Failed to generate path. Unable to load path file: " + pathFile);
    }
    auto pathFoundationToTerrainScale = pathFoundationSize / terrainSize;

    for (const auto& element : svgFile->documentElement().children()) {
        if (element.isElement()) {
            auto pts = SvgDElementPathParser::parsePath( element.toElement().getAttribute("d"));

            for(auto& p : pts) {
                path.push_back(glm::vec3{
                    (p.x + offsetX) / pathFoundationToTerrainScale,
                    0.0f,
                    (p.y - svgFile->height() - offsetZ) / pathFoundationToTerrainScale});
            }
        }
    }
    Log::logInfo("Successfully generated path. Path contains " + std::to_string(path.size()) + " points.");
}
