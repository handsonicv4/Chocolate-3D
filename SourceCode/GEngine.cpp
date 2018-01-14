#include"GEngine.h"
using namespace std;

GEngine::GEngine()
{
	numBonePerVertex = 4;
	numBonePerBatch = 1024;
	animationMatrix.reserve(numBonePerBatch);

	maxInstances = 256;
	instanceData.reserve(maxInstances);

	vsync_enabled = false;
	fullscreen = false;
	MaxLightNumber = 1024;
	oldResolutionX = 0;
	oldResolutionY = 0;

	resolutionX = 1280;
	resolutionY = 720;

	maxTileNumber = 8160;//...
	tileSize = 16;
	tileBatchSize = 16;
}

GEngine::~GEngine()
{
	Shutdown();
}

bool GEngine::Init(HWND window, bool fullscreen) 
{
	bool result = D3DManager::Init(window, fullscreen);
	if (!result)
		return false;

	result = InitBuffers();
	if (!result)
		return false;

	return true;
}

bool GEngine::InitBuffers()
{
	//Depth Stencil Buffer
	depthStencilBufferID = resources.CreateTexture2D(Bind_Depth_Stencil, DXGI_FORMAT_D24_UNORM_S8_UINT, false, false, resolutionX, resolutionY);
	if (depthStencilBufferID == -1)
		return false;

	//Constant Buffer PerFrame
	frameBufferID = resources.CreateBuffer(Bind_Constant_Buffer, true, sizeof(FrameBufferData));
	if (frameBufferID == -1)
		return false;

	//Constant Buffer PerObject
	objBufferID = resources.CreateBuffer(Bind_Constant_Buffer, true, sizeof(ObjBufferData));
	if (objBufferID == -1)
		return false;

	//BindMatrix Buffer 
	animationMatrixBufferID = resources.CreateBuffer(Bind_Shader_Resource, true, numBonePerBatch * sizeof(float[16]), NULL, 0, sizeof(float[16]));
	if (animationMatrixBufferID == -1)
		return false;

	//Instance Buffer 
	instanceBufferID = resources.CreateBuffer(Bind_Shader_Resource, true, maxInstances * sizeof(InstanceData), NULL, 0, sizeof(InstanceData));
	if (instanceBufferID == -1)
		return false;

	//Light Buffer
	lightBufferID = resources.CreateBuffer(Bind_Shader_Resource, true, MaxLightNumber * sizeof(Light), NULL, 0, sizeof(Light));
	if (lightBufferID == -1)
		return false;

	resources.SetBinding(Stage_Output_Merge, Bind_Depth_Stencil, 0, depthStencilBufferID);

	resources.SetBinding(Stage_Vertex_Shader, Bind_Constant_Buffer, Slot_CBuffer_Frame, frameBufferID);
	resources.SetBinding(Stage_Pixel_Shader, Bind_Constant_Buffer, Slot_CBuffer_Frame, frameBufferID);
	resources.SetBinding(Stage_Compute_Shader, Bind_Constant_Buffer, Slot_CBuffer_Frame, frameBufferID);

	resources.SetBinding(Stage_Vertex_Shader, Bind_Constant_Buffer, Slot_CBuffer_Object, objBufferID);
	resources.SetBinding(Stage_Pixel_Shader, Bind_Constant_Buffer, Slot_CBuffer_Object, objBufferID);
	resources.SetBinding(Stage_Compute_Shader, Bind_Constant_Buffer, Slot_CBuffer_Object, objBufferID);

	resources.SetBinding(Stage_Vertex_Shader, Bind_Shader_Resource, Slot_Texture_AnimMatrix, animationMatrixBufferID);
	resources.SetBinding(Stage_Vertex_Shader, Bind_Shader_Resource, Slot_Texture_Instance, instanceBufferID);
	resources.SetBinding(Stage_Pixel_Shader, Bind_Shader_Resource, Slot_Texture_Light, lightBufferID);

	return true;
}

