/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
              Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include "MeshState.h"

#include "Magnum/Context.h"
#include "Magnum/Extensions.h"

#include "State.h"

namespace Magnum { namespace Implementation {

MeshState::MeshState(Context& context, std::vector<std::string>& extensions): currentVAO(0)
    #ifndef MAGNUM_TARGET_GLES2
    , maxElementsIndices(0), maxElementsVertices(0)
    #endif
{
    #ifndef MAGNUM_TARGET_GLES
    if(context.isExtensionSupported<Extensions::GL::APPLE::vertex_array_object>())
    #elif defined(MAGNUM_TARGET_GLES2)
    if(context.isExtensionSupported<Extensions::GL::OES::vertex_array_object>())
    #else
    static_cast<void>(context);
    static_cast<void>(extensions);
    #endif
    {
        #ifndef MAGNUM_TARGET_GLES
        extensions.push_back(Extensions::GL::APPLE::vertex_array_object::string());
        #elif defined(MAGNUM_TARGET_GLES2)
        extensions.push_back(Extensions::GL::OES::vertex_array_object::string());
        #endif

        createImplementation = &Mesh::createImplementationVAO;
        destroyImplementation = &Mesh::destroyImplementationVAO;

        #ifndef MAGNUM_TARGET_GLES
        if(context.isExtensionSupported<Extensions::GL::EXT::direct_state_access>()) {
            extensions.push_back(Extensions::GL::EXT::direct_state_access::string());

            attributePointerImplementation = &Mesh::attributePointerImplementationDSA;
            attributeIPointerImplementation = &Mesh::attributePointerImplementationDSA;
            attributeLPointerImplementation = &Mesh::attributePointerImplementationDSA;
        } else
        #endif
        {
            attributePointerImplementation = &Mesh::attributePointerImplementationVAO;
            #ifndef MAGNUM_TARGET_GLES2
            attributeIPointerImplementation = &Mesh::attributePointerImplementationVAO;
            #ifndef MAGNUM_TARGET_GLES
            attributeLPointerImplementation = &Mesh::attributePointerImplementationVAO;
            #endif
            #endif
        }

        bindIndexBufferImplementation = &Mesh::bindIndexBufferImplementationVAO;
        bindImplementation = &Mesh::bindImplementationVAO;
        unbindImplementation = &Mesh::unbindImplementationVAO;
    }
    #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
    else {
        createImplementation = &Mesh::createImplementationDefault;
        destroyImplementation = &Mesh::destroyImplementationDefault;
        attributePointerImplementation = &Mesh::attributePointerImplementationDefault;
        #ifndef MAGNUM_TARGET_GLES2
        attributeIPointerImplementation = &Mesh::attributePointerImplementationDefault;
        #ifndef MAGNUM_TARGET_GLES
        attributeLPointerImplementation = &Mesh::attributePointerImplementationDefault;
        #endif
        #endif
        bindIndexBufferImplementation = &Mesh::bindIndexBufferImplementationDefault;
        bindImplementation = &Mesh::bindImplementationDefault;
        unbindImplementation = &Mesh::unbindImplementationDefault;
    }
    #endif

    #ifdef MAGNUM_TARGET_GLES2
    /* Instanced draw ímplementation on ES2 */
    if(context.isExtensionSupported<Extensions::GL::ANGLE::instanced_arrays>()) {
        extensions.push_back(Extensions::GL::ANGLE::instanced_arrays::string());

        drawArraysInstancedImplementation = &Mesh::drawArraysInstancedImplementationANGLE;
        drawElementsInstancedImplementation = &Mesh::drawElementsInstancedImplementationANGLE;

    } else if(context.isExtensionSupported<Extensions::GL::EXT::draw_instanced>()) {
        extensions.push_back(Extensions::GL::EXT::draw_instanced::string());

        drawArraysInstancedImplementation = &Mesh::drawArraysInstancedImplementationEXT;
        drawElementsInstancedImplementation = &Mesh::drawElementsInstancedImplementationEXT;

    } else if(context.isExtensionSupported<Extensions::GL::NV::draw_instanced>()) {
        extensions.push_back(Extensions::GL::NV::draw_instanced::string());

        drawArraysInstancedImplementation = &Mesh::drawArraysInstancedImplementationNV;
        drawElementsInstancedImplementation = &Mesh::drawElementsInstancedImplementationNV;

    } else {
        drawArraysInstancedImplementation = nullptr;
        drawElementsInstancedImplementation = nullptr;
    }

    /* Instanced arrays implementation on ES2 */
    if(context.isExtensionSupported<Extensions::GL::ANGLE::instanced_arrays>()) {
        /* Extension added above */

        vertexAttribDivisorImplementation = &Mesh::vertexAttribDivisorImplementationANGLE;

    } else if(context.isExtensionSupported<Extensions::GL::EXT::instanced_arrays>()) {
        extensions.push_back(Extensions::GL::EXT::instanced_arrays::string());

        vertexAttribDivisorImplementation = &Mesh::vertexAttribDivisorImplementationEXT;

    } else if(context.isExtensionSupported<Extensions::GL::NV::instanced_arrays>()) {
        extensions.push_back(Extensions::GL::NV::instanced_arrays::string());

        vertexAttribDivisorImplementation = &Mesh::vertexAttribDivisorImplementationNV;

    } else vertexAttribDivisorImplementation = nullptr;
    #endif
}

void MeshState::reset() {
    currentVAO = State::DisengagedBinding;
}

}}
