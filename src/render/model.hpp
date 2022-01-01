// -- Model
// Renders models.

#include <inttypes.h>
using boneIndex_t = uint16_t; // Max 65,535 bones (If you reach this, what the hell are you doing?)
using vertexIndex_t = uint32_t; // Max 4,294,967,295 vertices (If you reach this, seriously, what on earth are you doing?)

#include <string>
#include <vector>

#include "utility/vector.hpp"

#pragma once

namespace rendering {
    // name-space'd forward declarations
    namespace materials {
        class CTexture;
        class CShader;
    }

    namespace models {

        bool Init();
        void Uninit();
        
        // Generic model class.
        class CModel {
            public:
            CModel();
            ~CModel();

            void Render( Vector3f pos, Vector3f rot, Vector3f scale );

            void Update();

            uint64_t m_nAnimationTick;
            
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

            std::vector<Vertex> m_vertices;
            std::vector<Face> m_faces;
            std::vector<Bone> m_bones;

            materials::CShader *m_pShader;
            materials::CTexture *m_pTexture;

            void LoadOBJ( const std::string &filename ); // Loads the given OBJ file.
            std::string DumpOBJ(); // Dumps the model data as an OBJ file.

            protected:
            unsigned int m_nVAO;
            unsigned int m_nVBO;
            unsigned int m_nEBO;
        };

        // Gets a named model.    
        CModel *GetModel( const std::string &name );
    }
}
// TODO: Instanced model class