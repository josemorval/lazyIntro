#ifndef VERTEX_INPUT_LAYOUT_HEADER_H
#define VERTEX_INPUT_LAYOUT_HEADER_H

static char shader_input_layout_signature[] =
"struct VS_INPUT_LAYOUT_SIGNATURE {"
"    float3 position : POSITION;"
"    float2 texcoord : TEXCOORD;"
"    float3 normal : NORMAL;"
"};"
"float4 vertex_input_layout_signature(VS_INPUT_LAYOUT_SIGNATURE input) : SV_POSITION"
"{"
"    return float4(0.0, 0.0, 0.0, 0.0);"
"}";

#endif // VERTEX_INPUT_LAYOUT_HEADER_H
