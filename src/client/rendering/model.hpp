// -- Model
// Renders models.

#include <inttypes.h>
using boneIndex_t = uint16_t; // Max 65,535 bones (If you reach this, what the hell are you doing?)
using vertexIndex_t = uint32_t; // Max 4,294,967,295 vertices (If you reach this, seriously, what on earth are you doing?)

#include <string>
#include <vector>

#include "utility/vector.hpp"

// name-space'd forward declarations
namespace materialSystem {
    class CTexture;
    class CShader;
}

// Generic model class.
class CModel {

    private:
    struct Vertex {
        float x, y, z;
        float nx, ny, nz;
        float u, v;
        float r, g, b;
    };

    struct Face {
        vertexIndex_t v1, v2, v3;
    };

    struct Bone {
        boneIndex_t parent;
        boneIndex_t id;
        float x, y, z;
        float rx, ry, rz;
        float sx, sy, sz;
    };

    public:
    CModel();
    ~CModel();

    void Render( Vector3f pos, Vector3f rot, Vector3f scale );

    void Update();

    uint64_t m_nAnimationTick;

    std::vector<Vertex> m_vertices;
    std::vector<Face> m_faces;
    std::vector<Bone> m_bones;

    materialSystem::CShader *m_pShader;
    materialSystem::CTexture *m_pTexture;

    void LoadOBJ( const std::string &filename );

    protected:
    unsigned int m_nVAO;
    unsigned int m_nVBO;
    unsigned int m_nEBO;
};

// TODO: Instanced model class