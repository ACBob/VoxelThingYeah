#include "chunkmodel.h"
#include "shadermanager.h"

class ChunkRenderer {
	public:
		ChunkRenderer(ChunkModel *mdl);
		~ChunkRenderer();
		// Updated the index buffer and vertex buffer
		void Populate();

		void Render(shader_t shader);
	
	private:
		// Internal model (pointer to chunk model)
		ChunkModel *_mdl;

		// Index Buffer
		unsigned short idx;
		// Vertex buffer
		unsigned short vtx;
};