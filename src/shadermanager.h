// TODO: shader manager

class Shader
{
	public:
	// Opengl Id
	// TODO: abstract
	unsigned int id;

	Shader(const char* vs, const char* fs);

	void Use();
};