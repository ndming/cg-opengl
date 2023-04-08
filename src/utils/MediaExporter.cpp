// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <iostream>
#include <stb_image_write.h>

#include "utils/MediaExporter.h"

MediaExporter::Builder& MediaExporter::Builder::folderPath(const std::string_view path) {
    _folderPath = std::string{ path.data(), path.size() };
    if (*_folderPath.end() != '/') {
        _folderPath += '/';
    }
    return *this;
}

std::unique_ptr<MediaExporter> MediaExporter::Builder::build() const {
    auto path = std::filesystem::path{ _folderPath };
    return std::unique_ptr<MediaExporter>{ new MediaExporter(std::move(path)) };
}

void MediaExporter::exportImage(
    const std::string_view name,
    const void* const data,
    const int width,
    const int height
) const {
    std::error_code err;
    if (std::filesystem::create_directories(_dirPath, err) || std::filesystem::exists(_dirPath)) {
        const auto path = _dirPath.string() + std::string{ name } + ".png";
        stbi_write_png(path.c_str(), width, height, 4, data, width * 4);
    } else {
        std::cerr << err.message() << '\n';
    }
}
