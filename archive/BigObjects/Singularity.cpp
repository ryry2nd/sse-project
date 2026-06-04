#include "BigObjects.hpp"

using namespace BigObjects;
using namespace Rendering;

bool Singularity::disableBrightness = false;
const Bigint Singularity::near = Bigint(0.1);
const Bigint Singularity::far = Bigint("1000000000000000000000000000");

Singularity::Singularity(Mesh *mesh)
{
	setupObject();
	meshes.push_back(mesh);
}

Singularity::~Singularity()
{
}

void Singularity::appendUpdate(const float &deltaTime)
{
}

// I just made the default update to rotate all around
void Singularity::Update(const float &deltaTime)
{
	appendUpdate(deltaTime);
}

Bigint Singularity::calculateDistanceSquared(const BigVec3 &subtractedPos) const
{
	return subtractedPos.x * subtractedPos.x +
		   subtractedPos.y * subtractedPos.y +
		   subtractedPos.z * subtractedPos.z;
}

void Singularity::renderMesh()
{

	glm::vec3 newPos;

	Bigint distance = distanceSquared.sqrt();

	Bigint transform;

	if (distance > Bigint(10000))
	{
		transform = distance * Bigint(0.1f) + Bigint(0.1f);
		newPos = (tempLocalPosition / transform).toFloatVec3();
	}
	else
	{
		transform = Bigint(1);
		newPos = tempLocalPosition.toFloatVec3();
	}

	glm::mat4 matrix(1.0f);
	matrix = glm::translate(matrix, newPos);
	matrix = glm::rotate(matrix, rotation.x, glm::vec3(1, 0, 0));
	matrix = glm::rotate(matrix, rotation.y, glm::vec3(0, 1, 0));
	matrix = glm::rotate(matrix, rotation.z, glm::vec3(0, 0, 1));

	float passedTransform = transform.toFloat();
	if (std::isinf(passedTransform))
	{
		passedTransform = 0;
	}

	Bigint passedNear;
	Bigint passedFar;
	float clampedNear, clampedFar;

	if (distance < Bigint(100000))
	{
		passedNear = near;
		passedFar = Bigint(100000);
		clampedNear = 0.0f;
		clampedFar = 0.5f;
	}
	else if (distance < Bigint(10000000000))
	{
		passedNear = Bigint(100000);
		passedFar = Bigint(10000000000);
		clampedNear = 0.5f;
		clampedFar = 0.7f;
	}
	else
	{
		passedNear = Bigint(10000000000);
		passedFar = far;
		clampedNear = 0.7f;
		clampedFar = 0.999999f;
	}

	float mappedDepth;
	float floatTransform = transform.toFloat();

	glm::mat4 uModel;

	bool isInf = std::isinf(floatTransform);
	bool check = floatTransform != 0 && !isInf;

	float floatPassedNear = passedNear.toFloat();
	float floatPassedFar = passedFar.toFloat();

	Shader *shader;
	for (Mesh *mesh : meshes)
	{
		if (check)
		{
			uModel = glm::translate(matrix, mesh->posOffset / floatTransform);
			uModel = glm::scale(uModel, mesh->sizeOffset / floatTransform);
		}
		else
		{
			uModel = matrix;
		}

		if (isInf)
		{
			mappedDepth = glm::clamp(((Bigint(glm::length(glm::vec3(uModel[3]))) * transform - passedNear) / (passedFar - passedNear)).toFloat() * (clampedFar - clampedNear) + clampedNear, clampedNear, clampedFar);
		}
		else
		{
			mappedDepth = glm::clamp(((glm::length(glm::vec3(uModel[3])) * floatTransform - floatPassedNear) / (floatPassedFar - floatPassedNear)) * (clampedFar - clampedNear) + clampedNear, clampedNear, clampedFar);
		}
		shader = mesh->shader;

		shader->SetShader();
		shader->setUniform("u_fullBright", disableBrightness);
		shader->setUniform("uView", BigObjects::globalCamera.getViewMatrix());
		shader->setUniform("uProjection", BigObjects::globalCamera.getProjectionMatrix());
		shader->setUniform("uModel", uModel);
		shader->setUniform("depth", mappedDepth);
		mesh->Draw();
	}
}

void Singularity::Draw()
{
	tempLocalPosition = BigObjects::globalCamera.convertToLocal(getTruePos());
	distanceSquared = calculateDistanceSquared(tempLocalPosition);
	renderMesh();
}