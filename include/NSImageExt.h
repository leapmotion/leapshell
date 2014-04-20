#if !defined(__NSIMAGEEXT_H__)
#define __NSIMAGEEXT_H__

#if defined(CINDER_COCOA)
#include <Foundation/Foundation.h>

@implementation NSImage (ProportionalScaling)

- (NSSize)pixelSize
{
  NSInteger actualWidth = 0;
  NSInteger actualHeight = 0;
  NSArray* representations = [self representations];
  for (NSImageRep* nsImageRep in representations) {
    if ([nsImageRep pixelsWide] > actualWidth)  { actualWidth =  [nsImageRep pixelsWide]; }
    if ([nsImageRep pixelsHigh] > actualHeight) { actualHeight = [nsImageRep pixelsHigh]; }
  }
  return NSMakeSize(actualWidth, actualHeight);
}

// Specify targetSize to set the desired size of the image (due to screen scaling, the actual size may still be scaled)
// Specify fill to prevent "bars" on one axis due to different aspect ratios (instead, image data may be clipped)
- (NSImage*)imageByScalingProportionallyToSize:(NSSize)targetSize fill:(BOOL)fill
{
  NSImage* sourceImage = self;
  NSImage* newImage = nil;

  if ([sourceImage isValid]) {
    NSSize imageSize = [sourceImage size];

    float width  = imageSize.width;
    float height = imageSize.height;

    float targetWidth  = targetSize.width;
    float targetHeight = targetSize.height;

    float scaleFactor  = 0.0f;
    float scaledWidth  = targetWidth;
    float scaledHeight = targetHeight;

    NSPoint thumbnailPoint = NSZeroPoint;

    if (NSEqualSizes(imageSize, targetSize) == NO) {
      float widthFactor  = targetWidth/width;
      float heightFactor = targetHeight/height;

      if (widthFactor < heightFactor) {
        scaleFactor = fill ? heightFactor : widthFactor;
      } else {
        scaleFactor = fill ? widthFactor : heightFactor;
      }
      scaledWidth  = width*scaleFactor;
      scaledHeight = height*scaleFactor;

      if (widthFactor < heightFactor) {
        if (fill) {
          thumbnailPoint.x = (targetWidth - scaledWidth)*0.5;
        } else {
          thumbnailPoint.y = (targetHeight - scaledHeight)*0.5;
        }
      } else if (widthFactor > heightFactor) {
        if (fill) {
          thumbnailPoint.y = (targetHeight - scaledHeight)*0.5;
        } else {
          thumbnailPoint.x = (targetWidth - scaledWidth)*0.5;
        }
      }
    }
    newImage = [[NSImage alloc] initWithSize:targetSize];
    [newImage lockFocus];
      NSRect thumbnailRect;
      thumbnailRect.origin = thumbnailPoint;
      thumbnailRect.size.width = scaledWidth;
      thumbnailRect.size.height = scaledHeight;
      [sourceImage drawInRect: thumbnailRect
                     fromRect: NSZeroRect
                    operation: NSCompositeSourceOver
                     fraction: 1.0];
    [newImage unlockFocus];
  }
  return [newImage autorelease];
}

@end

#endif

#endif // __NSIMAGEEXT_H__
