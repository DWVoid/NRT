#pragma once

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include <string>

class VRam {
	
};

class HRam {
	
};

class TRam {
	
};

class Mesh {
	
};

class ISceneBuilder {
public:
	virtual void Position2(Char2 position) noexcept;
	virtual void Position2(Byte2 position) noexcept;
	virtual void Position2(Short2 position) noexcept;
	virtual void Position2(UShort2 position) noexcept;
	virtual void Position2(Int2 position) noexcept;
	virtual void Position2(UInt2 position) noexcept;
	virtual void Position2(Long2 position) noexcept;
	virtual void Position2(ULong2 position) noexcept;
	virtual void Position2(Float2 position) noexcept;
	virtual void Position2(Double2 position) noexcept;

	virtual void Position3(Char3 position) noexcept;
	virtual void Position3(Byte3 position) noexcept;
	virtual void Position3(Short3 position) noexcept;
	virtual void Position3(UShort3 position) noexcept;
	virtual void Position3(Int3 position) noexcept;
	virtual void Position3(UInt3 position) noexcept;
	virtual void Position3(const Long3& position) noexcept;
	virtual void Position3(const ULong3& position) noexcept;
	virtual void Position3(Float3 position) noexcept;
	virtual void Position3(const Double3& position) noexcept;

	virtual void TexCoord2(Char3 texCoord) noexcept;
	virtual void TexCoord2(Byte2 texCoord) noexcept;
	virtual void TexCoord2(Short2 texCoord) noexcept;
	virtual void TexCoord2(UShort2 texCoord) noexcept;
	virtual void TexCoord2(Int2 texCoord) noexcept;
	virtual void TexCoord2(UInt2 texCoord) noexcept;
	virtual void TexCoord2(Long2 texCoord) noexcept;
	virtual void TexCoord2(ULong2 texCoord) noexcept;
	virtual void TexCoord2(Float2 texCoord) noexcept;
	virtual void TexCoord2(Double2 texCoord) noexcept;

	virtual void Color2(Char2 color) noexcept;
	virtual void Color2(Byte2 color) noexcept;
	virtual void Color2(Short2 color) noexcept;
	virtual void Color2(UShort2 color) noexcept;
	virtual void Color2(Int2 color) noexcept;
	virtual void Color2(UInt2 color) noexcept;
	virtual void Color2(Long2 color) noexcept;
	virtual void Color2(ULong2 color) noexcept;
	virtual void Color2(Float2 color) noexcept;
	virtual void Color2(Double2 color) noexcept;

	virtual void Color3(Char3 color) noexcept;
	virtual void Color3(Byte3 color) noexcept;
	virtual void Color3(Short3 color) noexcept;
	virtual void Color3(UShort3 color) noexcept;
	virtual void Color3(Int3 color) noexcept;
	virtual void Color3(UInt3 color) noexcept;
	virtual void Color3(const Long3& color) noexcept;
	virtual void Color3(const ULong3& color) noexcept;
	virtual void Color3(Float3 color) noexcept;
	virtual void Color3(const Double3& color) noexcept;

	void BgeinShape();
	void EndShape();
};

