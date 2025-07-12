// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/Cube.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <luvk/Libraries/ShaderCompiler.hpp>

using namespace luvk_example;

namespace
{
    constexpr auto CubeMeshSrc = R"(#version 450
                                    #extension GL_EXT_mesh_shader : require

                                    layout(local_size_x = 1) in;
                                    layout(max_vertices = 4, max_primitives = 2) out;
                                    layout(triangles) out;

                                    layout(push_constant) uniform Push
                                    {
                                        mat4 mvp;
                                    } pc;

                                    layout(location = 0) perprimitiveEXT out PerPrim
                                    {
                                        vec4 color;
                                    } prim[];

                                    void main()
                                    {
                                        const vec3 positions[6][4] = vec3[6][4](
                                            vec3[4](vec3(-0.5,-0.5,-0.5), vec3(0.5,-0.5,-0.5), vec3(0.5,0.5,-0.5), vec3(-0.5,0.5,-0.5)),
                                            vec3[4](vec3(-0.5,-0.5,0.5), vec3(0.5,-0.5,0.5), vec3(0.5,0.5,0.5), vec3(-0.5,0.5,0.5)),
                                            vec3[4](vec3(-0.5,-0.5,-0.5), vec3(-0.5,0.5,-0.5), vec3(-0.5,0.5,0.5), vec3(-0.5,-0.5,0.5)),
                                            vec3[4](vec3(0.5,-0.5,-0.5), vec3(0.5,0.5,-0.5), vec3(0.5,0.5,0.5), vec3(0.5,-0.5,0.5)),
                                            vec3[4](vec3(-0.5,0.5,-0.5), vec3(0.5,0.5,-0.5), vec3(0.5,0.5,0.5), vec3(-0.5,0.5,0.5)),
                                            vec3[4](vec3(-0.5,-0.5,-0.5), vec3(0.5,-0.5,-0.5), vec3(0.5,-0.5,0.5), vec3(-0.5,-0.5,0.5)));

                                        const vec4 colors[6] = vec4[6](
                                            vec4(1,0,0,1), vec4(0,1,0,1), vec4(0,0,1,1),
                                            vec4(1,1,0,1), vec4(1,0,1,1), vec4(0,1,1,1));

                                        uint id = gl_WorkGroupID.x;
                                        SetMeshOutputsEXT(4,2);
                                        for (int i = 0; i < 4; ++i)
                                        {
                                            gl_MeshVerticesEXT[i].gl_Position = pc.mvp * vec4(positions[id][i], 1.0);
                                        }
                                        gl_PrimitiveTriangleIndicesEXT[0] = uvec3(0,1,2);
                                        gl_PrimitiveTriangleIndicesEXT[1] = uvec3(2,3,0);
                                        prim[0].color = colors[id];
                                        prim[1].color = colors[id];
                                    })";

    constexpr auto CubeFragSrc = R"(#version 450
                                    #extension GL_EXT_mesh_shader : require

                                    layout(location = 0) perprimitiveEXT in vec4 color;
                                    layout(location = 0) out vec4 outColor;

                                    void main()
                                    {
                                        outColor = color;
                                    })";

    constexpr std::array<glm::vec3, 8> CubeVertices{{{-0.5f, -0.5f, -0.5f},
                                                     {0.5f, -0.5f, -0.5f},
                                                     {0.5f, 0.5f, -0.5f},
                                                     {-0.5f, 0.5f, -0.5f},
                                                     {-0.5f, -0.5f, 0.5f},
                                                     {0.5f, -0.5f, 0.5f},
                                                     {0.5f, 0.5f, 0.5f},
                                                     {-0.5f, 0.5f, 0.5f}}};

    constexpr std::array<std::uint16_t, 36> CubeIndices{{0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 0, 1, 5, 5, 4, 0, 2, 3, 7, 7, 6, 2, 1, 2, 6, 6, 5, 1, 3, 0, 4, 4, 7, 3}};
} // namespace

luvk_example::Cube::Cube(std::shared_ptr<luvk::MeshRegistry> Registry,
                         const std::shared_ptr<luvk::Device>& Device,
                         const std::shared_ptr<luvk::SwapChain>& Swap)
    : m_Registry(std::move(Registry)),
      m_Pipeline(std::make_shared<luvk::Pipeline>())
{
    auto CubeFrag = luvk::CompileGLSLToSPIRV(CubeFragSrc, EShLangFragment);

    const VkExtent2D Extent = Swap->GetExtent();
    const std::array Formats{Swap->m_Arguments.Format};

    constexpr VkPushConstantRange CubePC{VK_SHADER_STAGE_MESH_BIT_EXT, 0, sizeof(glm::mat4)};

    auto CubeMesh = luvk::CompileGLSLToSPIRV(CubeMeshSrc, EShLangMesh);

    m_Pipeline->CreateMeshPipeline(Device,
                                   {.Extent = Extent,
                                    .ColorFormats = Formats,
                                    .RenderPass = Swap->GetRenderPass(),
                                    .Subpass = 0,
                                    .MeshShader = CubeMesh,
                                    .FragmentShader = CubeFrag,
                                    .SetLayouts = {},
                                    .PushConstants = std::array{CubePC}});

    m_Index = m_Registry->RegisterMesh({},
                                       std::as_bytes(std::span<std::uint16_t const>{}),
                                       VK_NULL_HANDLE,
                                       nullptr,
                                       nullptr,
                                       nullptr,
                                       nullptr,
                                       {},
                                       m_Pipeline,
                                       Device);

    m_Mesh = luvk::Mesh(m_Registry, m_Index);
}

void luvk_example::Cube::Update(const float DeltaTime, glm::mat4 const& View, glm::mat4 const& Proj) const
{
    static float Elapsed = 0.F;
    Elapsed += DeltaTime;

    const glm::mat4 Model = glm::rotate(glm::mat4(1.F), Elapsed, glm::vec3(0.F, 1.F, 0.F));
    const glm::mat4 Mvp = Proj * View * Model;

    m_Registry->UpdateUniform(m_Index, std::as_bytes(std::span{&Mvp, 1}));
}

luvk::Mesh& luvk_example::Cube::GetMesh() noexcept
{
    return m_Mesh;
}
