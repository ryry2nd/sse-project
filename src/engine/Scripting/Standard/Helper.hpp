#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

#include <engine/Rendering/Rendering.hpp>

#include "Logging.hpp"

namespace Engine::Helper {
	struct CameraStruct {
		glm::mat4 view;
		glm::mat4 proj;
	};

	struct ModelStruct {
		glm::mat4 model;
		glm::mat3 normalMatrix;
	};

	inline void printMat4Flat(const glm::mat4& m)
	{
		const float* p = glm::value_ptr(m);

		Logging::info(
			"\n"
			"{: .4f} {: .4f} {: .4f} {: .4f}\n"
			"{: .4f} {: .4f} {: .4f} {: .4f}\n"
			"{: .4f} {: .4f} {: .4f} {: .4f}\n"
			"{: .4f} {: .4f} {: .4f} {: .4f}",
			p[0], p[1], p[2], p[3],
			p[4], p[5], p[6], p[7],
			p[8], p[9], p[10], p[11],
			p[12], p[13], p[14], p[15]
		);
	}

	inline glm::vec3 getForwardVector(glm::vec3 rot) {
		float yawRad = glm::radians(rot.y);
		float pitchRad = glm::radians(rot.x);
		return glm::normalize(glm::vec3(
			cos(pitchRad) * sin(yawRad),
			-sin(pitchRad),
			cos(pitchRad) * cos(yawRad)));
	}
	inline glm::vec3 getRightVector(glm::vec3 rot) {
		float yawRad = glm::radians(rot.y);
		return glm::normalize(glm::vec3(
			-cos(yawRad),
			0.0f,
			sin(yawRad)));
	}
	inline glm::vec3 getDownVector(glm::vec3 rot) {
		glm::vec3 forward = getForwardVector(rot);
		glm::vec3 right = getRightVector(rot);
		return glm::normalize(glm::cross(right, forward));
	}

	inline glm::mat4 cameraGetView(glm::vec3 pos, glm::vec3 rot) {
		glm::mat4 T = glm::translate(glm::mat4(1.0f), -pos);

		glm::mat4 R =
			glm::rotate(glm::mat4(1.0f), glm::radians(-rot.x), glm::vec3(1,0,0)) *
			glm::rotate(glm::mat4(1.0f), glm::radians(-rot.y), glm::vec3(0,1,0)) *
			glm::rotate(glm::mat4(1.0f), glm::radians(-rot.z), glm::vec3(0,0,1));

		return R * T;
	}

	inline glm::mat4 cameraGetProj(glm::vec2 res, float fov = 90.0f, float zNear = 0.01f, float zFar = 10000.0f) {
		return glm::perspective(glm::radians(fov), res.x / res.y, zNear, zFar);
	}

	inline void rotateCamera(float deltaTime, glm::vec3 *rot, glm::vec2 motion, float mouse_sensitivity) {
		if (deltaTime == 0)
		{
			return;
		}
		glm::vec3 rotation;
		rotation.y = rot->y - ((motion.x / deltaTime) * mouse_sensitivity * deltaTime);
		rotation.x = rot->x - ((motion.y / deltaTime) * mouse_sensitivity * deltaTime);
		rotation.z = rot->z;
		*rot = rotation;
}
}