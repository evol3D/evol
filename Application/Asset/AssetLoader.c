//TODO Comments / Logging
#include "AssetLoader.h"
#include "AssetStore.h"
#include <World/World.h>
#include <Physics/Physics.h>
#include <World/modules/transform_module.h>
#include <World/modules/geometry_module.h>
#include <World/modules/physics_module.h>

#include <ev_log/ev_log.h>

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
  unsigned int dummy;
} AssetLoaderData;

static int ev_assetloader_init()
{
  return 0;
}

static int ev_assetloader_deinit()
{
  return 0;
}

static void ev_assetloader_load_gltf_node(cgltf_node curr_node, Entity parent)
{ 
    // World module imports
    ImportModule(TransformModule);
    ImportModule(GeometryModule);
    /* ImportModule(PhysicsModule); */

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
    Entity_SetComponent(curr, TransformComponent, {});
    TransformComponent *tr = Entity_GetComponent_mut(curr, TransformComponent);

    {
      mat4 tr_mat, rot_mat;
      cgltf_node_transform_world(&curr_node, (float*)tr_mat);
      glm_decompose(tr_mat, (float*)&tr->position, rot_mat, (float*)&tr->scale);
      glm_mat4_quat(rot_mat, (float*)&tr->rotation);
    }

    { // Mesh Component
      if(curr_node.mesh)
      {
        Entity_AddComponent(curr, MeshComponent);
        MeshComponent* meshComp = Entity_GetComponent_mut(curr, MeshComponent);
        meshComp->primitives_count = curr_node.mesh->primitives_count;
        meshComp->primitives = (MeshPrimitive*)malloc(meshComp->primitives_count * sizeof(MeshPrimitive));
        ev_log_debug("Malloc'ed %u bytes", meshComp->primitives_count * sizeof(MeshPrimitive));

        for(int primitive_idx = 0; primitive_idx < meshComp->primitives_count; primitive_idx++)
        {
          cgltf_primitive curr_primitive = curr_node.mesh->primitives[primitive_idx];

          meshComp->primitives[primitive_idx].indexBuffer = (BufferView){
            .buffer_idx = AssetStore.getBufferIndex(curr_primitive.indices->buffer_view->buffer->uri),
            .offset     = curr_primitive.indices->buffer_view->offset,
            .size       = curr_primitive.indices->buffer_view->size,
          };

          for(unsigned int attribute_idx = 0; attribute_idx < curr_primitive.attributes_count; attribute_idx++)
          {
            switch(curr_primitive.attributes[attribute_idx].type)
            {
              // Positions Buffer
              case cgltf_attribute_type_position:
                meshComp->primitives[primitive_idx].positionBuffer = (BufferView) {
                  .buffer_idx = AssetStore.getBufferIndex(curr_primitive.attributes[attribute_idx].data->buffer_view->buffer->uri),
                  .offset     = curr_primitive.attributes[attribute_idx].data->offset + curr_primitive.attributes[attribute_idx].data->buffer_view->offset,
                  .size       = curr_primitive.attributes[attribute_idx].data->buffer_view->size,
                };
                break;

              // Normal buffer
              case cgltf_attribute_type_normal:
                meshComp->primitives[primitive_idx].normalBuffer = (BufferView) {
                  .buffer_idx = AssetStore.getBufferIndex(curr_primitive.attributes[attribute_idx].data->buffer_view->buffer->uri),
                  .offset     = curr_primitive.attributes[attribute_idx].data->offset + curr_primitive.attributes[attribute_idx].data->buffer_view->offset,
                  .size       = curr_primitive.attributes[attribute_idx].data->buffer_view->size,
                };
                break;

              default:
                break;
            }
          }
        }
      }
    }


    { // Texture Component
    }

    for(int child_idx = 0; child_idx < curr_node.children_count; ++child_idx)
      ev_assetloader_load_gltf_node(*curr_node.children[child_idx], curr);
}

static int ev_assetloader_load_gltf(const char *path)
{
  cgltf_options options = {0};
  cgltf_data *data = NULL;
  cgltf_result result = cgltf_parse_file(&options, path, &data);
  assert(result == cgltf_result_success);

  for(unsigned int buffer_idx = 0; buffer_idx < data->buffers_count; buffer_idx++)
  {
    AssetStore.loadBuffer(data->buffers[buffer_idx].uri);
  }

  for(unsigned int node_idx = 0; node_idx < data->nodes_count; ++node_idx)
  {
    cgltf_node curr_node = data->nodes[node_idx];
    if(curr_node.parent) continue;

    ev_assetloader_load_gltf_node(curr_node, 0);
  }

  cgltf_free(data);

  return 0;
}
