#include "BigObjects.hpp"

using namespace BigObjects;

std::vector<Drawable *> Drawable::drawables;

void Drawable::setupObject()
{
    drawables.push_back(this);
}

Drawable::~Drawable()
{
    drawables.erase(std::find(drawables.begin(), drawables.end(), this));
}

void Drawable::UpdateAllObjects()
{
    const float &deltaTime = Rendering::Window::deltaTime;
    for (size_t i = 0; i < drawables.size(); i++)
    {
        drawables[i]->Update(deltaTime);
    }
}

void Drawable::DrawAllObjects()
{
    for (size_t i = 0; i < drawables.size(); i++)
    {
        drawables[i]->Draw();
    }
}