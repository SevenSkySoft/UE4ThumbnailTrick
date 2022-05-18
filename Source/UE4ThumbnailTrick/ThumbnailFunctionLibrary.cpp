// Fill out your copyright notice in the Description page of Project Settings.


#include "ThumbnailFunctionLibrary.h"
#include "UObject/Object.h"
#include "Misc/Paths.h"
#include "HAL/UnrealMemory.h"
#include "UObject/UObjectGlobals.h"
#include "ObjectTools.h"


UTexture2D* UThumbnailFunctionLibrary::MakeThumbnail(UObject * _Object, int width, int  height)
{
	FString FileName = FString("MyTexture");
	FObjectThumbnail  ObjectThumbnail;
	FString pathPackage = FString("/Game/MyTextures/");
	FString absolutePathPackage = FPaths::ProjectContentDir() + "/MyTextures/";

	FPackageName::RegisterMountPoint(*pathPackage, *absolutePathPackage);

	UPackage * Package = CreatePackage(nullptr, *pathPackage);
	// Create the Texture
	FName TextureName = MakeUniqueObjectName(Package, UTexture2D::StaticClass(), FName(*FileName));
	UTexture2D* Texture = NewObject<UTexture2D>();// NewObject(Package, TextureName, RF_Public | RF_Standalone);
	//UTexture2D* Texture = NewObject<UTexture2D>(nullptr, Package, TextureName, RF_Public | RF_Standalone, UTexture2D.GetDefaultObject());
		// NewObject(nullptr,Package, TextureName, RF_Public | RF_Standalone,nullptr,false,nullptr,nullptr);



	TArray<uint8>  temp;
	if (_Object != nullptr)
	{
		//UThumbnailInfo * temp = MeshAsset.Object->ThumbnailInfo;
		ThumbnailTools::RenderThumbnail(_Object, width, height,
			ThumbnailTools::EThumbnailTextureFlushMode::NeverFlush, NULL, &ObjectThumbnail);
		temp = ObjectThumbnail.GetUncompressedImageData();
		///

		uint8 * pixels = (uint8 *)malloc(height * width * 4);
		//pixels = temp;
		for (int y = 0; y < height*width * 4; y++)
		{

			pixels[y] = temp[y];

		}

		// Texture Settings
		Texture->PlatformData = new FTexturePlatformData();
		Texture->PlatformData->SizeX = width;
		Texture->PlatformData->SizeY = height;
		Texture->PlatformData->PixelFormat = PF_R8G8B8A8;

		// Passing the pixels information to the texture
		FTexture2DMipMap* Mip = new(Texture->PlatformData->Mips) FTexture2DMipMap();
		Mip->SizeX = width;
		Mip->SizeY = height;
		Mip->BulkData.Lock(LOCK_READ_WRITE);
		uint8* TextureData = (uint8 *)Mip->BulkData.Realloc(height * width * sizeof(uint8) * 4);
		FMemory::Memcpy(TextureData, pixels, sizeof(uint8) * height * width * 4);
		Mip->BulkData.Unlock();
		//		FMemory::Memcpy()
				// Updating Texture & mark it as unsaved
		Texture->AddToRoot();
		Texture->UpdateResource();

		/////------------------------------------------
		return Texture;
	}


	return Texture;
}