#ifndef _Image565_cpp_
#define _Image565_cpp_


#include <cstdio>
#include <cmath>
#include "Image565.h"
using namespace std;

const int MAX_COMMENT_LENGTH=256;


template <class Item>
void Image<Item>:: ResizeImage(int bands, int rows, int cols)
{
    if(pItem)
        _resetItemPointers();

    _setItemPointers(bands, rows, cols);
    Bands = bands;
    Rows = rows;
    Cols = cols;
}

template <class Item>
void Image<Item>:: _setItemPointers(int bands, int rows, int cols)
{
    pItem = new Item[bands*rows*cols];
    assert(pItem);

    pppItem = new ptr2D_Data[bands];
    assert(pppItem);
    int i;
    for(i=0; i<bands; ++i)
    {
        pppItem[i] = new ptr1D_Data [rows];
        assert(pppItem[i]);
    }

    Item* ptr = &(pItem[0]);
    for(i=0; i<bands; ++i)
    {
        for(int j=0; j<rows; ++j)
        {
            pppItem[i][j] = ptr;
            ptr += cols;
        }
    }
#ifdef DEBUG_Image
    cerr << "_setItemPointers: " << pItem << "," << pppItem << endl;
#endif
}

template <class Item>
void Image<Item>:: _resetItemPointers()
{
#ifdef DEBUG_Image
    cerr << "_resetItemPointers: " << pItem << "," << pppItem << endl;
#endif

    for(int i=0; i<Bands; ++i)
        delete [] pppItem[i];
    delete [] pppItem;

    delete [] pItem;
}

//
// Default Constructor
//

template <class Item>
Image<Item>:: Image()
        : pItem(0)
{
#ifdef DEBUG_Image
    cout << "constructing an image..." << endl;
#endif

    ResizeImage(1, 1, 1);
}

//
// this is kind of redundant.  I wanted to make this as a
// default constructor, but I get weird compiling errors
// regarding >> and << operators...
//
template <class Item>
Image<Item>:: Image(int bands, int rows, int cols)
        : pItem(0)
{
#ifdef DEBUG_Image
    cout << "constructing an image..." << endl;
#endif

    ResizeImage(bands, rows, cols);
}

//
// Constructor
//
template <class Item>
Image<Item>:: Image(int bands, int rows, int cols,
                    const Item& pixel)
        : pItem(0)
{
#ifdef DEBUG_Image
    cout << "constructing an image (" << rows << "x" << cols;
    cout << ") with initial pixel value=" << pixel << endl;
#endif

    ResizeImage(bands, rows, cols);

    for(int i=0; i < Bands*Rows*Cols; ++i)
    {
        pItem[i] = pixel;
    }

}


template <class Item>
Image<Item>:: Image(const Image<Item>& image)
        : pItem(0)
{
    ResizeImage(image.NumBands(), image.NumRows(), image.NumCols());

    for(int i=0; i < Bands*Rows*Cols; ++i)
    {
        pItem[i] = image.GetPixel(i);
    }
}

//
// A Desctructor:
//

template <class Item>
Image<Item>:: ~Image()
{
#ifdef DEBUG_Image
    cout << "destroying an image..." << endl;
#endif

    _resetItemPointers();
}

//
// Assignment:
//

template <class Item>
const Image<Item>& Image<Item>:: operator =(const Image<Item>& image)
{
    if(&image != this)
    {
        if(Cols != image.NumCols() || Rows != image.NumRows() ||
                Bands != image.NumBands())
        {
            ResizeImage(image.NumBands(), image.NumRows(), image.NumCols());
        }

        for(int i=0; i < Bands*Cols*Rows; ++i)
            pItem[i] = image.GetPixel(i);
    }
    return *this;
}

//
// Getting a pixel value and Setting a pixel value
//

/*
this routine implement GetPixel function with boundary extention.
Pixels outside boundaries are computed using pixel folding
*/
template <class Item>
Item
Image<Item>:: GetPixelFold(int band_index, int row_index, int col_index) const
{
    assert(Rows > 1 && Cols > 1); // the routie can fail in this condition...

    row_index = (row_index<0) ? -row_index: \
                ((row_index>=Rows)? 2*Rows-row_index-2 : row_index);
    col_index = (col_index<0) ? -col_index: \
                ((col_index>=Cols)? 2*Cols-col_index-2 : col_index);

    return(pppItem[band_index][row_index][col_index]);
}

/*
this routine implement GetPixel function with boundary extention.
Pixels outside boundaries are computed using image wrapping
*/
template <class Item>
Item
Image<Item>:: GetPixelWrap(int band_index, int row_index, int col_index) const
{
    while(row_index < 0)
    {
        row_index += Rows;
    }
    while(row_index >= Rows)
    {
        row_index -= Rows;
    }
    while(col_index < 0)
    {
        col_index += Cols;
    }
    while(col_index >= Cols)
    {
        col_index -= Cols;
    }

    return(pppItem[band_index][row_index][col_index]);
}

/*
this routine implement GetPixel function with boundary extention.
Pixels outside boundaries are computed using pixel repeatition.
*/
template <class Item>
Item
Image<Item>::GetPixelRepeat(int band_index, int row_index, int col_index)const
{
    row_index = (row_index<0) ? 0: \
                ((row_index>=Rows)? Rows-1: row_index);
    col_index = (col_index<0) ? 0: \
                ((col_index>=Cols)? Cols-1: col_index);

    return(pppItem[band_index][row_index][col_index]);
}

/*
this routine implement GetPixel function with boundary extention.
Pixels outside boundaries are set to Zero.
 
Note: this may not work on some classes of Item such as Complex.
*/
template <class Item>
Item
Image<Item>::GetPixelZero(int band_index, int row_index, int col_index)const
{
    if(band_index>=0 && band_index < Bands &&\
            row_index>=0 && row_index<Rows && \
            col_index>=0 && col_index<Cols)
        return(pppItem[band_index][row_index][col_index]);
    else
    {
        return 0;
    }
}

/*
this routine implement GetPixel function with boundary extention.
Pixels outside boundaries are set to the given default value.
 
*/
template <class Item>
Item
Image<Item>::GetPixelDefault(int band_index, int row_index,
                             int col_index, const Item& item)const
{
    if(band_index>=0 && band_index < Bands &&\
            row_index>=0 && row_index<Rows && \
            col_index>=0 && col_index<Cols)
        return(pppItem[band_index][row_index][col_index]);
    else
    {
        return item;
    }
}

//
// Arithmetic operators for Images.
//
template <class Item>
Image<Item>
Image<Item>:: operator+(const Image<Item>& image) const
{
    assert(Rows==image.NumRows()&&Cols==image.NumCols());
    assert(Bands==image.NumBands() || Bands==1 || image.NumBands()==1);

    Image<Item> result;
    if(Bands==image.NumBands())
    {
        result = image;
        for(int m=0; m<Bands*Rows*Cols; ++m)
        {
            result.SetPixel(m, pItem[m]+image.GetPixel(m));
        }
    }
    else if(Bands==1)
    {
        result = image;
        for(int k=0, m=0; k<image.NumBands(); ++k)
        {
            for(int i=0; i<Rows; ++i)
            {
                for(int j=0; j<Cols; ++j, ++m)
                {
                    result.SetPixel(m, pppItem[0][i][j]+image.GetPixel(m));
                }
            }
        }
    }
    else if (image.NumBands()==1)
    {
        result = *this;
        for(int k=0, m=0; k<Bands; ++k)
        {
            for(int i=0; i<Rows; ++i)
            {
                for(int j=0; j<Cols; ++j, ++m)
                {
                    result.SetPixel(m, GetPixel(m)+image.GetPixel(0,i,j));
                }
            }
        }
    }
    return result;
}

