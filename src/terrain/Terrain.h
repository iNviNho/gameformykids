#ifndef TERRAIN_H
#define TERRAIN_H
#include <memory>
#include <array>
#include <filesystem>
#include <algorithm>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "Grasses.h"
#include "../images/Image.h"
#include "../shaders/shader.h"

class Terrain
{
private:

    /**
     * Size of the terrain in vertices.
     * Must be one of 128, 256, 512, 1024 ...
     */
    static constexpr int SIZE = 256;

    /**
     * Defines minimum and maximum height of the terrain.
     */
    static constexpr float MAX_HEIGHT = 30.0f;

    /**
     * Defines how much terrain is covered with grass.
     * e.g. 1 means 10%
     */
    static constexpr int GRASS_DENSITY = 1;

    /**
     * Terrain is modeled with a triangular mesh (2 triangles, 3 vertices per triangle)
     */
    static constexpr int GL_VERTICES_PER_LOC = 2 * 3;

    /**
     * 8 data per vertex (x,y,z,u,v,nx,ny,nz)
     */
    static constexpr int DATA_PER_GL_VERTEX = 8;

    /**
     * GL data per location
     * location is a grid square
     */
    static constexpr int DATA_PER_LOC = DATA_PER_GL_VERTEX * GL_VERTICES_PER_LOC;

    unsigned int VAO, VBO;
    unsigned int grassTexture;
    unsigned int pathTexture;
    unsigned int mudTexture;
    unsigned int flowersTexture;
    unsigned int blendMapTexture;
    Image heightMap;
    Image blendMap;

    void generateTextures();
    void generateVaoVbo(const std::unique_ptr<GLfloat[]>& dataPoints, const GLsizeiptr dataPointsSz);

    void generateTerrain(const std::unique_ptr<GLfloat[]>& dataPoints);

    GLfloat* setGLVertexData(GLfloat* dataPtr, const glm::vec3& pos, const glm::vec<2,int> posInt, const float u, const float v) const
    {
        return setGLVertexData(dataPtr, pos, u, v, calculateNormal(posInt.x, posInt.y));
    }

    static constexpr GLfloat* setGLVertexData(GLfloat* dataPtr, const glm::vec3& pos, const float u, const float v, const glm::vec3& normal) noexcept
    {
        // position
        *dataPtr++ = pos.x;
        *dataPtr++ = pos.y;
        *dataPtr++ = pos.z;

        // normal
        *dataPtr++ = normal.x;
        *dataPtr++ = normal.y;
        *dataPtr++ = normal.z;

        // texture coordinate
        *dataPtr++ = u;
        *dataPtr++ = v;

        return dataPtr;
    }

    static GLfloat* copyGLVertexData(GLfloat* dataPtr, const GLfloat * const src, const float u, const float v) noexcept
    {
        // we do -2 because we want to copy all but the texture coordinates
        // as we can reuse vertex attributes and normals
        dataPtr = std::copy(src, src + DATA_PER_GL_VERTEX - 2, dataPtr);
        *dataPtr++ = u;
        *dataPtr++ = v;
        return dataPtr;
    }

    // generic ("false", "false" )
    template<bool X_IS_ZERO, bool Z_IS_ZERO>
    struct SetVertexDataHelper
    {
        const int xPlus1;
        const float floatXPlus1;

        const GLfloat* const dataPtrZMinus1;

        const int zPlus1;
        const float floatZPlus1;

        const GLfloat* const dataPtrXMinus1;

        constexpr
        SetVertexDataHelper(const GLfloat * const dataPoints, const int x, const int z)
        noexcept
        : xPlus1(x + 1), floatXPlus1(static_cast<float>(xPlus1)), dataPtrZMinus1(dataPoints + ((z - 1) * SIZE * DATA_PER_LOC + x * DATA_PER_LOC)),
          zPlus1(z + 1), floatZPlus1(-static_cast<float>(zPlus1)), dataPtrXMinus1(dataPoints + (z * SIZE * DATA_PER_LOC + (x - 1) * DATA_PER_LOC))
        { }
    };

    template<>
    struct SetVertexDataHelper<true, true>
    {
        static constexpr int xPlus1 = 1;
        static constexpr float floatX = 0.0f;
        static constexpr float floatXPlus1 = 1.0f;

        static constexpr int zPlus1 = 1;
        static constexpr float floatZ = 0.0f;
        static constexpr float floatZPlus1 = -1.0f;

        constexpr
        SetVertexDataHelper(const GLfloat * const, const int, const int)
        noexcept
        { }
    };

    template<>
    struct SetVertexDataHelper<false, true>
    {
        const int xPlus1;
        const float floatXPlus1;

        static constexpr int zPlus1 = 1;
        static constexpr float floatZ = 0.0f;
        static constexpr float floatZPlus1 = -1.0f;

        const GLfloat* const dataPtrXMinus1;

        constexpr
        SetVertexDataHelper(const GLfloat * const dataPoints, const int x, const int)
        noexcept
        : xPlus1(x + 1), floatXPlus1(static_cast<float>(xPlus1)), dataPtrXMinus1(dataPoints + ((x - 1) * DATA_PER_LOC))
        { }
    };

    template<>
    struct SetVertexDataHelper<true, false>
    {
        static constexpr int xPlus1 = 1;
        static constexpr float floatX = 0.0f;
        static constexpr float floatXPlus1 = 1.0f;

        const GLfloat* const dataPtrZMinus1;

        const int zPlus1;
        const float floatZPlus1;

