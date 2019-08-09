#include "core/PlatformUtils.h"

#import <Cocoa/Cocoa.h>

namespace core
{
namespace platformUtils
{

void
showWindow(void* handle)
{
    NSView* view = (NSView*)handle;
    [view setHidden:NO];

    NSWindow* window = [view window];
    [window makeKeyAndOrderFront:NULL];
    [NSApp activateIgnoringOtherApps:YES];
}

void
hideWindow(void* handle)
{
    NSView* view = (NSView*)handle;
    [view setHidden:YES];

    NSWindow* window = [view window];
    [window orderOut:NULL];
}

}
}
