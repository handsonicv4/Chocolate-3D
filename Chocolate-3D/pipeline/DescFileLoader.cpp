#include"DescFileLoader.h"
#include"json11/json11.hpp"
#include<fstream>
#include <sstream>
using namespace FileLoader;
using namespace json11;

#define CHECKKEY(OBJ,MAP) (OBJ.is_string() && MAP.count(OBJ.string_value()))

unordered_map<string, ResourceType> resourceType;
unordered_map<string, AccessType> accessType;
unordered_map<string, BindFlag> bindFlag;
unordered_map<string, PiplineStage> piplineStage;
unordered_map<string, DXGI_FORMAT> format;
unordered_map<string, D3D11_COMPARISON_FUNC> comparisionFunc;
unordered_map<string, D3D11_STENCIL_OP> stencilOp;
unordered_map<string, D3D11_BLEND> blend;
unordered_map<string, D3D11_BLEND_OP> blendOp;
unordered_map<string, D3D11_FILL_MODE> fillMode;
unordered_map<string, D3D11_CULL_MODE> cullMode;
unordered_map<string, D3D11_FILTER> filter;
unordered_map<string, D3D11_TEXTURE_ADDRESS_MODE> textureAddressMode;
unordered_map<string, D3D11_PRIMITIVE_TOPOLOGY> topology;

