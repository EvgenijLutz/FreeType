//
//  CommonError.hpp
//  FreeType
//
//  Created by Evgenij Lutz on 08.09.25.
//

#pragma once

#include "Common.hpp"


#if defined FREETYPE_H_
#define FT_Library_impl FT_Library
#define FT_Face_impl FT_Face
#else
#define FT_Library_impl void*
#define FT_Face_impl void*
#endif // FREETYPE_H_



#define FTErrorC_MessageLength 64

struct CommonError {
    /// Legends say, the error code `0` means no error.
    int errorCode;
    
    char message[FTErrorC_MessageLength];
};


void resetError(CommonError* fn_nullable error);


void setFTError(CommonError* fn_nullable error, int errorCode);
