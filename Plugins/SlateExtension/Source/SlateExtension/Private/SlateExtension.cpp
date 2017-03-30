// Copyright 2016 Gamemakin LLC. All Rights Reserved.




#include "SlateExtension.h"
//#include "ContentBrowserExtensions.h"


#define LOCTEXT_NAMESPACE "FSlateExtensionModule"

void FSlateExtensionModule::StartupModule()
{


}

void FSlateExtensionModule::ShutdownModule()
{

}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSlateExtensionModule, SlateExtension)
DEFINE_LOG_CATEGORY(LogSlateExtension);