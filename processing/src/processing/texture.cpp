#include <processing/processing.hpp>

#include <glad/gl.h>
#include <stb/stb_image.h>

#include <format>

namespace processing
{
    Texture::Texture(std::unique_ptr<TextureImpl> impl) : m_impl(std::move(impl))
    {
    }

    uint2 Texture::getSize() const
    {
        if (m_impl == nullptr)
        {
            return {};
        }

        return m_impl->getSize();
    }

    TextureId Texture::getResourceId() const
    {
        if (m_impl == nullptr)
        {
            return TextureId{.value = 0};
        }

        return m_impl->getResourceId();
    }
} // namespace processing

namespace processing
{
    class OpenGLTextureImpl : public TextureImpl
    {
    public:
        static std::unique_ptr<OpenGLTextureImpl> fromFile(const std::filesystem::path& filepath)
        {
            const std::string path = filepath.string();

            int width, height, channels;
            stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
            if (data == nullptr)
            {
                error(std::format("Failed to load image \"{}\"", path));
            }

            printf("Size: %dx%d", width, height);

            TextureId id = {.value = 0};
            glGenTextures(1, &id.value);
            glBindTexture(GL_TEXTURE_2D, id.value);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

            stbi_image_free(data);

            const uint2 size = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
            return std::unique_ptr<OpenGLTextureImpl>(new OpenGLTextureImpl(id, size));
        }

        ~OpenGLTextureImpl() override
        {
            glDeleteTextures(1, &m_textureId.value);
        }

        uint2 getSize() const override
        {
            return m_size;
        }

        TextureId getResourceId() const override
        {
            return m_textureId;
        }

    private:
        explicit OpenGLTextureImpl(const TextureId id, const uint2 size) : m_textureId(id), m_size(size)
        {
        }

        TextureId m_textureId;
        uint2 m_size;
    };
} // namespace processing

namespace processing
{
    Texture loadTexture(const std::filesystem::path& filepath)
    {
        return Texture(OpenGLTextureImpl::fromFile(filepath));
    }
} // namespace processing
