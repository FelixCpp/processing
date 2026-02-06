#include <processing/image.hpp>

#include <glad/gl.h>
#include <stb/stb_image.h>

namespace processing
{
    Pixels::Pixels(u32 width, u32 height, PlatformImage* parent, const std::vector<u8>& data)
        : m_width{width},
          m_height{height},
          m_parent{parent},
          m_data{data}
    {
    }

    void Pixels::set(const u32 x, const u32 y, const Color color)
    {
        if (x < m_width and y < m_height)
        {
            const usize index = (y * m_width + x) * 4;
            m_data[index + 0] = color.r;
            m_data[index + 1] = color.g;
            m_data[index + 2] = color.b;
            m_data[index + 3] = color.a;
        }
    }

    Color Pixels::get(const u32 x, const u32 y) const
    {
        if (x < m_width and y < m_height)
        {
            const usize index = (y * m_width + x) * 4;
            return Color(m_data[index + 0], m_data[index + 1], m_data[index + 2], m_data[index + 3]);
        }

        return Color(0);
    }

    void Pixels::commit()
    {
        glBindTexture(GL_TEXTURE_2D, m_parent->getResourceId().value);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, m_data.data());
    }
} // namespace processing

namespace processing
{
    class OpenGLPlatformImage : public PlatformImage
    {
    private:
        inline static constexpr GLenum filterModeToGLId(const FilterModeType type)
        {
            switch (type)
            {
                case FilterModeType::linear:
                    return GL_LINEAR;
                case FilterModeType::nearest:
                    return GL_NEAREST;
            }
        }

        inline static constexpr GLenum extendModeToGLId(const ExtendModeType type)
        {
            switch (type)
            {
                case ExtendModeType::clamp:
                    return GL_CLAMP_TO_EDGE;
                case ExtendModeType::repeat:
                    return GL_REPEAT;
                case ExtendModeType::mirroredRepeat:
                    return GL_MIRRORED_REPEAT;
            }
        }

    public:
        static std::unique_ptr<OpenGLPlatformImage> create(u32 width, u32 height, const u8* data, FilterMode filterMode, ExtendMode extendMode)
        {
            ResourceId resourceId = {.value = 0};
            glGenTextures(1, &resourceId.value);
            glBindTexture(GL_TEXTURE_2D, resourceId.value);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterModeToGLId(filterMode.mag));
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterModeToGLId(filterMode.min));
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, extendModeToGLId(extendMode.horizontal));
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, extendModeToGLId(extendMode.vertical));
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glBindTexture(GL_TEXTURE_2D, 0);

            return std::unique_ptr<OpenGLPlatformImage>(new OpenGLPlatformImage(uint2{width, height}, resourceId, filterMode, extendMode));
        }

        static std::unique_ptr<OpenGLPlatformImage> load(const std::filesystem::path& filepath, FilterMode filterMode, ExtendMode extendMode)
        {
            int width, height;
            std::unique_ptr<stbi_uc, decltype(&stbi_image_free)> data(stbi_load(filepath.string().c_str(), &width, &height, nullptr, STBI_rgb_alpha), &stbi_image_free);
            if (data == nullptr)
            {
                return nullptr;
            }

            ResourceId resourceId = {.value = 0};
            glGenTextures(1, &resourceId.value);
            glBindTexture(GL_TEXTURE_2D, resourceId.value);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterModeToGLId(filterMode.mag));
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterModeToGLId(filterMode.min));
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, extendModeToGLId(extendMode.horizontal));
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, extendModeToGLId(extendMode.vertical));
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.get());
            glBindTexture(GL_TEXTURE_2D, 0);

            return std::unique_ptr<OpenGLPlatformImage>(new OpenGLPlatformImage(uint2{static_cast<u32>(width), static_cast<u32>(height)}, resourceId, filterMode, extendMode));
        }

        ~OpenGLPlatformImage() override
        {
            glDeleteTextures(1, &m_resourceId.value);
        }

        void setFilterMode(FilterMode mode) override
        {
            if (m_filterMode != mode)
            {
                glBindTexture(GL_TEXTURE_2D, m_resourceId.value);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterModeToGLId(mode.mag));
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterModeToGLId(mode.min));
                m_filterMode = mode;
            }
        }

        FilterMode getFilterMode() const override
        {
            return m_filterMode;
        }

        void setExtendMode(ExtendMode mode) override
        {
            if (m_extendMode != mode)
            {
                glBindTexture(GL_TEXTURE_2D, m_resourceId.value);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, extendModeToGLId(mode.horizontal));
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, extendModeToGLId(mode.vertical));
                m_extendMode = mode;
            }
        }

        ExtendMode getExtendMode() const override
        {
            return m_extendMode;
        }

        uint2 getSize() const override
        {
            return m_size;
        }

        Pixels loadPixels() override
        {
            std::vector<u8> data(m_size.x * m_size.y * 4);
            glBindTexture(GL_TEXTURE_2D, m_resourceId.value);
            glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

            return Pixels(m_size.x, m_size.y, this, data);
        }

        ResourceId getResourceId() const override
        {
            return m_resourceId;
        }

    private:
        explicit OpenGLPlatformImage(const uint2& size, ResourceId resourceId, FilterMode filterMode, ExtendMode extendMode)
            : m_size(size),
              m_resourceId(resourceId),
              m_extendMode(extendMode),
              m_filterMode(filterMode)
        {
        }

        uint2 m_size;
        ResourceId m_resourceId;
        FilterMode m_filterMode;
        ExtendMode m_extendMode;
    };
} // namespace processing

