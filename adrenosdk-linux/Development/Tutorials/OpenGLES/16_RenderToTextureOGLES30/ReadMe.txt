
Compared to example 15, this examples shows the following new OpenGL ES 3.0 features

1. Shader version: a shader for OpenGL ES 3.0 needs to be marked with #version 300
2. Method to pre-assign attribute locations to be named as vertex shader inputs and color numbers to be named in fragment shader outputs (ARB_explicit_attrib_location)
3. Uniform buffer objects (ARB_uniform_buffer_object)
4. Vertex Array Objects  (APPLE_vertex_array_object)
5. Fine control over mapping buffer subranges into client space and flushing modfied data: mapping (glMapBufferRange) and unmapping of buffer objects (Apple_flush_buffer_range)
6. Half-float (16-bit) vertex array support (NV_half_float)
7. Mechanism to swizzle the components of a texture before they are returned to the shader (ARB_texture_swizzle)
8. Sampler objects, which separate sample state from texture image data  (ARB_sampler_objects)
9. New vertex attribute data formats, signed 2:10:10:10 and unsigned 2:10:10:10 (ARB_vertex_type_2_10_10_10_rev)
10. Simple boolean occlusion queries (ARB_occlusion_query2)
11. Instanced rendering with a per-instance counter accessible to vertex shaders (ARB_draw_instanced)
12. Data copying between buffer objects (ARB_copy_buffer)
13. Fence sync objects (ARB_sync)
14. new extension query method via indexed string queries via glGetStringi

