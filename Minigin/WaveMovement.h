#pragma once

#include <glm.hpp>

class WaveMovement
{
public:
	static glm::vec2 ElipticalMovementTowardsTarget(
		float orthogonalMovementSpeed,
		float orthogonalMovementRange,
		float perpendicularMovementSpeed,
		float perpendicularMovementRange,
		float currentMovementTime,
		const glm::vec2& deltaTowardsTarget);

	static glm::vec2 SineMoveTowardsTarget(
		float orthogonalMovementSpeed,
		float orthogonalMovementRange,
		float perpendicularMovementSpeed,
		float perpendicularOffsetMovementRange,
		float currentMovementTime,
		const glm::vec2& deltaTowardsTarget);


	static glm::vec2 MoveOrthogonallyTowardsTarget(
		float orthogonalMovementSpeed,
		float orthogonalMovementRange,
		float perpendicularMovementSpeed,
		float currentMovementTime,
		const glm::vec2& deltaTowardsTarget);

	static glm::vec2 MovePerpendicularlyTowardsTarget(
		float perpendicularMovementSpeed,
		float perpendicularMovementRange,
		float currentMovementTime,
		const glm::vec2& deltaTowardsTarget);

};