typedef enum VkFormat {
	VK_FORMAT_UNDEFINED = 0,
	VK_FORMAT_R4G4_UNORM_PACK8 = 1,
	VK_FORMAT_R4G4B4A4_UNORM_PACK16 = 2,
	VK_FORMAT_B4G4R4A4_UNORM_PACK16 = 3,
	VK_FORMAT_R5G6B5_UNORM_PACK16 = 4,
	VK_FORMAT_B5G6R5_UNORM_PACK16 = 5,
	VK_FORMAT_R5G5B5A1_UNORM_PACK16 = 6,
	VK_FORMAT_B5G5R5A1_UNORM_PACK16 = 7,
	VK_FORMAT_A1R5G5B5_UNORM_PACK16 = 8,
	VK_FORMAT_R8_UNORM = 9,
	VK_FORMAT_R8_SNORM = 10,
	VK_FORMAT_R8_USCALED = 11,
	VK_FORMAT_R8_SSCALED = 12,
	VK_FORMAT_R8_UINT = 13,
	VK_FORMAT_R8_SINT = 14,
	VK_FORMAT_R8_SRGB = 15,
	VK_FORMAT_R8G8_UNORM = 16,
	VK_FORMAT_R8G8_SNORM = 17,
	VK_FORMAT_R8G8_USCALED = 18,
	VK_FORMAT_R8G8_SSCALED = 19,
	VK_FORMAT_R8G8_UINT = 20,
	VK_FORMAT_R8G8_SINT = 21,
	VK_FORMAT_R8G8_SRGB = 22,
	VK_FORMAT_R8G8B8_UNORM = 23,
	VK_FORMAT_R8G8B8_SNORM = 24,
	VK_FORMAT_R8G8B8_USCALED = 25,
	VK_FORMAT_R8G8B8_SSCALED = 26,
	VK_FORMAT_R8G8B8_UINT = 27,
	VK_FORMAT_R8G8B8_SINT = 28,
	VK_FORMAT_R8G8B8_SRGB = 29,
	VK_FORMAT_B8G8R8_UNORM = 30,
	VK_FORMAT_B8G8R8_SNORM = 31,
	VK_FORMAT_B8G8R8_USCALED = 32,
	VK_FORMAT_B8G8R8_SSCALED = 33,
	VK_FORMAT_B8G8R8_UINT = 34,
	VK_FORMAT_B8G8R8_SINT = 35,
	VK_FORMAT_B8G8R8_SRGB = 36,
	VK_FORMAT_R8G8B8A8_UNORM = 37,
	VK_FORMAT_R8G8B8A8_SNORM = 38,
	VK_FORMAT_R8G8B8A8_USCALED = 39,
	VK_FORMAT_R8G8B8A8_SSCALED = 40,
	VK_FORMAT_R8G8B8A8_UINT = 41,
	VK_FORMAT_R8G8B8A8_SINT = 42,
	VK_FORMAT_R8G8B8A8_SRGB = 43,
	VK_FORMAT_B8G8R8A8_UNORM = 44,
	VK_FORMAT_B8G8R8A8_SNORM = 45,
	VK_FORMAT_B8G8R8A8_USCALED = 46,
	VK_FORMAT_B8G8R8A8_SSCALED = 47,
	VK_FORMAT_B8G8R8A8_UINT = 48,
	VK_FORMAT_B8G8R8A8_SINT = 49,
	VK_FORMAT_B8G8R8A8_SRGB = 50,
	VK_FORMAT_A8B8G8R8_UNORM_PACK32 = 51,
	VK_FORMAT_A8B8G8R8_SNORM_PACK32 = 52,
	VK_FORMAT_A8B8G8R8_USCALED_PACK32 = 53,
	VK_FORMAT_A8B8G8R8_SSCALED_PACK32 = 54,
	VK_FORMAT_A8B8G8R8_UINT_PACK32 = 55,
	VK_FORMAT_A8B8G8R8_SINT_PACK32 = 56,
	VK_FORMAT_A8B8G8R8_SRGB_PACK32 = 57,
	VK_FORMAT_A2R10G10B10_UNORM_PACK32 = 58,
	VK_FORMAT_A2R10G10B10_SNORM_PACK32 = 59,
	VK_FORMAT_A2R10G10B10_USCALED_PACK32 = 60,
	VK_FORMAT_A2R10G10B10_SSCALED_PACK32 = 61,
	VK_FORMAT_A2R10G10B10_UINT_PACK32 = 62,
	VK_FORMAT_A2R10G10B10_SINT_PACK32 = 63,
	VK_FORMAT_A2B10G10R10_UNORM_PACK32 = 64,
	VK_FORMAT_A2B10G10R10_SNORM_PACK32 = 65,
	VK_FORMAT_A2B10G10R10_USCALED_PACK32 = 66,
	VK_FORMAT_A2B10G10R10_SSCALED_PACK32 = 67,
	VK_FORMAT_A2B10G10R10_UINT_PACK32 = 68,
	VK_FORMAT_A2B10G10R10_SINT_PACK32 = 69,
	VK_FORMAT_R16_UNORM = 70,
	VK_FORMAT_R16_SNORM = 71,
	VK_FORMAT_R16_USCALED = 72,
	VK_FORMAT_R16_SSCALED = 73,
	VK_FORMAT_R16_UINT = 74,
	VK_FORMAT_R16_SINT = 75,
	VK_FORMAT_R16_SFLOAT = 76,
	VK_FORMAT_R16G16_UNORM = 77,
	VK_FORMAT_R16G16_SNORM = 78,
	VK_FORMAT_R16G16_USCALED = 79,
	VK_FORMAT_R16G16_SSCALED = 80,
	VK_FORMAT_R16G16_UINT = 81,
	VK_FORMAT_R16G16_SINT = 82,
	VK_FORMAT_R16G16_SFLOAT = 83,
	VK_FORMAT_R16G16B16_UNORM = 84,
	VK_FORMAT_R16G16B16_SNORM = 85,
	VK_FORMAT_R16G16B16_USCALED = 86,
	VK_FORMAT_R16G16B16_SSCALED = 87,
	VK_FORMAT_R16G16B16_UINT = 88,
	VK_FORMAT_R16G16B16_SINT = 89,
	VK_FORMAT_R16G16B16_SFLOAT = 90,
	VK_FORMAT_R16G16B16A16_UNORM = 91,
	VK_FORMAT_R16G16B16A16_SNORM = 92,
	VK_FORMAT_R16G16B16A16_USCALED = 93,
	VK_FORMAT_R16G16B16A16_SSCALED = 94,
	VK_FORMAT_R16G16B16A16_UINT = 95,
	VK_FORMAT_R16G16B16A16_SINT = 96,
	VK_FORMAT_R16G16B16A16_SFLOAT = 97,
	VK_FORMAT_R32_UINT = 98,
	VK_FORMAT_R32_SINT = 99,
	VK_FORMAT_R32_SFLOAT = 100,
	VK_FORMAT_R32G32_UINT = 101,
	VK_FORMAT_R32G32_SINT = 102,
	VK_FORMAT_R32G32_SFLOAT = 103,
	VK_FORMAT_R32G32B32_UINT = 104,
	VK_FORMAT_R32G32B32_SINT = 105,
	VK_FORMAT_R32G32B32_SFLOAT = 106,
	VK_FORMAT_R32G32B32A32_UINT = 107,
	VK_FORMAT_R32G32B32A32_SINT = 108,
	VK_FORMAT_R32G32B32A32_SFLOAT = 109,
	VK_FORMAT_R64_UINT = 110,
	VK_FORMAT_R64_SINT = 111,
	VK_FORMAT_R64_SFLOAT = 112,
	VK_FORMAT_R64G64_UINT = 113,
	VK_FORMAT_R64G64_SINT = 114,
	VK_FORMAT_R64G64_SFLOAT = 115,
	VK_FORMAT_R64G64B64_UINT = 116,
	VK_FORMAT_R64G64B64_SINT = 117,
	VK_FORMAT_R64G64B64_SFLOAT = 118,
	VK_FORMAT_R64G64B64A64_UINT = 119,
	VK_FORMAT_R64G64B64A64_SINT = 120,
	VK_FORMAT_R64G64B64A64_SFLOAT = 121,
	VK_FORMAT_MAX_ENUM = 0x7FFFFFFF
} VkFormat;

template <class T>
struct AttributeFormat {
	
};

template <class AttributeFormat>
struct BindingInfo {
	std::string_view Name;
	unsigned int BindingPoint;
	AttributeFormat Attribute;
};