template <class Item>
void
Image<Item>:: operator+=(const Image<Item>& image)
{
    assert(Rows==image.NumRows()&&Cols==image.NumCols());
    assert(Bands==image.NumBands() || Bands==1 || image.NumBands()==1);

    if(Bands == image.NumBands())
    {
        for(int m=0; m<Bands*Rows*Cols; ++m)
            pItem[m] += image.GetPixel(m);
    }
    else if(Bands == 1)
    {
        Item* oldPixel = new Item[Rows*Cols];
        for(int i=0; i<Rows*Cols; ++i)
            oldPixel[i] = pItem[i];

        ResizeImage(image.NumBands(), Rows, Cols);

        for(int k=0, m=0; k<image.NumBands(); ++k)
        {
            int n=0;
            for(int i=0; i<Rows; ++i)
            {
                for(int j=0; j<Cols; ++j,++n, ++m)
                {
                    pItem[m] = oldPixel[n]+image.GetPixel(m);
                }
            }
        }
    }
    else if (image.NumBands()==1)
    {
        int m=0;
        for(int k=0; k<Bands; ++k)
        {
            int n=0;
            for(int i=0; i<Rows; ++i)
            {
                for(int j=0; j<Cols; ++j, ++m, ++n)
                {
                    pItem[m] += image.GetPixel(n);
                }
            }
        }
    }
}

//
// Arithmetic operators for Images.
//
template <class Item>
Image<Item>
Image<Item>:: operator-(const Image<Item>& image) const
{
    assert(Rows==image.NumRows()&&Cols==image.NumCols());
    assert(Bands==image.NumBands() || Bands==1 || image.NumBands()==1);

    Image<Item> result;
    if(Bands==image.NumBands())
    {
        result = image;
        for(int m=0; m<Bands*Rows*Cols; ++m)
        {
            result.SetPixel(m, pItem[m]-image.GetPixel(m));
        }
    }
    else if(Bands==1)
    {
        result = image;
        for(int k=0, m=0; k<image.NumBands(); ++k)
        {
            for(int i=0; i<Rows; ++i)
            {
                for(int j=0; j<Cols; ++j, ++m)
                {
                    result.SetPixel(m, pppItem[0][i][j]-image.GetPixel(m));
                }
            }
        }
    }
    else if (image.NumBands()==1)
    {
        result = *this;
        for(int k=0, m=0; k<Bands; ++k)
        {
            for(int i=0; i<Rows; ++i)
            {
                for(int j=0; j<Cols; ++j, ++m)
                {
                    result.SetPixel(m, GetPixel(m)-image.GetPixel(0,i,j));
                }
            }
        }
    }
    return result;
}

template <class Item>
void
Image<Item>:: operator-=(const Image<Item>& image)
{
    assert(Rows==image.NumRows()&&Cols==image.NumCols());
    assert(Bands==image.NumBands() || Bands==1 || image.NumBands()==1);

    if(Bands == image.NumBands())
    {
        for(int m=0; m<Bands*Rows*Cols; ++m)
            pItem[m] -= image.GetPixel(m);
    }
    else if(Bands == 1)
    {
        Item* oldPixel = new Item[Rows*Cols];
        for(int i=0; i<Rows*Cols; ++i)
            oldPixel[i] = pItem[i];

        ResizeImage(image.NumBands(), Rows, Cols);

        for(int k=0, m=0; k<image.NumBands(); ++k)
        {
            int n=0;
            for(int i=0; i<Rows; ++i)
            {
                for(int j=0; j<Cols; ++j,++n, ++m)
                {
                    pItem[m] = oldPixel[n]-image.GetPixel(m);
                }
            }
        }
    }
    else if (image.NumBands()==1)
    {
        int m=0;
        for(int k=0; k<Bands; ++k)
        {
            int n=0;
            for(int i=0; i<Rows; ++i)
            {
                for(int j=0; j<Cols; ++j, ++m, ++n)
                {
                    pItem[m] -= image.GetPixel(n);
                }
            }
        }
    }
}

//
// Arithmetic operators for Images.
//
template <class Item>
Image<Item>
Image<Item>:: operator*(const Image<Item>& image) const
{
    assert(Rows==image.NumRows()&&Cols==image.NumCols());
    assert(Bands==image.NumBands() || Bands==1 || image.NumBands()==1);

    Image<Item> result;
    if(Bands==image.NumBands())
    {
        result = image;
        for(int m=0; m<Bands*Rows*Cols; ++m)
        {
            result.SetPixel(m, pItem[m]*image.GetPixel(m));
        }
    }
    else if(Bands==1)
    {
        result = image;
        for(int k=0, m=0; k<image.NumBands(); ++k)
        {
            for(int i=0; i<Rows; ++i)
            {
                for(int j=0; j<Cols; ++j, ++m)
                {
                    result.SetPixel(m, pppItem[0][i][j]*image.GetPixel(m));
                }
            }
        }
    }
    else if (image.NumBands()==1)
    {
        result = *this;
        for(int k=0, m=0; k<Bands; ++k)
        {
            for(int i=0; i<Rows; ++i)
            {
                for(int j=0; j<Cols; ++j, ++m)
                {
                    result.SetPixel(m, GetPixel(m)*image.GetPixel(0,i,j));
                }
            }
        }
    }
    return result;
}

template <class Item>
void
Image<Item>:: operator*=(const Image<Item>& image)
{
    assert(Rows==image.NumRows()&&Cols==image.NumCols());
    assert(Bands==image.NumBands() || Bands==1 || image.NumBands()==1);

    if(Bands == image.NumBands())
    {
        for(int m=0; m<Bands*Rows*Cols; ++m)
            pItem[m] *= image.GetPixel(m);
    }
    else if(Bands == 1)
    {
        Item* oldPixel = new Item[Rows*Cols];
        for(int i=0; i<Rows*Cols; ++i)
            oldPixel[i] = pItem[i];

        ResizeImage(image.NumBands(), Rows, Cols);

        for(int k=0, m=0; k<image.NumBands(); ++k)
        {
            int n=0;
            for(int i=0; i<Rows; ++i)
            {
                for(int j=0; j<Cols; ++j,++n, ++m)
                {
                    pItem[m] = oldPixel[n]*image.GetPixel(m);
                }
            }
        }
    }
    else if (image.NumBands()==1)
    {
        int m=0;
        for(int k=0; k<Bands; ++k)
        {
            int n=0;
            for(int i=0; i<Rows; ++i)
            {
                for(int j=0; j<Cols; ++j, ++m, ++n)
                {
                    pItem[m] *= image.GetPixel(n);
                }
            }
        }
    }
}

