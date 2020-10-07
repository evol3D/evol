#include "AssetLoader.h"

#include <assert.h>

#include <cgltf.h>

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
    return 0;
}

static int ev_assetloader_deinit()
{
    return 0;
}

static int ev_assetloader_load_gltf(const char *path)
{
  cgltf_options options = {0};
  AssetLoaderData.gltfScene = NULL;
  cgltf_result result = cgltf_parse_file(&options, path, &AssetLoaderData.gltfScene);
  assert(result == cgltf_result_success);
  cgltf_load_buffers(&options, AssetLoaderData.gltfScene, path);

  // TODO: load scene in ecs world

  return 0;
}
