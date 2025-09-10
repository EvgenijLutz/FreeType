//
//  CommonError.hpp
//  FreeType
//
//  Created by Evgenij Lutz on 08.09.25.
//

#ifndef FreeTypeC_FTErrorC_hpp
#define FreeTypeC_FTErrorC_hpp

#if defined __cplusplus

#include "Common.hpp"

#define FTErrorC_MessageLength 64

struct CommonError {
    /// Legends say, the error code `0` means no error.
    int errorCode;
    
    char message[FTErrorC_MessageLength];
};


void resetError(CommonError* nullable error);


void setFTError(CommonError* nullable error, int errorCode);

#endif // __cplusplus

#endif // FreeTypeC_FTErrorC_hpp
