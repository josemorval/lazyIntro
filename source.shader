float3 hash33(uint3 x)
{
	const uint k = 1103515245U;
	x = ((x >> 8U) ^ x.yzx) * k;
	x = ((x >> 8U) ^ x.yzx) * k;
	x = ((x >> 8U) ^ x.yzx) * k;
	return float3(x) * (1.0 / float(0xffffffffU));
}


float4 quaternion_from_axis_angle(float3 axis, float angle)
{
	axis = normalize(axis);
	float2 s;
	sincos(angle * 0.5, s.x, s.y);
	return float4(axis * s.x, s.y);
}

float4x4 quaternion_to_matrix(float4 q)
{
	float x = q.x, y = q.y, z = q.z, w = q.w;

	float4x4 mat;
	mat[0] = float4(1 - 2 * (y * y + z * z), 2 * (x * y - w * z), 2 * (x * z + w * y), 0);
	mat[1] = float4(2 * (x * y + w * z), 1 - 2 * (x * x + z * z), 2 * (y * z - w * x), 0);
	mat[2] = float4(2 * (x * z - w * y), 2 * (y * z + w * x), 1 - 2 * (x * x + y * y), 0);
	mat[3] = float4(0, 0, 0, 1);

	return mat;
}

float4 quaternion_multiply(float4 q1, float4 q2)
{
	return float4(
		q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
		q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,
		q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w,
		q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z
		);
}

float4x4 compute_view_matrix(float3 _position_camera, float3 _view_point)
{

	float3 up = { 0.0f,1.0f,0.0f };
	float3 forward = _view_point - _position_camera;
	forward = normalize(forward);
	float3 right = normalize(cross(up, forward));
	float3 newUp = normalize(cross(forward, right));

	float aa = -dot(right, _position_camera);
	float bb = -dot(newUp, _position_camera);
	float cc = -dot(forward, _position_camera);

	float4x4 view_matrix = {
			right.x,    right.y,    right.z,    -dot(right, _position_camera),
			newUp.x,    newUp.y,    newUp.z,    -dot(newUp, _position_camera),
			forward.x,    forward.y, forward.z,    -dot(forward, _position_camera),
			0.0f,   0.0f,  0.0f,   1.0f
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
		0.0f, 0.0f, a, b,
		0.0f, 0.0f, 1.0f, 0.0f
		);

	return projection_matrix;
}

float4x4 compute_orthographic_matrix(float _width, float _height, float _znear, float _zfar) {
	float4x4 orthographic_matrix = float4x4(
		2.0f / _width, 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / _height, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f / (_zfar - _znear), -_znear / (_zfar - _znear),
		0.0f, 0.0f, 0.0f, 1.0f
		);
	return orthographic_matrix;
}

cbuffer constants : register(b0)
{
	float frame;
	float width;
	float height;
	float _padding0;
	float3 tposition;
	float _padding1;
	float4 trotation;
	float3 tscale;
	float lightpass;
	float3 campos;
	float _padding2;
	float3 camview;
	float nthreads;
}

struct MeshVertex
{
	float3 position;
	float2 texcoord;
	float3 normal;
};

struct InstancePoint
{
	float3 position;
	float4 rotation;
	float3 scale;
	float age;
	float lifetime;
	int trailindex;
	int index;
};

RWStructuredBuffer<MeshVertex> rwbMeshVertex : register(u0);
RWStructuredBuffer<uint> rwbMeshIndices : register(u1);
RWStructuredBuffer<InstancePoint> rwbBufferInstancePoints : register(u2);

StructuredBuffer<MeshVertex> bMeshVertex : register(t0);
StructuredBuffer<uint> bMeshIndices : register(t1);
StructuredBuffer<InstancePoint> bBufferInstancePoints : register(t2);

static float4x4 view_matrix()
{
	return compute_view_matrix(campos, camview);
}

static float4x4 projection_matrix()
{
	return compute_perspective_matrix(1.0, 1.77, 0.1, 50.0);
}

#ifdef VERTEX_SHADER

struct VS_INPUT_VERTEXID
{
	uint	vertexid 	: 	SV_VertexID;
	uint    instanceid  :   SV_InstanceID;
	bool 	isfrontface : VFACE;
};

struct VS_INPUT
{
	float3  position    :   POSITION;
	float2  texcoord    :   TEXCOORD;
	float3  normal      :   NORMAL;
	uint    instanceid  :   SV_InstanceID;
	bool 	isfrontface : VFACE;
};