namespace processing
{
    Image ImageAssetHandler::createImage(u32 width, u32 height, const u8* data, FilterMode filterMode, ExtendMode extendMode)
    {
        if (auto image = OpenGLPlatformImage::create(width, height, data, filterMode, extendMode))
        {
            std::shared_ptr<PlatformImage>& ptr = m_assets.emplace_back(std::move(image));
            AssetId assetId = {.value = m_assets.size()};

            return Image(assetId, ptr);
        }

        return Image(AssetId{.value = 0}, nullptr);
    }

    Image ImageAssetHandler::loadImage(const std::filesystem::path& filepath, FilterMode filterMode, ExtendMode extendMode)
    {
        if (auto image = OpenGLPlatformImage::load(filepath, filterMode, extendMode))
        {
            std::shared_ptr<PlatformImage>& ptr = m_assets.emplace_back(std::move(image));
            AssetId assetId = {.value = m_assets.size()};

            return Image(assetId, ptr);
        }

        return Image(AssetId{.value = 0}, nullptr);
    }

    Image ImageAssetHandler::loadAsset(AssetId assetId)
    {
        return Image(assetId, m_assets[assetId.value]);
    }
} // namespace processing

namespace processing
{
    Image::Image()
        : m_assetId{.value = 0},
          m_impl(nullptr)
    {
    }

    Image::Image(const AssetId assetId, std::shared_ptr<PlatformImage> impl)
        : m_assetId{assetId},
          m_impl{std::move(impl)}
    {
    }

    void Image::setFilterMode(FilterMode mode)
    {
        m_impl->setFilterMode(mode);
    }

    FilterMode Image::getFilterMode() const
    {
        return m_impl->getFilterMode();
    }

    void Image::setExtendMode(ExtendMode mode)
    {
        m_impl->setExtendMode(mode);
    }

    ExtendMode Image::getExtendMode() const
    {
        return m_impl->getExtendMode();
    }

    uint2 Image::getSize() const
    {
        return m_impl->getSize();
    }

    Pixels Image::loadPixels()
    {
        return m_impl->loadPixels();
    }

    ResourceId Image::getResourceId() const
    {
        return m_impl->getResourceId();
    }

    AssetId Image::getAssetId() const
    {
        return m_assetId;
    }
} // namespace processing
