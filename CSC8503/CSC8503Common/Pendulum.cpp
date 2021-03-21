#include "Pendulum.h"
using namespace NCL;
using namespace CSC8503;
using namespace Maths;

bool print(tweeny::tween<int>&, int x)
{
	printf("%d\n", x); return false;
}
Pendulum::Pendulum(float timeToSwing, bool isSwingingLeft)
{
	speed = timeToSwing;
	name = "Pendulum";
	timer = 0.f;
	this->isSwingingLeft = isSwingingLeft;

}


void Pendulum::FixedUpdate(float fixedDT)
{

	if (isSwingingLeft)
	{
		if (timer == 0.0f)
		{
			Quaternion q = transform.GetOrientation();
			float zAngle = q.ToEuler().z;
			tween = tweeny::from(zAngle).to(-45.0f).during(200).onStep([this](float outAngle)
			{
				this->SetOrientation(PxQuat(Maths::DegreesToRadians(outAngle), PxVec3(0, 0, 1)));
				return false;
			});
			tween.via(0, tweeny::easing::cubicOut);
		}

		timer += fixedDT;
		if (tween.progress() < 1.0f)
		{
			tween.step(1);
		}
		else
		{
			timer = 0;
			isSwingingLeft = false;
		}

	}
	else
	{
		if (timer == 0.f)
		{
			Quaternion q = transform.GetOrientation();
			float zAngle = q.ToEuler().z;
			tween = tweeny::from(zAngle).to(45.0f).during(200).onStep([this](float outAngle)
				{
					this->SetOrientation(PxQuat(Maths::DegreesToRadians(outAngle), PxVec3(0, 0, 1))); return false;
					return false;
				});
			tween.via(0, tweeny::easing::cubicOut);
		}

		timer -= fixedDT;
		if (tween.progress() < 1.0f)
		{
			tween.step(1);
		}
		else
		{
			timer = 0;
			isSwingingLeft = true;
		}

	}
}
