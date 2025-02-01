#ifndef GENERATED_HEADER_H
#define GENERATED_HEADER_H

const char shader_content[] = R"raw(
float4x4 compute_view_matrix(float3 _position_camera, float3 _view_point) {

	float3 up = { 0.0f,1.0f,0.0f };
	float3 forward = _view_point - _position_camera;
	forward = normalize(forward);
	float3 right = normalize(cross(up, forward));
	float3 newUp = normalize(cross(forward, right));

	float aa = -dot(right, _position_camera);
	float bb = -dot(newUp, _position_camera);
	float cc = -dot(forward, _position_camera);

	float4x4 view_matrix = {
			right.x,    newUp.x,    forward.x,    0.0f,
			right.y,    newUp.y,    forward.y,    0.0f,
			right.z,    newUp.z,    forward.z,    0.0f,
			-dot(right, _position_camera),   -dot(newUp, _position_camera),  -dot(forward, _position_camera),   1.0f
	};

	return view_matrix;
}

float4x4 compute_perspective_matrix(float _fov, float _aspect_ratio, float _znear, float _zfar)
{
	float h = 1 / tan(_fov * 0.5);
	float w = h / _aspect_ratio;
	float a = _zfar / (_zfar - _znear);
	float b = (-_znear * _zfar) / (_zfar - _znear);

	float4x4 projection_matrix = float4x4(
		w, 0.0f, 0.0f, 0.0f,
		0.0f, h, 0.0f, 0.0f,
		0.0f, 0.0f, a, 1.0f,
		0.0f, 0.0f, b, 0.0f
	);

	return projection_matrix;
}

//Constant buffer
cbuffer constants{
	float frame;
	float width;
	float height;
		float _padding0;
	float3 tposition;
		float _padding1;
}

#define TIME 0.02*frame

static float4x4 view_matrix = compute_view_matrix(float3(0.0,1.0,3.0), float3(0.0,0.0,0.0));
static float4x4 projection_matrix = compute_perspective_matrix(1.0,1.6,0.1,50.0);
RWTexture2D<float3> g_rwtTexture0;

#ifdef VERTEX_SHADER

struct VS_INPUT {
    float3  position    :   POSITION;
    float2  texcoord    :   TEXCOORD;
    float3  normal      :   NORMAL;
    uint    instanceid  :   SV_InstanceID;
};

struct VS_OUTPUT {
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

VS_OUTPUT vs_main(VS_INPUT i) 
{
    VS_OUTPUT o;

    float4 local_pos = float4(i.position,1.0);
    local_pos.xz = mul( float2x2( cos(TIME), sin(TIME), -sin(TIME), cos(TIME) ), local_pos.xz );
    local_pos.xy = mul( float2x2( cos(TIME), sin(TIME), -sin(TIME), cos(TIME) ), local_pos.xy );
	local_pos.xyz += 0.1 * tposition;

    o.position = mul( float4( local_pos.xyz, 1.0 ), view_matrix );
    o.position = mul( o.position, projection_matrix );
    
    o.texcoord = i.texcoord;

    return o;
}

#endif

#ifdef PIXEL_SHADER

struct PS_INPUT {
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

float4 ps_main(PS_INPUT i) : SV_TARGET
{
	float fMask = step( length( fmod( 3.0 * i.texcoord.xy, 1.0 ) - 0.5 ), 0.5 );
    return float4( fMask * float3( 1.0, 1.0, 1.0 ), 1.0 );
}

#endif

#ifdef COMPUTE_SHADER
[numthreads(8,8,1)]
void plain_color_cs(uint3 id: SV_DispatchThreadID )
{
    g_rwtTexture0[ id.xy ] = float3( 1.0 * id.x / width, 1.0 * id.y / height, 0.0 );
}

#endif
)raw";

#endif // GENERATED_HEADER_H
