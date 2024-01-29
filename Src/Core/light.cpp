#include "light.h"
#include "sceneObject.h"

Eigen::Matrix4f Light::getLightViewMatrix()
{
    return this->sceneObject->transform.matrix().inverse();
}

Eigen::Matrix4f Light::getLightProjectionMatrix()
{
    switch (this->lightParameters.lightType)
    {
        case LightType::Directional:
			return Eigen::Matrix4f::Identity();
            break;
        case LightType::Spot:
            return Eigen::Matrix4f::Identity();
			break;
        default:
            return Eigen::Matrix4f::Identity();
    }
}

void Light::start(const EngineState& engineState)
{
}

void Light::update(const EngineState& engineState)
{
}