bool GEngine::Tiling()
{
	//dataFrm.screenDimensions.x = resolutionX;
	//dataFrm.screenDimensions.y = resolutionY;
	//XMStoreFloat4x4(&(dataFrm.projectionInv), XMMatrixTranspose(XMMatrixInverse(NULL, camera.GetProjectionMatrix())));
	//Update some data
	//UpdateConstantBufferFrm();

	//DispatchNum and UAV Size
	UINT numTileX, numTileY, numBatchX, numBatchY;
	numTileX = resolutionX / tileSize + (resolutionX % tileSize ? 1 : 0);
	numTileY = resolutionY / tileSize + (resolutionY % tileSize ? 1 : 0);
	numBatchX = numTileX / tileBatchSize + (numTileX % tileBatchSize ? 1 : 0);
	numBatchY = numTileY / tileBatchSize + (numTileY % tileBatchSize ? 1 : 0);

	Compute(numBatchX, numBatchY, 1);

	return true;
}

void GEngine::Shutdown()
{
	D3DManager::Shutdown();
	return;
}

void GEngine::UpdateLight(vector<Light> lights)
{
	frameData.numLights = lights.size();
	lightList = lights;
}

void GEngine::UpdateInstanceBuffer(Model *pModel, unsigned int meshID)
{
	Mesh &mesh = pModel->meshList[meshID];

	unsigned int stride = mesh.boneList.size();
	animationMatrix.clear();
	instanceData.clear();
	map<int, Instance> ::iterator it = pModel->instances.begin();
	
	unsigned int index = 0;
	InstanceData idata;
	//Per instance loop
	while (it != pModel->instances.end())
	{
		Instance &instance = it->second;
		if (!instance.visable)
		{
			it++;
			continue;
		}
		//Write Instance data
		memcpy(&idata.color, &instance.color, sizeof(float[4]));	
		memcpy(&idata.world, &instance.transformMatrix, sizeof(float[16]));
		aiMatrix4x4 wvp = camera.GetProjectionMatrix()*camera.GetViewMatrix()*instance.transformMatrix;
		memcpy(&idata.wVP, &wvp, sizeof(float[16]));
		idata.bindMatrixOffset = index*stride;
		instanceData.push_back(idata);

		//Write animation data
		if (pModel->hasAnimation)
		{
			//accumulate bind matrix
			vector<aiMatrix4x4> matrixArray = instance.GetBindMatrix(meshID);
			animationMatrix.insert(animationMatrix.end(), matrixArray.begin(), matrixArray.end());
		}

		index++;
		it++;
	}
	//Update instance data
	resources.UpdateResourceData(instanceBufferID, &instanceData[0], sizeof(InstanceData)*instanceData.size());

	//Update animation data
	if (pModel->hasAnimation)
	{
		resources.UpdateResourceData(animationMatrixBufferID, &animationMatrix[0], sizeof(float[16])*animationMatrix.size());
	}
}

bool GEngine::UpdateFrameBuffer()
{
	aiMatrix4x4 projection = camera.GetProjectionMatrix();
	aiMatrix4x4 projectionInv = projection;
	projectionInv.Inverse();
	memcpy(&frameData.projection, &projection, sizeof(float[16]));
	memcpy(&frameData.projectionInv, &projectionInv, sizeof(float[16]));

	aiVector3D position = camera.GetPosition();
	memcpy(frameData.cameraPos, &position, sizeof(float[3]));
	frameData.numLights = lightList.size();
	frameData.screenDimensions[0] = resolutionX;
	frameData.screenDimensions[1] = resolutionY;
	//maxNumLightsPerTile;
	return resources.UpdateResourceData(frameBufferID, &frameData, sizeof(frameData));
}

bool GEngine::UpdateObjBuffer()
{
	return resources.UpdateResourceData(objBufferID, &objData, sizeof(objData));
}

bool GEngine::UpdateLightBuffer()
{
	return resources.UpdateResourceData(lightBufferID, &lightList[0], sizeof(Light)*lightList.size());
}

