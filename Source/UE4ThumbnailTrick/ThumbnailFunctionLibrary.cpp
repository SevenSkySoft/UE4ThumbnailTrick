// Fill out your copyright notice in the Description page of Project Settings.


#include "ThumbnailFunctionLibrary.h"

#include "Misc/Paths.h"
#include "HAL/UnrealMemory.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "UObject/UObjectGlobals.h"
#include "ObjectTools.h"
#include <Runtime\AssetRegistry\Public\AssetRegistry\AssetRegistryModule.h>



UTexture2D* UThumbnailFunctionLibrary::MakeThumbnail(UObject * _Object, int width, int  height)
{
	FString FileName = FString("Texture_")+ _Object->GetName();
	
	FString pathPackage = FString("/Game/MyTextures/");
	FString absolutePathPackage = FPaths::ProjectContentDir() + "/MyTextures/";
	
	
	UPackage* Package = CreatePackage(nullptr, *pathPackage);

	IPlatformFile & PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// Directory Exists?
	if (!PlatformFile.DirectoryExists(*absolutePathPackage))
	{
		PlatformFile.CreateDirectory(*absolutePathPackage);
	}
	
	// Create the Texture
	FName TextureName = MakeUniqueObjectName(_Object, UTexture2D::StaticClass(), FName(*FileName));
	UTexture2D* Texture = NewObject<UTexture2D>();


	FObjectThumbnail  ObjectThumbnail;
	TArray<uint8>  temp;

	if (_Object != nullptr)	
	{
		
		ThumbnailTools::RenderThumbnail(_Object, width, height,
			ThumbnailTools::EThumbnailTextureFlushMode::NeverFlush, NULL, &ObjectThumbnail);

		temp = ObjectThumbnail.GetUncompressedImageData();
		

		uint8 * pixels = (uint8 *)malloc(height * width * 4);
		
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

		Texture->Source.Init(width, height, 1, 1, ETextureSourceFormat::TSF_BGRA8, pixels);
		Texture->AddToRoot();
		Texture->UpdateResource();
		Package->MarkPackageDirty();
		
		FAssetRegistryModule::AssetCreated(Texture);
	
		FString PackageName = FString("/Game/MyTextures/");
		PackageName += TextureName.ToString();

		FString PackageFileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
		bool bSaved = UPackage::SavePackage(Package, Texture, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *PackageFileName, GError, nullptr, true, true, SAVE_NoError);


		free(pixels);
		pixels = NULL;
	
	}


	return Texture;
}
