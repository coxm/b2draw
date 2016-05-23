#ifndef HEADER_INCLUDE__RECURSION__PHYSICS__B2__PRIMITIVERENDERER__H
#define HEADER_INCLUDE__RECURSION__PHYSICS__B2__PRIMITIVERENDERER__H
#include <vector>

#include <GL/glew.h>

#include <Box2D/Common/b2Draw.h> // For b2Color.

#include <macros/class_macros.h>


namespace b2draw {


class PrimitiveRenderer
{
public:
	PrimitiveRenderer(
		GLuint const programId,
		char const* const pVertexAttrib,
		char const* const pColourAttrib,
		uint16 const numCircleSegments = 16
	);

	CPLURGE_NON_COPYABLE(PrimitiveRenderer)
	PrimitiveRenderer(PrimitiveRenderer&&) noexcept;
	PrimitiveRenderer& operator=(PrimitiveRenderer&&) noexcept;

	~PrimitiveRenderer() noexcept;

	void addPolygon(
		b2Vec2 const* pCoords,
		int32 numCoords,
		b2Color const& colour
	);

	void addCircle(
		b2Vec2 const& centre,
		float32 const radius,
		b2Color const& colour,
		float32 const initialAngle = 0.0f
	);

	void addSegment(
		b2Vec2 const& begin,
		b2Vec2 const& end,
		b2Color const& colour
	);

	/** Buffer data. */
	void bufferData();

	/** Render data. */
	void render(GLenum const mode);

	/**
	 * Clear internally buffered data.
	 *
	 * Should be called after every b2World::DrawDebugData call.
	 */
	void clear();

	inline uint16 const numCircleSegments() const noexcept;
	inline uint16 const vertexCount() const noexcept;
	inline uint16 const polygonCount() const noexcept;

private:
	static constexpr unsigned char s_vertexBufferIndex = 0;
	static constexpr unsigned char s_colourBufferIndex = 1;

	std::vector<b2Vec2> m_verts;
	std::vector<GLint> m_firstIndices;
	std::vector<GLsizei> m_polygonSizes;
	std::vector<b2Color> m_vertexColours;

	GLuint m_vbos[2];
	GLuint m_vao;

	GLuint m_program;

	GLint m_vertexAttribLocation;
	GLint m_colourAttribLocation;

	uint16 m_numCircleSegments;
	uint16 m_vertexCount;
	uint16 m_polygonCount;
};


uint16 const
PrimitiveRenderer::numCircleSegments() const noexcept
{ return m_numCircleSegments; }


uint16 const
PrimitiveRenderer::vertexCount() const noexcept
{ return m_vertexCount; }


uint16 const
PrimitiveRenderer::polygonCount() const noexcept
{ return m_polygonCount; }


} // namespace b2draw
#endif // #ifndef HEADER_INCLUDE__RECURSION__PHYSICS__B2__PRIMITIVERENDERER__H