void GEngine::RenderMesh(MeshResource * pMesh)
{
	resources.SetBinding(Stage_Input_Assembler, Bind_Vertex_Buffer, Slot_Input_Position, pMesh->positionID);
	resources.SetBinding(Stage_Input_Assembler, Bind_Vertex_Buffer, Slot_Input_Normal, pMesh->normalID);
	resources.SetBinding(Stage_Input_Assembler, Bind_Vertex_Buffer, Slot_Input_Tangent, pMesh->tangentID);
	resources.SetBinding(Stage_Input_Assembler, Bind_Vertex_Buffer, Slot_Input_Binormal, pMesh->bitangentID);
	resources.SetBinding(Stage_Input_Assembler, Bind_Vertex_Buffer, Slot_Input_TexCoord, pMesh->texCoordID);
	resources.SetBinding(Stage_Input_Assembler, Bind_Vertex_Buffer, Slot_Input_BlendIndices, pMesh->boneIndexID);
	resources.SetBinding(Stage_Input_Assembler, Bind_Vertex_Buffer, Slot_Input_BlendWeight, pMesh->boneWeightID);
	resources.SetBinding(Stage_Input_Assembler, Bind_Index_Buffer, 0, pMesh->indiceID);
}

void GEngine::RenderMaterial(MaterialResource * pMaterial)
{
	resources.SetBinding(Stage_Pixel_Shader, Bind_Shader_Resource, Slot_Texture_Diffuse, pMaterial->diffuseMap);
	resources.SetBinding(Stage_Pixel_Shader, Bind_Shader_Resource, Slot_Texture_Normal, pMaterial->normalMap);
	resources.SetBinding(Stage_Pixel_Shader, Bind_Shader_Resource, Slot_Texture_Ambient, pMaterial->ambientMap);
	resources.SetBinding(Stage_Pixel_Shader, Bind_Shader_Resource, Slot_Texture_Specular, pMaterial->specularMap);

}

void GEngine::RenderModel(ModelResource * pModel)
{
	UINT instanceCount = pModel->model->UpdateVisableInstances();
	objData.HasAnimation = pModel->hasAnimation;

	for (int i = 0; i < pModel->meshes.size(); i++)
	{
		MeshResource &mesh = pModel->meshes[i];
		MaterialResource &material = pModel->materials[mesh.materialID];

		RenderMesh(&mesh);
		RenderMaterial(&material);

		objData.HasAmbientMap = material.ambientMap == -1 ? 0 : 1;
		objData.HasDiffuseMap = material.diffuseMap == -1 ? 0 : 1;
		objData.HasSpecularMap = material.specularMap == -1 ? 0 : 1;
		objData.HasNormalMap = (material.normalMap == -1 || mesh.tangentID == -1 || mesh.bitangentID == -1) ? 0 : 1;

		UpdateObjBuffer();
		UpdateInstanceBuffer(pModel->model, i);


		Draw(pModel->meshes[i].indexCount, instanceCount);
	}
}