void init() {
	resourceType["buffer"] = Resource_Buffer;
	resourceType["texture1d"] = Resource_Texture1D;
	resourceType["texture2d"] = Resource_Texture2D;
	resourceType["texture3d"] = Resource_Texture3D;

	accessType["default"] = Access_Default;
	accessType["immutable"] = Access_Immutable;
	accessType["dynamic"] = Access_Dynamic;
	accessType["staging"] = Access_Staging;

	//D3D11_BIND_FLAG
	bindFlag["vertex_buffer"] = Bind_Vertex_Buffer;
	bindFlag["index_buffer"] = Bind_Index_Buffer;
	bindFlag["constant_buffer"] = Bind_Constant_Buffer;
	bindFlag["shader_resource"] = Bind_Shader_Resource;
	bindFlag["stream_out"] = Bind_Stream_Out;
	bindFlag["render_target"] = Bind_Render_Target;
	bindFlag["depth_stencil"] = Bind_Depth_Stencil;
	bindFlag["unordered_access"] = Bind_Unordered_Access;
	bindFlag["decoder"] = Bind_Decoder;
	bindFlag["encoder"] = Bind_Video_Encoder;

	piplineStage["input_assembler"] = Stage_Input_Assembler;
	piplineStage["vertex_shader"] = Stage_Vertex_Shader;
	piplineStage["hull_shader"] = Stage_Hull_Shader;
	piplineStage["domain_shader"] = Stage_Domain_Shader;
	piplineStage["geometry_shader"] = Stage_Geometry_Shader;
	piplineStage["stream_out"] = Stage_Stream_Out;
	piplineStage["rasterizer"] = Stage_Rasterizer_Stage;
	piplineStage["pixel_shader"] = Stage_Pixel_Shader;
	piplineStage["output_merge"] = Stage_Output_Merge;
	piplineStage["compute_shader"] = Stage_Compute_Shader;

	comparisionFunc["never"] = D3D11_COMPARISON_NEVER;
	comparisionFunc["less"] = D3D11_COMPARISON_LESS;
	comparisionFunc["equal"] = D3D11_COMPARISON_EQUAL;
	comparisionFunc["less_equal"] = D3D11_COMPARISON_LESS_EQUAL;
	comparisionFunc["greater"] = D3D11_COMPARISON_GREATER;
	comparisionFunc["not_equal"] = D3D11_COMPARISON_NOT_EQUAL;
	comparisionFunc["greater_equal"] = D3D11_COMPARISON_GREATER_EQUAL;
	comparisionFunc["always"] = D3D11_COMPARISON_ALWAYS;

	stencilOp["op_keep"] = D3D11_STENCIL_OP_KEEP;
	stencilOp["op_zero"] = D3D11_STENCIL_OP_ZERO;
	stencilOp["op_replace"] = D3D11_STENCIL_OP_REPLACE;
	stencilOp["op_incr_sat"] = D3D11_STENCIL_OP_INCR_SAT;
	stencilOp["op_decr_sat"] = D3D11_STENCIL_OP_DECR_SAT;
	stencilOp["op_invert"] = D3D11_STENCIL_OP_INVERT;
	stencilOp["op_incr"] = D3D11_STENCIL_OP_INCR;
	stencilOp["op_decr"] = D3D11_STENCIL_OP_DECR;

	blendOp["add"] = D3D11_BLEND_OP_ADD;
	blendOp["subtract"] = D3D11_BLEND_OP_SUBTRACT;
	blendOp["rev_subtract"] = D3D11_BLEND_OP_REV_SUBTRACT;
	blendOp["min"] = D3D11_BLEND_OP_MIN;
	blendOp["max"] = D3D11_BLEND_OP_MAX;

	fillMode["wireframe"] = D3D11_FILL_WIREFRAME;
	fillMode["solid"] = D3D11_FILL_SOLID;

	cullMode["none"] = D3D11_CULL_NONE;
	cullMode["front"] = D3D11_CULL_FRONT;
	cullMode["back"] = D3D11_CULL_BACK;

	blend["zero"] = D3D11_BLEND_ZERO;
	blend["one"] = D3D11_BLEND_ONE;
	blend["src_color"] = D3D11_BLEND_SRC_COLOR;
	blend["inv_src_color"] = D3D11_BLEND_INV_SRC_COLOR;
	blend["src_alpha"] = D3D11_BLEND_SRC_ALPHA;
	blend["inv_src_alpha"] = D3D11_BLEND_INV_SRC_ALPHA;
	blend["dest_alpha"] = D3D11_BLEND_DEST_ALPHA;
	blend["inv_dest_alpha"] = D3D11_BLEND_INV_DEST_ALPHA;
	blend["dest_color"] = D3D11_BLEND_DEST_COLOR;
	blend["inv_dest_color"] = D3D11_BLEND_INV_DEST_COLOR;
	blend["src_alpha_sat"] = D3D11_BLEND_SRC_ALPHA_SAT;
	blend["blend_factor"] = D3D11_BLEND_BLEND_FACTOR;
	blend["inv_blend_factor"] = D3D11_BLEND_INV_BLEND_FACTOR;
	blend["src1_color"] = D3D11_BLEND_SRC1_COLOR;
	blend["inv_src1_color"] = D3D11_BLEND_INV_SRC1_COLOR;
	blend["src1_alpha"] = D3D11_BLEND_SRC1_ALPHA;
	blend["inv_src1_alpha"] = D3D11_BLEND_INV_SRC1_ALPHA;

	textureAddressMode["wrap"] = D3D11_TEXTURE_ADDRESS_WRAP;
	textureAddressMode["mirror"] = D3D11_TEXTURE_ADDRESS_MIRROR;
	textureAddressMode["clamp"] = D3D11_TEXTURE_ADDRESS_CLAMP;
	textureAddressMode["border"] = D3D11_TEXTURE_ADDRESS_BORDER;
	textureAddressMode["mirror_once"] = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;

	filter["min_mag_mip_point"] = D3D11_FILTER_MIN_MAG_MIP_POINT;
	filter["min_mag_point_mip_linear"] = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
	filter["min_point_mag_linear_mip_point"] = D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
	filter["min_point_mag_mip_linear"] = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
	filter["min_linear_mag_mip_point"] = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
	filter["min_linear_mag_point_mip_linear"] = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
	filter["min_mag_linear_mip_point"] = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	filter["min_mag_mip_linear"] = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	filter["anisotropic"] = D3D11_FILTER_ANISOTROPIC;
	filter["comparison_min_mag_mip_point"] = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
	filter["comparison_min_mag_point_mip_linear"] = D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
	filter["comparison_min_point_mag_linear_mip_point"] = D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
	filter["comparison_min_point_mag_mip_linear"] = D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
	filter["comparison_min_linear_mag_mip_point"] = D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
	filter["comparison_min_linear_mag_point_mip_linear"] = D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
	filter["comparison_min_mag_linear_mip_point"] = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	filter["comparison_min_mag_mip_linear"] = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	filter["comparison_anisotropic"] = D3D11_FILTER_COMPARISON_ANISOTROPIC;
	filter["minimum_min_mag_mip_point"] = D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT;
	filter["minimum_min_mag_point_mip_linear"] = D3D11_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR;
	filter["minimum_min_point_mag_linear_mip_point"] = D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
	filter["minimum_min_point_mag_mip_linear"] = D3D11_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR;
	filter["minimum_min_linear_mag_mip_point"] = D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT;
	filter["minimum_min_linear_mag_point_mip_linear"] = D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
	filter["minimum_min_mag_linear_mip_point"] = D3D11_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT;
	filter["minimum_min_mag_mip_linear"] = D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR;
	filter["minimum_anisotropic"] = D3D11_FILTER_MINIMUM_ANISOTROPIC;
	filter["maximum_min_mag_mip_point"] = D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT;
	filter["maximum_min_mag_point_mip_linear"] = D3D11_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR;
	filter["maximum_min_point_mag_linear_mip_point"] = D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
	filter["maximum_min_point_mag_mip_linear"] = D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR;
	filter["maximum_min_linear_mag_mip_point"] = D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT;
	filter["maximum_min_linear_mag_point_mip_linear"] = D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
	filter["maximum_min_mag_linear_mip_point"] = D3D11_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT;
	filter["maximum_min_mag_mip_linear"] = D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR;
	filter["maximum_anisotropic"] = D3D11_FILTER_MAXIMUM_ANISOTROPIC;

	topology["undefined"] = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	topology["pointlist"] = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	topology["linelist"] = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	topology["linestrip"] = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	topology["trianglelist"] = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	topology["trianglestrip"] = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	topology["linelist_adj"] = D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
	topology["linestrip_adj"] = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
	topology["trianglelist_adj"] = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
	topology["trianglestrip_adj"] = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
	topology["1_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST;
	topology["2_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST;
	topology["3_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
	topology["4_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;
	topology["5_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST;
	topology["6_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST;
	topology["7_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST;
	topology["8_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST;
	topology["9_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST;
	topology["10_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST;
	topology["11_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST;
	topology["12_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST;
	topology["13_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST;
	topology["14_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST;
	topology["15_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST;
	topology["16_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST;
	topology["17_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST;
	topology["18_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST;
	topology["19_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST;
	topology["20_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST;
	topology["21_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST;
	topology["22_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST;
	topology["23_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST;
	topology["24_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST;
	topology["25_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST;
	topology["26_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST;
	topology["27_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST;
	topology["28_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST;
	topology["29_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST;
	topology["30_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST;
	topology["31_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST;
	topology["32_control_point_patchlist"] = D3D_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST;


	format["unknown"] = DXGI_FORMAT_UNKNOWN;
	format["r32g32b32a32_typeless"] = DXGI_FORMAT_R32G32B32A32_TYPELESS;
	format["r32g32b32a32_float"] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	format["r32g32b32a32_uint"] = DXGI_FORMAT_R32G32B32A32_UINT;
	format["r32g32b32a32_sint"] = DXGI_FORMAT_R32G32B32A32_SINT;
	format["r32g32b32_typeless"] = DXGI_FORMAT_R32G32B32_TYPELESS;
	format["r32g32b32_float"] = DXGI_FORMAT_R32G32B32_FLOAT;
	format["r32g32b32_uint"] = DXGI_FORMAT_R32G32B32_UINT;
	format["r32g32b32_sint"] = DXGI_FORMAT_R32G32B32_SINT;
	format["r16g16b16a16_typeless"] = DXGI_FORMAT_R16G16B16A16_TYPELESS;
	format["r16g16b16a16_float"] = DXGI_FORMAT_R16G16B16A16_FLOAT;
	format["r16g16b16a16_unorm"] = DXGI_FORMAT_R16G16B16A16_UNORM;
	format["r16g16b16a16_uint"] = DXGI_FORMAT_R16G16B16A16_UINT;
	format["r16g16b16a16_snorm"] = DXGI_FORMAT_R16G16B16A16_SNORM;
	format["r16g16b16a16_sint"] = DXGI_FORMAT_R16G16B16A16_SINT;
	format["r32g32_typeless"] = DXGI_FORMAT_R32G32_TYPELESS;
	format["r32g32_float"] = DXGI_FORMAT_R32G32_FLOAT;
	format["r32g32_uint"] = DXGI_FORMAT_R32G32_UINT;
	format["r32g32_sint"] = DXGI_FORMAT_R32G32_SINT;
	format["r32g8x24_typeless"] = DXGI_FORMAT_R32G8X24_TYPELESS;
	format["d32_float_s8x24_uint"] = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	format["r32_float_x8x24_typeless"] = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
	format["x32_typeless_g8x24_uint"] = DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
	format["r10g10b10a2_typeless"] = DXGI_FORMAT_R10G10B10A2_TYPELESS;
	format["r10g10b10a2_unorm"] = DXGI_FORMAT_R10G10B10A2_UNORM;
	format["r10g10b10a2_uint"] = DXGI_FORMAT_R10G10B10A2_UINT;
	format["r11g11b10_float"] = DXGI_FORMAT_R11G11B10_FLOAT;
	format["r8g8b8a8_typeless"] = DXGI_FORMAT_R8G8B8A8_TYPELESS;
	format["r8g8b8a8_unorm"] = DXGI_FORMAT_R8G8B8A8_UNORM;
	format["r8g8b8a8_unorm_srgb"] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	format["r8g8b8a8_uint"] = DXGI_FORMAT_R8G8B8A8_UINT;
	format["r8g8b8a8_snorm"] = DXGI_FORMAT_R8G8B8A8_SNORM;
	format["r8g8b8a8_sint"] = DXGI_FORMAT_R8G8B8A8_SINT;
	format["r16g16_typeless"] = DXGI_FORMAT_R16G16_TYPELESS;
	format["r16g16_float"] = DXGI_FORMAT_R16G16_FLOAT;
	format["r16g16_unorm"] = DXGI_FORMAT_R16G16_UNORM;
	format["r16g16_uint"] = DXGI_FORMAT_R16G16_UINT;
	format["r16g16_snorm"] = DXGI_FORMAT_R16G16_SNORM;
	format["r16g16_sint"] = DXGI_FORMAT_R16G16_SINT;
	format["r32_typeless"] = DXGI_FORMAT_R32_TYPELESS;
	format["d32_float"] = DXGI_FORMAT_D32_FLOAT;
	format["r32_float"] = DXGI_FORMAT_R32_FLOAT;
	format["r32_uint"] = DXGI_FORMAT_R32_UINT;
	format["r32_sint"] = DXGI_FORMAT_R32_SINT;
	format["r24g8_typeless"] = DXGI_FORMAT_R24G8_TYPELESS;
	format["d24_unorm_s8_uint"] = DXGI_FORMAT_D24_UNORM_S8_UINT;
	format["r24_unorm_x8_typeless"] = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	format["x24_typeless_g8_uint"] = DXGI_FORMAT_X24_TYPELESS_G8_UINT;
	format["r8g8_typeless"] = DXGI_FORMAT_R8G8_TYPELESS;
	format["r8g8_unorm"] = DXGI_FORMAT_R8G8_UNORM;
	format["r8g8_uint"] = DXGI_FORMAT_R8G8_UINT;
	format["r8g8_snorm"] = DXGI_FORMAT_R8G8_SNORM;
	format["r8g8_sint"] = DXGI_FORMAT_R8G8_SINT;
	format["r16_typeless"] = DXGI_FORMAT_R16_TYPELESS;
	format["r16_float"] = DXGI_FORMAT_R16_FLOAT;
	format["d16_unorm"] = DXGI_FORMAT_D16_UNORM;
	format["r16_unorm"] = DXGI_FORMAT_R16_UNORM;
	format["r16_uint"] = DXGI_FORMAT_R16_UINT;
	format["r16_snorm"] = DXGI_FORMAT_R16_SNORM;
	format["r16_sint"] = DXGI_FORMAT_R16_SINT;
	format["r8_typeless"] = DXGI_FORMAT_R8_TYPELESS;
	format["r8_unorm"] = DXGI_FORMAT_R8_UNORM;
	format["r8_uint"] = DXGI_FORMAT_R8_UINT;
	format["r8_snorm"] = DXGI_FORMAT_R8_SNORM;
	format["r8_sint"] = DXGI_FORMAT_R8_SINT;
	format["a8_unorm"] = DXGI_FORMAT_A8_UNORM;
	format["r1_unorm"] = DXGI_FORMAT_R1_UNORM;
	format["r9g9b9e5_sharedexp"] = DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
	format["r8g8_b8g8_unorm"] = DXGI_FORMAT_R8G8_B8G8_UNORM;
	format["g8r8_g8b8_unorm"] = DXGI_FORMAT_G8R8_G8B8_UNORM;
	format["bc1_typeless"] = DXGI_FORMAT_BC1_TYPELESS;
	format["bc1_unorm"] = DXGI_FORMAT_BC1_UNORM;
	format["bc1_unorm_srgb"] = DXGI_FORMAT_BC1_UNORM_SRGB;
	format["bc2_typeless"] = DXGI_FORMAT_BC2_TYPELESS;
	format["bc2_unorm"] = DXGI_FORMAT_BC2_UNORM;
	format["bc2_unorm_srgb"] = DXGI_FORMAT_BC2_UNORM_SRGB;
	format["bc3_typeless"] = DXGI_FORMAT_BC3_TYPELESS;
	format["bc3_unorm"] = DXGI_FORMAT_BC3_UNORM;
	format["bc3_unorm_srgb"] = DXGI_FORMAT_BC3_UNORM_SRGB;
	format["bc4_typeless"] = DXGI_FORMAT_BC4_TYPELESS;
	format["bc4_unorm"] = DXGI_FORMAT_BC4_UNORM;
	format["bc4_snorm"] = DXGI_FORMAT_BC4_SNORM;
	format["bc5_typeless"] = DXGI_FORMAT_BC5_TYPELESS;
	format["bc5_unorm"] = DXGI_FORMAT_BC5_UNORM;
	format["bc5_snorm"] = DXGI_FORMAT_BC5_SNORM;
	format["b5g6r5_unorm"] = DXGI_FORMAT_B5G6R5_UNORM;
	format["b5g5r5a1_unorm"] = DXGI_FORMAT_B5G5R5A1_UNORM;
	format["b8g8r8a8_unorm"] = DXGI_FORMAT_B8G8R8A8_UNORM;
	format["b8g8r8x8_unorm"] = DXGI_FORMAT_B8G8R8X8_UNORM;
	format["r10g10b10_xr_bias_a2_unorm"] = DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;
	format["b8g8r8a8_typeless"] = DXGI_FORMAT_B8G8R8A8_TYPELESS;
	format["b8g8r8a8_unorm_srgb"] = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
	format["b8g8r8x8_typeless"] = DXGI_FORMAT_B8G8R8X8_TYPELESS;
	format["b8g8r8x8_unorm_srgb"] = DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
	format["bc6h_typeless"] = DXGI_FORMAT_BC6H_TYPELESS;
	format["bc6h_uf16"] = DXGI_FORMAT_BC6H_UF16;
	format["bc6h_sf16"] = DXGI_FORMAT_BC6H_SF16;
	format["bc7_typeless"] = DXGI_FORMAT_BC7_TYPELESS;
	format["bc7_unorm"] = DXGI_FORMAT_BC7_UNORM;
	format["bc7_unorm_srgb"] = DXGI_FORMAT_BC7_UNORM_SRGB;
	format["ayuv"] = DXGI_FORMAT_AYUV;
	format["y410"] = DXGI_FORMAT_Y410;
	format["y416"] = DXGI_FORMAT_Y416;
	format["nv12"] = DXGI_FORMAT_NV12;
	format["p010"] = DXGI_FORMAT_P010;
	format["p016"] = DXGI_FORMAT_P016;
	format["420_opaque"] = DXGI_FORMAT_420_OPAQUE;
	format["yuy2"] = DXGI_FORMAT_YUY2;
	format["y210"] = DXGI_FORMAT_Y210;
	format["y216"] = DXGI_FORMAT_Y216;
	format["nv11"] = DXGI_FORMAT_NV11;
	format["ai44"] = DXGI_FORMAT_AI44;
	format["ia44"] = DXGI_FORMAT_IA44;
	format["p8"] = DXGI_FORMAT_P8;
	format["a8p8"] = DXGI_FORMAT_A8P8;
	format["b4g4r4a4_unorm"] = DXGI_FORMAT_B4G4R4A4_UNORM;
	format["p208"] = DXGI_FORMAT_P208;
	format["v208"] = DXGI_FORMAT_V208;
	format["v408"] = DXGI_FORMAT_V408;
	format["force_uint"] = DXGI_FORMAT_FORCE_UINT;

}

Json  LoadJson(const string & filePath) {
	ifstream file;
	try
	{
		file.open(filePath);
	}
	catch (exception err)
	{
		throw exception(("Open File Error: " + filePath).c_str());
	}
	stringstream ss;
	ss << file.rdbuf();
	file.close();

	
	string err;
	Json json = Json::parse(ss.str(), err,json11::COMMENTS);
	if (err != "") {
		throw exception(("Json Format Error: " + filePath + err).c_str());
	}
	return move(json);
}

ResourceDesc FileLoader::LoadResourceDesc(const string & filePath)
{
	if (!resourceType.size()) init();
	Json json = LoadJson(filePath);
	ResourceDesc desc;
	ZeroMemory(&desc, sizeof(desc));

	if (!CHECKKEY(json["type"], resourceType)) 
	{
		throw exception(("Can't find or recognize \"type\". In file : " + filePath).c_str());
	}
	desc.type = resourceType[json["type"].string_value()];

	if (!CHECKKEY(json["access"], accessType)) 
	{
		throw exception(("Can't find or recognize \"access\". In file : " + filePath).c_str());
	}
	desc.access = accessType[json["access"].string_value()];

	if (!json["bind_flag"].is_array()) 
	{
		throw exception(("Can't find or recognize \"bind_flag\". In file : " + filePath).c_str());
	}
	auto arr = json["bind_flag"].array_items();
	for (auto& e : arr) 
	{
		if (!CHECKKEY(e, bindFlag)) 
		{
			throw exception(("bind_flag item error" + filePath).c_str());
		}
		desc.bindFlag |= bindFlag[e.string_value()];
	}

	if (json["name"].is_string())
		desc.name = json["name"].string_value();

	if (desc.type == Resource_Buffer)
	{
		if (!json["element_stride"].is_number())
		{
			throw exception(("Can't find or recognize \"element_stride\". In file : " + filePath).c_str());
		}
		desc.elementStride = json["element_stride"].int_value();
	}
	else
	{
		if (!CHECKKEY(json["format"], format))
		{
			throw exception(("Can't find or recognize \"format\". In file : " + filePath).c_str());
		}
		desc.format = format[json["format"].string_value()];

		if (!json["mip_level"].is_number())
		{
			throw exception(("Can't find or recognize \"mip_level\". In file : " + filePath).c_str());
		}
		desc.mipLevel = json["mip_level"].int_value();
	}

	if (!json["size"].is_array() || json["size"].array_items().size() == 0)
	{
		throw exception(("Can't find or recognize \"size\". In file : " + filePath).c_str());
	}
	arr = json["size"].array_items();
	for (size_t i = 0; i < arr.size() && i < 3; i++) 
	{
		if (!arr[i].is_number())
		{
			throw exception(("size item error" + filePath).c_str());
		}
		desc.size[i] = arr[i].int_value();
	}
	return desc;
}

D3D11_DEPTH_STENCILOP_DESC LoadStencilOpDesc(const Json &json) 
{
	D3D11_DEPTH_STENCILOP_DESC desc;
	if (!CHECKKEY(json["stencil_fail_op"], stencilOp)) throw exception("Can't find or recognize \"stencil_fail_op\". ");
	desc.StencilFailOp = stencilOp[json["stencil_fail_op"].string_value()];

	if (!CHECKKEY(json["stencil_depth_fail_op"], stencilOp)) throw exception("Can't find or recognize \"stencil_depth_fail_op\". ");
	desc.StencilDepthFailOp = stencilOp[json["stencil_depth_fail_op"].string_value()];

	if (!CHECKKEY(json["stencil_pass_op"], stencilOp)) throw exception("Can't find or recognize \"stencil_pass_op\". ");
	desc.StencilPassOp = stencilOp[json["stencil_pass_op"].string_value()];

	if (!CHECKKEY(json["stencil_func"], comparisionFunc)) throw exception("Can't find or recognize \"stencil_func\". ");
	desc.StencilFunc = comparisionFunc[json["stencil_func"].string_value()];

	return move(desc);
}

DepthStencilDesc FileLoader::LoadDepthStencilDesc(const string & filePath) 
{
	if (!resourceType.size()) init();
	Json json = LoadJson(filePath);
	DepthStencilDesc desc;
	ZeroMemory(&desc, sizeof(desc));

	if (!json["depth_enable"].is_bool())
	{
		throw exception(("Can't find or recognize \"depth_enable\". In file : " + filePath).c_str());
	}
	desc.DepthEnable = json["depth_enable"].bool_value();

	if (desc.DepthEnable)
	{
		if (!json["depth_write"].is_bool())
		{
			throw exception(("Can't find or recognize \"depth_write\". In file : " + filePath).c_str());
		}
		desc.DepthWriteMask = json["depth_write"].bool_value() ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;

		if (!CHECKKEY(json["depth_func"], comparisionFunc))
		{
			throw exception(("Can't find or recognize \"depth_func\". In file : " + filePath).c_str());
		}
		desc.DepthFunc = comparisionFunc[json["depth_func"].string_value()];
	}
	
	if (!json["stencil_enable"].is_bool())
	{
		throw exception(("Can't find or recognize \"depth_enable\". In file : " + filePath).c_str());
	}
	desc.StencilEnable = json["stencil_enable"].bool_value();

	if (desc.StencilEnable)
	{
		if (!json["stencil_read_mask"].is_number() || json["stencil_read_mask"].number_value() < 0  || json["stencil_read_mask"].number_value() > 0xff)
		{
			throw exception(("Can't find or recognize \"stencil_read_mask\". In file : " + filePath).c_str());
		}
		desc.StencilReadMask = json["stencil_read_mask"].int_value();

		if (!json["stencil_write_mask"].is_number() || json["stencil_write_mask"].number_value() < 0 || json["stencil_write_mask"].number_value() > 0xff)
		{
			throw exception(("Can't find or recognize \"stencil_write_mask\". In file : " + filePath).c_str());
		}
		desc.StencilWriteMask = json["stencil_write_mask"].int_value();

		if (!json["front_face"].is_object())
		{
			throw exception(("Can't find or recognize \"front_face\". In file : " + filePath).c_str());
		}
		desc.FrontFace = LoadStencilOpDesc(json["front_face"]);
	
		if (!json["back_face"].is_object())
		{
			throw exception(("Can't find or recognize \"back_face\". In file : " + filePath).c_str());
		}
		desc.BackFace = LoadStencilOpDesc(json["back_face"]);
	}
	return move(desc);
}

BlendDesc FileLoader::LoadBlendDesc(const string & filePath)
{
	if (!resourceType.size()) init();
	Json json = LoadJson(filePath);
	BlendDesc desc;
	ZeroMemory(&desc, sizeof(desc));

	if (!json["alpha_to_coverage"].is_bool())
	{
		throw exception(("Can't find or recognize \"alpha_to_coverage\". In file : " + filePath).c_str());
	}
	desc.AlphaToCoverageEnable = json["alpha_to_coverage"].bool_value();

	if (!json["independent_blend"].is_bool())
	{
		throw exception(("Can't find or recognize \"independent_blend\". In file : " + filePath).c_str());
	}
	desc.IndependentBlendEnable = json["independent_blend"].bool_value();

	if (!json["render_target"].is_array())
	{
		throw exception(("Can't find or recognize \"render_target\". In file : " + filePath).c_str());
	}
	auto arr = json["render_target"].array_items();

	for (size_t i = 0; i < arr.size() && i < 8; i++) 
	{
		if (!arr[i].is_object())
		{
			throw exception(("Can't  recognize elements in \"render_target\". In file : " + filePath).c_str());
		}
		if (!arr[i]["blend_enable"].is_bool())
		{
			throw exception(("Can't find or recognize \"blend_enable\". In file : " + filePath).c_str());
		}
		desc.RenderTarget[i].BlendEnable = arr[i]["blend_enable"].bool_value();

		if (!CHECKKEY(arr[i]["src_blend"], blend)) 
		{
			throw exception(("Can't find or recognize \"src_blend\". In file : " + filePath).c_str());
		}
		desc.RenderTarget[i].SrcBlend = blend[arr[i]["src_blend"].string_value()];

		if (!CHECKKEY(arr[i]["dest_blend"], blend))
		{
			throw exception(("Can't find or recognize \"dest_blend\". In file : " + filePath).c_str());
		}
		desc.RenderTarget[i].DestBlend = blend[arr[i]["dest_blend"].string_value()];

		if (!CHECKKEY(arr[i]["blend_op"], blendOp))
		{
			throw exception(("Can't find or recognize \"blend_op\". In file : " + filePath).c_str());
		}
		desc.RenderTarget[i].BlendOp = blendOp[arr[i]["blend_op"].string_value()];

		if (!CHECKKEY(arr[i]["src_blend_alpha"], blend))
		{
			throw exception(("Can't find or recognize \"src_blend_alpha\". In file : " + filePath).c_str());
		}
		desc.RenderTarget[i].SrcBlendAlpha = blend[arr[i]["src_blend_alpha"].string_value()];

		if (!CHECKKEY(arr[i]["dest_blend_alpha"], blend))
		{
			throw exception(("Can't find or recognize \"dest_blend_alpha\". In file : " + filePath).c_str());
		}
		desc.RenderTarget[i].DestBlendAlpha = blend[arr[i]["dest_blend_alpha"].string_value()];

		if (!CHECKKEY(arr[i]["blend_op_alpha"], blendOp))
		{
			throw exception(("Can't find or recognize \"blend_op_alpha\". In file : " + filePath).c_str());
		}
		desc.RenderTarget[i].BlendOpAlpha = blendOp[arr[i]["blend_op_alpha"].string_value()];
		if (!arr[i]["write_mask"].is_number() || arr[i]["write_mask"].int_value() < 0 || arr[i]["write_mask"].int_value() > 0xff)
		{
			throw exception(("\"write_mask\" not legal. In file : " + filePath).c_str());
		}
		desc.RenderTarget[i].RenderTargetWriteMask = arr[i]["write_mask"].int_value();
	}
	return move(desc);
}

RasterizerDesc FileLoader::LoadRasterizerDesc(const string & filePath)
{
	if (!resourceType.size()) init();
	Json json = LoadJson(filePath);
	RasterizerDesc desc;
	ZeroMemory(&desc, sizeof(desc));

	if (!CHECKKEY(json["fill_mode"], fillMode))
	{
		throw exception(("Can't find or recognize \"fill_mode\". In file : " + filePath).c_str());
	}
	desc.FillMode = fillMode[json["fill_mode"].string_value()];

	if (!CHECKKEY(json["cull_mode"], cullMode))
	{
		throw exception(("Can't find or recognize \"cull_mode\". In file : " + filePath).c_str());
	}
	desc.CullMode = cullMode[json["cull_mode"].string_value()];

	if (!json["front_counter_clockwise"].is_bool()) 
	{
		throw exception(("Can't find or recognize \"front_counter_clockwise\". In file : " + filePath).c_str());
	}
	desc.FrontCounterClockwise = json["front_counter_clockwise"].bool_value();

	if (!json["depth_bias"].is_number())
	{
		throw exception(("Can't find or recognize \"depth_bias\". In file : " + filePath).c_str());
	}
	desc.DepthBias = json["depth_bias"].int_value();

	if (!json["depth_bias_clamp"].is_number())
	{
		throw exception(("Can't find or recognize \"depth_bias_clamp\". In file : " + filePath).c_str());
	}
	desc.DepthBiasClamp = (float)json["depth_bias_clamp"].number_value();

	if (!json["slope_scaled_depth_bias"].is_number())
	{
		throw exception(("Can't find or recognize \"slope_scaled_depth_bias\". In file : " + filePath).c_str());
	}
	desc.SlopeScaledDepthBias = (float)json["slope_scaled_depth_bias"].number_value();

	if (!json["depth_clip_enable"].is_bool())
	{
		throw exception(("Can't find or recognize \"depth_clip_enable\". In file : " + filePath).c_str());
	}
	desc.DepthClipEnable = json["depth_clip_enable"].bool_value();

	if (!json["scissor_enable"].is_bool())
	{
		throw exception(("Can't find or recognize \"scissor_enable\". In file : " + filePath).c_str());
	}
	desc.ScissorEnable = json["scissor_enable"].bool_value();

	if (!json["multisample_enable"].is_bool())
	{
		throw exception(("Can't find or recognize \"multisample_enable\". In file : " + filePath).c_str());
	}
	desc.MultisampleEnable = json["multisample_enable"].bool_value();

	if (!json["antialiased_line_enable"].is_bool())
	{
		throw exception(("Can't find or recognize \"antialiased_line_enable\". In file : " + filePath).c_str());
	}
	desc.AntialiasedLineEnable = json["antialiased_line_enable"].bool_value();

	return desc;
}

SamplerDesc FileLoader::LoadSamplerDesc(const string & filePath)
{
	if (!resourceType.size()) init();
	Json json = LoadJson(filePath);
	SamplerDesc desc;
	ZeroMemory(&desc, sizeof(desc));

	if (!CHECKKEY(json["filter"], filter))
	{
		throw exception(("Can't find or recognize \"filter\". In file : " + filePath).c_str());
	}
	desc.Filter = filter[json["filter"].string_value()];

	if (!CHECKKEY(json["address_u"], textureAddressMode))
	{
		throw exception(("Can't find or recognize \"address_u\". In file : " + filePath).c_str());
	}
	desc.AddressU = textureAddressMode[json["address_u"].string_value()];

	if (!CHECKKEY(json["address_v"], textureAddressMode))
	{
		throw exception(("Can't find or recognize \"address_v\". In file : " + filePath).c_str());
	}
	desc.AddressV = textureAddressMode[json["address_v"].string_value()];

	if (!CHECKKEY(json["address_w"], textureAddressMode))
	{
		throw exception(("Can't find or recognize \"address_w\". In file : " + filePath).c_str());
	}
	desc.AddressW = textureAddressMode[json["address_w"].string_value()];

	if (!CHECKKEY(json["comparison_func"], comparisionFunc))
	{
		throw exception(("Can't find or recognize \"comparison_func\". In file : " + filePath).c_str());
	}
	desc.ComparisonFunc = comparisionFunc[json["comparison_func"].string_value()];

	if (!json["max_anisotropy"].is_number())
	{
		throw exception(("Can't find or recognize \"max_anisotropy\". In file : " + filePath).c_str());
	}
	desc.MaxAnisotropy = json["max_anisotropy"].int_value();

	if (!json["border_color"].is_array() || json["border_color"].array_items().size() != 4)
	{
		throw exception(("Can't find or recognize \"border_color\". In file : " + filePath).c_str());
	}
	auto arr = json["border_color"].array_items();
	for (int i = 0; i < 4; i++) {
		if (!arr[i].is_number())
		{
			throw exception(("Can't recognize elements in \"border_color\". In file : " + filePath).c_str());
		}
		desc.BorderColor[i] = (float)arr[i].number_value();
	}

	if (!json["mip_lod_bias"].is_number())
	{
		throw exception(("Can't find or recognize \"mip_lod_bias\". In file : " + filePath).c_str());
	}
	desc.MipLODBias = (float)json["mip_lod_bias"].number_value();

	if (!json["min_lod"].is_number())
	{
		throw exception(("Can't find or recognize \"min_lod\". In file : " + filePath).c_str());
	}
	desc.MinLOD = (float)json["min_lod"].number_value();

	if (!json["max_lod"].is_number())
	{
		throw exception(("Can't find or recognize \"max_lod\". In file : " + filePath).c_str());
	}
	desc.MaxLOD = (float)json["max_lod"].number_value();

	return desc;
}