//
// Arithmetic operators for Images.
//
template <class Item>
Image<Item>
Image<Item>:: operator/(const Image<Item>& image) const
{
    assert(Rows==image.NumRows()&&Cols==image.NumCols());
    assert(Bands==image.NumBands() || Bands==1 || image.NumBands()==1);

    Image<Item> result;
    if(Bands==image.NumBands())
    {
        result = image;
        for(int m=0; m<Bands*Rows*Cols; ++m)
        {
            result.SetPixel(m, pItem[m]/image.GetPixel(m));
        }
    }
    else if(Bands==1)
    {
        result = image;
        for(int k=0, m=0; k<image.NumBands(); ++k)
        {
            for(int i=0; i<Rows; ++i)
            {
                for(int j=0; j<Cols; ++j, ++m)
                {
                    result.SetPixel(m, pppItem[0][i][j]/image.GetPixel(m));
                }
            }
        }
    }
    else if (image.NumBands()==1)
    {
        result = *this;
        for(int k=0, m=0; k<Bands; ++k)
        {
            for(int i=0; i<Rows; ++i)
            {
                for(int j=0; j<Cols; ++j, ++m)
                {
                    result.SetPixel(m, GetPixel(m)/image.GetPixel(0,i,j));
                }
            }
        }
    }
    return result;
}

template <class Item>
void
Image<Item>:: operator/=(const Image<Item>& image)
{
    assert(Rows==image.NumRows()&&Cols==image.NumCols());
    assert(Bands==image.NumBands() || Bands==1 || image.NumBands()==1);

    if(Bands == image.NumBands())
    {
        for(int m=0; m<Bands*Rows*Cols; ++m)
            pItem[m] /= image.GetPixel(m);
    }
    else if(Bands == 1)
    {
        Item* oldPixel = new Item[Rows*Cols];
        for(int i=0; i<Rows*Cols; ++i)
            oldPixel[i] = pItem[i];

        ResizeImage(image.NumBands(), Rows, Cols);

        for(int k=0, m=0; k<image.NumBands(); ++k)
        {
            int n=0;
            for(int i=0; i<Rows; ++i)
            {
                for(int j=0; j<Cols; ++j,++n, ++m)
                {
                    pItem[m] = oldPixel[n]/image.GetPixel(m);
                }
            }
        }
    }
    else if (image.NumBands()==1)
    {
        int m=0;
        for(int k=0; k<Bands; ++k)
        {
            int n=0;
            for(int i=0; i<Rows; ++i)
            {
                for(int j=0; j<Cols; ++j, ++m, ++n)
                {
                    pItem[m] /= image.GetPixel(n);
                }
            }
        }
    }
}

//
// Logic operators for Images.
//
template <class Item>
Image<Item>
Image<Item>:: operator|(const Image<Item>& image) const
{
    assert(Rows==image.NumRows()&&Cols==image.NumCols());
    assert(Bands==image.NumBands() || Bands==1 || image.NumBands()==1);

    Image<Item> result;
    if(Bands==image.NumBands())
    {
        result = image;
        for(int m=0; m<Bands*Rows*Cols; ++m)
        {
            result.SetPixel(m, pItem[m]|image.GetPixel(m));
        }
    }
    else if(Bands==1)
    {
        result = image;
        for(int k=0, m=0; k<image.NumBands(); ++k)
        {
            for(int i=0; i<Rows; ++i)
            {
                for(int j=0; j<Cols; ++j, ++m)
                {
                    result.SetPixel(m, pppItem[0][i][j]|image.GetPixel(m));
                }
            }
        }
    }
    else if (image.NumBands()==1)
    {
        result = *this;
        for(int k=0, m=0; k<Bands; ++k)
        {
            for(int i=0; i<Rows; ++i)
            {
                for(int j=0; j<Cols; ++j, ++m)
                {
                    result.SetPixel(m, GetPixel(m)|image.GetPixel(0,i,j));
                }
            }
        }
    }
    return result;
}

template <class Item>
void
Image<Item>:: operator|=(const Image<Item>& image)
{
    assert(Rows==image.NumRows()&&Cols==image.NumCols());
    assert(Bands==image.NumBands() || Bands==1 || image.NumBands()==1);

    if(Bands == image.NumBands())
    {
        for(int m=0; m<Bands*Rows*Cols; ++m)
            pItem[m] |= image.GetPixel(m);
    }
    else if(Bands == 1)
    {
        Item* oldPixel = new Item[Rows*Cols];
        for(int i=0; i<Rows*Cols; ++i)
            oldPixel[i] = pItem[i];

        ResizeImage(image.NumBands(), Rows, Cols);

        for(int k=0, m=0; k<image.NumBands(); ++k)
        {
            int n=0;
            for(int i=0; i<Rows; ++i)
            {
                for(int j=0; j<Cols; ++j,++n, ++m)
                {
                    pItem[m] = oldPixel[n]|image.GetPixel(m);
                }
            }
        }
    }
    else if (image.NumBands()==1)
    {
        int m=0;
        for(int k=0; k<Bands; ++k)
        {
            int n=0;
            for(int i=0; i<Rows; ++i)
            {
                for(int j=0; j<Cols; ++j, ++m, ++n)
                {
                    pItem[m] |= image.GetPixel(n);
                }
            }
        }
    }
}
template <class Item>
Image<Item>
Image<Item>:: operator&(const Image<Item>& image) const
{
    assert(Rows==image.NumRows()&&Cols==image.NumCols());
    assert(Bands==image.NumBands() || Bands==1 || image.NumBands()==1);

    Image<Item> result;
    if(Bands==image.NumBands())
    {
        result = image;
        for(int m=0; m<Bands*Rows*Cols; ++m)
        {
            result.SetPixel(m, pItem[m]&image.GetPixel(m));
        }
    }
    else if(Bands==1)
    {
        result = image;
        for(int k=0, m=0; k<image.NumBands(); ++k)
        {
            for(int i=0; i<Rows; ++i)
            {
                for(int j=0; j<Cols; ++j, ++m)
                {
                    result.SetPixel(m, pppItem[0][i][j]&image.GetPixel(m));
                }
            }
        }
    }
    else if (image.NumBands()==1)
    {
        result = *this;
        for(int k=0, m=0; k<Bands; ++k)
        {
            for(int i=0; i<Rows; ++i)
            {
                for(int j=0; j<Cols; ++j, ++m)
                {
                    result.SetPixel(m, GetPixel(m)&image.GetPixel(0,i,j));
                }
            }
        }
    }
    return result;
}

template <class Item>
void
Image<Item>:: operator&=(const Image<Item>& image)
{
    assert(Rows==image.NumRows()&&Cols==image.NumCols());
    assert(Bands==image.NumBands() || Bands==1 || image.NumBands()==1);

    if(Bands == image.NumBands())
    {
        for(int m=0; m<Bands*Rows*Cols; ++m)
            pItem[m] &= image.GetPixel(m);
    }
    else if(Bands == 1)
    {
        Item* oldPixel = new Item[Rows*Cols];
        for(int i=0; i<Rows*Cols; ++i)
            oldPixel[i] = pItem[i];

        ResizeImage(image.NumBands(), Rows, Cols);

        for(int k=0, m=0; k<image.NumBands(); ++k)
        {
            int n=0;
            for(int i=0; i<Rows; ++i)
            {
                for(int j=0; j<Cols; ++j,++n, ++m)
                {
                    pItem[m] = oldPixel[n]&image.GetPixel(m);
                }
            }
        }
    }
    else if (image.NumBands()==1)
    {
        int m=0;
        for(int k=0; k<Bands; ++k)
        {
            int n=0;
            for(int i=0; i<Rows; ++i)
            {
                for(int j=0; j<Cols; ++j, ++m, ++n)
                {
                    pItem[m] &= image.GetPixel(n);
                }
            }
        }
    }
}

