#ifndef HEADER_INCLUDE__RECURSION__PHYSICS__B2__DEBUGDRAW__H
#define HEADER_INCLUDE__RECURSION__PHYSICS__B2__DEBUGDRAW__H
#include <vector>
#include <mutex>

#include <Box2D/Common/b2Draw.h>

#include <macros/class_macros.h>

#include "b2draw/PrimitiveRenderer.h"

namespace b2draw {


/**
 * Simple DebugDraw class.
 *
 * @code
 * // Configuration...
 * DebugDraw debugDraw;
 * b2World world;
 * world.SetDebugDraw(&debugDraw);
 * // ...
 *
 * // Logic loop:
 * debugDraw.Clear(); // Clears the previous geometry set.
 * world.DrawDebugData(); // Adds world geometry to the DebugDraw.
 * debugDraw.BufferData(); // Sends geometry to the GPU.
 * // ...
 *
 * // Render loop:
 * debugDraw.Render();
 * @endcode
 */
class DebugDraw
	:	public b2Draw
{
public:
	DebugDraw(
		GLuint const programId,
		char const* const pVertexAttrib,
		char const* const pColourAttrib,
		uint16 const numCircleSegments = 16,
		float32 const fillAlpha = 0.5f,
		float32 const axisScale = 4.0f
	);

	CPLURGE_NON_COPYABLE(DebugDraw);

	DebugDraw(DebugDraw&& rvOther);
	DebugDraw& operator=(DebugDraw&& rvOther);

	virtual ~DebugDraw() noexcept override;

	virtual void DrawPolygon(
		b2Vec2 const* pVertices,
		int32 vertexCount,
		b2Color const& colour
	) override;

	virtual void DrawSolidPolygon(
		b2Vec2 const* pVertices,
		int32 vertexCount,
		b2Color const& colour
	) override;

	virtual void DrawCircle(
		b2Vec2 const& centre,
		float32 radius,
		b2Color const& colour
	);

	virtual void DrawSolidCircle(
		b2Vec2 const& centre,
		float32 radius,
		b2Vec2 const& axis,
		b2Color const& colour
	) override;

	virtual void DrawSegment(
		b2Vec2 const& begin,
		b2Vec2 const& end,
		b2Color const& colour
	) override;

	virtual void DrawPoint(
		b2Vec2 const& point,
		float32 size,
		b2Color const& colour
	) override;

	virtual void DrawTransform(b2Transform const& xf) override;

	void BufferData();

	void Render();

	void Clear();

private:
	PrimitiveRenderer m_lineRenderer;
	PrimitiveRenderer m_fillRenderer;

	std::mutex m_mutex;

	float32 m_fillAlpha;
	float32 m_axisScale;
};


} // namespace b2draw
#endif // #ifndef HEADER_INCLUDE__RECURSION__PHYSICS__B2__DEBUGDRAW__H