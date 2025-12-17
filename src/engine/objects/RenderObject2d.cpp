#include "Objects.hpp"

using namespace Objects;
using namespace Rendering;

RenderObject2d::RenderObject2d(Shader *shader, Image *image)
{
    Drawable::setupObject();
    mesh2d = defaultMeshAPI->makeNewMesh(shader, vertices2d, indices2d, {2, 2});
    mesh2d->shader = shader;
    mesh2d->images.push_back(image);
    mesh2d->shader->cullingEnabled = false;
}

RenderObject2d::~RenderObject2d() {
    if (mesh2d != nullptr) {
        delete mesh2d;
    }
}

void RenderObject2d::Draw()
{
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(scale * mesh2d->images[0]->imageSizes, 1.0f));
    // shader2d->setUniform("texture1", image);
    mesh2d->shader->SetShader();
    mesh2d->shader->setUniform("uProjection", Objects::globalCamera.getProjectionMatrix2d());
    mesh2d->shader->setUniform("uModel", model);
    mesh2d->Draw();
}