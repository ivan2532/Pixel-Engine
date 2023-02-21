#pragma once

#include <string>
#include <vector>

#include "Mat4.h"

class Entity
{
public:
	Entity
	(
		const std::vector<Vec3>& vertices,
		const std::vector<Vec3>& normals,
		const std::vector<size_t>& indices,
		const Vec3& position = Vec3::Zero(),
		const Vec3& eulerAngles = Vec3::Zero()
	);

	Entity
	(
		const std::string& modelPath,
		const Vec3& position = Vec3::Zero(),
		const Vec3& eulerAngles = Vec3::Zero()
	);

	const std::vector<Vec3>& GetVertices() const;
	const std::vector<Vec3>& GetNormals() const;
	const std::vector<size_t>& GetIndices() const;

	void SetPosition(const Vec3& position);
	void SetRotation(const Vec3& eulerAngles);

	void Translate(const Vec3& translation);
	void Rotate(const Vec3& rotation);

	Mat4 GetModelTransform() const;
	void UpdateModelTransform();

private:
	void LoadModelFromFile(const std::string& path);

	std::vector<Vec3> m_Vertices;
	std::vector<Vec3> m_Normals;
	std::vector<size_t> m_Indices;

	Vec3 m_Position;
	Vec3 m_EulerAngles;

	Mat4 m_ModelTransform;
};