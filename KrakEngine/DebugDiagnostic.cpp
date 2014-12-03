/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: DebugDiagnostic.cpp
Purpose: Implementation for error/assertion and debug printing
Language: C++
Platform: Windows
Project: go_ninja
Author: Tommy Walton, t.walton
Creation date: 10/3/2013
- End Header -----------------------------------------------------*/
#include "DebugDiagnostic.h"

// If the condition is true, print a message
void ThrowErrorIf(bool conditional, char * message)
{
    if(conditional)
    {
        // TODO: Create a more robust IfError function
        printf_s("%s\n", message);
        throw;
    }
}