struct VS_OUTPUT
{
	float4 clip_position : SV_POSITION;
	float3 pos_in_world : POSITION;
	float3 normal_in_world : NORMAL;
	float2 texcoord : TEXCOORD0;
};

VS_OUTPUT vs_main(VS_INPUT i)
{
	VS_OUTPUT o;

	const float4 q = quaternion_from_axis_angle(trotation.xyz, trotation.w);
	const float4x4 rot_matrix = quaternion_to_matrix(q);

	const float4 pos_in_local = float4(tscale * i.position, 1.0);
	const float4 pos_in_world = float4(tposition, 0.0) + mul(rot_matrix, pos_in_local);

	const float4 pos_in_view = mul(view_matrix(), pos_in_world);
	o.clip_position = mul(projection_matrix(), pos_in_view);
	o.pos_in_world = pos_in_world.xyz;
	o.normal_in_world = mul(rot_matrix, (i.isfrontface ? -1.0 : 1.0) * i.normal).xyz;
	o.texcoord = i.texcoord;

	return o;
}

VS_OUTPUT vs_instanced_main(VS_INPUT i)
{
	VS_OUTPUT o;

	InstancePoint ip = bBufferInstancePoints[i.instanceid];

	const float4 q = quaternion_from_axis_angle(ip.rotation.xyz, ip.rotation.w);
	const float4x4 rot_matrix = quaternion_to_matrix(q);

	const float4 pos_in_local = float4(ip.scale * i.position, 1.0);
	const float4 pos_in_world = float4(ip.position, 0.0) + mul(rot_matrix, pos_in_local);
	const float4 pos_in_view = mul(view_matrix(), pos_in_world);

	o.clip_position = mul(projection_matrix(), pos_in_view);
	o.pos_in_world = pos_in_world.xyz;
	o.normal_in_world = mul(rot_matrix, (i.isfrontface ? -1.0 : 1.0) * i.normal).xyz;
	o.texcoord = i.texcoord;

	return o;
}

VS_OUTPUT vs_vertexid_main(VS_INPUT_VERTEXID i)
{
	VS_OUTPUT o;

	uint index = bMeshIndices[i.vertexid];
	const float3 iposition = bMeshVertex[index].position;
	const float3 inormal = bMeshVertex[index].normal;
	const float2 itexcoord = bMeshVertex[index].texcoord;

	const float4 q = quaternion_from_axis_angle(trotation.xyz, trotation.w);
	const float4x4 rot_matrix = quaternion_to_matrix(q);

	const float4 pos_in_local = float4(tscale * iposition, 1.0);
	const float4 pos_in_world = float4(tposition, 0.0) + mul(rot_matrix, pos_in_local);
	const float4 pos_in_view = mul(view_matrix(), pos_in_world);

	o.clip_position = mul(projection_matrix(), pos_in_view);
	o.pos_in_world = pos_in_world.xyz;
	o.normal_in_world = mul(rot_matrix, (i.isfrontface ? 1.0 : -1.0) * inormal).xyz;
	o.texcoord = itexcoord;

	return o;
}

VS_OUTPUT vs_vertexid_instanced_main(VS_INPUT_VERTEXID i)
{
	VS_OUTPUT o;

	uint index = bMeshIndices[i.vertexid];
	const float3 iposition = bMeshVertex[index].position;
	const float3 inormal = bMeshVertex[index].normal;
	const float2 itexcoord = bMeshVertex[index].texcoord;

	InstancePoint ip = bBufferInstancePoints[i.instanceid];

	const float4 q = quaternion_from_axis_angle(ip.rotation.xyz, ip.rotation.w);
	const float4x4 rot_matrix = quaternion_to_matrix(q);

	const float4 pos_in_local = float4(ip.scale * iposition, 1.0);
	const float4 pos_in_world = float4(bBufferInstancePoints[i.instanceid].position, 0.0) + mul(rot_matrix, pos_in_local);
	const float4 pos_in_view = mul(view_matrix(), pos_in_world);

	o.clip_position = mul(projection_matrix(), pos_in_view);
	o.pos_in_world = pos_in_world.xyz;
	o.normal_in_world = mul(rot_matrix, inormal).xyz;
	o.texcoord = itexcoord;

	return o;
}

#endif

#ifdef PIXEL_SHADER

struct PS_INPUT
{
	float4 pixel_pos : SV_POSITION;
	float3 pos_in_world : POSITION;
	float3 normal_in_world : NORMAL;
	float2 texcoord : TEXCOORD0;
};

