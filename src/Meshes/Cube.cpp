// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/Meshes/Cube.hpp"
#include <array>
#include <glm/gtc/matrix_transform.hpp>
#include <luvk/Libraries/ShaderCompiler.hpp>

using namespace luvk_example;

constexpr auto g_MeshShader = R"(
    #version 460
    #extension GL_EXT_mesh_shader : enable

    layout(local_size_x = 1) in;
    layout(triangles, max_vertices = 8, max_primitives = 12) out;

    layout(push_constant) uniform Push
    {
        mat4 mvp;
    } pc;

    struct PrimData
    {
        vec3 color;
    };

    layout(location = 0) perprimitiveEXT out PrimData prim[];

    void main()
    {
        const vec3 positions[8] = vec3[8](vec3(-0.5,-0.5,-0.5),
                                         vec3(0.5,-0.5,-0.5),
                                         vec3(0.5,0.5,-0.5),
                                         vec3(-0.5,0.5,-0.5),
                                         vec3(-0.5,-0.5,0.5),
                                         vec3(0.5,-0.5,0.5),
                                         vec3(0.5,0.5,0.5),
                                         vec3(-0.5,0.5,0.5));

        const uint indices[36] = uint[36](0,1,2, 2,3,0,
                                          4,5,6, 6,7,4,
                                          0,1,5, 5,4,0,
                                          2,3,7, 7,6,2,
                                          1,2,6, 6,5,1,
                                          3,0,4, 4,7,3);

        const vec3 colors[6] = vec3[6](vec3(1,0,0), vec3(0,1,0), vec3(0,0,1), vec3(1,1,0), vec3(1,0,1), vec3(0,1,1));

        SetMeshOutputsEXT(8, 12);

        for (uint v = 0; v < 8; ++v)
        {
            gl_MeshVerticesEXT[v].gl_Position = pc.mvp * vec4(positions[v], 1.0);
        }

        for (uint face = 0; face < 6; ++face)
        {
            for (uint t = 0; t < 2; ++t)
            {
                uint primId = face * 2 + t;
                uint base = primId * 3;
                gl_PrimitiveTriangleIndicesEXT[primId] = uvec3(indices[base], indices[base + 1], indices[base + 2]);
                prim[primId].color = colors[face];
            }
        }
    }
)";

constexpr auto g_FragmentShader = R"(
    #version 460
    #extension GL_EXT_mesh_shader : enable

    layout(location = 0) out vec4 outColor;
    layout(location = 0) perprimitiveEXT in PrimData
    {
        vec3 color;
    } prim;

    void main()
    {
        outColor = vec4(prim.color, 1.0);
    }
)";

constexpr VkPushConstantRange g_ConstantRange{VK_SHADER_STAGE_MESH_BIT_EXT, 0, sizeof(glm::mat4)};

Cube::Cube(const std::shared_ptr<luvk::MeshRegistry>& Registry,
           const std::shared_ptr<luvk::Device>& Device,
           const std::shared_ptr<luvk::SwapChain>& Swap,
           const std::shared_ptr<luvk::Memory>& Memory)
    : m_Registry(Registry),
      m_Pipeline(std::make_shared<luvk::Pipeline>(Device)),
      m_UBO(std::make_shared<luvk::Buffer>(Device, Memory))
{
    m_Pipeline->CreateMeshPipeline({.Extent = Swap->GetExtent(),
                                    .ColorFormats = std::array{Swap->GetCreationArguments().Format},
                                    .RenderPass = Swap->GetRenderPass(),
                                    .Subpass = 0,
                                    .TaskShader = {},
                                    .MeshShader = luvk::CompileGLSLToSPIRV(g_MeshShader, EShLangMesh),
                                    .FragmentShader = luvk::CompileGLSLToSPIRV(g_FragmentShader, EShLangFragment),
                                    .SetLayouts = {},
                                    .PushConstants = std::array{g_ConstantRange},
                                    .CullMode = VK_CULL_MODE_NONE});

    m_UBO->CreateBuffer({.Name = "Cube Uniform",
                         .Size = sizeof(glm::mat4),
                         .Usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                         .MemoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU});

    m_Index = m_Registry->RegisterMesh({},
                                       {},
                                       VK_NULL_HANDLE,
                                       nullptr,
                                       nullptr,
                                       nullptr,
                                       m_UBO,
                                       {},
                                       m_Pipeline);

    m_Mesh = std::make_shared<luvk::Mesh>(m_Registry, m_Index);
}

void Cube::Update(const float DeltaTime, glm::mat4 const& View, glm::mat4 const& Proj) const
{
    static constinit float Elapsed = 0.F;
    Elapsed += DeltaTime;

    const glm::mat4 Model = glm::rotate(glm::mat4(1.F), Elapsed, glm::vec3(0.F, 1.F, 0.F));
    const glm::mat4 Mvp = Proj * View * Model;

    m_Registry->UpdateUniform(m_Index, std::as_bytes(std::span{ &Mvp, 1 }));
}
