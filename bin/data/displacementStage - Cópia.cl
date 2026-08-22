
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
	
	__local float2 _position;
   	__local float _depth;
	__local float _radius;

	_position = *position;
   	_depth    = *depth;
	_radius   = *radius;

    float2 _pos = {get_global_id(0),get_global_id(1)};

    int2 imgCoords = (int2)(_pos.x,_pos.y);

	float4 imgVal = read_imagef(in, sampler, imgCoords);

	float dist = distance(_pos, _position);

    if (dist < _radius)
		imgVal.z -= _depth;

	write_imagef(out, imgCoords, imgVal);	
	
	//barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);	
	//mem_fence(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);	  		       		
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
	
	__local float2 _position;
	__local float4 _force;
	__local float max;

	_position = *position;
	_force    = *force;

	max = fmax(fabs(_force.x), fabs(_force.y));
	max = fmax(fabs(_force.z), max);
	float area = max * 8;
	
    float2 _pos = {get_global_id(0),get_global_id(1)};

    int2 imgCoords = (int2)(_pos.x,_pos.y);

	float4 imgVal = read_imagef(in, sampler, imgCoords);

	float dist = distance(_pos, _position);

	float perc = 1 - dist / area;

	if (perc > 0)
		imgVal += _force * perc;
	
	write_imagef(out, imgCoords, imgVal);		
	
	//barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);	       		
	//mem_fence(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);	       		
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
	
	__local float t;

	t = *time;

    int2 imgCoords = (int2)(get_global_id(0),get_global_id(1));

	float4 imgVal  = read_imagef( in, sampler, imgCoords);
	float4 imgVal2 = read_imagef(in2, sampler, imgCoords);

	float4 imgOut = imgVal * (1 - t) + imgVal2 * t;

	write_imagef(out, imgCoords, imgOut);		
	
	//barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);	
	//mem_fence(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);	
}


//-------------------------------------------------------------------------------------------------
// CUSTOM KERNEL
//-------------------------------------------------------------------------------------------------
__kernel void customKernel(__read_only image2d_t in,
						   __write_only image2d_t out,
						   __global float2 * position,
						   __global float4 * normal,
						   __global float * radius )
{   
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
	
	__local float2 _position;
    __local float4 _normal;
	__local float _radius;

	_position = *position;
    _normal   = *normal;
	_radius   = *radius;

    float2 _pos = {get_global_id(0),get_global_id(1)};

    int2 imgCoords = (int2)(_pos.x,_pos.y);

	float4 imgVal = read_imagef(in, sampler, imgCoords);

	float dist = distance(_pos, _position);
	
    if (dist < _radius)
		imgVal += _normal;

	write_imagef(out, imgCoords, imgVal);	
	
	//barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);	
	//mem_fence(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);	     		
}
