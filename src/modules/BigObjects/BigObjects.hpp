#pragma once

#include "CustomMath.hpp"
#include <engine/Rendering/Rendering.hpp>
#include <glm/glm.hpp>
#include <vector>

namespace BigObjects
{
	struct BaseObject
	{
		BigVec3 position;
		glm::vec3 rotation;

		BaseObject() : position(BigVec3()), rotation(glm::vec3(0.0f)) {}
		BaseObject(const BigVec3 &pos, const glm::vec3 &rot) : position(pos), rotation(rot) {}
		~BaseObject();

		// makes the other object the parent to the current object
		void makeParent(BaseObject &other);
		// gets the exact position in the world instead of relative
		BigVec3 getTruePos();
		BaseObject *parent = nullptr;
		std::vector<BaseObject *> children;
		void removeParent();
	};

	#define CAMERA_FOV 90.0f
	#define MOUSE_SENSITIVITY 0.1f
	#define NEAR 0.00001f
	#define FAR 100000.0f

	class Camera : public BaseObject
	{
	public:
		float fov = CAMERA_FOV; // if this number isn't 90 then you got a small wiener (or your zooming in which is chill but only if your zooming in)
		float mouse_sensitivity = MOUSE_SENSITIVITY;
		Camera() : BaseObject() {}
		Camera(BigVec3 &pos, glm::vec3 &rot) : BaseObject(pos, rot) {}
		// takes the cameras rotation into account
		glm::mat4 getViewMatrix() const;
		// do the thing with the projection, but like pass in near and far
		glm::mat4 getProjectionMatrix() const;
		// does all of them projections but in 2d
		glm::mat4 getProjectionMatrix2d() const;
		// it gets the vector right in front
		glm::vec3 getForwardVector() const;
		// it gets the vector to the right
		glm::vec3 getRightVector() const;
		// it gets the vector down
		glm::vec3 getDownVector() const;
		// converts the regular position into the local position relative to the camera
		BigVec3 convertToLocal(const BigVec3 &otherPosition) const;
		// rotates the camera by the event motion
		void rotateCamera(glm::vec2 motion);
		// the two clip distances
		float near = NEAR;
		float far = FAR;

	private:
		// it gets all them rotation craziness
		glm::mat4 getRotationMatrix() const;
	};

	inline Camera globalCamera;

	struct Drawable
	{
		void setupObject();
		~Drawable();
		static void UpdateAllObjects();
		static void DrawAllObjects();

	protected:
		virtual void Draw() = 0;
		virtual void Update(const float &deltaTime) = 0;

	private:
		static std::vector<Drawable *> drawables;
	};

	class Singularity : public Drawable, public BaseObject
	{
	public:
		// run before you setup any object
		Singularity() {}
		Singularity(Rendering::Mesh *mesh);
		~Singularity();

	protected:
		// the list of meshes
		std::vector<Rendering::Mesh *> meshes;

		virtual void appendUpdate(const float &deltaTime);
		BigVec3 tempLocalPosition;

		void Update(const float &deltaTime) override;
		void Draw();

	private:
		Bigint calculateInverseSquareLaw(const BigVec3 &subtractedPos, const Bigint &intensity) const;
		Bigint calculateDistanceSquared(const BigVec3 &subtractedPos) const;
		Bigint distanceSquared;
		void renderMesh();

		BigVec3 localSize;

		const static Bigint near;
		const static Bigint far;
		static bool disableBrightness;
	};
}