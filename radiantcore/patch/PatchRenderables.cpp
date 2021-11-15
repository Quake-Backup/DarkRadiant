#include "PatchRenderables.h"

void RenderablePatchWireframe::render(const RenderInfo& info) const
{
    // No colour changing
    glDisableClientState(GL_COLOR_ARRAY);
    if (info.checkFlag(RENDER_VERTEX_COLOUR))
    {
        glColor3f(1, 1, 1);
    }

    if (_tess.vertices.empty()) return;

    if (_needsUpdate)
    {
        _needsUpdate = false;

        // Create a VBO and add the vertex data
        VertexBuffer_T currentVBuf;
        currentVBuf.addVertices(_tess.vertices.begin(), _tess.vertices.end());

        // Submit index batches
        const RenderIndex* strip_indices = &_tess.indices.front();
        for (std::size_t i = 0;
            i < _tess.numStrips;
            i++, strip_indices += _tess.lenStrips)
        {
            currentVBuf.addIndexBatch(strip_indices, _tess.lenStrips);
        }

        // Render all index batches
        _vertexBuf.replaceData(currentVBuf);
    }

    _vertexBuf.renderAllBatches(GL_QUAD_STRIP);
}

void RenderablePatchWireframe::queueUpdate()
{
    _needsUpdate = true;
}

RenderablePatchSolid::RenderablePatchSolid(PatchTesselation& tess) :
    _tess(tess),
    _needsUpdate(true)
{}

void RenderablePatchSolid::render(const RenderInfo& info) const
{
    if (_tess.vertices.empty() || _tess.indices.empty()) return;

    if (!info.checkFlag(RENDER_BUMP))
    {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    // No colour changing
    glDisableClientState(GL_COLOR_ARRAY);

    if (info.checkFlag(RENDER_VERTEX_COLOUR))
    {
        glColor3f(1, 1, 1);
    }

    if (_needsUpdate)
    {
        _needsUpdate = false;

        // Add vertex geometry to vertex buffer
        VertexBuffer_T currentVBuf;
        currentVBuf.addVertices(_tess.vertices.begin(), _tess.vertices.end());

        // Submit indices
        const RenderIndex* strip_indices = &_tess.indices.front();
        for (std::size_t i = 0;
            i < _tess.numStrips;
            i++, strip_indices += _tess.lenStrips)
        {
            currentVBuf.addIndexBatch(strip_indices, _tess.lenStrips);
        }

        // Render all batches
        _vertexBuf.replaceData(currentVBuf);
    }

    _vertexBuf.renderAllBatches(GL_QUAD_STRIP, info.checkFlag(RENDER_BUMP));

	if (!info.checkFlag(RENDER_BUMP))
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}

void RenderablePatchSolid::queueUpdate()
{
    _needsUpdate = true;
}
#ifdef RENDERABLE_GEOMETRY
RenderableGeometry::Type RenderablePatchSolid::getType() const
{
    return RenderableGeometry::Type::Quads;
}

const Vector3& RenderablePatchSolid::getFirstVertex()
{
    return _tess.vertices.front().vertex;
}

std::size_t RenderablePatchSolid::getVertexStride()
{
    return sizeof(ArbitraryMeshVertex);
}

const unsigned int& RenderablePatchSolid::getFirstIndex()
{
    updateIndices();
    return _indices.front();
}

std::size_t RenderablePatchSolid::getNumIndices()
{
    updateIndices();
    return _indices.size();
}

void RenderablePatchSolid::updateIndices()
{
    // To render the patch mesh as quads, we need 4 indices per quad
    auto numRequiredIndices = (_tess.height - 1) * (_tess.width - 1) * 4;

    if (_indices.size() == numRequiredIndices)
    {
        return;
    }

    if (_tess.height == 0 || _tess.width == 0)
    {
        _indices.clear();
        return;
    }

    _indices.resize(numRequiredIndices);

    auto index = 0;
    for (auto h = 0; h < _tess.height - 1; ++h)
    {
        for (auto w = 0; w < _tess.width - 1; ++w)
        {
            _indices[index++] = static_cast<unsigned int>(h * _tess.width + w + 0);
            _indices[index++] = static_cast<unsigned int>((h + 1) * _tess.width + w + 0);
            _indices[index++] = static_cast<unsigned int>((h + 1) * _tess.width + w + 1);
            _indices[index++] = static_cast<unsigned int>(h * _tess.width + w + 1);
        }
    }
}
#endif
const ShaderPtr& RenderablePatchVectorsNTB::getShader() const
{
	return _shader;
}

RenderablePatchVectorsNTB::RenderablePatchVectorsNTB(const PatchTesselation& tess) :
	_tess(tess)
{}

void RenderablePatchVectorsNTB::setRenderSystem(const RenderSystemPtr& renderSystem)
{
	if (renderSystem)
	{
		_shader = renderSystem->capture("$PIVOT");
	}
	else
	{
		_shader.reset();
	}
}

#define	VectorMA( v, s, b, o )		((o)[0]=(v)[0]+(b)[0]*(s),(o)[1]=(v)[1]+(b)[1]*(s),(o)[2]=(v)[2]+(b)[2]*(s))

void RenderablePatchVectorsNTB::render(const RenderInfo& info) const
{
	if (_tess.vertices.empty()) return;

	glBegin(GL_LINES);

	for (const ArbitraryMeshVertex& v : _tess.vertices)
	{
		Vector3 end;

		glColor3f(0, 0, 1);
		glVertex3dv(v.vertex);
		VectorMA(v.vertex, 5, v.normal, end);
		glVertex3dv(end);

		glColor3f(1, 0, 0);
		glVertex3dv(v.vertex);
		VectorMA(v.vertex, 5, v.tangent, end);
		glVertex3dv(end);

		glColor3f(0, 1, 0);
		glVertex3dv(v.vertex);
		VectorMA(v.vertex, 5, v.bitangent, end);
		glVertex3dv(end);

		glColor3f(1, 1, 1);
		glVertex3dv(v.vertex);
		glVertex3dv(v.vertex);
	}

	glEnd();
}

void RenderablePatchVectorsNTB::render(IRenderableCollector& collector, const VolumeTest& volume, const Matrix4& localToWorld) const
{
	collector.setHighlightFlag(IRenderableCollector::Highlight::Primitives, false);
	collector.addRenderable(*_shader, *this, localToWorld);
}
