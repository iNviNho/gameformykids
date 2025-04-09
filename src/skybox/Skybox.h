

#ifndef SKYBOX_H
#define SKYBOX_H
#include <vector>


class Skybox {
private:
    unsigned int cubemapTexture;
    unsigned int VAO;

    void loadCubemap(const char* skyboxName);
    static std::vector<float> generateCubemapPositions();
public:
    explicit Skybox(const char* skyboxName) {
        loadCubemap(skyboxName);
    }
    const unsigned int GetCubemapTexture() const {
        return cubemapTexture;
    };
    const unsigned int GetVAO() const {
        return VAO;
    };
};



#endif //SKYBOX_H
