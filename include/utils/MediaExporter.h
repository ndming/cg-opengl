// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>

class MediaExporter {
public:
    class Builder {
    public:
        Builder& folderPath(std::string_view path);

        [[nodiscard]] std::unique_ptr<MediaExporter> build() const;
    private:
        std::string _folderPath{ "./capture/" };
    };

    void exportImage(std::string_view name, const void* data, int width, int height) const;

private:
    explicit MediaExporter(std::filesystem::path&& dirPath) noexcept : _dirPath{ dirPath } {}

    const std::filesystem::path _dirPath;
};
