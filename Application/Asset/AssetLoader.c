//TODO Comments / Logging
#include "AssetLoader.h"
#include "Renderer/Renderer.h"
#include "World/World.h"
#include "World/modules/transform_module.h"
#include "physics_types.h"
#include "types.h"
#include <Physics/Physics.h>

#include <World/WorldModules.h>

#include <ev_log/ev_log.h>

#include <assert.h>

#include <cgltf.h>
#include <vec.h>

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

#include <Game/Game.h>

static void ev_assetloader_load_gltf_node(cgltf_node curr_node, Entity parent, cgltf_data *data, Entity *mesh_entities)
{
    Entity curr;

    if(parent)
      curr = Entity_AddChild(parent);
    else
      curr = CreateEntity();



#ifdef DEBUG
    if(curr_node.name)
    {
      char * name = strdup(curr_node.name);
      Entity_SetComponent(curr, EcsName, {.alloc_value = name});
      ev_log_debug("Created named entity. Name: %s", curr_node.name);
    }
#endif

    // Transform
    Entity_SetComponent(curr, TransformComponent, {0});
    TransformComponent *tr = Entity_GetComponent_mut(curr, TransformComponent);

    cgltf_node_transform_world(&curr_node, (real*)tr->worldTransform);
    cgltf_node_transform_local(&curr_node, (real*)tr->localTransform);

    ev_log_trace("Added TransformComponent to entity: %s", curr_node.name);

    if(curr_node.mesh)
    {
      ev_log_trace("Creating MeshComponent for entity: %s", curr_node.name);
      unsigned int mesh_idx = curr_node.mesh - data->meshes;
      ev_log_trace("Entity's mesh maps to MeshEntity #%d", mesh_idx);
      Entity_InheritComponents(curr, mesh_entities[mesh_idx]);
      ev_log_trace("Added MeshComponent to entity: %s", curr_node.name);
    }

    // Pseudo-physics
    // TODO: Currently, collision shapes are pure convex hulls around objects.
    // Support should be added for CompoundShapes, PrimitiveShapes, and
    // TriangleMeshes for static objects.
    // TODO: Not all objects that have meshes need collision shapes
    if(curr_node.mesh)
    {
      RigidBodyType rbType = EV_RIGIDBODY_DYNAMIC;
      for(int i = 0; i < curr_node.extensions_count; ++i)
      {
        if(!strcmp(curr_node.extensions[i].name, "EV_Physics_Rigidbody")) {
          rbType = EV_RIGIDBODY_KINEMATIC;
        }
        if(!strcmp(curr_node.extensions[i].name, "EV_Game_Player")) {
          Game.setPlayer(curr);
        }
        if(!strcmp(curr_node.extensions[i].name, "EV_Game_Map")) {
          rbType = EV_RIGIDBODY_STATIC;
        }
      }
      const MeshComponent *meshComponent = Entity_GetComponent(curr, MeshComponent);
      if(rbType == EV_RIGIDBODY_STATIC) {
        for(int i = 0; i < meshComponent->primitives_count; i++) {
          RigidBody rb = {
            .entt_id = curr,
            .type = EV_RIGIDBODY_STATIC,
            .mass = 0,
            .restitution = 0.0,
            .collisionShape =
              Physics.createStaticFromTriangleIndexVertex(
                meshComponent->primitives[i].indexCount/3,
                meshComponent->primitives[i].indexBuffer,
                3 * sizeof(unsigned int),
                meshComponent->primitives[i].vertexCount,
                (real*)meshComponent->primitives[i].positionBuffer,
                sizeof(ev_Vector3)
              ),
          };
          Physics.addRigidBody(&rb);
        }
      }
      else if(rbType == EV_RIGIDBODY_DYNAMIC) {
        Entity_SetComponent(curr, RigidBodyComponent, {
            .mass = 1000,
            .type = rbType,
            .collisionShape =
            Physics.createBox(1, 0.5, 2),
            /* Physics.createSphere(1), */
            /* Physics.generateConvexHull( */
            /*     meshComponent->primitives->vertexCount, */
            /*     meshComponent->primitives->positionBuffer */
            /* ), */
            .restitution = 1,
        });
      }
      ev_log_trace("Added RigidBodyComponent to entity: %s", curr_node.name);
    }

    { // Texture Component
    }

    ev_log_trace("Starting to loop on children of entity: %s", curr_node.name);
    for(int child_idx = 0; child_idx < curr_node.children_count; ++child_idx)
    {
      ev_log_debug("Loading child of Entity: %s", Entity_GetName(curr));
      ev_assetloader_load_gltf_node(*curr_node.children[child_idx], curr, data, mesh_entities);
    }
}