//
// Scalar arithmetic operators for Images
//
template <class Item>
Image<Item>
Image<Item>:: operator+(const Item& value) const
{

    Image<Item> result(*this);
    for(int i=0; i<Bands*Rows*Cols; ++i)
        result.SetPixel(i, pItem[i]+value);

    return result;
}

template <class Item>
void
Image<Item>:: operator+=(const Item& value)
{

    for(int i=0; i<Bands*Rows*Cols; ++i)
        pItem[i] = pItem[i] + value;
}

template <class Item>
Image<Item>
Image<Item>:: operator-(const Item& value) const
{

    Image<Item> result(*this);
    for(int i=0; i<Bands*Rows*Cols; ++i)
        result.SetPixel(i, pItem[i]-value);

    return result;
}

template <class Item>
void
Image<Item>:: operator-=(const Item& value)
{

    for(int i=0; i<Bands*Rows*Cols; ++i)
        pItem[i] = pItem[i] - value;
}

template <class Item>
Image<Item>
Image<Item>:: operator*(const Item& value) const
{

    Image<Item> result(*this);
    for(int i=0; i<Bands*Rows*Cols; ++i)
        result.SetPixel(i, pItem[i]*value);

    return result;
}

template <class Item>
void
Image<Item>:: operator*=(const Item& value)
{

    for(int i=0; i<Bands*Rows*Cols; ++i)
        pItem[i] = pItem[i] * value;
}

template <class Item>
Image<Item>
Image<Item>:: operator^(const Item& value) const
{

    Image<Item> result(*this);
    for(int i=0; i<Bands*Rows*Cols; ++i)
        result.SetPixel(i, value - pItem[i]);

    return result;
}

template <class Item>
void
Image<Item>:: operator^=(const Item& value)
{

    for(int i=0; i<Bands*Rows*Cols; ++i)
        pItem[i] = value - pItem[i];
}

/*
the routine extracts Region of Interest (ROI) from the image.
the parameters specifies the region and they are taken as
inclusive.
*/

template <class Item>
void
Image<Item>:: extractROI(int low_band, int up_band,
                         int low_row, int up_row,
                         int low_col, int up_col)
{
    //assertion takes care of unexpected conditions
    assert(low_band<=up_band && low_row<=up_row && low_col<=up_col);
    assert(up_band<Bands && up_row <Rows && up_col < Cols);
    assert(low_band>=0 && low_row>= 0 && low_col>=0);

    Image<Item> tmp_im(*this);
    ResizeImage(up_band-low_band+1, up_row-low_row+1, up_col-low_col+1);

    //copy items with in the ROI
    int k, index;
    for(k=low_band, index=0; k<=up_band; ++k)
    {
        for(int i=low_row; i<=up_row; ++i)
        {
            for(int j=low_col; j<=up_col; ++j, ++index)
            {
                pItem[index] = tmp_im.GetPixel(k,i,j);
            }
        }
    }
}

template <class Item>
Image<Item>
Image<Item>:: ExtractROI(int low_band, int up_band,
                         int low_row, int up_row,
                         int low_col, int up_col) const
{
    //assertion takes care of unexpected conditions
    assert(low_band<=up_band && low_row<=up_row && low_col<=up_col);
    assert(up_band<Bands && up_row <Rows && up_col < Cols);
    assert(low_band>=0 && low_row>= 0 && low_col>=0);

    Image<Item> result = Image<Item>(up_band-low_band+1, up_row-low_row+1,
                                     up_col-low_col+1);

    //copy items with in the ROI
    for(int k=low_band, index=0; k<=up_band; ++k)
    {
        for(int i=low_row; i<=up_row; ++i)
        {
            for(int j=low_col; j<=up_col; ++j, ++index)
            {
                result.SetPixel(index, pppItem[k][i][j]);
            }
        }
    }

    return result;
}

/*
the routine expand an image with arbitrary offset.
*/

template <class Item>
void
Image<Item>:: expand(int new_bands, int new_rows, int new_cols,
                     int band_offset, int row_offset, int col_offset,
                     Item pad_value)
{
    //assertion takes care of unexpected conditions
    assert(band_offset<new_bands && row_offset<new_rows && col_offset<new_cols);

    Image<Item> temp_im = *this;

    *this = Image<Item>(new_bands, new_rows, new_cols, pad_value);

    for(int k=band_offset,k2=0; k2<temp_im.NumBands()&&k<Bands; ++k,++k2)
    {
        for(int i=row_offset,i2=0; i2<temp_im.NumRows()&&i<Rows; ++i,++i2)
        {
            for(int j=col_offset,j2=0; j2<temp_im.NumCols()&&j<Cols; ++j,++j2)
            {
                SetPixel(k,i,j,temp_im.GetPixel(k2,i2,j2));
            }
        }
    }
}

template <class Item>
Image<Item>
Image<Item>:: Expand(int new_bands, int new_rows, int new_cols,
                     int band_offset, int row_offset, int col_offset,
                     Item pad_value) const
{
    //assertion takes care of unexpected conditions
    assert(band_offset<new_bands && row_offset<new_rows && col_offset<new_cols);

    Image<Item> result(new_bands, new_rows, new_cols, pad_value);

    for(int k=band_offset,k2=0; k2<result.NumBands()&&k<Bands; ++k,++k2)
    {
        for(int i=row_offset,i2=0; i2<result.NumRows()&&i<Rows; ++i,++i2)
        {
            for(int j=col_offset,j2=0; j2<result.NumCols()&&j<Cols; ++j,++j2)
            {
                result.SetPixel(k2,i2,j2, pppItem[k][i][j]);
            }
        }
    }
    return result;
}

/*
the routine inserts another image into this image at an arbitrary offset
and return the new image.
*/

template <class Item>
Image<Item>
Image<Item>:: Insert(const Image<Item>& insert,
                     int band_offset, int row_offset, int col_offset)
{
    //assertion takes care of unexpected conditions
    assert(band_offset<Bands && row_offset<Rows && col_offset<Cols);

    Image<Item> res(Bands,Rows,Cols);
    //copy items with in the ROI
    for(int k=band_offset,k2=0; k2<insert.NumBands()&&k<Bands; ++k,++k2)
    {
        for(int i=row_offset,i2=0; i2<insert.NumRows()&&i<Rows; ++i,++i2)
        {
            for(int j=col_offset,j2=0; j2<insert.NumCols()&&j<Cols; ++j,++j2)
            {
                res.SetPixel(k,i,j,insert.GetPixel(k2,i2,j2));
            }
        }
    }
    return res;
}

/*
the routine inserts another image into this image at an arbitrary offset
*/

template <class Item>
void
Image<Item>:: insert(const Image<Item>& insert,
                     int band_offset, int row_offset, int col_offset)
{
    //assertion takes care of unexpected conditions
    assert(band_offset<Bands && row_offset<Rows && col_offset<Cols);

    //copy items with in the ROI
    for(int k=band_offset,k2=0; k2<insert.NumBands()&&k<Bands; ++k,++k2)
    {
        for(int i=row_offset,i2=0; i2<insert.NumRows()&&i<Rows; ++i,++i2)
        {
            for(int j=col_offset,j2=0; j2<insert.NumCols()&&j<Cols; ++j,++j2)
            {
                pppItem[k][i][j] = insert.GetPixel(k2,i2,j2);
            }
        }
    }
}

