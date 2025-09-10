//
//  CommonError.cpp
//  FreeType
//
//  Created by Evgenij Lutz on 08.09.25.
//

#include <libfreetype.h>
#include <FreeTypeC/CommonError.hpp>


void resetError(CommonError* nullable error) {
    // Check if error can be reset
    if (error == nullptr) {
        return;
    }
    
    // Reset
    error->errorCode = 0;
    error->message[0] = 0;
}


void setFTError(CommonError* nullable error, int errorCode) {
    // Check if error can be set
    if (error == nullptr) {
        return;
    }
    
    // Set error code
    error->errorCode = errorCode;
    
    // Set error message
    auto errorMessage = FT_Error_String(errorCode);
    if (errorMessage) {
        auto len = strnlen(errorMessage, FTErrorC_MessageLength - 2);
        memcpy(error->message, errorMessage, len);
        error->message[len] = 0;
    }
    else {
        error->message[0] = 0;
    }
}
