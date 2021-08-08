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
		{0, 1, 2},
		{1, 3, 2},

		{4, 6, 5},
		{5, 6, 7},

		{0, 2, 4},
		{4, 2, 6},

		{1, 5, 3},
		{5, 7, 3},

		{0, 4, 1},
		{4, 5, 1},
		
		{2, 3, 6},
		{6, 3, 7}
	};
	m.Update();
	
	return m;
}