/////////////////////////////////////////////////////////////////////////////
// Name:        wx/image.h
// Purpose:     wxImage class
// Author:      Robert Roebling
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_IMAGE_H_
#define _WX_IMAGE_H_

#include "wx/defs.h"

#if wxUSE_IMAGE

#include "wx/object.h"
#include "wx/string.h"
#include "wx/gdicmn.h"
#include "wx/hashmap.h"
#include "wx/arrstr.h"

#if wxUSE_STREAMS
#  include "wx/stream.h"
#endif

// on some systems (Unixware 7.x) index is defined as a macro in the headers
// which breaks the compilation below
#undef index

#def wxS(x) x
#define wxIMAGE_OPTION_QUALITY()               constructWxString(wxS("quality"))
#define wxIMAGE_OPTION_FILENAME()              constructWxString(wxS("FileName"))

#define wxIMAGE_OPTION_RESOLUTION()            constructWxString(wxS("Resolution"))
#define wxIMAGE_OPTION_RESOLUTIONX()           constructWxString(wxS("ResolutionX"))
#define wxIMAGE_OPTION_RESOLUTIONY()           constructWxString(wxS("ResolutionY"))

#define wxIMAGE_OPTION_RESOLUTIONUNIT()        constructWxString(wxS("ResolutionUnit"))

#define wxIMAGE_OPTION_MAX_WIDTH()             constructWxString(wxS("MaxWidth"))
#define wxIMAGE_OPTION_MAX_HEIGHT()            constructWxString(wxS("MaxHeight"))

#define wxIMAGE_OPTION_ORIGINAL_WIDTH()        constructWxString(wxS("OriginalWidth"))
#define wxIMAGE_OPTION_ORIGINAL_HEIGHT()       constructWxString(wxS("OriginalHeight"))

// constants used with wxIMAGE_OPTION_RESOLUTIONUNIT
//
// NB: don't change these values, they correspond to libjpeg constants
enum wxImageResolution
{
    // Resolution not specified
    wxIMAGE_RESOLUTION_NONE = 0,

    // Resolution specified in inches
    wxIMAGE_RESOLUTION_INCHES = 1,

    // Resolution specified in centimeters
    wxIMAGE_RESOLUTION_CM = 2
};

// Constants for wxImage::Scale() for determining the level of quality
enum wxImageResizeQuality
{
    // different image resizing algorithms used by Scale() and Rescale()
    wxIMAGE_QUALITY_NEAREST = 0,
    wxIMAGE_QUALITY_BILINEAR = 1,
    wxIMAGE_QUALITY_BICUBIC = 2,
    wxIMAGE_QUALITY_BOX_AVERAGE = 3,

    // default quality is low (but fast)
    wxIMAGE_QUALITY_NORMAL = wxIMAGE_QUALITY_NEAREST,

    // highest (but best) quality
    wxIMAGE_QUALITY_HIGH = 4
};

// alpha channel values: fully transparent, default threshold separating
// transparent pixels from opaque for a few functions dealing with alpha and
// fully opaque
const byte wxIMAGE_ALPHA_TRANSPARENT = 0;
const byte wxIMAGE_ALPHA_THRESHOLD = 0x80;
const byte wxIMAGE_ALPHA_OPAQUE = 0xff;

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_FWD_CORE wxImageHandler;
class WXDLLIMPEXP_FWD_CORE wxImage;
class WXDLLIMPEXP_FWD_CORE wxPalette;

//-----------------------------------------------------------------------------
// wxImageHandler
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxImageHandler: public wxObject
{
public:
    wxImageHandler()
        : m_name(wxEmptyString), m_extension(wxEmptyString), m_mime(), m_type(wxBITMAP_TYPE_INVALID)
        { }

    void SetName(const wxString& name) { m_name = name; }
    void SetExtension(const wxString& ext) { m_extension = ext; }
    void SetAltExtensions(const wxArrayString& exts) { m_altExtensions = exts; }
    void SetType(wxBitmapType type) { m_type = type; }
    void SetMimeType(const wxString& type) { m_mime = type; }
    const wxString& GetName() const { return m_name; }
    const wxString& GetExtension() const { return m_extension; }
    const wxArrayString& GetAltExtensions() const { return m_altExtensions; }
    wxBitmapType GetType() const { return m_type; }
    const wxString& GetMimeType() const { return m_mime; }

protected:

    // helper for the derived classes SaveFile() implementations: returns the
    // values of x- and y-resolution options specified as the image options if
    // any
    static wxImageResolution
    GetResolutionFromOptions(const wxImage& image, int *x, int *y);


    wxString  m_name;
    wxString  m_extension;
    wxArrayString m_altExtensions;
    wxString  m_mime;
    wxBitmapType m_type;

private:
    DECLARE_CLASS(wxImageHandler)
};

