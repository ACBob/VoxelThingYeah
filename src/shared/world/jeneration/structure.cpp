#include "structure.hpp"

namespace {
    constexpr CStructure _treeStructure(TREE);
}

const CStructure &GetStructure(StructureType type) {
    switch (type) {
        case TREE:
            return _treeStructure;
        default:
            return _treeStructure;
    }
}