#pragma once

#include "SwirlData.h"
#include "StringToDataModifier.h"

namespace dae {

	using StringToSwirlType = StringToDataModifier<EnemyUnitSwirlData>;

	class DefineData_SwirlData {

	public:

		static void DefineTypes() {

			StringToSwirlType::AddDataModificationFunction("CircularUp",		AddCircularUpSwirls);
			StringToSwirlType::AddDataModificationFunction("CircularDown",		AddCircularDownSwirls);
			StringToSwirlType::AddDataModificationFunction("DownUTurn",			AddDownUTurns);
			StringToSwirlType::AddDataModificationFunction("EllipticalDown",	AddEllipticalDownSwirls);
		}

		static void AddCircularUpSwirls(EnemyUnitSwirlData& data) {

			SwirlData swirlData_0_right{};
			swirlData_0_right.StartWorldPos.x = g_WindowWidth;
			swirlData_0_right.StartWorldPos.y = int((g_WindowHeight * 2) / 3.f);
			swirlData_0_right.PreSwirlGoalDistance = 200;
			swirlData_0_right.PreSwirlSpeed = 300.f;//m/s
			swirlData_0_right.PreSwirlMovementDirection = { -1, -1 };


			swirlData_0_right.SwirlTimeMultiplier = 1;
			swirlData_0_right.SwirlSpeed = 300.f; // m/s
			swirlData_0_right.SwirlOrthogonalMovementRadius = 50;
			swirlData_0_right.SwirlPerpendicularMovementRadius = 50;
			swirlData_0_right.SwirlNumberOfCircles = 1.f;
			swirlData_0_right.SwirlStartPointRelativeToCenter = { 0,1 };

			swirlData_0_right.PostSwirlSpeedToReturn = 300.f;
			swirlData_0_right.PostSwirlRadiusToAttach = 8;

			//-----

			SwirlData swirlData_0_left{ swirlData_0_right };
			swirlData_0_left.StartWorldPos.x = 0;
			swirlData_0_left.StartWorldPos.y = int((g_WindowHeight * 2) / 3.f);
			swirlData_0_left.PreSwirlMovementDirection = { 1, -1 };

			swirlData_0_left.SwirlTimeMultiplier = -1;

			data.SwirlDataPerUnit.push_back(swirlData_0_right);
			data.SwirlDataPerUnit.push_back(swirlData_0_left);
		}

		static void AddCircularDownSwirls(EnemyUnitSwirlData& data) {

			SwirlData swirlData_1_right{};
			swirlData_1_right.StartWorldPos.x = int((g_WindowWidth * 2) / 3.f);
			swirlData_1_right.StartWorldPos.y = 0;
			swirlData_1_right.PreSwirlGoalDistance = 200;
			swirlData_1_right.PreSwirlSpeed = 300.f;//m/s
			swirlData_1_right.PreSwirlMovementDirection = { -1, 1 };

			swirlData_1_right.SwirlTimeMultiplier = -1;
			swirlData_1_right.SwirlSpeed = 300.f;
			swirlData_1_right.SwirlOrthogonalMovementRadius = 50;
			swirlData_1_right.SwirlPerpendicularMovementRadius = 50;
			swirlData_1_right.SwirlNumberOfCircles = 1.f;
			swirlData_1_right.SwirlStartPointRelativeToCenter = { 0, -1 };

			swirlData_1_right.PostSwirlSpeedToReturn = 300.f;
			swirlData_1_right.PostSwirlRadiusToAttach = 8;

			//-----

			SwirlData swirlData_1_left{ swirlData_1_right };
			swirlData_1_left.StartWorldPos.x = int(g_WindowWidth / 3.f);
			swirlData_1_left.StartWorldPos.y = 0;
			swirlData_1_left.PreSwirlMovementDirection = { 1, 1 };

			swirlData_1_left.SwirlTimeMultiplier = 1;

			data.SwirlDataPerUnit.push_back(swirlData_1_right);
			data.SwirlDataPerUnit.push_back(swirlData_1_left);
		}

		static void AddDownUTurns(EnemyUnitSwirlData& data) {

			//-----

			SwirlData swirlData_2_right{};
			swirlData_2_right.StartWorldPos.x = int(g_WindowWidth * 2 / 3.f);
			swirlData_2_right.StartWorldPos.y = 0;
			swirlData_2_right.PreSwirlGoalDistance = 200;
			swirlData_2_right.PreSwirlSpeed = 300.f;//m/s
			swirlData_2_right.PreSwirlMovementDirection = { 0, 1 };

			swirlData_2_right.SwirlTimeMultiplier = 1;
			swirlData_2_right.SwirlSpeed = 300.f;
			swirlData_2_right.SwirlOrthogonalMovementRadius = 50;
			swirlData_2_right.SwirlPerpendicularMovementRadius = 50;
			swirlData_2_right.SwirlNumberOfCircles = 0.5f;
			swirlData_2_right.SwirlStartPointRelativeToCenter = { 0, -1 };

			swirlData_2_right.PostSwirlSpeedToReturn = 300.f;
			swirlData_2_right.PostSwirlRadiusToAttach = 8;

			//-----

			SwirlData swirlData_2_left{ swirlData_2_right };
			swirlData_2_left.StartWorldPos.x = int(g_WindowWidth / 3.f);
			swirlData_2_left.StartWorldPos.y = 0;
			swirlData_2_left.PreSwirlMovementDirection = { 0, 1 };

			swirlData_2_left.SwirlTimeMultiplier = -1;


			data.SwirlDataPerUnit.push_back(swirlData_2_right);
			data.SwirlDataPerUnit.push_back(swirlData_2_left);
		}

		static void AddEllipticalDownSwirls(EnemyUnitSwirlData& data) {


			SwirlData swirlData_3_right{};
			swirlData_3_right.StartWorldPos.x = g_WindowWidth;
			swirlData_3_right.StartWorldPos.y = g_WindowHeight;
			swirlData_3_right.PreSwirlGoalDistance = 200;
			swirlData_3_right.PreSwirlSpeed = 300.f;//m/s
			swirlData_3_right.PreSwirlMovementDirection = { -1, -1 };


			swirlData_3_right.SwirlTimeMultiplier = 1;
			swirlData_3_right.SwirlSpeed = 300.f;
			swirlData_3_right.SwirlOrthogonalMovementRadius = 50;
			swirlData_3_right.SwirlPerpendicularMovementRadius = 100;
			swirlData_3_right.SwirlNumberOfCircles = 1.25f;
			swirlData_3_right.SwirlStartPointRelativeToCenter = { 0, 1 };

			swirlData_3_right.PostSwirlSpeedToReturn = 300.f;
			swirlData_3_right.PostSwirlRadiusToAttach = 8;

			//-----

			SwirlData swirlData_3_left{ swirlData_3_right };
			swirlData_3_left.StartWorldPos.x = 0;
			swirlData_3_left.PreSwirlMovementDirection = { 1, -1 };

			swirlData_3_left.SwirlTimeMultiplier = -1;


			data.SwirlDataPerUnit.push_back(swirlData_3_right);
			data.SwirlDataPerUnit.push_back(swirlData_3_left);

		}
	};



}