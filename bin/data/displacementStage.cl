// AMD APP Profiler
//http://developer.amd.com/tools/AMDAPPProfiler/Pages/default.aspx

//-------------------------------------------------------------------------------------------------
// CONTACT KERNEL
//-------------------------------------------------------------------------------------------------
__kernel void contactKernel(__read_only image2d_t in,
						    __write_only image2d_t out,
						    __global float2 * position,
						    __global float * depth,
							__global float * radius )
{   
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
	
	__local float2 _position; _position = *position;
   	__local float _depth;     _depth    = *depth;
	__local float _radius;    _radius   = *radius;
	
    float2 _pos    = { get_global_id(0), get_global_id(1) };
	int2 imgCoords = (int2)(_pos.x, _pos.y);
	float4 imgVal  = read_imagef(in, sampler, imgCoords);
	float dist     = fast_distance(_pos, _position);

    if (dist < _radius)
		imgVal.z -= _depth;

	write_imagef(out, imgCoords, imgVal);	
	
	//barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);	
	mem_fence(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);	  		       		
}


//-------------------------------------------------------------------------------------------------
// FORCE KERNEL
//-------------------------------------------------------------------------------------------------
__kernel void forceKernel(__read_only image2d_t in,
						  __write_only image2d_t out,
						  __global float2 * position,
						  __global float4 * force)
{   
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
	
	__local float2 _position; _position = *position;
	__local float4 _force;    _force    = *force;

	float length = fast_length(_force.xyz);
	float area   = length * _force.w;
	
	float2 _pos    = { get_global_id(0), get_global_id(1) };
	int2 imgCoords = (int2)(_pos.x, _pos.y);
	float4 imgVal  = read_imagef(in, sampler, imgCoords);
	float dist     = fast_distance(_pos, _position);
	float perc     = 1 - dist / area;

	if (perc > 0)
		imgVal += _force * perc;
	
	write_imagef(out, imgCoords, imgVal);		
	
	//barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);	       		
	mem_fence(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);	       		
}


//-------------------------------------------------------------------------------------------------
// MORPHING KERNEL
//-------------------------------------------------------------------------------------------------
__kernel void morphingKernel(__read_only image2d_t in,
							 __read_only image2d_t in2,
						     __write_only image2d_t out,
						     __global float * time)
{   
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
	
	__local float t; t = *time;

    int2 imgCoords = (int2)(get_global_id(0), get_global_id(1));
	float4 imgVal  = read_imagef( in, sampler, imgCoords);
	float4 imgVal2 = read_imagef(in2, sampler, imgCoords);
	float4 imgOut  = imgVal * (1 - t) + imgVal2 * t;

	write_imagef(out, imgCoords, imgOut);		
	
	//barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);	
	mem_fence(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);	
}


//-------------------------------------------------------------------------------------------------
// CUSTOM KERNEL
//-------------------------------------------------------------------------------------------------
__kernel void customKernel(__read_only image2d_t in,
					       __write_only image2d_t out,
					       __global float4 * arg1,
					       __global float4 * arg2,
					       __global float4 * arg3,
					       __global float4 * arg4 )
{   
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
	
    int x = get_global_id(0);
	int y = get_global_id(1);
	
	int2 imgCoords = (int2)(x,y);

	float4 imgVal  = read_imagef(in, sampler, imgCoords);

	//insira seu código aqui! 
	//if (x%2==0) imgVal.x -= 0.1f;

	write_imagef(out, imgCoords, imgVal);	

	//barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);	
	mem_fence(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);	
}


//-------------------------------------------------------------------------------------------------
// TEST KERNEL
//-------------------------------------------------------------------------------------------------
__kernel void testKernel(__read_only image2d_t in,
					     __write_only image2d_t out,
					     __global float4 * arg1,
					     __global float4 * arg2,
					     __global float4 * arg3,
					     __global float4 * arg4 )
{   
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;

	__local float4 _position; _position = *arg1;
    __local float4 _normal;   _normal   = *arg2;
	__local float4 _radius;   _radius   = *arg3;
	
    int x = get_global_id(0);
	int y = get_global_id(1);
	
	int2 imgCoords = (int2)(x,y);

	float4 imgVal  = read_imagef(in, sampler, imgCoords);

	//insira seu código aqui! 
	float dist = fast_distance((float2)(x,y), _position.xy);
		
    if (dist < _radius.x)
		imgVal += _normal;

	write_imagef(out, imgCoords, imgVal);	

	//barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);	
	mem_fence(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);	 
}