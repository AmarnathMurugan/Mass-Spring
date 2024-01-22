#include "light.h"

Eigen::Matrix4f Light::getLightSpaceMatrix()
{
    /*switch (this->lightParameters.lightType)
    {
        case LightType::Directional:

    }*/
    return Eigen::Matrix4f::Identity();
}
