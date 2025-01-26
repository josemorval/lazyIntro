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
int frame;
int width;
int height;

#define TIME 0.001*frame

static float4x4 view_matrix = compute_view_matrix(float3(0.0,0.0,5.0), float3(0.0,0.0,0.0));
static float4x4 projection_matrix = compute_perspective_matrix(1.0,1.6,0.1,50.0);

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

    o.position = mul( float4( local_pos.xyz, 1.0 ), view_matrix );
    o.position = mul( o.position, projection_matrix );
    
    o.texcoord = i.texcoord;

    return o;
}

float4 ps_main(VS_OUTPUT i) : SV_TARGET
{
    return float4( i.texcoord, 0.0, 1.0 );
}

RWTexture2D<float3> g_rwtTexture0;
[numthreads(8,8,1)]
void plain_color_cs(uint3 id: SV_DispatchThreadID )
{
    g_rwtTexture0[ id.xy ] = float3( 1.0 * id.x / width, 1.0 * id.y / height, 0.0 );
}