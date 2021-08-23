#include "model.hpp"
#include "utility/direction.hpp"

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
	// Don't waste timet rying to render
	if (vertices.size() == 0) return;
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
		// +Z
		{2, 1, 0},
		{3, 2, 0},

		// -X
		{3, 0, 5},
		{6, 3, 5},

		// -Z
		{6, 5, 4},
		{7, 6, 4},

		// +X
		{7, 4, 1},
		{2, 7, 1},

		// +Y
		{1, 4, 5},
		{0, 1, 5},
		
		// -Y
		{7, 2, 3},
		{6, 7, 3}
	};
	for (int i = 0; i < 6; i+=2)
	{
		Vector normal = DirectionVector[i];
		m.vertices[m.faces[i].v].nx = normal.x;
		m.vertices[m.faces[i].v].ny = normal.y;
		m.vertices[m.faces[i].v].nz = normal.z;
		m.vertices[m.faces[i].vv].nx = normal.x;
		m.vertices[m.faces[i].vv].ny = normal.y;
		m.vertices[m.faces[i].vv].nz = normal.z;
		m.vertices[m.faces[i].vvv].nx = normal.x;
		m.vertices[m.faces[i].vvv].ny = normal.y;
		m.vertices[m.faces[i].vvv].nz = normal.z;
	}
	m.Update();
	
	return m;
}