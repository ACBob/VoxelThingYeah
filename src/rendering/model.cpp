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
	renderer.Render();
}