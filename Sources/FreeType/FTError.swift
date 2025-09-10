//
//  FTError.swift
//  FreeType
//
//  Created by Evgenij Lutz on 08.09.25.
//

import FreeTypeC


public enum FTError: Error {
    case unknownCode(_ code: Int)
    case unknown
}


extension FTError: CustomStringConvertible {
    public var description: String {
        switch self {
        case .unknownCode(let code):
            return "FreeType error \(code)"
            
        case .unknown:
            return "Unknown FreeType error"
        }
    }
}


public extension CommonError {
    var ftError: FTError {
        .unknownCode(.init(errorCode))
    }
}
