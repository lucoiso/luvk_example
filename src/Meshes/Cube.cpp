// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/Meshes/Cube.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <luvk/Libraries/ShaderCompiler.hpp>
#include <luvk/Modules/SwapChain.hpp>
#include <luvk/Resources/Buffer.hpp>
#include <luvk/Resources/DescriptorSet.hpp>
#include <luvk/Resources/Pipeline.hpp>
#include <luvk/Types/Material.hpp>

using namespace luvk_example;

constexpr auto g_TaskShader = R"(
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

constexpr auto g_MeshShader = R"(
[[vk::binding(0, 0)]] cbuffer Uniforms
{
    float4x4 modelViewProjection;
}

[[vk::binding(1, 0)]] RWStructuredBuffer<float> outputBuffer;

const static float3 cube_positions[8] = {
    float3(-0.5, -0.5,  0.5),
    float3( 0.5, -0.5,  0.5),
    float3( 0.5,  0.5,  0.5),
    float3(-0.5,  0.5,  0.5),
    float3(-0.5, -0.5, -0.5),
    float3( 0.5, -0.5, -0.5),
    float3( 0.5,  0.5, -0.5),
    float3(-0.5,  0.5, -0.5)
};

const static uint3 cube_indices[12] = {
    uint3(0, 1, 2), uint3(0, 2, 3),
    uint3(5, 4, 7), uint3(5, 7, 6),
    uint3(1, 5, 6), uint3(1, 6, 2),
    uint3(4, 0, 3), uint3(4, 3, 7),
    uint3(3, 2, 6), uint3(3, 6, 7),
    uint3(4, 5, 1), uint3(4, 1, 0)
};

const static float3 cube_colors[6] = {
    float3(1.0, 0.0, 0.0),
    float3(0.0, 1.0, 0.0),
    float3(0.0, 0.0, 1.0),
    float3(1.0, 1.0, 0.0),
    float3(1.0, 0.0, 1.0),
    float3(0.0, 1.0, 1.0)
};

struct Vertex
{
    float4 pos : SV_Position;
    float3 color : Color;
};

const static uint MAX_VERTS = 36;
const static uint MAX_PRIMS = 12;

[outputtopology("triangle")]
[numthreads(12, 1, 1)]
[shader("mesh")]
void meshMain(
    in uint tig : SV_GroupIndex,
    OutputVertices<Vertex, MAX_VERTS> verts,
    OutputIndices<uint3, MAX_PRIMS> triangles)
{
    const uint numVertices = MAX_VERTS;
    const uint numPrimitives = MAX_PRIMS;
    SetMeshOutputCounts(numVertices, numPrimitives);

    if(tig < numPrimitives)
    {
        uint baseVertexIndex = tig * 3;
        triangles[tig] = baseVertexIndex + uint3(0, 1, 2);

        uint3 indices = cube_indices[tig];
        float3 faceColor = cube_colors[tig / 2];

        for(int i = 0; i < 3; i++)
        {
            uint vertexIndex = indices[i];
            float3 position = cube_positions[vertexIndex];
            float4 transformedPos = mul(modelViewProjection, float4(position, 1.0));
            verts[baseVertexIndex + i] = {transformedPos, faceColor};
        }
    }
}
)";

constexpr auto g_FragmentShader = R"(
struct PrimData
{
    float3 color : COLOR0;
};

[shader("fragment")]
float4 entry_fragment(PrimData input) : SV_Target
{
    return float4(input.color, 1.0f);
}
)";

constexpr VkDescriptorSetLayoutBinding g_UboBinding{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_MESH_BIT_EXT, nullptr};

Cube::Cube(const std::shared_ptr<luvk::Device>&         Device,
           const std::shared_ptr<luvk::SwapChain>&      Swap,
           const std::shared_ptr<luvk::Memory>&         Memory,
           const std::shared_ptr<luvk::DescriptorPool>& Pool)
    : Mesh(Device, Memory),
      m_Pipeline(std::make_shared<luvk::Pipeline>(Device)),
      m_UBO(std::make_shared<luvk::Buffer>(Device, Memory)),
      m_Mat(std::make_shared<luvk::Material>())
{
    const auto Set = std::make_shared<luvk::DescriptorSet>(Device, Pool, Memory);
    Set->CreateLayout({.Bindings = luvk::Array{g_UboBinding}});
    Set->Allocate();

    m_Pipeline->CreateMeshPipeline({.Extent = Swap->GetExtent(),
                                    .ColorFormats = luvk::Array{Swap->GetCreationArguments().Format},
                                    .RenderPass = Swap->GetRenderPass(),
                                    .Subpass = 0,
                                    .TaskShader = luvk::CompileShader(g_TaskShader),
                                    .MeshShader = luvk::CompileShader(g_MeshShader),
                                    .FragmentShader = luvk::CompileShader(g_FragmentShader),
                                    .SetLayouts = luvk::Array{Set->GetLayout()},
                                    .CullMode = VK_CULL_MODE_NONE});

    m_UBO->CreateBuffer({.Name = "Cube Uniform",
                         .Size = sizeof(glm::mat4),
                         .Usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                         .MemoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU});

    Set->UpdateBuffer(m_UBO->GetHandle(), m_UBO->GetSize(), 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

    m_Mat->SetPipeline(m_Pipeline);
    m_Mat->SetDescriptorSet(Set);

    SetMaterial(m_Mat);
    SetDispatchCount(1, 1, 1);
}

void Cube::Update(const float DeltaTime, glm::mat4 const& View, glm::mat4 const& Proj) const
{
    static constinit float Elapsed = 0.F;
    Elapsed                        += DeltaTime;

    const glm::mat4 Model = glm::rotate(glm::mat4(1.F), Elapsed, glm::vec3(0.F, 1.F, 0.F));
    m_Mvp                 = Proj * View * Model;

    m_UBO->Upload(std::as_bytes(std::span{&m_Mvp, 1}));
}
