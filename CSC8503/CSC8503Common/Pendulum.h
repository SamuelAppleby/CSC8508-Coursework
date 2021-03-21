#pragma once
#include "GameObject.h"
#include "tweeny/tweeny.h"
#include "../Common/Maths.h"
namespace NCL
{
	namespace CSC8503
	{
		class Pendulum :public GameObject
		{
		public:
			Pendulum(float timeToSwing, bool isSwingingLeft = true);
			void FixedUpdate(float fixedDT) override;


		private:

			float speed;
			float timer;
			bool isSwingingLeft;


			tweeny::tween<float> tween;
		};
	}
}
