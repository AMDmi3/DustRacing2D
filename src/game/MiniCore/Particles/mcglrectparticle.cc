// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2010 Jussi Lind <jussi.lind@iki.fi>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA  02110-1301, USA.
//

#include "mcglrectparticle.hh"
#include "mcglshaderprogram.hh"
#include "mcglvertex.hh"
#include "mccamera.hh"

#include <GL/gl.h>
#include <GL/glext.h>

#include <cassert>

static const int gNumVertices        = 6;
static const int gNumColorComponents = 4;
static const int gAlphaFrames        = 10;

MCGLRectParticle::MCGLRectParticle(const std::string & typeID)
: MCParticle(typeID)
, m_r(1.0)
, m_g(1.0)
, m_b(1.0)
, m_a(1.0)
, m_frameCount(gAlphaFrames)
, m_program(nullptr)
{
    // Disable shadow by default
    setHasShadow(false);

    // Init vertice data for a quad
    const MCGLVertex vertices[gNumVertices] =
    {
        {-1, -1, 0},
        {-1,  1, 0},
        { 1,  1, 0},
        {-1, -1, 0},
        { 1,  1, 0},
        { 1, -1, 0}
    };

    const MCGLVertex normals[gNumVertices] =
    {
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1}
    };

    const GLfloat colors[gNumVertices * gNumColorComponents] =
    {
        m_r, m_g, m_b, m_a,
        m_r, m_g, m_b, m_a,
        m_r, m_g, m_b, m_a,
        m_r, m_g, m_b, m_a,
        m_r, m_g, m_b, m_a,
        m_r, m_g, m_b, m_a
    };

    glGenBuffers(VBOTypes, m_vbos);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbos[VBOVertex]);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(MCGLVertex) * gNumVertices, vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbos[VBONormal]);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(MCGLVertex) * gNumVertices, normals, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbos[VBOColor]);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(GLfloat) * gNumVertices * gNumColorComponents, colors, GL_DYNAMIC_DRAW);
}

MCGLRectParticle::~MCGLRectParticle()
{
    glDeleteBuffers(VBOTypes, m_vbos);
}

void MCGLRectParticle::setShaderProgram(MCGLShaderProgram * program)
{
    m_program = program;
}

void MCGLRectParticle::setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    m_r = r;
    m_g = g;
    m_b = b;
    m_a = a;

    glBindBuffer(GL_ARRAY_BUFFER, m_vbos[VBOColor]);

    // This should make things a bit faster especially on NVIDIA.
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(GLfloat) * gNumVertices * gNumColorComponents, nullptr, GL_DYNAMIC_DRAW);

    GLfloat * pColorData = (GLfloat *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    if (pColorData)
    {
        for (int i = 0; i < gNumVertices; i++)
        {
            const int offset = (i << 2);
            pColorData[offset + 0] = r;
            pColorData[offset + 1] = g;
            pColorData[offset + 2] = b;
            pColorData[offset + 3] = a;
        }
        glUnmapBuffer(GL_ARRAY_BUFFER);
    }
}

void MCGLRectParticle::setAlpha(MCFloat a)
{
    if (m_a != a)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbos[VBOColor]);

        // This should make things a bit faster especially on NVIDIA.
        glBufferData(GL_ARRAY_BUFFER,
            sizeof(GLfloat) * gNumVertices * gNumColorComponents, nullptr, GL_DYNAMIC_DRAW);

        GLfloat * pColorData = (GLfloat *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        if (pColorData)
        {
            for (int i = 0; i < gNumVertices; i++)
            {
                pColorData[(i << 2) + 3] = a;
            }
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }
    }
}

void MCGLRectParticle::beginBatch()
{
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
}

void MCGLRectParticle::endBatch()
{
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    glDisable(GL_BLEND);
}

void MCGLRectParticle::render(MCCamera * pCamera)
{
    // Scale radius if fading out
    MCFloat r = radius();
    if (animationStyle() == Shrink)
    {
        r *= scale();
    }

    if (r > 0 && m_program)
    {
        MCFloat x = location().i();
        MCFloat y = location().j();

        if (pCamera)
        {
            pCamera->mapToCamera(x, y);
        }

        m_program->bind();
        m_program->translate(MCVector3dF(x, y, location().k()));
        m_program->rotate(angle());

        // Scale alpha if fading out. Don't do this on
        // every frame, because it's expensive.
        if (animationStyle() == FadeOut)
        {
            if (!--m_frameCount)
            {
                setAlpha(m_a * scale());
                m_frameCount = gAlphaFrames;
            }
        }

        m_program->setScale(r, r, 1.0);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbos[VBOVertex]);
        glVertexPointer(3, GL_FLOAT, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbos[VBONormal]);
        glNormalPointer(GL_FLOAT, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbos[VBOColor]);
        glColorPointer(4, GL_FLOAT, 0, 0);
        glDrawArrays(GL_TRIANGLES, 0, gNumVertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        m_program->release();
    }
}

void MCGLRectParticle::renderShadow(MCCamera *)
{
    return;
}
