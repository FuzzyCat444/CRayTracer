#ifndef MATERIAL_H_INCLUDED
#define MATERIAL_H_INCLUDED

#include "Vec3.h"

typedef struct Material
{
    Vec3 diffuse;
    Vec3 specular;
    int hasSpecular;
} Material;

Material Material_create(Vec3 diffColor, Vec3 specColor, float specRatio);

#endif // MATERIAL_H_INCLUDED
