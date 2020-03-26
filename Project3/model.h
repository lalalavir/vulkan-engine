#pragma once
#include <vulkan/vulkan.hpp>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>
#include <iostream>
#include <unordered_map>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/hash.hpp>
struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec3 normal;
    glm::vec2 texCoord;
    static std::array<VkVertexInputAttributeDescription, 4> get_attribute_description();
    static std::array<VkVertexInputBindingDescription, 1> get_binding_descriptions();
    bool operator==(const Vertex& other) const {
        return pos == other.pos && color == other.color && texCoord == other.texCoord;
    }
};
namespace std {
    template<> struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^
                (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}
/*
class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    UINT materialIndex;
	Mesh(std::vector<Vertex> vertices,std::vector<uint32_t>,UINT materialIndex)
	{
        this->vertices = vertices;
        this->indices = indices;
        this->materialIndex = materialIndex;
	}
};
class Model
{
public:
    Mesh loadModel(std::string path);
private:
    std::vector<Mesh> meshes;
    std::string directory;
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	void processNode(aiNode* node, const aiScene* scene);
    struct Texture {
        unsigned int id;
        std::string type;
        aiString path;  // 我们储存纹理的路径用于与其它纹理进行比较
    };
    std::vector<Texture> textures_loaded;
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

*/