// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Templates/Tuple.h"

/*
* Expands a tuple into a list of function arguments. 
* The number at the end specifies the number of arguments.
*/
#define _TARGS1(T) T.Get<0>()
#define _TARGS2(T) _TARGS1(T), T.Get<1>()
#define _TARGS3(T) _TARGS2(T), T.Get<2>()
#define _TARGS4(T) _TARGS3(T), T.Get<3>()
#define _TARGS5(T) _TARGS4(T), T.Get<4>()
#define _TARGS6(T) _TARGS5(T), T.Get<5>()
#define _TARGS7(T) _TARGS6(T), T.Get<6>()
#define _TARGS8(T) _TARGS7(T), T.Get<7>()
#define _TARGS9(T) _TARGS8(T), T.Get<8>()
#define _TARGS10(T) _TARGS9(T), T.Get<9>()
#define _TARGS11(T) _TARGS10(T), T.Get<10>()