/*
the routine increases the size of the image by integer factors 
with pixel duplication.
*/

template <class Item>
Image<Item>
Image<Item>:: ScaleUp(int scale_bands, int scale_rows, int scale_cols) const
{
    //assertion takes care of unexpected conditions
    assert(scale_bands>0);
    assert(scale_rows>0);
    assert(scale_cols>0);

    Image<Item> res(Bands*scale_bands,Rows*scale_rows,Cols*scale_cols);

    for(int k=0; k<res.NumBands(); ++k)
    {
        for(int i=0; i<res.NumRows(); ++i)
        {
            for(int j=0; j<res.NumCols(); ++j)
            {
                res.SetPixel(k,i,j,GetPixel(k/scale_bands,i/scale_rows,j/scale_cols));
            }
        }
    }
    return res;
}

/*
the routine increases the size of the image by integer factors 
with pixel duplication.
*/

template <class Item>
void
Image<Item>:: scaleup(int scale_bands, int scale_rows, int scale_cols)
{
    //assertion takes care of unexpected conditions
    assert(scale_bands>0);
    assert(scale_rows>0);
    assert(scale_cols>0);


    Image<Item> temp_im = *this;

    *this = Image<Item>(Bands*scale_bands,Rows*scale_rows,Cols*scale_cols);

    for(int k=0; k<Bands; ++k)
    {
        for(int i=0; i<Rows; ++i)
        {
            for(int j=0; j<Cols; ++j)
            {
                SetPixel(k,i,j,temp_im.GetPixel(k/scale_bands,i/scale_rows,j/scale_cols));
            }
        }
    }
}

/*
the routine performs a circular shift on the image.
*/
template <class Item>
void
Image<Item>:: circularShift(int band_shift, int row_shift, int col_shift)
{
    //assertion takes care of unexpected conditions
    assert(band_shift<Bands && row_shift<Rows && col_shift<Cols);
    assert(band_shift> -Bands && row_shift> -Rows && col_shift> -Cols);
    band_shift = (band_shift<0) ? Bands+band_shift: band_shift;
    row_shift = (row_shift<0) ? Rows+row_shift: row_shift;
    col_shift = (col_shift<0) ? Cols+col_shift: col_shift;

    Image<Item> tmp_im(*this);

    for(int k=0, k2=band_shift; k<Bands; ++k,++k2)
    {
        for(int i=0, i2=row_shift; i<Rows; ++i,++i2)
        {
            for(int j=0,j2=col_shift; j<Cols; ++j,++j2)
            {
                pppItem[k][i][j] = tmp_im.GetPixel(k2%Bands,i2%Rows,j2%Cols);
            }
        }
    }
}

/*
the routine performs a circular shift on the image.
*/
template <class Item>
Image<Item>
Image<Item>:: CircularShift(int band_shift, int row_shift, int col_shift)
{
    //assertion takes care of unexpected conditions
    assert(band_shift<Bands && row_shift<Rows && col_shift<Cols);
    assert(band_shift> -Bands && row_shift> -Rows && col_shift> -Cols);
    band_shift = (band_shift<0) ? Bands+band_shift: band_shift;
    row_shift = (row_shift<0) ? Rows+row_shift: row_shift;
    col_shift = (col_shift<0) ? Cols+col_shift: col_shift;

    Image<Item> result(Bands, Rows, Cols);

    for(int k=0, k2=band_shift; k<Bands; ++k,++k2)
    {
        for(int i=0, i2=row_shift; i<Rows; ++i,++i2)
        {
            for(int j=0,j2=col_shift; j<Cols; ++j,++j2)
            {
                result.SetPixel(k,i,j,pppItem[k2%Bands][i2%Rows][j2%Cols]);
            }
        }
    }
    return result;
}

//
// This routine computes and returns the gradient of an image.
// the result image has twice the number of bands as the original.
// a pair of bands represent the 2D gradient vector where the 1st
// one represent the horizontal direction and the 2nd the vertical
// direction.
//
// Currently, the gradient is computed by a simple forward difference.
//
template <class Item>
Image<Item>
Image<Item>:: Gradient() const
{
    Image<Item> result(Bands*2, Rows, Cols);

    for(int k=0; k<Bands; k++)
    {
        for(int i=0; i<Rows; ++i)
        {
            for(int j=0; j<Cols; ++j)
            {
                Item here = GetPixel(k, i, j);
                Item up = GetPixelRepeat(k,i-1,j);
                Item right = GetPixelRepeat(k,i,j+1);
                result.SetPixel(2*k, i, j, right-here);
                result.SetPixel(2*k+1, i, j, up-here);
            }
        }
    }
    return result;
}

//
// This routine computes and returns the strength of the gradient.
// the strength is computed by L2 norm squared.
//
template <class Item>
Image<Item>
Image<Item>:: GradientStrength() const
{
    Item item;  //whatever the default value for item.
    Image<Item> result(Bands, Rows, Cols, item);

    for(int k=0; k<Bands; ++k)
    {
        for(int i=0; i<Rows; ++i)
        {
            for(int j=0; j<Cols; ++j)
            {
                Item here = GetPixel(k, i, j);
                Item down = GetPixelRepeat(k,i+1,j);
                Item right = GetPixelRepeat(k,i,j+1);
                Item val = (here-down)*(here-down)+(here-right)*(here-right);
                result.SetPixel(k, i, j, val);
            }
        }
    }

    return result;
}

//
// This routine computes and returns the strength of the gradient.
// the gradient is 2-points approximation.
// the strength is computed by L2 norm squared.
//
template <class Item>
Image<Item>
Image<Item>:: GradientStrength2() const
{
    Item item;  //whatever the default value for item.
    Image<Item> result(Bands, Rows, Cols, item);

    for(int k=0; k<Bands; ++k)
    {
        for(int i=0; i<Rows; ++i)
        {
            for(int j=0; j<Cols; ++j)
            {
                Item up = GetPixelRepeat(k, i-1, j);
                Item down = GetPixelRepeat(k,i+1,j);
                Item right = GetPixelRepeat(k,i,j+1);
                Item left = GetPixelRepeat(k,i,j-1);
                Item val = .25*((up-down)*(up-down)+(left-right)*(left-right));
                result.SetPixel(k, i, j, val);
            }
        }
    }

    return result;
}

template <class Item>
Item**
Image<Item>:: Image2NRarray(int band) const
{
    Item** a;
    a = new ptr1D_Data[Rows]-1;
    for(int i=1; i<=Rows;++i)
        a[i]=pItem+band*Rows*Cols+(i-1)*Cols-1;
    return a;
}


template<class Item>
int
CompareImageSize(const Image<Item>& image1, const Image<Item>& image2)
{
    if (image1.NumBands() > image2.NumBands())
        return 1;
    else if (image1.NumBands() < image2.NumBands())
        return -1;
    else if (image1.NumRows() > image2.NumRows())
        return 1;
    else if (image1.NumRows() < image2.NumRows())
        return -1;
    else if (image1.NumCols() > image2.NumCols())
        return 1;
    else if (image1.NumCols() < image2.NumCols())
        return -1;
    else
        return 0;
}

