#include <algorithm>
#include <iterator>
#include <stdexcept>

#include "b2draw/algorithm.h"
#include "b2draw/PrimitiveRenderer.h"

namespace b2draw {


PrimitiveRenderer::PrimitiveRenderer(
	GLint const positionAttribLocation,
	GLint const colourAttribLocation,
	unsigned const numCircleSegments
)
	:	m_vertices{}
	,	m_firstIndices{}
	,	m_polygonSizes{}
	,	m_vbo{0u}
	,	m_vao{0u}
	,	m_tmpCircleBuffer{std::max(numCircleSegments, 3u)}
{
	glGenBuffers(1, &m_vbo);
	if (m_vbo == 0u) {
		throw std::runtime_error{"Invalid VBO"};
	}

	glGenVertexArrays(1, &m_vao);
	if (!m_vao)
	{
		glDeleteBuffers(1, &m_vbo);
		throw std::runtime_error{"Invalid VAO"};
	}

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glEnableVertexAttribArray(positionAttribLocation);
	setPositionAttribLocation(positionAttribLocation);

	glEnableVertexAttribArray(colourAttribLocation);
	setColourAttribLocation(colourAttribLocation);
}


void
PrimitiveRenderer::setPositionAttribLocation(GLint loc) noexcept
{
	glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
}


void
PrimitiveRenderer::setColourAttribLocation(GLint loc) noexcept
{
	glVertexAttribPointer(
		loc, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		reinterpret_cast<void const*>(offsetof(Vertex, second)));
}


PrimitiveRenderer::PrimitiveRenderer(PrimitiveRenderer&& other) noexcept
	:	m_vertices{std::move(other.m_vertices)}
	,	m_firstIndices{std::move(other.m_firstIndices)}
	,	m_polygonSizes{std::move(other.m_polygonSizes)}
	,	m_vbo{other.m_vbo}
	,	m_vao{other.m_vao}
	,	m_tmpCircleBuffer{std::move(other.m_tmpCircleBuffer)}
{
	other.m_vbo = 0;
	other.m_vao = 0;
}


PrimitiveRenderer::~PrimitiveRenderer() noexcept
{
	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);
}


void
PrimitiveRenderer::addPolygon(
	b2Vec2 const* const pVertices,
	int32 const numNewVertices,
	b2Color const& colour
)
{
	assert(numNewVertices != 0 && "Can't render an empty polygon!");
	// Reserve the space before we do anything.
	auto const totalVertices = m_vertices.size() + numNewVertices;
	m_vertices.reserve(totalVertices);
	auto const newPolygonCount = m_polygonSizes.size() + 1;
	m_firstIndices.reserve(newPolygonCount);
	m_polygonSizes.reserve(newPolygonCount);

	// Create a new polygon.
	m_firstIndices.push_back(m_vertices.size());
	m_polygonSizes.push_back(numNewVertices);

	// Copy vertices.
	b2Vec2 const* const pEnd = pVertices + numNewVertices;
	for (b2Vec2 const* pVertex = pVertices; pVertex < pEnd; ++pVertex)
	{
		m_vertices.emplace_back(*pVertex, colour);
	}
}


void
PrimitiveRenderer::addCircle(
	b2Vec2 const& centre,
	float32 const radius,
	b2Color const& colour,
	float32 const initialAngle
)
{
	algorithm::chebyshevSegments(
		m_tmpCircleBuffer.data(),
		numCircleSegments(),
		centre.x,
		centre.y,
		radius,
		initialAngle
	);

	addPolygon(m_tmpCircleBuffer.data(), numCircleSegments(), colour);
}


void
PrimitiveRenderer::addSegment(
	b2Vec2 const& begin,
	b2Vec2 const& end,
	b2Color const& colour
)
{
	m_vertices.reserve(m_vertices.size() + 2);
	auto const polygonCount = m_polygonSizes.size() + 1;
	m_polygonSizes.reserve(polygonCount);
	m_firstIndices.reserve(polygonCount);

	m_polygonSizes.push_back(2);
	m_firstIndices.push_back(m_vertices.size());
	m_vertices.emplace_back(begin, colour);
	m_vertices.emplace_back(end, colour);
}


void
PrimitiveRenderer::bufferData()
{
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(
		GL_ARRAY_BUFFER,
		m_vertices.size() * sizeof(Vertex),
		m_vertices.data(),
		GL_DYNAMIC_DRAW
	);
}


void
PrimitiveRenderer::render(GLenum const mode)
{
	glBindVertexArray(m_vao);
	glMultiDrawArrays(
		mode,
		m_firstIndices.data(),
		m_polygonSizes.data(),
		m_polygonSizes.size()
	);
}


void
PrimitiveRenderer::clear()
{
	m_vertices.clear();
	m_firstIndices.clear();
	m_polygonSizes.clear();
}


} // namespace b2draw
