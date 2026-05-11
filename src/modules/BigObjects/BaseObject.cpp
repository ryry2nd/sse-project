#include "BigObjects.hpp"
#include <algorithm>

using namespace BigObjects;

BaseObject::~BaseObject()
{
	removeParent();

	for (BaseObject *obj : children)
	{
		obj->position = obj->getTruePos();
		obj->parent = nullptr;
	}
}

void BaseObject::makeParent(BaseObject &other)
{
	removeParent();
	parent = &other;
	parent->children.push_back(this);
	position = position - other.getTruePos(); // make position relative to new parent
}

BigVec3 BaseObject::getTruePos()
{
	BigVec3 ret = position;

	BaseObject *curr = parent;

	while (curr)
	{
		ret += curr->position;
		curr = curr->parent;
	}
	return ret;
}

void BaseObject::removeParent()
{
	if (parent)
	{
		auto &vec = parent->children;
		auto it = std::find(vec.begin(), vec.end(), this);
		if (it != vec.end())
			vec.erase(it);
		parent = nullptr;
	}
}