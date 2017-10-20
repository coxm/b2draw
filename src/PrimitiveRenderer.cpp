/**
 * @todo Currently the renderer stores a b2Color for each vertex, which results
 * in a lot of duplication. Is there a nice way to fix this?
 */
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
	uint16 const numCircleSegments
)
	:	m_verts{}
	,	m_firstIndices{}
	,	m_polygonSizes{}
	,	m_vertexColours{}
	,	m_vbos{0, 0}
	,	m_vao{0}
	,	m_program{programId}
	,	m_vertexAttribLocation{glGetAttribLocation(programId, pVertexAttrib)}
	,	m_colourAttribLocation{glGetAttribLocation(programId, pColourAttrib)}
	,	m_numCircleSegments{std::max(numCircleSegments, uint16(3u))}
	,	m_vertexCount{0}
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

	glGenBuffers(2, m_vbos);
	for (auto const vbo: m_vbos)
	{
		if (!vbo)
		{
			throw std::runtime_error{"Invalid VBO"};
		}
	}

	glGenVertexArrays(1, &m_vao);
	if (!m_vao)
	{
		glDeleteBuffers(2, m_vbos);
		throw std::runtime_error{"Invalid VAO"};
	}
}


PrimitiveRenderer::PrimitiveRenderer(PrimitiveRenderer&& other) noexcept
	:	m_verts{std::forward<decltype(m_verts)>(other.m_verts)}
	,	m_firstIndices{
			std::forward<decltype(m_firstIndices)>(other.m_firstIndices)
		}
	,	m_polygonSizes{
			std::forward<decltype(m_polygonSizes)>(other.m_polygonSizes)
		}
	,	m_vertexColours{
			std::forward<decltype(m_vertexColours)>(other.m_vertexColours)
		}
	,	m_vbos{}
	,	m_vao{other.m_vao}
	,	m_program{other.m_program}
	,	m_vertexAttribLocation{other.m_vertexAttribLocation}
	,	m_colourAttribLocation{other.m_colourAttribLocation}
	,	m_numCircleSegments{other.m_numCircleSegments}
	,	m_vertexCount{other.m_vertexCount}
	,	m_polygonCount{other.m_polygonCount}
{
	other.m_vbos[0] = other.m_vbos[1] = 0;
	other.m_vao = 0;
	other.m_program = 0;
	other.m_vertexAttribLocation = 0;
	other.m_colourAttribLocation = 0;
	other.m_numCircleSegments = 0;
	other.m_vertexCount = 0;
	other.m_polygonCount = 0;
}


PrimitiveRenderer::~PrimitiveRenderer() noexcept
{
	glDeleteBuffers(2, m_vbos);
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
	auto const totalVertices = m_vertexCount + numNewVertices;
	m_verts.reserve(totalVertices);
	m_vertexColours.reserve(totalVertices);
	auto const newPolygonCount = m_polygonCount + 1;
	m_firstIndices.reserve(newPolygonCount);
	m_polygonSizes.reserve(newPolygonCount);

	// Copy vertices.
	std::copy(
		pVertices,
		pVertices + numNewVertices,
		std::back_inserter(m_verts)
	);

	// This is pretty horrible. See note above.
	for (int32 i = 0; i < numNewVertices; ++i)
	{
		m_vertexColours.push_back(colour);
	}

	// Create a new polygon.
	m_firstIndices.push_back(m_vertexCount);
	m_polygonSizes.push_back(numNewVertices);
	m_vertexCount = totalVertices;
	m_polygonCount = newPolygonCount;

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
	decltype(m_vertexCount) const vertexCount = m_vertexCount + 2;
	m_verts.reserve(vertexCount);
	m_vertexColours.reserve(vertexCount);
	m_polygonSizes.reserve(polygonCount);
	m_firstIndices.reserve(polygonCount);

	m_verts.push_back(begin);
	m_verts.push_back(end);
	m_vertexColours.push_back(colour);
	m_vertexColours.push_back(colour);
	m_polygonSizes.push_back(2);
	m_firstIndices.push_back(m_vertexCount);

	m_vertexCount = vertexCount;
	m_polygonCount = polygonCount;
	assert(
		m_polygonCount == m_polygonSizes.size() &&
		m_polygonCount == m_firstIndices.size() &&
		m_vertexCount == m_verts.size() &&
		m_vertexCount == m_vertexColours.size() &&
		"PrimitiveRenderer internally inconsistent"
	);
}


void
PrimitiveRenderer::bufferData()
{
	using vertex_type = decltype(m_verts)::value_type;
	using colour_type = decltype(m_vertexColours)::value_type;

	glBindVertexArray(m_vao);

	// Vertices.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbos[s_vertexBufferIndex]);
	glVertexAttribPointer(
		m_vertexAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glBufferData(
		GL_ARRAY_BUFFER,
		m_vertexCount * sizeof(vertex_type),
		m_verts.data(),
		GL_DYNAMIC_DRAW
	);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbos[s_colourBufferIndex]);
	glVertexAttribPointer(
		m_colourAttribLocation, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
	glBufferData(
		GL_ARRAY_BUFFER,
		m_vertexCount * sizeof(colour_type),
		m_vertexColours.data(),
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
	m_verts.clear();
	m_firstIndices.clear();
	m_polygonSizes.clear();
	m_vertexColours.clear();
	m_vertexCount = 0;
	m_polygonCount = 0;
}


} // namespace b2draw
