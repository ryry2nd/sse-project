#include "Objects.hpp"

using namespace Objects;
using namespace Rendering;

RenderObject2d::RenderObject2d(Shader *shader, Image *im) : shader2d(shader), image(im)
{
    Drawable::setupObject();
    mesh2d = defaultMeshAPI->makeNewMesh(vertices2d, indices2d, {2, 2});
}

RenderObject2d::~RenderObject2d() {
    if (image != nullptr) {
        delete image;
    }
}

void RenderObject2d::updateImage(Image *im)
{
    if (image != nullptr)
    {
        delete image;
    }
    image = im;
}
void RenderObject2d::Draw()
{
    shader2d->disableCulling();
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(scale * image->imageSizes, 1.0f));
    shader2d->includeShader();
    shader2d->setUniform("texture1", image);
    shader2d->setUniform("uProjection", Objects::globalCamera.getProjectionMatrix2d());
    shader2d->setUniform("uModel", model);
    mesh2d->Draw();
}