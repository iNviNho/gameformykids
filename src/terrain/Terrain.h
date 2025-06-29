#ifndef TERRAIN_H
#define TERRAIN_H
#include <memory>
#include <filesystem>
#include "Grasses.h"
#include "../images/Image.h"
#include "../models/Model.h"
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

        // texture coordinate
        *dataPtr++ = u;
        *dataPtr++ = v;

        // normal
        *dataPtr++ = normal.x;
        *dataPtr++ = normal.y;
        *dataPtr++ = normal.z;

        return dataPtr;
    }

    static GLfloat* copyGLVertexData(GLfloat* dataPtr, const GLfloat * const src, const float u, const float v) noexcept
    {
        dataPtr = std::copy(src, src + DATA_PER_GL_VERTEX, dataPtr);
        *(dataPtr - 5) = u;
        *(dataPtr - 4) = v;
        return dataPtr;
    }

    void setVertexData(const std::unique_ptr<GLfloat[]>& dataPoints);
    
    /**
     *  'z' is 0
     */
    void setVertexData_x(const std::unique_ptr<GLfloat[]>& dataPoints, const int x);
    
    /**
     *  'x' is 0
	 */
    void setVertexData_z(const std::unique_ptr<GLfloat[]>& dataPoints, const int z);

    void setVertexData_xz(const std::unique_ptr<GLfloat[]>& dataPoints, const int x, const int z);
    /**
	 * @param[in] x height map column coordinate
	 * @param[in] z height map negated row coordinate
	 * @return terrain height at (x,z) coordinate
     */
    float getHeight(const int x, int z) const;

    glm::vec3 calculateNormal(const int x, const int z) const;

    Grasses grasses;
    void generateGrasses();

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
    int GetSize() { return SIZE; }
    const Grasses& GetGrasses() const { return grasses;}
    void activateTextures(Shader& shader);

    const float GetHeightOfTerrain(float x, float z) const;
};



#endif //TERRAIN_H
