
#ifndef FULL_SCREEN_TRIANGLE_HLSL
#define FULL_SCREEN_TRIANGLE_HLSL

//A vertex shader that uses the vertex ID to generate a full-screen quad. 
// Don't bind vertex buffer, index buffer or input layout. 
// Just render three vertices!
// https://m.blog.naver.com/hsg556/110141428985
// 삼각형을 사용하여 NDC 공간을 덮습니다.
// (-1, 1)________ (3, 1)
//        |   |  /
// (-1,-1)|___|/ (1, -1)   
//        |  /
// (-1,-3)|/      
float4 VS_FullScreenTriangle(uint vertexID : SV_VertexID) : SV_Position
{
    float2 grid = float2((vertexID << 1) & 2, vertexID & 2);
    float2 xy = grid * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
    return float4(xy, 1.0f, 1.0f);
}

struct VS_OUT
{
    float4 posH     : SV_Position;
    float2 texcoord : TEXCOORD;
};

VS_OUT VS_FullScreenTriangleTexcoord(uint vertexID : SV_VertexID)
    
{
    VS_OUT output;

    float2 grid = float2((vertexID << 1) & 2, vertexID & 2);
    float2 xy = grid * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);

    output.texcoord     = grid * float2(1.0f, 1.0f);
    output.posH         = float4(xy, 1.0f, 1.0f);

    return output;
}

#endif // FULL_SCREEN_TRIANGLE_HLSL
