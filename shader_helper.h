#ifndef SHADER_HELPER_HEADER_H
#define SHADER_HELPER_HEADER_H

static char shader_vertex_input_layout_signature[] =
"struct VS_INPUT_LAYOUT_SIGNATURE {"
"    float3 position : POSITION;"
"    float2 texcoord : TEXCOORD;"
"    float3 normal : NORMAL;"
"};"
"float4 vertex_input_layout_signature(VS_INPUT_LAYOUT_SIGNATURE input) : SV_POSITION"
"{"
"    return float4(0.0, 0.0, 0.0, 0.0);"
"}";

static char shader_error[] =
"struct VS_INPUT {"
"    float3  position : POSITION;"
"    float2  texcoord : TEXCOORD;"
"    float3  normal : NORMAL;"
"};"
"struct VS_OUTPUT {"
"    float4 position : POSITION;"
"    float2 texcoord : TEXCOORD;"
"    float3 normal : NORMAL;"
"};"
"struct GS_INPUT {"
"    float4 position : POSITION;"
"    float2 texcoord : TEXCOORD;"
"    float3 normal : NORMAL;"
"};"
"struct PS_INPUT {"
"    float4 position : SV_POSITION;"
"    float2 texcoord : TEXCOORD;"
"    float3 normal : NORMAL;"
"};"
"VS_OUTPUT vs_error(VS_INPUT i)"
"{"
"    VS_OUTPUT o;"
"    o.position = float4(0.0,0.0,0.0,1.0);"
"    o.texcoord = float2(0.0,0.0);"
"    o.normal = float3(0.0,0.0,0.0);"
"    return o;"
"}"
"[maxvertexcount(3)]"
"void gs_error(triangle GS_INPUT input[3], inout TriangleStream<PS_INPUT> triStream)"
"{"
"}"
"float4 ps_error(PS_INPUT i) : SV_TARGET"
"{"
"    return float4( 1.0, 0.0, 1.0, 1.0 );"
"}"
"[numthreads(8, 8, 1)]"
"void cs_error(uint3 id : SV_DispatchThreadID)"
"{}";


#endif // VERTEX_INPUT_LAYOUT_HEADER_H
