$input v_texcoord0

#include "bgfx_shader.sh"

SAMPLER2D(texture,  0);

void main() {
    gl_FragColor = texture2D(texture, v_texcoord0);
}