static int ev_assetloader_load_gltf(const char *path)
{
  cgltf_options options = {0};
  cgltf_data *data = NULL;
  cgltf_result result = cgltf_parse_file(&options, path, &data);
  if(result != cgltf_result_success) {
    printf("CGLTF Load Error: %d\n", result);
  }
  cgltf_load_buffers(&options, data, path);
  assert(result == cgltf_result_success);

  Entity *mesh_entities = malloc(sizeof(Entity) * data->meshes_count);
  for(unsigned int mesh_idx = 0; mesh_idx < data->meshes_count; ++mesh_idx)
  {
    mesh_entities[mesh_idx] = CreateEntity();
    /* Entity_SetComponent(mesh_entities[mesh_idx], EcsName, {"Mesh"}); */

    Entity_AddComponent(mesh_entities[mesh_idx], MeshComponent);
    MeshComponent* meshComp = Entity_GetComponent_mut(mesh_entities[mesh_idx], MeshComponent);
    meshComp->primitives_count = data->meshes[mesh_idx].primitives_count;
    meshComp->primitives = (MeshPrimitive*)malloc(meshComp->primitives_count * sizeof(MeshPrimitive));
    ev_log_debug("Malloc'ed %u bytes", meshComp->primitives_count * sizeof(MeshPrimitive));

    for(int primitive_idx = 0; primitive_idx < meshComp->primitives_count; primitive_idx++)
    {
      cgltf_primitive curr_primitive = data->meshes[mesh_idx].primitives[primitive_idx];

      unsigned int indices_count = curr_primitive.indices->count;
      meshComp->primitives[primitive_idx].indexCount = indices_count;
      meshComp->primitives[primitive_idx].indexBuffer = malloc(indices_count * sizeof(unsigned int));
      for(unsigned int idx = 0; idx < indices_count; ++idx)
      {
        meshComp->primitives[primitive_idx].indexBuffer[idx] = cgltf_accessor_read_index(curr_primitive.indices, idx);
      }

      /* ev_log_info("Indices loaded for mesh #%d, primitive #%d", mesh_idx, primitive_idx); */
      /* for(unsigned int idx = 0; idx < indices_count; ++idx) */
      /*   printf("%u, ", meshComp->primitives[primitive_idx].indexBuffer[idx]); */
      /* printf("\n"); */
      /* ev_log_info("End of Indices"); */

      for(unsigned int attribute_idx = 0; attribute_idx < curr_primitive.attributes_count; attribute_idx++)
      {
        switch(curr_primitive.attributes[attribute_idx].type)
        {
          // Positions Buffer
          case cgltf_attribute_type_position:
            {
              unsigned int vertex_count = curr_primitive.attributes[attribute_idx].data->count;
              meshComp->primitives[primitive_idx].vertexCount = vertex_count;
              meshComp->primitives[primitive_idx].positionBuffer = malloc(vertex_count * sizeof(ev_Vector3));
              for(unsigned int vertex_idx = 0; vertex_idx < vertex_count; ++vertex_idx)
              {
                cgltf_accessor_read_float(curr_primitive.attributes[attribute_idx].data, vertex_idx,
                (cgltf_float*)&meshComp->primitives[primitive_idx].positionBuffer[vertex_idx], 3);
              }

              /* ev_log_info("Positions loaded for mesh #%d, primitive #%d", mesh_idx, primitive_idx); */
              /* for(unsigned int vertex_idx = 0; vertex_idx < vertex_count; ++vertex_idx) */
              /*   printf("\t\tVertex #%d: ( %f, %f, %f)\n", vertex_idx, */
              /*       meshComp->primitives[primitive_idx].positionBuffer[vertex_idx].x, */
              /*       meshComp->primitives[primitive_idx].positionBuffer[vertex_idx].y, */
              /*       meshComp->primitives[primitive_idx].positionBuffer[vertex_idx].z */
              /*       ); */
              /* ev_log_info("End of Positions"); */
            }
            break;

          // Normal buffer
          case cgltf_attribute_type_normal:
            {
              unsigned int vertex_count = curr_primitive.attributes[attribute_idx].data->count;
              meshComp->primitives[primitive_idx].vertexCount = vertex_count;
              meshComp->primitives[primitive_idx].normalBuffer = malloc(vertex_count * sizeof(ev_Vector3));
              for(unsigned int vertex_idx = 0; vertex_idx < vertex_count; ++vertex_idx)
              {
                cgltf_accessor_read_float(curr_primitive.attributes[attribute_idx].data, vertex_idx,
                (cgltf_float*)&meshComp->primitives[primitive_idx].normalBuffer[vertex_idx], 3);
              }

              /* ev_log_info("Normals loaded for mesh #%d, primitive #%d", mesh_idx, primitive_idx); */
              /* for(unsigned int vertex_idx = 0; vertex_idx < vertex_count; ++vertex_idx) */
              /*   printf("\t\tVertex #%d: ( %f, %f, %f)\n", vertex_idx, */
              /*       meshComp->primitives[primitive_idx].normalBuffer[vertex_idx].x, */
              /*       meshComp->primitives[primitive_idx].normalBuffer[vertex_idx].y, */
              /*       meshComp->primitives[primitive_idx].normalBuffer[vertex_idx].z */
              /*       ); */
              /* ev_log_info("End of Normals"); */
            }
            break;

          default:
            break;
        }
      }
    }
  }

  // Giving a RenderingComponent to the mesh entities so that all instanced entities
  // have the same RenderingComponent
  for(unsigned int mesh_idx = 0; mesh_idx < data->meshes_count; ++mesh_idx)
  {
    // This should be done at the beginning of the function as this leads to a switch in
    // the archetype and, in turn, leads to a change in the internal data layout.
    Entity_AddComponent(mesh_entities[mesh_idx], RenderingComponent);

    const MeshComponent* meshComp = Entity_GetComponent(mesh_entities[mesh_idx], MeshComponent);
    RenderingComponent* rendComp = Entity_GetComponent_mut(mesh_entities[mesh_idx], RenderingComponent);
    for(unsigned int primitive_idx = 0; primitive_idx < meshComp->primitives_count; ++primitive_idx)
    {
      MeshPrimitive meshPrim = meshComp->primitives[primitive_idx];
      PrimitiveRenderData primRendData;
      primRendData.indexCount = meshPrim.indexCount;
      primRendData.indexBufferId = Renderer.registerIndexBuffer(meshPrim.indexBuffer, meshPrim.indexCount * sizeof(*meshPrim.indexBuffer));
      primRendData.vertexBufferId = Renderer.registerVertexBuffer((real*)meshPrim.positionBuffer, meshPrim.vertexCount * sizeof(*meshPrim.positionBuffer));
      primRendData.normalBufferId = Renderer.registerNormalBuffer((real*)meshPrim.normalBuffer, meshPrim.vertexCount * sizeof(*meshPrim.normalBuffer));
      /* printf("Normal buffer id for this primitive: %d\n", primRendData.normalBufferId); */
      vec_push(&rendComp->meshRenderData.primitives, primRendData);
    }
    rendComp->meshRenderData.pipelineType = EV_GRAPHICS_PIPELINE_PBR;
  }

  World.lockSceneAccess();
  for(unsigned int node_idx = 0; node_idx < data->nodes_count; ++node_idx)
  {
    // We're only loading root nodes. All children are then loaded recursively.
    cgltf_node curr_node = data->nodes[node_idx];
    if(curr_node.parent) continue;

    ev_assetloader_load_gltf_node(curr_node, 0, data, mesh_entities);
  }
  World.unlockSceneAccess();


  cgltf_free(data);

  free(mesh_entities);
  return 0;
}
