// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
//
// DustRAC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// DustRAC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DustRAC. If not, see <http://www.gnu.org/licenses/>.

#version 130

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform float     fade;

in vec2 texCoord0;
in vec2 texCoord1;
in vec4 vColor;

out vec4 fragColor;

void main(void)
{
    vec4 texColor = texture2D(tex0, texCoord0);
    vec4 sky      = texture2D(tex1, texCoord1);
    
    // Alpha test
    if (texColor.a < 0.1)
    {
        discard;
    }
    else
    {
        // Sky reflection
        vec4 ambient = fade * vec4(1.0, 0.95, 0.9, 1.0);
        if (texColor.r < 0.75 && texColor.g > 0.75 && texColor.b < 0.75)
        {
            fragColor = sky * 0.75 * texColor.g * ambient;
        }
        else
        {
            fragColor = texColor * ambient;
        }
    }
}