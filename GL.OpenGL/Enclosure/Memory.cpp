#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
#include <stdexcept>
#include <GL/Memory.h>
#include <cassert>
#include "../Objects/Objects.h"

namespace GL {
    namespace {
        struct Buffer {
            explicit Buffer(int size, GLbitfield flags) : mBuffer(OpenGL::Create) {
                mBuffer.Storage(size, nullptr, flags);
            }

        protected:
            OpenGL::Buffer mBuffer;
        };

        struct ClientImpl : ClientBuffer, Buffer {
            friend struct GL::DeviceBuffer;

            explicit ClientImpl(int size)
                    : Buffer(size, GL_CLIENT_STORAGE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT) {
            }

            void *MapImpl(const BufferAccess access) {
                const auto ret = mBuffer.Map(TranslateAccess(access));
                if (!ret) ConvertErrorException();
                return ret;
            }

            void *MapRangeImpl(const int start, const int length, BufferAccess access, BufferMappingOptions options) {
                const auto ret = mBuffer.MapRange(start, length, TranslateAccess(access, options));
                if (!ret) ConvertErrorException();
                return ret;
            }

            void FlushRegionImpl(int start, int length) {
                mBuffer.FlushMappedRange(start, length);
            }

            bool UnmapImpl() { return mBuffer.Unmap(); }

        private:
            [[noreturn]] static void ConvertErrorException() {
                switch (OpenGL::Error()) {
                    case GL_OUT_OF_MEMORY:
                        throw std::bad_alloc();
                    case GL_INVALID_OPERATION:
                        throw std::runtime_error("Invalid operation");
                    case GL_INVALID_VALUE:
                        throw std::runtime_error("Invalid value");
                    default: // should not reach here
                        std::terminate();
                }
            }

            static constexpr GLbitfield TranslateAccess(
                    const BufferAccess access, const BufferMappingOptions options) noexcept {
                GLbitfield result = GL_MAP_PERSISTENT_BIT;
                if (static_cast<unsigned int>(access) & 0b1u) result |= GL_MAP_READ_BIT;
                if (static_cast<unsigned int>(access) & 0b10u) result |= GL_MAP_WRITE_BIT;
                if (static_cast<unsigned int>(options) & 0b1u) result |= GL_MAP_COHERENT_BIT;
                if (static_cast<unsigned int>(options) & 0b10u) result |= GL_MAP_INVALIDATE_RANGE_BIT;
                if (static_cast<unsigned int>(options) & 0b100u) result |= GL_MAP_INVALIDATE_BUFFER_BIT;
                if (static_cast<unsigned int>(options) & 0b1000u) result |= GL_MAP_FLUSH_EXPLICIT_BIT;
                if (static_cast<unsigned int>(options) & 0b10000u) result |= GL_MAP_UNSYNCHRONIZED_BIT;
                return result;
            }

            static constexpr GLenum TranslateAccess(const BufferAccess access) noexcept {
                switch (access) {
                    case BufferAccess::Read:
                        return GL_READ_ONLY;
                    case BufferAccess::Write:
                        return GL_WRITE_ONLY;
                    case BufferAccess::ReadWrite:
                        return GL_READ_WRITE;
                }
                assert(false);
                return 0; // should not reach here
            }

        public:
            OpenGL::Buffer mBuffer;
        };

        struct DeviceImpl : DeviceBuffer, Buffer {
            explicit DeviceImpl(int size) : Buffer(size, 0) {}

            void UploadImpl(ClientImpl &transfer, int srcOffset, int dstOffset, int size) {
                OpenGL::Buffer::CopySubData(transfer.mBuffer, mBuffer, srcOffset, dstOffset, size);
            }

            void DownloadImpl(ClientImpl &target, int srcOffset, int dstOffset, int size) {
                OpenGL::Buffer::CopySubData(mBuffer, target.mBuffer, srcOffset, dstOffset, size);
            }

            void CopyFromImpl(DeviceImpl &source, int srcOffset, int dstOffset, int size) {
                OpenGL::Buffer::CopySubData(source.mBuffer, mBuffer, srcOffset, dstOffset, size);
            }

        private:
            OpenGL::Buffer mBuffer;
        };

    }

    void *GL::ClientBuffer::Map(const BufferAccess access) {
        return down<ClientImpl>().MapImpl(access);
    }

    void *ClientBuffer::MapRegion(int start, int size, BufferAccess access, BufferMappingOptions options) {
        return down<ClientImpl>().MapRangeImpl(start, size, access, options);
    }

    void ClientBuffer::FlushRegion(int start, int length) {
        down<ClientImpl>().FlushRegionImpl(start, length);
    }

    bool ClientBuffer::Unmap() {
        return down<ClientImpl>().UnmapImpl();
    }

    ClientBuffer *ClientBuffer::Create(int size) {
        return new ClientImpl(size);
    }

    DeviceBuffer *DeviceBuffer::Create(int size) {
        return new DeviceImpl(size);
    }

    void DeviceBuffer::Upload(ClientBuffer &transfer, int srcOffset, int dstOffset, int size) {
        down<DeviceImpl>().UploadImpl(static_cast<ClientImpl&>(transfer), srcOffset, dstOffset, size); // NOLINT
    }

    void DeviceBuffer::Download(ClientBuffer &target, int srcOffset, int dstOffset, int size) {
        down<DeviceImpl>().DownloadImpl(static_cast<ClientImpl&>(target), srcOffset, dstOffset, size); // NOLINT
    }

    void DeviceBuffer::CopyFrom(DeviceBuffer &source, int srcOffset, int dstOffset, int size) {
        down<DeviceImpl>().CopyFromImpl(source.down<DeviceImpl>(), srcOffset, dstOffset, size); // NOLINT
    }
}

#pragma clang diagnostic pop
