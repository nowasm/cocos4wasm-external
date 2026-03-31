# libwebp — wasm32 预编译库

此目录用于存放为 `wasm32` 目标编译的 `libwebp.a` 静态库。

## 当前状态

`libwebp.a` 尚未放置。需要使用 Emscripten 工具链从 libwebp 源码编译。

## 编译方法

### 前提条件

- 已安装并激活 [emsdk](https://github.com/emscripten-core/emsdk)
- 已安装 CMake 和 Ninja

### 步骤

```bash
# 1. 激活 emsdk（路径根据实际安装位置调整）
source /path/to/emsdk/emsdk_env.sh

# 2. 下载 libwebp 源码（例如 v1.3.2）
git clone https://chromium.googlesource.com/webm/libwebp
cd libwebp
git checkout v1.3.2

# 3. 使用 emcmake 配置
mkdir build-wasm && cd build-wasm
emcmake cmake .. \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DWEBP_BUILD_ANIM_UTILS=OFF \
  -DWEBP_BUILD_CWEBP=OFF \
  -DWEBP_BUILD_DWEBP=OFF \
  -DWEBP_BUILD_GIF2WEBP=OFF \
  -DWEBP_BUILD_IMG2WEBP=OFF \
  -DWEBP_BUILD_VWEBP=OFF \
  -DWEBP_BUILD_WEBPINFO=OFF \
  -DWEBP_BUILD_WEBPMUX=OFF \
  -DWEBP_BUILD_EXTRAS=OFF

# 4. 编译
cmake --build .

# 5. 将产物复制到此目录
cp libwebp.a <engine_root>/external/emscripten/libs/libwebp/libwebp.a
```

## 头文件

WebP 头文件（`decode.h`、`encode.h` 等）应放置于：

```
external/emscripten/include/webp/
```

可直接从 libwebp 源码的 `src/webp/` 目录复制，或从 `external/linux/include/libwebp/webp/` 复制。
