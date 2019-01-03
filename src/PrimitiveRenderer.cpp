#include <algorithm>
#include <iterator>
#include <stdexcept>

#include "b2draw/algorithm.h"
#include "b2draw/PrimitiveRenderer.h"

namespace b2draw {


PrimitiveRenderer::PrimitiveRenderer(
	GLuint const programId,
	char const* const pVertexAttrib,
	char const* const pColourAttrib,
	unsigned const numCircleSegments
)
	:	m_vertices{}
	,	m_firstIndices{}
	,	m_polygonSizes{}
	,	m_vbo{0u}
	,	m_vao{0u}
	,	m_program{programId}
	,	m_vertexAttribLocation{glGetAttribLocation(programId, pVertexAttrib)}
	,	m_colourAttribLocation{glGetAttribLocation(programId, pColourAttrib)}
	,	m_numCircleSegments{std::max(numCircleSegments, 3u)}
	,	m_polygonCount{0}
{
	if (m_vertexAttribLocation < 0)
	{
		throw std::runtime_error{pVertexAttrib};
	}

	if (m_colourAttribLocation < 0)
	{
		throw std::runtime_error{pColourAttrib};
	}

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
}


PrimitiveRenderer::PrimitiveRenderer(PrimitiveRenderer&& other) noexcept
	:	m_vertices{std::move(other.m_vertices)}
	,	m_firstIndices{std::move(other.m_firstIndices)}
	,	m_polygonSizes{std::move(other.m_polygonSizes)}
	,	m_vbo{0u}
	,	m_vao{other.m_vao}
	,	m_program{other.m_program}
	,	m_vertexAttribLocation{other.m_vertexAttribLocation}
	,	m_colourAttribLocation{other.m_colourAttribLocation}
	,	m_numCircleSegments{other.m_numCircleSegments}
	,	m_polygonCount{other.m_polygonCount}
{
	std::swap(m_vbo, other.m_vbo);
	other.m_vao = 0;
	other.m_program = 0;
	other.m_vertexAttribLocation = 0;
	other.m_colourAttribLocation = 0;
	other.m_numCircleSegments = 0;
	other.m_polygonCount = 0;
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
	auto const newPolygonCount = m_polygonCount + 1;
	m_firstIndices.reserve(newPolygonCount);
	m_polygonSizes.reserve(newPolygonCount);

	// Create a new polygon.
	m_firstIndices.push_back(m_vertices.size());
	m_polygonSizes.push_back(numNewVertices);
	m_polygonCount = newPolygonCount;

	// Copy vertices.
	b2Vec2 const* const pEnd = pVertices + numNewVertices;
	for (b2Vec2 const* pVertex = pVertices; pVertex < pEnd; ++pVertex)
	{
		m_vertices.emplace_back(*pVertex, colour);
	}

	assert(
		m_polygonCount == m_firstIndices.size() &&
		m_polygonCount == m_polygonSizes.size() &&
		"PrimitiveRenderer consistency error"
	);
}


void
PrimitiveRenderer::addCircle(
	b2Vec2 const& centre,
	float32 const radius,
	b2Color const& colour,
	float32 const initialAngle
)
{
	std::vector<b2Vec2> vertices;
	vertices.reserve(m_numCircleSegments);
	algorithm::chebyshevSegments(
		vertices,
		centre.x,
		centre.y,
		radius,
		initialAngle,
		m_numCircleSegments
	);

	assert(
		vertices.size() == m_numCircleSegments &&
		"Wrong number of circle segments"
	);

	addPolygon(vertices.data(), m_numCircleSegments, colour);
}


void
PrimitiveRenderer::addSegment(
	b2Vec2 const& begin,
	b2Vec2 const& end,
	b2Color const& colour
)
{
	decltype(m_polygonCount) const polygonCount = m_polygonCount + 1;
	m_vertices.reserve(m_vertices.size() + 2);
	m_polygonSizes.reserve(polygonCount);
	m_firstIndices.reserve(polygonCount);

	m_polygonSizes.push_back(2);
	m_firstIndices.push_back(m_vertices.size());
	m_vertices.emplace_back(begin, colour);
	m_vertices.emplace_back(end, colour);

	m_polygonCount = polygonCount;
	assert(
		m_polygonCount == m_polygonSizes.size() &&
		m_polygonCount == m_firstIndices.size() &&
		"PrimitiveRenderer internally inconsistent"
	);
}


void
PrimitiveRenderer::bufferData()
{
	glBindVertexArray(m_vao);

	// Vertices.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glVertexAttribPointer(
		m_vertexAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		nullptr);
	glVertexAttribPointer(
		m_colourAttribLocation, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		reinterpret_cast<void const*>(offsetof(Vertex, second)));

	glBufferData(
		GL_ARRAY_BUFFER,
		m_vertices.size() * sizeof(Vertex),
		m_vertices.data(),
		GL_DYNAMIC_DRAW
	);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


void
PrimitiveRenderer::render(GLenum const mode)
{
	glUseProgram(m_program);
	glBindVertexArray(m_vao);
	glEnableVertexAttribArray(m_vertexAttribLocation);
	glEnableVertexAttribArray(m_colourAttribLocation);
	glMultiDrawArrays(
		mode,
		m_firstIndices.data(),
		m_polygonSizes.data(),
		m_polygonCount
	);

	glBindVertexArray(0);
}


void
PrimitiveRenderer::clear()
{
	m_vertices.clear();
	m_firstIndices.clear();
	m_polygonSizes.clear();
	m_polygonCount = 0;
}


} // namespace b2draw
