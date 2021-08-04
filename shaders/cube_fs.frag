$input v_texcoord0

#include "bgfx_shader.sh"

SAMPLER2D(texture,  0);

void main() {
    gl_FragColor = texture2D(texture, vec2(v_texcoord0.x, 1.0 - v_texcoord0.y));
}