template<class Item>
int
ComparePlaneSize(const Image<Item>& image1, const Image<Item>& image2)
{
    if (image1.NumRows() > image2.NumRows())
        return 1;
    else if (image1.NumRows() < image2.NumRows())
        return -1;
    else if (image1.NumCols() > image2.NumCols())
        return 1;
    else if (image1.NumCols() < image2.NumCols())
        return -1;
    else
        return 0;
}

// Overloaded input operator for class Array;
// inputs values for entire array.
template <class Item>
istream &operator>>( istream& input, Image<Item>& image )
{
    Item item;
    for ( int i = 0; i < image.NumBands*image.NumRows()*image.NumCols(); i++ )
    {
        input >> item;
        image.SetPixel(i, item);
    }

    return input;   // enables cin >> x >> y;
}

template <class Item>
ostream& operator<<(ostream& s, const Image<Item>& image)
{
    Item item;
    int rows, cols, bands;

    cols=image.NumCols();
    rows=image.NumRows();
    bands=image.NumBands();

    s << "Bands=" << bands << endl;
    s << "Rows=" << rows << endl;
    s << "Cols=" << cols << endl;
    for(int k=0; k<bands; ++k)
    {
        s << "Band " << k << endl;
        for(int i=0; i<rows; ++i)
        {
            for(int j=0; j<cols; ++j)
            {
                item=image.GetPixel(k,i,j);
                s << item << " ";
            }
            s << endl;
        }
    }

    return s;
}

// file IO routine.
// this is a friend of Image class.
//
template <class Item>
ifstream& operator>>( ifstream& input, Image<Item>& image )
{
    int bands, rows, cols;

    input >> bands;
    input >> rows;
    input >> cols;

    //I don't know why but I have to throw one char away
    // in order to read in data properly...
    char dummy;
    input.read(&dummy, 1);

    image.ResizeImage(image.Bands, image.Rows, image.Cols);

    int num_pixel = image.Bands*image.Rows*image.Cols;
    Item *buffer = new Item[num_pixel];
    input.read(reinterpret_cast<char *> (buffer),
               num_pixel*sizeof(Item));

    for(int i=0; i<num_pixel; ++i)
        image.pItem[i] = buffer[i];

    delete [] buffer;

    return input;
}

// file IO routine.
// this is a friend of Image class.
//
template <class Item>
ofstream& operator<<(ofstream& output, const Image<Item>& image)
{
    output << image.Bands << endl;
    output << image.Rows << endl;
    output << image.Cols << endl;

    int num_pixel=image.Bands*image.Rows*image.Cols;
    Item *buffer = new Item[num_pixel];

    for(int i=0; i<num_pixel; ++i)
        buffer[i] = image.pItem[i];

    output.write(reinterpret_cast<char *> (buffer),
                 num_pixel*sizeof(Item));

    delete [] buffer;

    return output;
}

//
// file IO routine
// this routine reads a pnm file and convert it into
// the Image class
//

template <class Item>
int
Image<Item>:: ReadPnmFile(char* filename)
{
    FILE  *fp;
    int bytes_read, size;
    int   msize, val, i, j, k;
    unsigned char *buf;
    char  pmode, c, firstchar, rname[] = "ReadPnmFile";

    printf("Reading data from %s\n", filename);

    /* try to open the file */
    if ((fp = fopen(filename, "rb")) == NULL)
    {
        fprintf(stderr, "%s: error loading image from %s.\n",
                "ReadPnmFile()", filename);
        fclose(fp);
        return(-1);
    }

    /* Check for the correct magic number */
    fscanf(fp, "%c%c\n", &firstchar, &pmode);
    if (firstchar != 'P')
    {
        fprintf(stderr, "%s: unknown image file type: %c\n", rname, firstchar);
        fclose(fp);
        return(-1);
    }

    pnm_type type;
    int bands;
    switch (pmode)
    {
    case '1':
        type = IO_PBMA;
        bands=1;
        break;
    case '2':
        type = IO_PGMA;
        bands=1;
        break;
    case '3':
        type = IO_PPMA;
        bands=3;
        break;
    case '4':
        type = IO_PBM;
        bands=1;
        break;
    case '5':
        type = IO_PGM;
        bands=1;
        break;
    case '6':
        type = IO_PPM;
        bands=3;
        break;
    default:
        fprintf(stderr, "%s: unrecognized magic number: %c\n", rname, pmode);
        fclose(fp);
        return(-1);
    }

    /* throw out any comments */
    while ((c = getc(fp)) == '#')
        while (!feof(fp) && ((c = getc(fp)) != '\n'))
            ;
    ungetc(c, fp);
    if (feof(fp))
    {
        fprintf(stderr, "%s: unexpected end of file in %s!\n", rname, filename);
        fclose(fp);
        return(-1);
    }

    int cols, rows;
    /* read the header information */
    fscanf(fp,"%d %d%c", &cols, &rows, &c);
    if (type != IO_PBM && type != IO_PBMA)
        fscanf(fp, "%d%c", &msize, &c);

    ResizeImage(bands, rows, cols);

    cout << "Bands=" << Bands << " Rows=" << Rows << ", Cols=" << Cols << endl;

    switch (type)
    {
    case IO_PBMA:
    case IO_PGMA:
        size = Cols * Rows;
        for(i = 0; i < size; ++i)
        {
            if (feof(fp))
            {
                fprintf(stderr, "%s: unexpected end of file\n", rname);
                return(-1);
            }
            fscanf(fp, "%d", &val);
            pItem[i] = (Item) val;
        }
        break;
    case IO_PPMA:
        if (Bands < 3)
        {
            fprintf(stderr, "%s: unmatched image depth! (%d)\n",
                    rname, Bands);
            return(-1);
        }
        size = Rows*Cols;
        for(i = 0; i < Rows; ++i)
        {
            for(j=0; j<Cols; ++j)
            {
                if (feof(fp))
                {
                    fprintf(stderr, "%s: unexpected end of file\n", rname);
                    return(-1);
                }
                fscanf(fp, "%d", &val);
                SetPixel(0,i,j,val);
                fscanf(fp, "%d", &val);
                SetPixel(1,i,j,val);
                fscanf(fp, "%d", &val);
                SetPixel(2,i,j,val);
            }
        }
        break;
    case IO_PBM:
        size = (int)ceil((double) .125 * Cols) * Rows;
        buf = (unsigned char*) malloc(size);
        bytes_read = fread(buf, sizeof(unsigned char), size, fp);
        if (bytes_read != size)
        {
            fprintf(stderr,
                    "%s: Warning: file not of expected size!\n", rname);
            free(buf);
            return(-1);
        }
        for(i=0, k=0; i<Rows; i++)
        {
            for(j=0; j<Cols; j++)
            {
                if(j % 8 == 0)
                    val = buf[k++];
                SetPixel(0,i,j,(Item)((val & 0x80) ? 1 : 0));
                val <<= 1;
            }
        }
        free(buf);
        break;
    case IO_PGM:
        size = Rows*Cols;
        buf = (unsigned char*) malloc(size);
        bytes_read = fread(buf, sizeof(unsigned char), size, fp);
        if (bytes_read != size)
        {
            fprintf(stderr,
                    "%s: Warning: file not of expected size!\n", rname);
            return(-1);
        }
        for(i=0; i<size; ++i)
        {
            pItem[i]=buf[i];
        }
        free(buf);
        break;
    case IO_PPM:
        size = 3 * Rows*Cols;
        buf = (unsigned char *) malloc(size);
        bytes_read = fread(buf, sizeof(unsigned char), size, fp);
        if (bytes_read != size)
        {
            fprintf(stderr,
                    "%s: Warning: file not of expected size!\n", rname);
            free(buf);
            return(-1);
        }
        for(i=0, k=0; i<Rows; ++i)
        {
            for(j=0; j<Cols; ++j)
            {
                SetPixel(0,i,j,(Item) buf[k++]);
                SetPixel(1,i,j,(Item) buf[k++]);
                SetPixel(2,i,j,(Item) buf[k++]);
            }
        }
        free(buf);
        break;
    default:
        break;
    }

    fclose(fp);
    return 0;
}

