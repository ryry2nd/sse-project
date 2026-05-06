#include "Objects.hpp"

using namespace Objects;

void Scene::drawScene() {

}

void Scene::clear() {
	cams.clear();
	objs.clear();
	params.clear();
	mats.clear();
	buffs.clear();
	meshes.clear();
	shaders.clear();
}