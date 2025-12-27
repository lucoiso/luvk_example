/*
 * Author: Lucas Vilas-Boas
 * Year: 2025
 * Repo: https://github.com/lucoiso/luvk_example
 */

#include "UserInterface/Meshes/Cube.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <luvk/Libraries/ShaderCompiler.hpp>
#include <luvk/Modules/SwapChain.hpp>
#include <luvk/Resources/DescriptorSet.hpp>
#include <luvk/Resources/Pipeline.hpp>
#include <luvk/Types/Material.hpp>
#include "UserInterface/Application/Application.hpp"
#include "UserInterface/Components/Camera.hpp"

using namespace UserInterface;

constexpr auto g_CubeTaskShader = R"(
struct DummyPayLoad {
    uint dummyData;
};

groupshared DummyPayLoad dummyPayLoad;

[shader("amplification")]
[numthreads(1, 1, 1)]
void entry_amplification()
{
    DispatchMesh(1, 1, 1, dummyPayLoad);
}
)";

constexpr auto g_CubeMeshShader = R"(
struct PC {
    float4x4 modelViewProjection;
};

[[vk::push_constant]] PC pc;

struct VertexOutput
{
    float4 pos : SV_Position;
    float3 color : Color;
};

float3 GetPosition(uint index) {
    switch(index) {
        case 0: return float3(-0.5, -0.5,  0.5);
        case 1: return float3( 0.5, -0.5,  0.5);
        case 2: return float3( 0.5,  0.5,  0.5);
        case 3: return float3(-0.5,  0.5,  0.5);
        case 4: return float3(-0.5, -0.5, -0.5);
        case 5: return float3( 0.5, -0.5, -0.5);
        case 6: return float3( 0.5,  0.5, -0.5);
        case 7: return float3(-0.5,  0.5, -0.5);
        default: return float3(0,0,0);
    }
}

uint3 GetIndices(uint index) {
    switch(index) {
        case 0: return uint3(0, 1, 2);
        case 1: return uint3(0, 2, 3);
        case 2: return uint3(5, 4, 7);
        case 3: return uint3(5, 7, 6);
        case 4: return uint3(1, 5, 6);
        case 5: return uint3(1, 6, 2);
        case 6: return uint3(4, 0, 3);
        case 7: return uint3(4, 3, 7);
        case 8: return uint3(3, 2, 6);
        case 9: return uint3(3, 6, 7);
        case 10: return uint3(4, 5, 1);
        case 11: return uint3(4, 1, 0);
        default: return uint3(0,0,0);
    }
}

float3 GetColor(uint index) {
    switch(index) {
        case 0: return float3(1.0, 0.0, 0.0);
        case 1: return float3(0.0, 1.0, 0.0);
        case 2: return float3(0.0, 0.0, 1.0);
        case 3: return float3(1.0, 1.0, 0.0);
        case 4: return float3(1.0, 0.0, 1.0);
        case 5: return float3(0.0, 1.0, 1.0);
        default: return float3(1,1,1);
    }
}

#define MAX_VERTS 36
#define MAX_PRIMS 12

[outputtopology("triangle")]
[numthreads(12, 1, 1)]
[shader("mesh")]
void meshMain(
    in uint tig : SV_GroupIndex,
    OutputVertices<VertexOutput, MAX_VERTS> verts,
    OutputIndices<uint3, MAX_PRIMS> triangles)
{
    const uint numVertices = MAX_VERTS;
    const uint numPrimitives = MAX_PRIMS;
    SetMeshOutputCounts(numVertices, numPrimitives);

    if(tig < numPrimitives)
    {
        uint baseVertexIndex = tig * 3;
        triangles[tig] = baseVertexIndex + uint3(0, 1, 2);

        uint3 indices = GetIndices(tig);
        float3 faceColor = GetColor(tig / 2);

        for(int i = 0; i < 3; i++)
        {
            uint vertexIndex = indices[i];
            float3 position = GetPosition(vertexIndex);
            float4 transformedPos = mul(pc.modelViewProjection, float4(position, 1.0));

            verts[baseVertexIndex + i].pos = transformedPos;
            verts[baseVertexIndex + i].color = faceColor;
        }
    }
}
)";

constexpr auto g_CubeFragmentShader = R"(
struct PrimData
{
    float4 pos : SV_Position;
    float3 color : Color;
};

[shader("fragment")]
float4 entry_fragment(PrimData input) : SV_Target
{
    return float4(input.color, 1.0f);
}
)";

Cube::Cube(const std::shared_ptr<luvk::Device>&         Device,
           const std::shared_ptr<luvk::SwapChain>&      Swap,
           const std::shared_ptr<luvk::Memory>&         Memory,
           const std::shared_ptr<luvk::DescriptorPool>& Pool)
    : Mesh(Device, Memory),
      m_Pipeline(std::make_shared<luvk::Pipeline>(Device))
{
    constexpr VkPushConstantRange PushConstantRange{VK_SHADER_STAGE_MESH_BIT_EXT, 0, sizeof(glm::mat4)};

    const auto Set = std::make_shared<luvk::DescriptorSet>(Device, Pool, Memory);
    Set->CreateLayout({});
    Set->Allocate();

    m_Pipeline->CreateMeshPipeline({.Extent = Swap->GetExtent(),
                                    .ColorFormats = std::array{Swap->GetCreationArguments().Format},
                                    .RenderPass = Swap->GetRenderPass(),
                                    .Subpass = 0,
                                    .TaskShader = luvk::CompileShader(g_CubeTaskShader, "spirv_1_4"),
                                    .MeshShader = luvk::CompileShader(g_CubeMeshShader, "spirv_1_4"),
                                    .FragmentShader = luvk::CompileShader(g_CubeFragmentShader),
                                    .SetLayouts = std::array{Set->GetLayout()},
                                    .PushConstants = std::array{PushConstantRange},
                                    .CullMode = VK_CULL_MODE_NONE});

    m_Material = std::make_shared<luvk::Material>();
    m_Material->SetPipeline(m_Pipeline);
    m_Material->SetDescriptorSet(Set);

    SetDispatchCount(1, 1, 1);
}

void Cube::Tick(const float DeltaTime)
{
    const auto AppInstance = Application::GetInstance();

    glm::mat4 Proj = glm::perspective(glm::radians(45.F), static_cast<float>(AppInstance->GetWidth()) / static_cast<float>(AppInstance->GetHeight()), 0.1F, 10.F);
    Proj[1][1]     *= -1.F;

    static constinit float Elapsed = 0.F;
    Elapsed                        += DeltaTime;

    const glm::mat4 Model = glm::rotate(glm::mat4(1.F), Elapsed, glm::vec3(0.F, 1.F, 0.F));
    m_Mvp                 = Proj * AppInstance->GetCamera()->GetViewMatrix() * Model;

    SetPushConstantData(std::as_bytes(std::span{&m_Mvp, 1}));
}