int GEngine::LoadModel(Model &model)
{
	//Generate ID
	int modelID;
	for (modelID = 1; modelID < INT_MAX; modelID++)
	{
		if (!modelList.count(modelID))
			break;
	}
	if (modelID == INT_MAX)
		return -1;

	ModelResource &rmodel = modelList[modelID];
	rmodel.meshes.resize(model.meshList.size());
	rmodel.hasAnimation = model.hasAnimation;
	rmodel.animationCount = model.animationList.size();
	rmodel.model = &model;
	for (int i = 0; i < model.meshList.size(); i++)
	{
		MeshResource &dstMesh = rmodel.meshes[i];
		Mesh &srcMesh = model.meshList[i];

		unsigned int dataSize;
		void* dataPtr;
		if (!srcMesh.vertexPositions.empty())
		{
			dataSize = srcMesh.vertexPositions.size() * sizeof(float[3]);
			dataPtr = &srcMesh.vertexPositions[0];
			dstMesh.positionID = resources.CreateBuffer(Bind_Vertex_Buffer, false, dataSize, dataPtr, dataSize, sizeof(float[3]));
		}
		if (!srcMesh.vertexNormals.empty())
		{
			dataSize = srcMesh.vertexNormals.size() * sizeof(float[3]);
			dataPtr = &srcMesh.vertexNormals[0];
			dstMesh.normalID = resources.CreateBuffer(Bind_Vertex_Buffer, false, dataSize, dataPtr, dataSize, sizeof(float[3]));
		}
		if (!srcMesh.vertexTangent.empty()&& !srcMesh.vertexBitangent.empty())
		{
			dataSize = srcMesh.vertexTangent.size() * sizeof(float[3]);
			dataPtr = &srcMesh.vertexTangent[0];
			dstMesh.tangentID= resources.CreateBuffer(Bind_Vertex_Buffer, false, dataSize, dataPtr, dataSize, sizeof(float[3]));

			dataSize = srcMesh.vertexBitangent.size() * sizeof(float[3]);
			dataPtr = &srcMesh.vertexBitangent[0];
			dstMesh.bitangentID = resources.CreateBuffer(Bind_Vertex_Buffer, false, dataSize, dataPtr, dataSize, sizeof(float[3]));
		}
		if (!srcMesh.vertexTexCoords.empty())
		{
			dataSize = srcMesh.vertexTexCoords.size() * sizeof(float[2]);
			dataPtr = &srcMesh.vertexTexCoords[0];
			dstMesh.texCoordID = resources.CreateBuffer(Bind_Vertex_Buffer, false, dataSize, dataPtr, dataSize, sizeof(float[2]));
		}
		if (rmodel.hasAnimation)
		{
			dataSize = srcMesh.vertexBindID.size() * sizeof(UINT);
			dataPtr = &srcMesh.vertexBindID[0];
			dstMesh.boneIndexID = resources.CreateBuffer(Bind_Vertex_Buffer, false, dataSize, dataPtr, dataSize, numBonePerVertex * sizeof(UINT));

			dataSize = srcMesh.vertexBindWight.size() * sizeof(float);
			dataPtr = &srcMesh.vertexBindWight[0];
			dstMesh.boneWeightID = resources.CreateBuffer(Bind_Vertex_Buffer, false, dataSize, dataPtr, dataSize, numBonePerVertex * sizeof(float));
		}
		if (!srcMesh.indices.empty())
		{
			dataSize = srcMesh.indices.size() * sizeof(UINT);
			dataPtr = &srcMesh.indices[0];
			dstMesh.indiceID = resources.CreateBuffer(Bind_Index_Buffer, false, dataSize, dataPtr, dataSize, sizeof(UINT));
			dstMesh.indexCount = srcMesh.indices.size();
		}

		dstMesh.materialID = srcMesh.materialID;
	}

	rmodel.materials.resize(model.materialList.size());
	for (int i = 0; i < model.materialList.size(); i++)
	{
		MaterialResource &dstMaterial = rmodel.materials[i];
		Material &srcMaterial = model.materialList[i];
		if (srcMaterial.hasDiffuseMap)
		{
			dstMaterial.diffuseMap = resources.CreateTexture2D(
				Bind_Shader_Resource | Bind_Render_Target, 
				DXGI_FORMAT_R8G8B8A8_UNORM,
				false, 
				true, 
				srcMaterial.diffuseMap.width, 
				srcMaterial.diffuseMap.height, 
				srcMaterial.diffuseMap.GetImageDataPtr());
		}

		if (srcMaterial.hasNormalMap)
		{
			dstMaterial.normalMap = resources.CreateTexture2D(
				Bind_Shader_Resource | Bind_Render_Target,
				DXGI_FORMAT_R8G8B8A8_UNORM,
				false,
				true,
				srcMaterial.normalMap.width,
				srcMaterial.normalMap.height,
				srcMaterial.normalMap.GetImageDataPtr());
		}
	}
	return 1;
}