template <class Item>
void
Image<Item>:: ReadPnmFile(ifstream& in)
{
    int mode;
    int bands, rows, cols;
    char magic;

    in >> magic;
    in >> mode;
    assert(magic == 'P');
    switch(mode)
    {
    case 2:
        cerr << "ReadPnmFile:: Ascii data not supported\n";
        abort();
        break;
    case 3:
        cerr << "ReadPnmFile:: Ascii data not supported\n";
        abort();
        break;
    case 5:
        bands=1;
        break;
    case 6:
        bands=3;
        break;
    default:
        cerr << "Mode " << mode << " is not supported." << endl;
        return;
    }

    char chbuffer[MAX_COMMENT_LENGTH];
    in.getline(chbuffer, MAX_COMMENT_LENGTH);
    do
    { // skip comment lines
        in.getline(chbuffer, MAX_COMMENT_LENGTH);
    }
    while(chbuffer[0] == '#');

    sscanf(chbuffer, "%d %d", &cols, &rows);
    ResizeImage(bands, rows, cols);

    cout << "Bands=" << Bands << " Rows=" << Rows << ", Cols=" << Cols << endl;

    in.getline(chbuffer, MAX_COMMENT_LENGTH); // skip the line with 256

    unsigned char *buffer = new unsigned char[Bands*Rows*Cols];
    in.read((char *)buffer, Bands*Rows*Cols*sizeof(unsigned char));

    int index = 0;
    for(int i=0; i<Rows; ++i)
    {
        for(int j=0; j<Cols; ++j)
        {
            for(int k=0; k<Bands; ++k, ++index)
            {
                pppItem[k][i][j] = (Item) buffer[index];
            }
        }
    }

    delete [] buffer;
}

//
//This routine writes Image class image into a pgm file.
//Bands can be either 1 or 3.  Other values are not allowed.
//if norm=1, then the pgm image streatches from 0 to 255 where
//the minimum if this image is mapped to 0 and the maximum pixel
//is mapped to 255.
//

template <class Item>
int
Image<Item>:: WritePnmFile(char* filename, pnm_type type, int norm)
{

    int bands;
    if(Bands > 1)
    {
        bands=3;
    }
    else
    {
        bands=1;
    }
    unsigned char *buffer = new unsigned char[bands*Rows*Cols];

    real max, min;
    if(norm)
    {  // Normalize the image so that min=0 and max=255
        max = min = pItem[0];
        for(int i=0; i<Bands*Rows*Cols; ++i)
        {
            if(pItem[i] > max)
                max = pItem[i];
            if(pItem[i] < min)
                min = pItem[i];
        }
        cout << "Normalization: min= " << min << " max=" << max << endl;
    }
    else
    {
        min = 0;
        max = 255.0;
    }

    if(max - min > 1.0e-5)
    {
        int index=0;
        for(int i=0; i<Rows; ++i)
        {
            for(int j=0; j<Cols; ++j)
            {
                for(int k=0; k<Bands; ++k)
                {
                    buffer[index++] = (unsigned char)(255*(pppItem[k][i][j]-min)/(max-min));
                }
            }
        }
        for(int k=Bands; k<bands; ++k)
        {
            for(int i=0; i<Rows; ++i)
            {
                for(int j=0; j<Cols; ++j)
                {
                    buffer[index++] = (unsigned char)0;
                }
            }
        }
    }
    else
        for(int i=0; i<bands*Rows*Cols; ++i)
            buffer[i] = (char) pItem[0];

    /* Open the file */
    FILE *fp;
    size_t size;
    int i, j, k;
    unsigned char *buf, val;
    char rname[] = "write_pnm";

    if ((fp = fopen(filename, "wb")) == NULL)
    {
        fprintf(stderr, "%s: error opening file %s.\n", rname, filename);
        return(-1);
    }

    /* write out magic number for pnm files */
    switch (type)
    {
    case IO_PBMA:
        fprintf(fp, "P1\n");
        break;
    case IO_PGMA:
        fprintf(fp, "P2\n");
        break;
    case IO_PPMA:
        fprintf(fp, "P3\n");
        break;
    case IO_PBM:
        fprintf(fp, "P4\n");
        break;
    case IO_PGM:
        fprintf(fp, "P5\n");
        break;
    case IO_PPM:
        fprintf(fp, "P6\n");
        break;
    }

    /* write out comment */
    //if (comment != NULL) fprintf(fp, "# %s\n", comment);

    /* write out header info */
    fprintf(fp, "%d %d\n", Cols,Rows);
    if (type != IO_PBM)
        fprintf(fp, "255\n");

    /* write out data */
    switch(type)
    {
    case IO_PBMA:
        for(i = 0, k = 0; i < Rows; i++)
        {
            for(j = 0, val = 0; j < Cols; j++, k++)
            {
                if(buffer[i]>0)
                    fprintf(fp, "1 ");
                else
                    fprintf(fp, "0 ");
                if(k % 35 == 34)
                    fprintf(fp, "\n");
            }
        }
        break;
    case IO_PGMA:
        for(i = 0, k = 0; i < Rows; i++)
        {
            for(j = 0, val = 0; j < Cols; j++, ++k)
            {
                fprintf(fp, "%u ", buffer[k]);
                if(k % 17 == 16)
                    fprintf(fp, "\n");
            }
        }
        break;
    case IO_PPMA:
        for(i = 0, k = 0; i < Rows; i++)
        {
            for(j = 0, val = 0; j < Cols; j++, ++k)
            {
                fprintf(fp, "%u ", buffer[k]);
                fprintf(fp, "%u ", buffer[k+1]);
                fprintf(fp, "%u ", buffer[k+2]);
                if(k % 5 == 4)
                    fprintf(fp, "\n");
            }
        }
        break;
    case IO_PBM:
        size = ceil((double)(Cols * 0.125)) * Rows;
        buf = (unsigned char *) malloc(size);
        int jj;
        for(i = 0, k = 0, jj=0; i < Rows; i++)
        {
            for(j = 0, val = 0; j < Cols; j++,jj++)
            {
                if(buffer[jj] != 0)
                    val += 1;
                if(j % 8 == 7)
                    buf[k ++] = val;
                val <<= 1;
            }
            if(j % 8 != 0)
            {
                val <<= (7 - (j % 8));
                buf[k ++] = val;
            }
        }
        if ((fwrite(buf, sizeof(unsigned char), size, fp)) != size)
        {
            fprintf(stderr, "%s: could not write all of image to %s!\n",
                    rname, filename);
            free(buf);
            return(-1);
        }
        free(buf);
        break;
    case IO_PGM:
        size = Cols * Rows;
        if ((fwrite(buffer, sizeof(unsigned char), size, fp)) != size)
        {
            fprintf(stderr, "%s: could not write all of image to %s!\n",
                    rname, filename);
            return(-1);
        }
        break;
    case IO_PPM:
        size = 3 * Cols * Rows;
        if ((fwrite(buffer, sizeof(unsigned char), size, fp)) != size)
        {
            fprintf(stderr, "%s: could not write all of image to %s!\n",
                    rname, filename);
            return(-1);
        }
        break;
default:
        break;
    }
    fclose(fp);

    delete [] buffer;

    return 0;
}
template <class Item>
void
Image<Item>:: WritePnmFile(ofstream& out, int norm)
{

    int bands;
    if(Bands > 1)
    {
        bands=3;
        out << "P6" << endl;
    }
    else
    {
        bands=1;
        out << "P5" << endl;
    }
    out << Cols << " " << Rows << endl;
    out << "255" << endl;

    unsigned char *buffer = new unsigned char[bands*Rows*Cols];

    real max, min;
    if(norm)
    {  // Normalize the image so that min=0 and max=255
        max = min = pItem[0];
        for(int i=0; i<Bands*Rows*Cols; ++i)
        {
            if(pItem[i] > max)
                max = pItem[i];
            if(pItem[i] < min)
                min = pItem[i];
        }
        cout << "Normalization: min= " << min << " max=" << max << endl;
    }
    else
    {
        min = 0;
        max = 255.0;
    }

    if(max - min > 1.0e-5)
    {
        int index=0;
        for(int i=0; i<Rows; ++i)
        {
            for(int j=0; j<Cols; ++j)
            {
                for(int k=0; k<Bands; ++k,++index)
                {
                    buffer[index] = (unsigned char)(255*(pppItem[k][i][j]-min)/(max-min));
                }
                for(int k=Bands; k<bands; ++k,++index)
                {
                    buffer[index] = 0;
                }
            }
        }
    }
    else
        for(int i=0; i<bands*Rows*Cols; ++i)
            buffer[i] = (char) pItem[0];

    out.write((char *)buffer, bands*Rows*Cols*sizeof(unsigned char));

    delete [] buffer;
}

