#include "Material.h"

using namespace Pengine;

void Material::operator=(const Material& material)
{
	m_Ambient = material.m_Ambient;
	m_Diffuse = material.m_Diffuse;
	m_Specular = material.m_Specular;
	m_Shininess = material.m_Shininess;
	m_BaseColor = material.m_BaseColor;
	m_Scale = material.m_Scale;
	m_Solid = material.m_Solid;
}