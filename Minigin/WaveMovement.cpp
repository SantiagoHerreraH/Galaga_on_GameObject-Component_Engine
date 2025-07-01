#include "WaveMovement.h"
#define _USE_MATH_DEFINES
#include <math.h>

glm::vec2 WaveMovement::ElipticalMovementTowardsTarget(float orthogonalMovementSpeed, float orthogonalMovementRange, float perpendicularMovementSpeed, float perpendicularMovementRange, float currentMovementTime, const glm::vec2& deltaTowardsTarget)
{

	// --- normalize movement speeds
	float averageMovementRange = (orthogonalMovementRange + perpendicularMovementRange)/2.f;

	orthogonalMovementSpeed /= averageMovementRange;
	perpendicularMovementSpeed /= averageMovementRange;

	//---- orthogonal

	float orthogonalScalar = std::sin(currentMovementTime * orthogonalMovementSpeed) * orthogonalMovementRange;

	glm::vec2 direction = glm::normalize(deltaTowardsTarget);

	glm::vec2 orthogonalVectorToDirection{};
	orthogonalVectorToDirection.x = -direction.y;
	orthogonalVectorToDirection.y = direction.x;

	orthogonalVectorToDirection *= orthogonalScalar;

	//---- perpendicular

	float perpendicularScalar = std::cos(currentMovementTime * perpendicularMovementSpeed) * perpendicularMovementRange;

	glm::vec2 perpendicularVectorToDirection{ direction };

	perpendicularVectorToDirection *= perpendicularScalar;

	return perpendicularVectorToDirection + orthogonalVectorToDirection;
}

glm::vec2 WaveMovement::SineMoveTowardsTarget(
	float orthogonalMovementSpeed, 
	float orthogonalMovementRange, 
	float perpendicularMovementSpeed, 
	float perpendicularOffsetMovementRange,
	float currentMovementTime,
	const glm::vec2& deltaTowardsTarget)
{

		//---- orthogonal

		float orthogonalScalar = std::sin(currentMovementTime * orthogonalMovementSpeed) * orthogonalMovementRange;

		glm::vec2 direction = glm::normalize(deltaTowardsTarget);

		glm::vec2 orthogonalVectorToDirection{};
		orthogonalVectorToDirection.x = -direction.y;
		orthogonalVectorToDirection.y = direction.x;

		orthogonalVectorToDirection *= orthogonalScalar;

		//---- perpendicular

		float perpendicularScalar = std::sin(currentMovementTime * perpendicularMovementSpeed) * (glm::length(deltaTowardsTarget) + perpendicularOffsetMovementRange);

		glm::vec2 perpendicularVectorToDirection{ direction };

		perpendicularVectorToDirection *= perpendicularScalar;

		return perpendicularVectorToDirection + orthogonalVectorToDirection;
}

glm::vec2 WaveMovement::MoveOrthogonallyTowardsTarget(
	float orthogonalMovementSpeed, 
	float orthogonalMovementRange,
	float perpendicularMovementSpeed,
	float currentMovementTime,
	const glm::vec2& deltaTowardsTarget)
{	

	float orthogonalScalar = std::sin(currentMovementTime * orthogonalMovementSpeed) * orthogonalMovementRange;

	glm::vec2 direction = glm::normalize(deltaTowardsTarget);

	glm::vec2 orthogonalVectorToDirection{};
	orthogonalVectorToDirection.x = -direction.y;
	orthogonalVectorToDirection.y = direction.x;

	orthogonalVectorToDirection *= orthogonalScalar;

	glm::vec2 perpendicularVectorToDirection{ direction * perpendicularMovementSpeed * currentMovementTime };

	return perpendicularVectorToDirection + orthogonalVectorToDirection;
}

glm::vec2 WaveMovement::MovePerpendicularlyTowardsTarget(float perpendicularMovementSpeed, float perpendicularOffsetMovementRange, float currentMovementTime, const glm::vec2& deltaTowardsTarget)
{
	glm::vec2 direction = glm::normalize(deltaTowardsTarget);

	float perpendicularScalar = std::sin(currentMovementTime * perpendicularMovementSpeed) * (glm::length(deltaTowardsTarget) + perpendicularOffsetMovementRange);

	glm::vec2 perpendicularVectorToDirection{ direction };

	perpendicularVectorToDirection *= perpendicularScalar;

	return perpendicularVectorToDirection;
}
