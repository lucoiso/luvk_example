// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/Cube.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <array>
#include <luvk/Libraries/ShaderCompiler.hpp>

using namespace luvk_example;

namespace
{
    constexpr std::array<glm::vec3, 8> CubeVertices{{{-0.5f, -0.5f, -0.5f},
                                                     { 0.5f, -0.5f, -0.5f},
                                                     { 0.5f,  0.5f, -0.5f},
                                                     {-0.5f,  0.5f, -0.5f},
                                                     {-0.5f, -0.5f,  0.5f},
                                                     { 0.5f, -0.5f,  0.5f},
                                                     { 0.5f,  0.5f,  0.5f},
                                                     {-0.5f,  0.5f,  0.5f}}};
    constexpr std::array<std::uint16_t, 36> CubeIndices{{0, 1, 2, 2, 3, 0,
                                                         4, 5, 6, 6, 7, 4,
                                                         0, 1, 5, 5, 4, 0,
                                                         2, 3, 7, 7, 6, 2,
                                                         1, 2, 6, 6, 5, 1,
                                                         3, 0, 4, 4, 7, 3}};

    constexpr auto CubeMeshSrc = R"(#version 460
                                    #extension GL_EXT_mesh_shader : enable
                                    layout(local_size_x = 1) in;
                                    layout(max_vertices = 36, max_primitives = 12) out;
                                    layout(triangles) out;
                                    layout(push_constant) uniform Push {
                                        mat4 mvp;
                                        vec4 color;
                                    } pc;
                                    layout(location = 0) out vec4 vColor[];
                                    void main() {
                                        vec3 pos[8] = vec3[](
                                            vec3(-0.5,-0.5,-0.5), vec3(0.5,-0.5,-0.5), vec3(0.5,0.5,-0.5), vec3(-0.5,0.5,-0.5),
                                            vec3(-0.5,-0.5,0.5), vec3(0.5,-0.5,0.5), vec3(0.5,0.5,0.5), vec3(-0.5,0.5,0.5));
                                        uint idx[36] = uint[](
                                            0,1,2,2,3,0,
                                            4,5,6,6,7,4,
                                            0,1,5,5,4,0,
                                            2,3,7,7,6,2,
                                            1,2,6,6,5,1,
                                            3,0,4,4,7,3);
                                        for(uint i=0;i<36;i++){
                                            gl_MeshVerticesEXT[i].gl_Position = pc.mvp * vec4(pos[idx[i]],1.0);
                                            vColor[i] = pc.color;
                                        }
                                        for(uint i=0;i<12;i++){
                                            gl_MeshPrimitivesEXT[i].firstVertex = i*3;
                                            gl_MeshPrimitivesEXT[i].primitiveCount = 1;
                                        }
                                    })";

    constexpr auto CubeFragSrc = R"(#version 450
                                    layout(location = 0) in vec4 vColor;
                                    layout(location = 0) out vec4 outColor;
                                    void main() {
                                        outColor = vColor;
                                    })";
}

Cube::Cube(std::shared_ptr<luvk::MeshRegistry> Registry,
           std::shared_ptr<luvk::Device> Device,
           std::shared_ptr<luvk::SwapChain> SwapChain,
           std::shared_ptr<luvk::Memory> Memory)
    : m_Registry(std::move(Registry))
{
    m_Pipeline = std::make_shared<luvk::Pipeline>();
    auto MeshSpv = luvk::CompileGLSLToSPIRV(CubeMeshSrc, EShLangMeshNV);
    auto FragSpv = luvk::CompileGLSLToSPIRV(CubeFragSrc, EShLangFragment);

    const VkExtent2D Extent = SwapChain->GetExtent();
    const std::array Formats{SwapChain->m_Arguments.Format};
    constexpr VkPushConstantRange PC{VK_SHADER_STAGE_MESH_BIT_EXT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(glm::mat4) + sizeof(glm::vec4)};
    m_Pipeline->CreateMeshPipeline(Device,
                                   {.Extent = Extent,
                                    .ColorFormats = Formats,
                                    .RenderPass = SwapChain->GetRenderPass(),
                                    .Subpass = 0,
                                    .MeshShader = MeshSpv,
                                    .FragmentShader = FragSpv,
                                    .SetLayouts = {},
                                    .PushConstants = std::array{PC}});

    auto Ubo = std::make_shared<luvk::Buffer>();
    Ubo->CreateBuffer(Memory,
                      {.Usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                       .Size = sizeof(glm::mat4) + sizeof(glm::vec4),
                       .MemoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU});

    m_Index = m_Registry->RegisterMesh(std::as_bytes(std::span{CubeVertices}),
                                       std::as_bytes(std::span{CubeIndices}),
                                       VK_NULL_HANDLE,
                                       nullptr,
                                       nullptr,
                                       nullptr,
                                       Ubo,
                                       {},
                                       m_Pipeline,
                                       Device);
    m_Mesh = luvk::Mesh(m_Registry, m_Index);
}

void Cube::Update(const float DeltaTime, glm::mat4 const& View, glm::mat4 const& Proj) const
{
    static float Elapsed = 0.f;
    Elapsed += DeltaTime;

    const glm::mat4 Model = glm::rotate(glm::mat4(1.f), Elapsed, glm::vec3(0.f, 1.f, 0.f));
    const glm::mat4 Mvp = Proj * View * Model;

    const glm::vec4 Color{0.5f + 0.5f * std::sin(Elapsed), 0.5f + 0.5f * std::sin(Elapsed + 2.094f), 0.5f + 0.5f * std::sin(Elapsed + 4.188f), 1.f};
    struct
    {
        glm::mat4 Mvp;
        glm::vec4 Color;
    } Pc{Mvp, Color};

    m_Registry->UpdateUniform(m_Index, std::as_bytes(std::span{&Pc, 1}));
}

luvk::Mesh& Cube::GetMesh() noexcept
{
    return m_Mesh;
}
