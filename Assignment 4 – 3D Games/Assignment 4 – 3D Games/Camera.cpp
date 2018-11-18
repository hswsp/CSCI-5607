#include"Camera.h"
glm::mat4 Camera::getWorldToViewMatrix() const
{
	return glm::lookAt(position, position + viewDirection, up);
}
void Camera::mouseUpdate(const glm::vec2& newMousePosition)
{
	/*glm::vec2 mouseDelta = newMousePosition - oldMousePosition;
	if (glm::length(mouseDelta) > 50.0f)
	{
		oldMousePosition = newMousePosition;
		return;
	}
	glm::vec3 pitchAxis = glm::cross(viewDirection, up);
	viewDirection = glm::mat3(
		glm::rotate(mouseDelta.x * 0.01f, up) *glm::rotate(mouseDelta.y * 0.01f, pitchAxis)
	) * viewDirection;
	oldMousePosition = newMousePosition;*/
}