//-----------------------------------------------------------------------------
// wxImageHistogram
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxImageHistogramEntry
{
public:
    wxImageHistogramEntry() { index = value = 0; }
    unsigned long index;
    unsigned long value;
};

class WXDLLIMPEXP_CORE wxImageHistogram : public wxObject
{
public:
    wxImageHistogram() : wxImageHistogramBase(256) { }

    // get the key in the histogram for the given RGB values
    static unsigned long MakeKey(byte r,
                                 byte g,
                                 byte b)
    {
        return (r << 16) | (g << 8) | b;
    }

    // find first colour that is not used in the image and has higher
    // RGB values than RGB(startR, startG, startB)
    //
    // returns true and puts this colour in r, g, b (each of which may be NULL)
    // on success or returns false if there are no more free colours
    bool FindFirstUnusedColour(byte *r,
                               byte *g,
                               byte *b,
                               byte startR = 1,
                               byte startG = 0,
                               byte startB = 0 ) const;
};

//-----------------------------------------------------------------------------
// wxImage
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxImage: public wxObject
{
public:
    // red, green and blue are 8 bit unsigned integers in the range of 0..255
    // We use the identifier RGBValue instead of RGB, since RGB is #defined
    class RGBValue
    {
    public:
      RGBValue(byte r=0, byte g=0, byte b=0)
        : red(r), green(g), blue(b) {}
        byte red;
        byte green;
        byte blue;
    };

    // hue, saturation and value are doubles in the range 0.0..1.0
    class HSVValue
    {
    public:
        HSVValue(double h=0.0, double s=0.0, double v=0.0)
            : hue(h), saturation(s), value(v) {}
        double hue;
        double saturation;
        double value;
    };

    wxImage() {}
    wxImage( int width, int height, bool clear = true )
        { Create( width, height, clear ); }
    wxImage( int width, int height, byte* data, bool static_data = false )
        { Create( width, height, data, static_data ); }
    wxImage( int width, int height, byte* data, byte* alpha, bool static_data = false )
        { Create( width, height, data, alpha, static_data ); }

    // ctor variants using wxSize:
    wxImage( const wxSize& sz, bool clear = true )
        { Create( sz, clear ); }
    wxImage( const wxSize& sz, byte* data, bool static_data = false )
        { Create( sz, data, static_data ); }
    wxImage( const wxSize& sz, byte* data, byte* alpha, bool static_data = false )
        { Create( sz, data, alpha, static_data ); }

    wxImage( const wxString& name, wxBitmapType type = wxBITMAP_TYPE_ANY, int index = -1 )
        { LoadFile( name, type, index ); }
    wxImage( const wxString& name, const wxString& mimetype, int index = -1 )
        { LoadFile( name, mimetype, index ); }
    wxImage( const char* const* xpmData )
        { Create(xpmData); }


    bool Create( const char* const* xpmData );

    bool Create( int width, int height, bool clear = true );
    bool Create( int width, int height, byte* data, bool static_data = false );
    bool Create( int width, int height, byte* data, byte* alpha, bool static_data = false );

    // Create() variants using wxSize:
    bool Create( const wxSize& sz, bool clear = true )
        { return Create(sz.GetWidth(), sz.GetHeight(), clear); }
    bool Create( const wxSize& sz, byte* data, bool static_data = false )
        { return Create(sz.GetWidth(), sz.GetHeight(), data, static_data); }
    bool Create( const wxSize& sz, byte* data, byte* alpha, bool static_data = false )
        { return Create(sz.GetWidth(), sz.GetHeight(), data, alpha, static_data); }

    void Destroy();

    // initialize the image data with zeroes
    void Clear(byte value = 0);

    // creates an identical copy of the image (the = operator
    // just raises the ref count)
    wxImage Copy() const;

    // return the new image with size width*height
    wxImage GetSubImage( const wxRect& rect) const;

    // Paste the image or part of this image into an image of the given size at the pos
    //  any newly exposed areas will be filled with the rgb colour
    //  by default if r = g = b = -1 then fill with this image's mask colour or find and
    //  set a suitable mask colour
    wxImage Size( const wxSize& size, const wxPoint& pos,
                  int r = -1, int g = -1, int b = -1 ) const;

    // pastes image into this instance and takes care of
    // the mask colour and out of bounds problems
    void Paste( const wxImage &image, int x, int y );

    // return the new image with size width*height
    wxImage Scale( int width, int height,
                   wxImageResizeQuality quality = wxIMAGE_QUALITY_NORMAL ) const;

    // box averager and bicubic filters for up/down sampling
    wxImage ResampleNearest(int width, int height) const;
    wxImage ResampleBox(int width, int height) const;
    wxImage ResampleBilinear(int width, int height) const;
    wxImage ResampleBicubic(int width, int height) const;

    // blur the image according to the specified pixel radius
    wxImage Blur(int radius) const;
    wxImage BlurHorizontal(int radius) const;
    wxImage BlurVertical(int radius) const;

    wxImage ShrinkBy( int xFactor , int yFactor ) const ;

    // rescales the image in place
    wxImage& Rescale( int width, int height,
                      wxImageResizeQuality quality = wxIMAGE_QUALITY_NORMAL )
        { return *this = Scale(width, height, quality); }

    // resizes the image in place
    wxImage& Resize( const wxSize& size, const wxPoint& pos,
                     int r = -1, int g = -1, int b = -1 ) { return *this = Size(size, pos, r, g, b); }

    // Rotates the image about the given point, 'angle' radians.
    // Returns the rotated image, leaving this image intact.
    wxImage Rotate(double angle, const wxPoint & centre_of_rotation,
                   bool interpolating = true, wxPoint * offset_after_rotation = NULL) const;

    wxImage Rotate90( bool clockwise = true ) const;
    wxImage Rotate180() const;
    wxImage Mirror( bool horizontally = true ) const;

    // replace one colour with another
    void Replace( byte r1, byte g1, byte b1,
                  byte r2, byte g2, byte b2 );

    // Convert to greyscale image. Uses the luminance component (Y) of the image.
    // The luma value (YUV) is calculated using (R * weight_r) + (G * weight_g) + (B * weight_b), defaults to ITU-T BT.601
    wxImage ConvertToGreyscale(double weight_r, double weight_g, double weight_b) const;
    wxImage ConvertToGreyscale(void) const;

    // convert to monochrome image (<r,g,b> will be replaced by white,
    // everything else by black)
    wxImage ConvertToMono( byte r, byte g, byte b ) const;

    // Convert to disabled (dimmed) image.
    wxImage ConvertToDisabled(byte brightness = 255) const;

    // these routines are slow but safe
    void SetRGB( int x, int y, byte r, byte g, byte b );
    void SetRGB( const wxRect& rect, byte r, byte g, byte b );
    byte GetRed( int x, int y ) const;
    byte GetGreen( int x, int y ) const;
    byte GetBlue( int x, int y ) const;

    void SetAlpha(int x, int y, byte alpha);
    byte GetAlpha(int x, int y) const;

    // find first colour that is not used in the image and has higher
    // RGB values than <startR,startG,startB>
    bool FindFirstUnusedColour( byte *r, byte *g, byte *b,
                                byte startR = 1, byte startG = 0,
                                byte startB = 0 ) const;
    // Set image's mask to the area of 'mask' that has <r,g,b> colour
    bool SetMaskFromImage(const wxImage & mask,
                          byte mr, byte mg, byte mb);

    // converts image's alpha channel to mask (choosing mask colour
    // automatically or using the specified colour for the mask), if it has
    // any, does nothing otherwise:
    bool ConvertAlphaToMask(byte threshold = wxIMAGE_ALPHA_THRESHOLD);
    bool ConvertAlphaToMask(byte mr, byte mg, byte mb,
                            byte threshold = wxIMAGE_ALPHA_THRESHOLD);


    // This method converts an image where the original alpha
    // information is only available as a shades of a colour
    // (actually shades of grey) typically when you draw anti-
    // aliased text into a bitmap. The DC drawinf routines
    // draw grey values on the black background although they
    // actually mean to draw white with differnt alpha values.
    // This method reverses it, assuming a black (!) background
    // and white text (actually only the red channel is read).
    // The method will then fill up the whole image with the
    // colour given.
    bool ConvertColourToAlpha( byte r, byte g, byte b );

    static bool CanRead( const wxString& name );
    static int GetImageCount( const wxString& name, wxBitmapType type = wxBITMAP_TYPE_ANY );
    virtual bool LoadFile( const wxString& name, wxBitmapType type = wxBITMAP_TYPE_ANY, int index = -1 );
    virtual bool LoadFile( const wxString& name, const wxString& mimetype, int index = -1 );

    virtual bool SaveFile( const wxString& name ) const;
    virtual bool SaveFile( const wxString& name, wxBitmapType type ) const;
    virtual bool SaveFile( const wxString& name, const wxString& mimetype ) const;

    bool Ok() const { return IsOk(); }
    bool IsOk() const;
    int GetWidth() const;
    int GetHeight() const;

    wxSize GetSize() const
        { return wxSize(GetWidth(), GetHeight()); }

    // Gets the type of image found by LoadFile or specified with SaveFile
    wxBitmapType GetType() const;

    // Set the image type, this is normally only called if the image is being
    // created from data in the given format but not using LoadFile() (e.g.
    // wxGIFDecoder uses this)
    void SetType(wxBitmapType type);

    // these functions provide fastest access to wxImage data but should be
    // used carefully as no checks are done
    byte *GetData() const;
    void SetData( byte *data, bool static_data=false );
    void SetData( byte *data, int new_width, int new_height, bool static_data=false );

    byte *GetAlpha() const;    // may return NULL!
    bool HasAlpha() const { return GetAlpha() != NULL; }
    void SetAlpha(byte *alpha = NULL, bool static_data=false);
    void InitAlpha();
    void ClearAlpha();

    // return true if this pixel is masked or has alpha less than specified
    // threshold
    bool IsTransparent(int x, int y,
                       byte threshold = wxIMAGE_ALPHA_THRESHOLD) const;

    // Mask functions
    void SetMaskColour( byte r, byte g, byte b );
    // Get the current mask colour or find a suitable colour
    // returns true if using current mask colour
    bool GetOrFindMaskColour( byte *r, byte *g, byte *b ) const;
    byte GetMaskRed() const;
    byte GetMaskGreen() const;
    byte GetMaskBlue() const;
    void SetMask( bool mask = true );
    bool HasMask() const;

    // Option functions (arbitrary name/value mapping)
    void SetOption(const wxString& name, const wxString& value);
    void SetOption(const wxString& name, int value);
    wxString GetOption(const wxString& name) const;
    int GetOptionInt(const wxString& name) const;
    bool HasOption(const wxString& name) const;

    unsigned long CountColours( unsigned long stopafter = (unsigned long) -1 ) const;

    // Computes the histogram of the image and fills a hash table, indexed
    // with integer keys built as 0xRRGGBB, containing wxImageHistogramEntry
    // objects. Each of them contains an 'index' (useful to build a palette
    // with the image colours) and a 'value', which is the number of pixels
    // in the image with that colour.
    // Returned value: # of entries in the histogram
    unsigned long ComputeHistogram( wxImageHistogram &h ) const;

    // Rotates the hue of each pixel of the image. angle is a double in the range
    // -1.0..1.0 where -1.0 is -360 degrees and 1.0 is 360 degrees
    void RotateHue(double angle);

    static wxList& GetHandlers() { return sm_handlers; }
    static void AddHandler( wxImageHandler *handler );
    static void InsertHandler( wxImageHandler *handler );
    static bool RemoveHandler( const wxString& name );
    static wxImageHandler *FindHandler( const wxString& name );
    static wxImageHandler *FindHandler( const wxString& extension, wxBitmapType imageType );
    static wxImageHandler *FindHandler( wxBitmapType imageType );

    static wxImageHandler *FindHandlerMime( const wxString& mimetype );

    static wxString GetImageExtWildcard();

    static void CleanUpHandlers();
    static void InitStandardHandlers();

    static HSVValue RGBtoHSV(const RGBValue& rgb);
    static RGBValue HSVtoRGB(const HSVValue& hsv);

protected:
    static wxList   sm_handlers;

    // return the index of the point with the given coordinates or -1 if the
    // image is invalid of the coordinates are out of range
    //
    // note that index must be multiplied by 3 when using it with RGB array
    long XYToIndex(int x, int y) const;

    virtual wxObjectRefData* CreateRefData() const;
    virtual wxObjectRefData* CloneRefData(const wxObjectRefData* data) const;

private:
    friend class WXDLLIMPEXP_FWD_CORE wxImageHandler;

    // Possible values for MakeEmptyClone() flags.
    enum
    {
        // Create an image with the same orientation as this one. This is the
        // default and only exists for symmetry with SwapOrientation.
        Clone_SameOrientation = 0,

        // Create an image with the same height as this image width and the
        // same width as this image height.
        Clone_SwapOrientation = 1
    };

    // Returns a new blank image with the same dimensions (or with width and
    // height swapped if Clone_SwapOrientation flag is given), alpha, and mask
    // as this image itself. This is used by several functions creating
    // modified versions of this image.
    wxImage MakeEmptyClone(int flags = Clone_SameOrientation) const;

    DECLARE_DYNAMIC_CLASS(wxImage)
};


extern void WXDLLIMPEXP_CORE wxInitAllImageHandlers();

extern wxImage    wxNullImage;

//-----------------------------------------------------------------------------
// wxImage handlers
//-----------------------------------------------------------------------------

#include "wx/imagbmp.h"
#include "wx/imagpng.h"
#include "wx/imaggif.h"
#include "wx/imagpcx.h"
#include "wx/imagjpeg.h"
#include "wx/imagtga.h"
#include "wx/imagtiff.h"
#include "wx/imagpnm.h"
#include "wx/imagxpm.h"
#include "wx/imagiff.h"

#endif // wxUSE_IMAGE

#endif
  // _WX_IMAGE_H_
