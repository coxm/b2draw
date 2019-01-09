#ifndef HEADER_INCLUDE__RECURSION__PHYSICS__B2__DEBUGDRAW__H
#define HEADER_INCLUDE__RECURSION__PHYSICS__B2__DEBUGDRAW__H
#include <vector>

#include <Box2D/Common/b2Draw.h>

#include "b2draw/PrimitiveRenderer.h"

namespace b2draw {


/**
 * Simple DebugDraw class.
 *
 * @code
 * // Configuration...
 * DebugDraw debugDraw(...);
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
	inline DebugDraw(
		unsigned numCircleSegments = 16,
		float32 fillAlpha = 0.9f,
		float32 axisScale = 4.0f
	)
		:	DebugDraw(-1, -1, numCircleSegments, fillAlpha, axisScale)
	{
	}

	DebugDraw(
		GLint positionAttribLocation,
		GLint colourAttribLocation,
		unsigned numCircleSegments = 16,
		float32 fillAlpha = 0.5f,
		float32 axisScale = 4.0f
	);

	DebugDraw(DebugDraw const&) = delete;
	DebugDraw& operator=(DebugDraw const&) = delete;

	DebugDraw(DebugDraw&&) = default;
	DebugDraw& operator=(DebugDraw&&) = default;

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

	inline void SetPositionAttribLocation(GLint location) noexcept
	{
		m_lineRenderer.setPositionAttribLocation(location);
		m_fillRenderer.setPositionAttribLocation(location);
	}

	inline void SetColourAttribLocation(GLint location) noexcept
	{
		m_lineRenderer.setColourAttribLocation(location);
		m_fillRenderer.setColourAttribLocation(location);
	}

	inline void SetAttribLocations(GLint position, GLint colour) noexcept
	{
		m_lineRenderer.setAttribLocations(position, colour);
		m_fillRenderer.setAttribLocations(position, colour);
	}

private:
	PrimitiveRenderer m_lineRenderer;
	PrimitiveRenderer m_fillRenderer;

	float32 m_fillAlpha;
	float32 m_axisScale;
};


} // namespace b2draw
#endif // #ifndef HEADER_INCLUDE__RECURSION__PHYSICS__B2__DEBUGDRAW__H
