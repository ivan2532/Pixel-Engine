#include <Windows.h>

#include "Entity.h"
#include "OBJ_Loader.h"

Entity::Entity(const std::vector<Vec3>& vertices, const std::vector<Vec3>& normals, const std::vector<size_t>& indices, const Vec3& position, const Vec3& eulerAngles)
	:
	m_Vertices(vertices),
	m_Normals(normals),
	m_Indices(indices),
	m_Position(position),
	m_EulerAngles(eulerAngles)
{
	UpdateModelTransform();
}

Entity::Entity(const std::string& modelPath, const Vec3& position, const Vec3& eulerAngles)
	:
	m_Position(position),
	m_EulerAngles(eulerAngles)
{
	LoadModelFromFile(modelPath);
	UpdateModelTransform();
}

const std::vector<Vec3>& Entity::GetVertices() const
{
	return m_Vertices;
}

const std::vector<Vec3>& Entity::GetNormals() const
{
	return m_Normals;
}

const std::vector<size_t>& Entity::GetIndices() const
{
	return m_Indices;
}

void Entity::SetPosition(const Vec3& position)
{
	m_Position = position;
}

void Entity::SetRotation(const Vec3& eulerAngles)
{
	m_EulerAngles = eulerAngles;
}

void Entity::Translate(const Vec3& translation)
{
	m_Position += translation;
}

void Entity::Rotate(const Vec3& rotation)
{
	m_EulerAngles += rotation;
}

Mat4 Entity::GetModelTransform() const
{
	return m_ModelTransform;
}

void Entity::LoadModelFromFile(const std::string& path)
{
	objl::Loader loader;
	if (loader.LoadFile(path))
	{
		m_Indices.clear();
		m_Vertices.clear();
		m_Normals.clear();

		for (auto index : loader.LoadedIndices)
		{
			m_Indices.push_back(index);
		}

		for (const auto& vertex : loader.LoadedVertices)
		{
			m_Vertices.push_back(Vec3(vertex.Position.X, vertex.Position.Y, vertex.Position.Z));
			m_Normals.push_back(Vec3(vertex.Normal.X, vertex.Normal.Y, vertex.Normal.Z));
		}
	}
	else
	{
		OutputDebugStringA("ERROR: LoadModelFromFile WRONG PATH!\n");
	}
}

void Entity::UpdateModelTransform()
{
	m_ModelTransform = Mat4::Translate(m_Position) * Mat4::RotateZ(m_EulerAngles.z) * Mat4::RotateY(m_EulerAngles.y) * Mat4::RotateX(m_EulerAngles.x);
}
