// Copyright 2016 Gamemakin LLC. All Rights Reserved.

#pragma once

#include "Linters/LinterBase.h"

class FWorldLinter : public FLinterBase
{
	virtual bool HasAGoodPath() override;
};