void GEngine::UnloadModel(UINT modelID)
{
	ModelResource &model = modelList[modelID];

	//Release Mesh data from graphics memory
	for (int i = 0; i < model.meshes.size(); i++)
	{
		MeshResource &mesh = model.meshes[i];
		if (mesh.positionID != -1)
			resources.Clear(mesh.positionID);
		if (mesh.normalID != -1)
			resources.Clear(mesh.normalID);
		if (mesh.colorID != -1)
			resources.Clear(mesh.colorID);
		if (mesh.texCoordID != -1)
			resources.Clear(mesh.texCoordID);
		if (mesh.indiceID != -1)
			resources.Clear(mesh.indiceID);
		if (mesh.tangentID != -1)
			resources.Clear(mesh.tangentID);
		if (mesh.bitangentID != -1)
			resources.Clear(mesh.bitangentID);
		if (mesh.boneIndexID != -1)
			resources.Clear(mesh.boneIndexID);
		if (mesh.boneWeightID != -1)
			resources.Clear(mesh.boneWeightID);
	}

	//Release Material data from graphics memory
	for (int i = 0; i < model.materials.size(); i++)
	{
		MaterialResource &material = model.materials[i];
		if (material.ambientMap != -1)
			resources.Clear(material.ambientMap);
		if (material.diffuseMap != -1)
			resources.Clear(material.diffuseMap);
		if (material.normalMap != -1)
			resources.Clear(material.normalMap);
		if (material.specularMap != -1)
			resources.Clear(material.specularMap);
	}

	//Delete model from list
	modelList.erase(modelID);

}

void GEngine::Frame()
{
	if(!UpdateFrameBuffer())
		MessageBox(0, L"UpdateConstantBuffer Error!", L"Error!", MB_OK);
	if(!UpdateLightBuffer())
		MessageBox(0, L"UpdateLightBuffer Error!", L"Error!", MB_OK);

	//Tiling----------------------
	if (
		oldResolutionX != frameData.screenDimensions[0] ||
		oldResolutionY != frameData.screenDimensions[1] ||
		memcmp(&oldProjection, &frameData.projection, sizeof(float[16])) != 0
		)
	{
		//Update tiles
		if (!Tiling())
			MessageBox(0, L"Tilling Error!", L"Error!", MB_OK);
		oldResolutionX = frameData.screenDimensions[0];
		oldResolutionY = frameData.screenDimensions[1];
		memcpy(oldProjection, frameData.projection, sizeof(float[16]));
	}

	//rendering----------------------
	float color[] = { 0,0,0.5,0 };
	resources.ResetRTV(backBufferID, color);
	resources.ResetDSV(depthStencilBufferID, D3D11_CLEAR_DEPTH, 1.0f, 0);
	
	//Z pre pass
	//deviceContextPtr->OMSetRenderTargets(1, &nullRTVPtr, depthStencilViewPtr);
	//deviceContextPtr->OMSetDepthStencilState(depthStencilStatePtr, 1);
	//passType = Pass_Pre_Z;
	//for (int i = 0;i < gModelListSize;i++)
	//{
	//	if (gModelList[i])
	//		RenderModle(gModelList[i]);
	//}

	//Normal pass
	//deviceContextPtr->OMSetRenderTargets(1, &backBufferRTVPtr, depthStencilViewPtr);

	//passType = Pass_Normal;
	map< int, ModelResource>::iterator it = modelList.begin();
	while (it != modelList.end())
	{
		RenderModel(&(it->second));
		it++;
	}
	Swap();
}

int GEngine::CreateSurfaceRec(float width, float hieght, float leftTopX, float leftTopY)
{
	/*
	Group panelGroup;
	Model panel;
	Material mt;
	//Four Corner clockwise

	panel.vertexPosition = { Position(0, 0, 0), Position(width, 0, 0), Position(width, -hieght, 0), Position(0, -hieght, 0) };

	panel.vertexTexCoord = { TexCoord(0, 0),TexCoord(1, 0),TexCoord(1, 1),TexCoord(0, 1) };

	panelGroup.indices = { 0,1,3,1,2,3 };

	panelGroup.materialName = "default";

	panel.materialTable["default"] = 0;

	panel.groups = { panelGroup };
	panel.materials = { mt };
	panel.passID = pass2D;

	int id = LoadModel(panel);
	if (id == -1)
		return -1;

	GModel* pGModel = GetModleByID(id);

	pGModel->positionX = leftTopX;
	pGModel->positionY = leftTopY;
	pGModel->positionZ = 0.0f;
	pGModel->UpdateWordMatrix();
	pGModel->gMaterial[0].opacity = 1;
	CreateTextureD2D(devicePtr, deviceContextPtr, resolutionX*width / 2, resolutionY*hieght / 2, 32, NULL, &pGModel->gMaterial[0].diffuseMap, &pGModel->gMaterial[0].diffuseMapView);
*/
	return 0;
}