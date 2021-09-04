#include "vector.hpp"

class Particle
{

	public:
		Particle();
		~Particle();

		Vector position;
		Vector size; // Z Ignored!!!

		bool shouldDie;
};