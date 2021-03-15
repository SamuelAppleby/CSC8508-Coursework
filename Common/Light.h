#pragma once
#include "Vector3.h"
using namespace NCL::Maths;

namespace NCL
{
	class Light
	{
	public:
		Light(Vector3 position, Vector3 color, float radius = 5)
		{
			this->radius     = radius;
			this->lightPos   = position;
			this->lightColor = color;
		}

	protected:

		float radius;
		Vector3 lightColor;
		Vector3 lightPos;
	};

}

