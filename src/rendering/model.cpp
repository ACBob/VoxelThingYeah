#include "model.h"

Model::Model() :
	renderer()
{
	renderer.Populate(this);
}

void Model::Update()
{
	renderer.Populate(this);
}

void Model::Render()
{
	renderer.Render(pos);
}

void Model::SetShader(Shader *shader)
{
	renderer.SetShader(shader);
}
Shader *Model::GetShader()
{
	return renderer.GetShader();
}


Model GetCubeModel(Vector size)
{
	Model m; // tesla
	m.vertices = {
		// NORTH +Z
		{  size.x,  size.y,  size.z},
		{ -size.x,  size.y,  size.z},
		{ -size.x, -size.y,  size.z},
		{  size.x, -size.y,  size.z},
		// SOUTH -Z
		{ -size.x,  size.y, -size.z},
		{  size.x,  size.y, -size.z},
		{  size.x, -size.y, -size.z},
		{ -size.x, -size.y, -size.z},
	};
	m.faces = {
		{2, 1, 0},
		{3, 2, 0},

		{3, 0, 5},
		{6, 3, 5},

		{6, 5, 4},
		{7, 6, 4},

		{7, 4, 1},
		{2, 7, 1},

		{1, 4, 5},
		{0, 1, 5},
		
		{7, 2, 3},
		{6, 7, 3}
	};
	m.Update();
	
	return m;
}