float4 ps_main(PS_INPUT i) : SV_TARGET
{
	float fDist = length(i.texcoord.xy - 0.5);
	float fFade = smoothstep(10.0, 0.0, length(i.pos_in_world.xyz));
	float3 vColor = float3(0.5, 0.5, 0.5) * fFade;

	return float4(vColor, 1.0);
}

float4 ps_ground_particles(PS_INPUT i) : SV_TARGET
{
	float fDist = length(i.texcoord.xy - 0.5);
	float fFade = smoothstep(10.0, 0.0, length(i.pos_in_world.xyz));
	float3 vColor = float3(0.5, 0.5, 0.5) * fFade;
	
	if (fDist > 0.5) discard;

	return float4( vColor, 1.0 );
}

#endif

#ifdef COMPUTE_SHADER

[numthreads(64, 1, 1)]
void cs_generate_sphere(uint3 id: SV_DispatchThreadID)
{
	const int slice1 = 35;
	const int slice2 = 20;

	int a = id.x % slice1;
	int b = id.x / slice1;

	if (id.x < slice1 * slice2)
	{
		float theta = 2.0 * 3.14159 * a / (slice1 - 1);
		float phi = 0.0 + 1.0 * 1.0 * 3.14159 * b / (slice2 - 1);

		float x = sin(phi) * sin(theta);
		float y = cos(phi);
		float z = sin(phi) * cos(theta);

		rwbMeshVertex[id.x].position = float3(x, y, z);
		rwbMeshVertex[id.x].normal = normalize(float3(x, y, z));
		rwbMeshVertex[id.x].texcoord = float2(theta, phi);
	}
	else {
		rwbMeshVertex[id.x].position = float3(0, 0, 0);
		rwbMeshVertex[id.x].normal = normalize(float3(0, 0, 1));
		rwbMeshVertex[id.x].texcoord = float2(0.0, 0.0);
	}

	if (id.x < slice1 * (slice2 - 1))
	{
		rwbMeshIndices[6 * id.x] = a + b * slice1;
		rwbMeshIndices[6 * id.x + 1] = a + (b + 1) * slice1;
		rwbMeshIndices[6 * id.x + 2] = (a + 1) % slice1 + b * slice1;

		rwbMeshIndices[6 * id.x + 3] = (a + 1) % slice1 + b * slice1;
		rwbMeshIndices[6 * id.x + 4] = a + (b + 1) * slice1;
		rwbMeshIndices[6 * id.x + 5] = (a + 1) % slice1 + (b + 1) * slice1;
	}
	else
	{
		for (int i = 0; i < 6; i++)
		{
			rwbMeshIndices[6 * id.x + i] = 0;
		}
	}
}


#define TIME 0.03*frame

[numthreads(64, 1, 1)]
void cs_rotating_cubes(uint3 id: SV_DispatchThreadID)
{
	if (id.x > nthreads) return;

	InstancePoint ip;
	ip = rwbBufferInstancePoints[id.x];

	float fNormalizedId = id.x / (nthreads - 1.0 );
	float fT = TIME;

	float fRotCube = 3.14159 / 2.0 * smoothstep( 0.0, 1.0, frac(fT));
	float fHeight = 0.25 * sin( 3.14159 * frac(fT));

	ip.position = float3(0.0, fHeight, 0.0);
	ip.rotation = float4(1.0, 0.0, 0.0, fRotCube);
	ip.scale = float3(2.0, 2.0, 2.0);

	rwbBufferInstancePoints[id.x] = ip;
}

[numthreads(64, 1, 1)]
void cs_ground_particles(uint3 id: SV_DispatchThreadID)
{
	if (id.x > nthreads) return;

	InstancePoint ip;
	ip = rwbBufferInstancePoints[id.x];

	float fNormalizedId = id.x / (nthreads - 1.0);
	float2 vIndex2D = float2(id.x % 50, id.x / 50);
	float fT = TIME;

	float3 vPosition = float3(vIndex2D.x - 25.0, -1.0, vIndex2D.y - 25.0);
	vPosition.z -= 2.0 * smoothstep(0.0, 1.0, frac(fT));
	vPosition.y -= smoothstep( 4.0, 0.0, length(vPosition.xz) );
	ip.position = vPosition;
	ip.rotation = float4(1.0, 0.0, 0.0, 1.57);
	ip.scale = float3(0.5,0.5,0.5);

	rwbBufferInstancePoints[id.x] = ip;
}


#endif