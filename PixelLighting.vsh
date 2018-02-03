//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

// World matrix, combined view / projection matrix and camera position for transformation
// and lighting calculations
float4x3 WorldMatrix    : WORLD;
float4x4 ViewProjMatrix : VIEWPROJECTION;


//-----------------------------------------------------------------------------
// Input / output structures
//-----------------------------------------------------------------------------

// Input to Vertex Shader - usual position, normal and UVs
struct VS_Input
{
	float3 Position  : POSITION;  // The position of the vertex in model space
	float3 Normal    : NORMAL;
	float2 TexCoord0 : TEXCOORD0;
};

// Output from Vertex Shader. This shader sends the world position and normal to the pixel
// shader to do the lighing calculations. There are no semantics for such a use so we use
// TEXCOORD semantics, this is usual for any non-standard data types passed to a pixel shader.
struct VS_Output
{
	float4 Position      : POSITION;
	float2 TexCoord0     : TEXCOORD0;
	float3 WorldPosition : TEXCOORD1; // Use TEXCOORD semantic for non-standard data - see above 
	float3 Normal        : TEXCOORD2;
};

//-----------------------------------------------------------------------------
// Main function
//-----------------------------------------------------------------------------

// Main vertex shader function. Basic shader that also passes world-space position and normal
// on to the pixel shader

void main( in VS_Input i, out VS_Output o ) 
{
    // Transform model vertex position to world space, then to viewport space. Pass both forms on
    // to the pixel shader
    float3 WorldPosition = mul( float4(i.Position, 1.0f), WorldMatrix );         
    o.Position = mul( float4(WorldPosition, 1.0f), ViewProjMatrix );
    o.WorldPosition = WorldPosition;

    // Transform model normal to world space and pass to the pixel shader
    o.Normal = mul( i.Normal, (float3x3)WorldMatrix );
    
    // Pass texture coordinates directly to pixel shader
    o.TexCoord0 = i.TexCoord0;
}