template<class Item>
void
WriteMultipleBand2PnmFile(const Image<Item>& image,
                          char* head, char* extension,
                          int normalize)
{
    Image<Item> buffer;
    int b;
    char filename[128];
    ofstream out;
    for(b=0; b<image.NumBands(); b+=3)
    {
        if(extension!=NULL)
        {
            if(image.NumBands()>3)
                sprintf(filename, "%s.%s%d", head,extension,b);
            else
                sprintf(filename, "%s.%s", head,extension);
        }
        else if(image.NumBands()>3)
            sprintf(filename, "%s%d", head, b);
        else
            sprintf(filename, "%s", head);
        out.open(filename, ios::out);
        buffer=image.ExtractROI(b,Min(b+2,image.NumBands()-1),
                                0,image.NumRows()-1,
                                0,image.NumCols()-1);
        buffer.WritePnmFile(out, normalize);
        out.close();
    }
}

template<class Item>
void
InteractiveImageWrite(const Image<Item>& image, char* filename, int normalize)
{

    Image<Item> tmp_im;
    int band;

    for(int done=0; !done; )
    {
        cout << "Which band to look at (type negative to quit): ";
        cin >> band;
        if(band < 0)
            done = 1;
        else
        {
            if(band >= image.NumBands())
                cerr << "Out of limit.  Has to be 0.." << image.NumBands()-1 << endl;
            else
            {
                tmp_im = image.ExtractROI(band, band, 0, image.NumRows()-1,
                                          0, image.NumCols()-1);

                tmp_im.WritePnmFile(filename,IO_PGM,normalize);
            }
        }
    }
}

//
// Raw file file I/O.
// The header for a raw data file is:
// #bands #rows #cols
// in the first line followed by the data.
//
//const int MAX_COMMENT_LENGTH=256;

template <class Item>
void
Image<Item>:: ReadRawFile(ifstream& in)
{
    int bands,rows,cols;
    char chbuf[MAX_COMMENT_LENGTH];
    //in.getline(chbuf, MAX_COMMENT_LENGTH);
    //sscanf(chbuf, "%d %d %d\n", &bands,&rows,&cols);

    in >> bands;
    in >> rows;
    in >> cols;
    in.read(chbuf,2);
    //in.getline(chbuf,MAX_COMMENT_LENGTH);
    ResizeImage(bands, rows, cols);

    cout << "Bands=" << Bands << " Rows=" << Rows << ", Cols=" << Cols << endl;

    in.read((char *)pItem, Bands*Rows*Cols*sizeof(Item));
    cout << pItem[0] << endl;
}

template <class Item>
int
Image<Item>:: ReadRawFile(char* filename)
{
    int bands,rows,cols;
    printf("Reading data from %s\n", filename);

    /* try to open the file */
    FILE* fp;
    if ((fp = fopen(filename, "rb")) == NULL)
    {
        fprintf(stderr, "%s: error loading image from %s.\n",
                "ReadRawFile()", filename);
        fclose(fp);
        return(-1);
    }
    fscanf(fp, "%d %d %d\n", &bands,&rows,&cols);

    ResizeImage(bands, rows, cols);

    cout << "Bands=" << Bands << " Rows=" << Rows << ", Cols=" << Cols << endl;
    int bytes_read;
    bytes_read = fread(pItem, sizeof(Item), Bands*Rows*Cols, fp);
    if (bytes_read != Bands*Rows*Cols)
    {
        fprintf(stderr,
                "%s: Warning: file %s not of expected size!\n",
                "ReadRawFile()", filename);
        return(-1);
    }

    return(1);
}

template <class Item>
void
Image<Item>:: WriteRawFile(ofstream& out)
{

    cout << "Bands=" << Bands << " Rows=" << Rows << ", Cols=" << Cols << endl;

    out << Bands << " " << Rows << " " << Cols << endl;
    out.write((char *)pItem, Bands*Rows*Cols*sizeof(Item));

}

template <class Item>
int
Image<Item>:: WriteRawFile(char* filename)
{

    cout << "Bands=" << Bands << " Rows=" << Rows << ", Cols=" << Cols << endl;

    printf("Writing data from %s\n", filename);

    /* try to open the file */
    FILE* fp;
    if ((fp = fopen(filename, "wb")) == NULL)
    {
        fprintf(stderr, "%s: error loading image from %s.\n",
                "WriteRawFile()", filename);
        fclose(fp);
        return(-1);
    }

    fprintf(fp, "%d %d %d\n", Bands,Rows,Cols);

    if ((fwrite(pItem, sizeof(Item), Bands*Rows*Cols, fp)) != Bands*Rows*Cols)
    {
        fprintf(stderr, "%s: could not write all of image to %s!\n",
                "WriteRawFile", filename);
        return(-1);
    }
    return 1;
}

#endif /* Image565_cpp */
