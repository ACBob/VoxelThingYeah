#include "chunkmodel.h"
#include "shadermanager.h"

class ChunkRenderer {
	public:
		ChunkRenderer(ChunkModel *mdl);
		~ChunkRenderer();
		// Updated the index buffer and vertex buffer
		void Populate();

		void Render();
	
	private:
		// Internal model (pointer to chunk model)
		ChunkModel *_mdl;

		// Vertex Buffer
		unsigned int vbo;
		// Array buffer
		unsigned int vao;
		// Element buffer
		unsigned int ebo;
};