        constexpr
        SetVertexDataHelper(const GLfloat * const dataPoints, const int, const int z)
        noexcept
        : zPlus1(z + 1), floatZPlus1(-static_cast<float>(zPlus1)), dataPtrZMinus1(dataPoints + ((z - 1) * SIZE * DATA_PER_LOC))
        { }
    };

    template<bool X_IS_ZERO, bool Z_IS_ZERO>
    void setVertexData(const std::unique_ptr<GLfloat[]>& dataPoints, const int x, const int z) const
    {
        // data start for location (x, z)
        GLfloat* const dataPtrStart = dataPoints.get() + (z * SIZE * DATA_PER_LOC + x * DATA_PER_LOC);
        GLfloat* dataPtr = dataPtrStart;

        // we construct the correct helper depending on the template parameters
        const SetVertexDataHelper<X_IS_ZERO, Z_IS_ZERO> hlp{ dataPoints.get(), x, z };

        // Let's define FIRST TRIANGLE in COUNTER-CLOCKWISE order
        // That means we do: C,A,B
        // For illustration please check the image 23terrainGenerationCABOverview.png

        // C (upper right corner of the grid square)
        // this corner we always have to calculate
        dataPtr = setGLVertexData(
            dataPtr,
            glm::vec3{ hlp.floatXPlus1, getHeight(hlp.xPlus1, -hlp.zPlus1), hlp.floatZPlus1 },
            glm::vec<2, int>{hlp.xPlus1, -hlp.zPlus1},
            1.0f, 1.0f);

        // A (bottom left corner of the grid square)
        // this corner we might be able to reuse
        if constexpr (X_IS_ZERO && Z_IS_ZERO)
            dataPtr = setGLVertexData(
                dataPtr,
                glm::vec3{ hlp.floatX, getHeight(x, -z), hlp.floatZ },
                glm::vec<2, int>{x,-z},
                0.0f, 0.0f);
        else if constexpr (!X_IS_ZERO)
            // if x is not zero,
            dataPtr = copyGLVertexData(dataPtr, hlp.dataPtrXMinus1 + 2 * DATA_PER_GL_VERTEX, 0.0f, 0.0f);
        else
            dataPtr = copyGLVertexData(dataPtr, hlp.dataPtrZMinus1 + 4 * DATA_PER_GL_VERTEX, 0.0f, 0.0f);

        // B (bottom right corner of the grid square)
        // this corner we might be able to reuse
        if constexpr (Z_IS_ZERO)
            dataPtr = setGLVertexData(dataPtr, glm::vec3{ hlp.floatXPlus1, getHeight(hlp.xPlus1, -z), hlp.floatZ }, glm::vec<2, int>{hlp.xPlus1, -z}, 1.0f, 0.0f);
        else
            dataPtr = copyGLVertexData(dataPtr, hlp.dataPtrZMinus1 + 0 * DATA_PER_GL_VERTEX, 1.0f, 0.0f);

        // Let's define SECOND triangle in COUNTER-CLOCKWISE order
        // That means we do: C,D,A
        // For illustration please check the image 24terrainGenerationCDAoverview.png

        // C (upper right corner of the grid square)
        // we can reuse this corner, it was already calculated as first vertex of the first triangle
        dataPtr = std::copy(dataPtrStart, dataPtrStart + DATA_PER_GL_VERTEX, dataPtr);

        // D (upper left corner of the grid square)
        // this corner we might be able to reuse
        if constexpr (X_IS_ZERO)
            dataPtr = setGLVertexData(dataPtr, glm::vec3{ hlp.floatX, getHeight(x, -hlp.zPlus1), hlp.floatZPlus1 }, glm::vec<2, int>{x, -hlp.zPlus1}, 0.0f, 1.0f);
        else
            dataPtr = copyGLVertexData(dataPtr, hlp.dataPtrXMinus1 + 0 * DATA_PER_GL_VERTEX, 0.0f, 1.0f);

        // A (bottom left corner of the grid square)
        // we can reuse this corner, it was already calculated as second vertex of the first triangle
        dataPtr = std::copy(dataPtrStart + 1 * DATA_PER_GL_VERTEX, dataPtrStart + 2 * DATA_PER_GL_VERTEX, dataPtr);
    }

    glm::vec3 calculateNormal(const int x, const int z) const;

    Grasses grasses;
    void generateGrasses();

    std::array<glm::vec3, 3> GetTriangle(const float x, const float z) const;

public:
    Terrain(const std::filesystem::path& heightMap, const std::filesystem::path& blendMap);
    ~Terrain() = default;
    const float GetCountOfVertices() const;
    unsigned int GetVAO() const { return VAO; }
    unsigned int GetGrassTexture() const { return grassTexture; }
    unsigned int GetPathTexturre() const { return pathTexture; }
    unsigned int GetMudTexture() const { return mudTexture; }
    unsigned int GetFlowersTexture() const { return flowersTexture; }
    unsigned int GetBlendMapTexture() const { return blendMapTexture; }
    constexpr int GetSize() const noexcept { return SIZE; }
    const Grasses& GetGrasses() const { return grasses;}
    void activateTextures(Shader& shader);

    /**
     * @param[in] x height map column coordinate
     * @param[in] z height map negated row coordinate
     * @return terrain height at (x,z) coordinate
     */
    float getHeight(const int x, int z) const;

    float GetHeightOfTerrain(const float x, const float z) const;
    glm::vec4 GetTrianglePlane(const float x, const float z) const;
    static glm::vec4 GetTrianglePlane(const std::array<glm::vec3, 3>& triangle) noexcept;
    static bool IsInsideTriangle(const std::array<glm::vec3, 3>& triangle, const glm::vec3& n, const glm::vec3& ptInPlane) noexcept;
};



#endif //TERRAIN_H
