#include "light.h"
#include "sceneObject.h"

Eigen::Matrix4f Light::getLightViewMatrix()
{
    return this->sceneObject->transform.matrix().inverse();
}

Eigen::Matrix4f Light::getLightProjectionMatrix()
{
    switch (this->lightParameters->lightType)
    {
        case LightType::Directional:
            float distance = this->lightParameters->direction.norm();
            return CustomUtils::orthographicProjection(distance,this->lightParameters->fov,1.0f,this->lightParameters->nearPlane,this->lightParameters->farPlane);            
        case LightType::Spot:
            return CustomUtils::perspectiveProjection(this->lightParameters->fov,1.0f,this->lightParameters->nearPlane,this->lightParameters->farPlane);)
        default:
            return Eigen::Matrix4f::Identity();
    }
}

void Light::start(const EngineState& engineState)
{
    
}

void Light::update(const EngineState& engineState)
{
    this->lightParameters->direction = this->lightParameters->target - this->sceneObject->transform.position;
}
