//
//  msdfgen.hpp
//  FreeType
//
//  Created by Evgenij Lutz on 14.11.25.
//

#pragma once

#if defined __cplusplus

#include <FreeTypeC/FreeTypeC.hpp>

class SDFImage final {
private:
    std::atomic<size_t> _referenceCounter;
    
    const char* fn_nonnull _contents;
    const long _numChannels;
    const long _width;
    const long _height;
    
    FN_FRIEND_SWIFT_INTERFACE(SDFImage)
    
    SDFImage(const char* fn_nonnull contents, long numChannels, long width, long height);
    ~SDFImage();
    
public:
    static SDFImage* fn_nonnull create(const float* fn_nonnull contents, long numChannels, long width, long height) SWIFT_RETURNS_RETAINED;
    const char* fn_nonnull getContents() SWIFT_COMPUTED_PROPERTY { return _contents; }
    const long getNumChannels() SWIFT_COMPUTED_PROPERTY { return _numChannels; }
    const long getWidth() SWIFT_COMPUTED_PROPERTY { return _width; }
    const long getHeight() SWIFT_COMPUTED_PROPERTY { return _height; }
    
}
FN_SWIFT_INTERFACE(SDFImage)
SWIFT_UNCHECKED_SENDABLE;





/// Generates an SDF image.
///
/// Sample code to bride SDFImage to an ImageContainer instance:
/// ```Swift
/// struct ContentView: View {
///     @State var unicodeIndexString: String = "1"
///     @State var cgImage: CGImage?
///
///     func genImage() {
///         guard let path = Bundle.main.path(forResource: "Some-Font", ofType: "otf") else {
///             print("Could not find font file")
///             cgImage = nil
///             return
///         }
///
///         guard let index = Int(unicodeIndexString.trimmingCharacters(in: .whitespacesAndNewlines)) else {
///             print("Wrong index")
///             cgImage = nil
///             return
///         }
///
///         Task {
///             do {
///                 guard let image = try testFont(path: path, index) else {
///                     print("Could not generate SDF image")
///                     return
///                 }
///
///                 let pixelformat = ImagePixelFormat(.float32, image.numChannels)
///                 let container = ImageContainer.create(image.contents, image.width, image.height, pixelformat)
///                 cgImage = try container.cgImage
///             }
///             catch {
///                 print(error)
///                 cgImage = nil
///             }
///         }
///     }
///
///     var body: some View {
///         VStack {
///             HStack {
///                 Button("-") {
///                     let value = Int(unicodeIndexString) ?? 1
///                     unicodeIndexString = .init(value - 1)
///                     genImage()
///                 }
///
///                 TextField("Index", text: $unicodeIndexString)
///                     .frame(maxWidth: 64)
///                     .onChange(of: unicodeIndexString) { oldValue, newValue in
///                         genImage()
///                     }
///
///                 Button("+") {
///                     let value = Int(unicodeIndexString) ?? 1
///                     unicodeIndexString = .init(value + 1)
///                     genImage()
///                 }
///             }
///
///             Button {
///                 genImage()
///             } label: {
///                 Text("Test font")
///             }
///
///             if let cgImage {
///                 Image(cgImage, scale: 1, label: Text("lala"))
///             }
///
///         }
///         .padding()
///     }
/// }
/// ```
SDFImage* fn_nullable generateSDFImage(FTFace* fn_nonnull face, unsigned long unicodeIndex) SWIFT_RETURNS_RETAINED;


FN_DEFINE_SWIFT_INTERFACE(SDFImage)


#endif
