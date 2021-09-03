#include "modelrenderer.hpp"
#include "modelmanager.hpp"
#include "chunkmodel.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>

#include <memory>

ModelRenderer::ModelRenderer() :
	nFaces(0),
	nVertices(0)
{	
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	
	glBindVertexArray(vao);

	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)offsetof(Model::Vertex, x));
	glEnableVertexAttribArray(0);
	// Normal
	glVertexAttribPointer(1, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)offsetof(Model::Vertex, nx));
	glEnableVertexAttribArray(1);
	// texture coordinate
	glVertexAttribPointer(2, 2, GL_FLOAT, false, 8 * sizeof(float), (void*)offsetof(Model::Vertex, u));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

void ModelRenderer::Populate(void *_mdl)
{
	Model *mdl = reinterpret_cast<Model*>(_mdl);

	nVertices = mdl->vertices.size();
	nFaces = mdl->faces.size();

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, nVertices * sizeof(Model::Vertex), mdl->vertices.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, nFaces * sizeof(Model::Face), mdl->faces.data(), GL_DYNAMIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

ModelRenderer::~ModelRenderer()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
}

void ModelRenderer::Render(Vector pos, Vector rot, Shader *shader, Texture *tex)
{

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, {pos.x, pos.y, pos.z});
	model = glm::rotate(
		glm::rotate(
			glm::rotate(
				model,
				(float)(rot.z * DEG2RAD),
				{0,0,-1}
			),
			(float)(rot.y * DEG2RAD),
			{0,-1,0}
		),
		(float)(rot.x * DEG2RAD),
		{1,0,0}
	);

	shader->SetMat4("model", model);
	shader->SetMat3("normalMat", glm::mat3(glm::transpose(glm::inverse(model))));

	if (tex != nullptr)
		glBindTexture(GL_TEXTURE_2D, tex->id);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	
	glDrawElements(GL_TRIANGLES, nFaces * sizeof(Model::Face), GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	if (tex != nullptr)
		glBindTexture(GL_TEXTURE_2D, 0);

	// model = glm::mat4(1.0f);
	// shader->SetMat4("model", model);
}