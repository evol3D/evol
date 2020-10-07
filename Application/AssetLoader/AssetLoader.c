#include "AssetLoader.h"
#include <World/World.h>
#include <Physics/Physics.h>
#include <World/modules/transform_module.h>
#include <World/modules/physics_module.h>

#include <assert.h>

#include <cgltf.h>

#include <cglm/cglm.h>

static int ev_assetloader_init();
static int ev_assetloader_deinit();
static int ev_assetloader_load_gltf(const char *path);

struct ev_AssetLoader AssetLoader = {
        .init   = ev_assetloader_init,
        .deinit = ev_assetloader_deinit,
        .loadGLTF = ev_assetloader_load_gltf,
};

struct ev_AssetLoader_Data {
  cgltf_data *gltfScene;
} AssetLoaderData;

static int ev_assetloader_init()
{
  AssetLoaderData.gltfScene = 0;

  return 0;
}

static int ev_assetloader_deinit()
{
  if(AssetLoaderData.gltfScene)
    cgltf_free(AssetLoaderData.gltfScene);

  return 0;
}

static void ev_assetloader_load_gltf_node(cgltf_node curr_node, Entity parent)
{
    // World module imports
    ImportModule(TransformModule);
    ImportModule(PhysicsModule);

    Entity curr;

    if(parent)
      curr = Entity_AddChild(parent);
    else
      curr = CreateEntity();

#ifdef DEBUG
    if(curr_node.name)
      Entity_SetComponent(curr, EcsName, {curr_node.name});
#endif

    // Transform
    Entity_SetComponent(curr, TransformComponent, {.position = {1, 1, 1, 1}, .rotation = {0, 0, 0, 0}, .scale = {1, 1, 1, 1}});
    TransformComponent *tr = Entity_GetComponent_mut(curr, TransformComponent);

    {
      mat4 tr_mat, rot_mat;
      cgltf_node_transform_world(&curr_node, (float*)tr_mat);
      glm_decompose(tr_mat, (float*)&tr->position, rot_mat, (float*)&tr->scale);
      glm_mat4_quat(rot_mat, (float*)&tr->rotation);
    }

    // RigidBody
    if(curr_node.mesh)
    {
      Entity_SetComponent(curr, RigidBodyComponent, {
          .mass = 0,
          .collisionShape = 
            Physics.createSphere(1),
          });
    }

    for(int child_idx = 0; child_idx < curr_node.children_count; ++child_idx)
      ev_assetloader_load_gltf_node(*curr_node.children[child_idx], curr);

}

static int ev_assetloader_load_gltf(const char *path)
{

  cgltf_options options = {0};
  AssetLoaderData.gltfScene = NULL;
  cgltf_result result = cgltf_parse_file(&options, path, &AssetLoaderData.gltfScene);
  assert(result == cgltf_result_success);
  cgltf_load_buffers(&options, AssetLoaderData.gltfScene, path);

  for(unsigned int node_idx = 0; node_idx < AssetLoaderData.gltfScene->nodes_count; ++node_idx)
  {
    cgltf_node curr_node = AssetLoaderData.gltfScene->nodes[node_idx];
    if(curr_node.parent) continue;

    ev_assetloader_load_gltf_node(curr_node, 0);
  }

  return 0;
}
