#include "world.hpp"

#include "chunk.hpp"

CClientWorld::CClientWorld() {}
CClientWorld::~CClientWorld() {
    clearChunks();
}

void CClientWorld::render() {
    for (auto &&c : m_chunks)
        c.second->render();
}