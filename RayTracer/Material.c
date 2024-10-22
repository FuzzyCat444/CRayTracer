#include "Material.h"

Material Material_create(Vec3 diffColor, Vec3 specColor, float specRatio)
{
    return (Material)
    {
        .diffuse = Vec3_mulScalar(diffColor, 1.0f - specRatio),
        .specular = Vec3_mulScalar(specColor, specRatio),
        .hasSpecular = specRatio > 0.0f
    };
}
