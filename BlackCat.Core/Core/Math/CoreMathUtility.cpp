
#include "CorePCH.h"
#include "CoreMathUtility.h"
//#include "Input.h"

namespace BlackCat
{
	namespace Core
	{
		//SmoothStep::SmoothStep()
		//{
		//};

		////template<typename T>
		//SmoothStep::SmoothStep(float stepValue, float maxValue, float minValue, float middleValue, Keys upKey, Keys downKey, bool downValueOnRelease, float onReleaseStepValue)
		//{
		//	this->value = middleValue;
		//	this->stepValue = stepValue;
		//	this->maxValue = maxValue;
		//	this->minValue = minValue;
		//	this->middleValue = middleValue;
		//	this->downValueOnRelease = downValueOnRelease;
		//	this->onReleaseStepValue = onReleaseStepValue;
		//	this->upKey = upKey;
		//	this->downKey = downKey;
		//};

		////template<typename T>
		//SmoothStep::~SmoothStep()
		//{

		//};

		////template<typename T>
		//void SmoothStep::Update(KeyboardState* keyState)
		//{
		//	KeyState upKState; 
		//	if(upKey != Keys::None)
		//		upKState = keyState->GetKeyState(upKey);
		//	KeyState downKState; 
		//	if(downKey != Keys::None)
		//		downKState = keyState->GetKeyState(downKey);
		//	
		//	if(upKey != Keys::None)
		//		if(upKState.Pressed)
		//		{
		//			value += stepValue;
		//			if(value < middleValue)
		//				value += onReleaseStepValue;
		//			if(value > maxValue)
		//				value = maxValue;
		//			return;
		//		}
		//	if(downKey != Keys::None)
		//		if(downKState.Pressed)
		//		{
		//			value -= stepValue;
		//			if(value > middleValue)
		//				value -= onReleaseStepValue;
		//			if(value < minValue)
		//				value = minValue;
		//			return;
		//		}
		//
		//	if(downValueOnRelease)
		//	{
		//		if(value > middleValue)
		//		{
		//			value -= onReleaseStepValue;
		//			if(value < middleValue)
		//					value = middleValue;
		//			return;
		//		}

		//		if(value < middleValue)
		//		{
		//			value += onReleaseStepValue;
		//			if(value > middleValue)
		//					value = middleValue;
		//			return;
		//		}
		//	}
		//};

		//void SmoothStep::Update(SmoothStepAction action)
		//{
		//	if(action == SmoothStepAction_Up)
		//	{
		//		value += stepValue;
		//		if(value < middleValue)
		//			value += onReleaseStepValue;
		//		if(value > maxValue)
		//			value = maxValue;
		//		return;
		//	}

		//	if(action == SmoothStepAction_Down)
		//	{
		//		value -= stepValue;
		//		if(value > middleValue)
		//			value -= onReleaseStepValue;
		//		if(value < minValue)
		//			value = minValue;
		//		return;
		//	}
		//
		//	if(action == SmoothStepAction_Release)
		//	{
		//		if(value > middleValue)
		//		{
		//			value -= onReleaseStepValue;
		//			if(value < middleValue)
		//					value = middleValue;
		//			return;
		//		}

		//		if(value < middleValue)
		//		{
		//			value += onReleaseStepValue;
		//			if(value > middleValue)
		//					value = middleValue;
		//			return;
		//		}
		//	}
		//};
	}
}