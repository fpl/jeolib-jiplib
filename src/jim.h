/**********************************************************************
jim.h: class to read raster files
History
2016/06/24 - Created by Pieter Kempeneers
Change log
***********************************************************************/
#ifndef _JIM_H_
#define _JIM_H_

#include "pktools/imageclasses/ImgRaster.h"
#include "pktools/imageclasses/ImgCollection.h"
#include "pktools/apps/AppFactory.h"
#include <string>
#include <vector>
#include <memory>
extern "C" {
#include "mialib/mialib_swig.h"
#include "mialib/mialib_convolve.h"
#include "mialib/mialib_dem.h"
#include "mialib/mialib_dist.h"
#include "mialib/mialib_erodil.h"
#include "mialib/mialib_format.h"
#include "mialib/mialib_geodesy.h"
#include "mialib/mialib_geometry.h"
#include "mialib/mialib_hmt.h"
#include "mialib/mialib_imem.h"
#include "mialib/mialib_io.h"
#include "mialib/mialib_label.h"
#include "mialib/mialib_miscel.h"
#include "mialib/mialib_opclo.h"
#include "mialib/mialib_pointop.h"
#include "mialib/mialib_proj.h"
#include "mialib/mialib_segment.h"
#include "mialib/mialib_stats.h"
#include "mialib/op.h"
}

/**
   Name space jiplib
**/
namespace jiplib{
  /** @brief class for raster dataset (read and write).

      Jim is a class that enables the integration of functionalities from both pktools and mia image processing libraries
      @author Pierre Soille, Pieter Kempeneers
      @date 2016
  */
  class Jim : public ImgRaster
  {
  public:
    ///default constructor
  Jim() : m_nplane(1), m_mia(0), ImgRaster(){};
    ///constructor opening an image in memory using an external data pointer (not tested yet)
  Jim(void* dataPointer, int ncol, int nrow, const GDALDataType& dataType) : Jim() {open(dataPointer,ncol,nrow,dataType);};
    ///constructor input image
  Jim(IMAGE *mia) : Jim() {setMIA(mia,0);};
    ///constructor input image
  Jim(const std::string& filename, unsigned int memory=0) : m_nplane(1), m_mia(0), ImgRaster(filename,memory){};
    ///constructor input image
  Jim(const std::string& filename, const Jim& imgSrc, unsigned int memory=0, const std::vector<std::string>& options=std::vector<std::string>()) : m_nplane(1), m_mia(0), ImgRaster(filename,imgSrc,memory,options){};
    ///constructor input image
    /* Jim(std::shared_ptr<ImgRaster> imgSrc, bool copyData=true) : m_nplane(1), m_mia(0), ImgRaster(imgSrc, copyData){}; */
    ///constructor input image
  Jim(Jim& imgSrc, bool copyData=true) : m_nplane(1), m_mia(0), ImgRaster(imgSrc, copyData){};
    ///constructor output image
  Jim(const std::string& filename, int ncol, int nrow, int nband, const GDALDataType& dataType, const std::string& imageType, unsigned int memory=0, const std::vector<std::string>& options=std::vector<std::string>()) : m_nplane(1), m_mia(0), ImgRaster(filename, ncol, nrow, nband, dataType, imageType, memory, options){};
    ///constructor output image
  Jim(int ncol, int nrow, int nband, const GDALDataType& dataType) : m_nplane(1), m_mia(0), ImgRaster(ncol, nrow, nband, dataType){};
    ///constructor from app
  Jim(app::AppFactory &theApp): m_nplane(1), m_mia(0), ImgRaster(theApp){};
    ///destructor
    ~Jim(void){if(m_mia) delete(m_mia);m_mia=0;};

    ///Open an image for writing in memory, defining image attributes.
    /* void open(int ncol, int nrow, int nband, int dataType); */

    ///Clone as new shared pointer to ImgRaster object
    /**
     *
     * @return shared pointer to new ImgRaster object alllowing polymorphism
     */
    virtual std::shared_ptr<ImgRaster> clone() {
      std::shared_ptr<Jim> pJim=std::dynamic_pointer_cast<Jim>(cloneImpl());
      if(pJim)
        return(pJim);
      else{
        std::cerr << "Warning: static pointer cast may slice object" << std::endl;
        return(std::static_pointer_cast<Jim>(cloneImpl()));
      }
    }
    ///Create new shared pointer to Jim object
    /**
     *
     * @return shared pointer to new Jim object
     */
    static std::shared_ptr<Jim> createImg() {
      return(std::make_shared<Jim>());
    };
    static std::shared_ptr<Jim> createImg(const std::shared_ptr<Jim> pSrc, bool copyData=true){
      std::shared_ptr<Jim> pJim=std::make_shared<Jim>(*pSrc,copyData);
      return(pJim);
    }
    static std::shared_ptr<Jim> createImg(app::AppFactory &theApp){
      std::shared_ptr<Jim> pJim=std::make_shared<Jim>(theApp);
      return(pJim);
    }

    ///reset all member variables
    void reset(void){ImgRaster::reset();m_nplane=1;m_mia=0;};
    ///Get the number of planes of this dataset
    int nrOfPlane(void) const { return m_nplane;};
    /// convert single plane multiband image to single band image with multiple planes
    CPLErr band2plane(){};//not implemented yet
    /// convert single band multiple plane image to single plane multiband image
    CPLErr plane2band(){};//not implemented yet
    ///get MIA representation for a particular band
    IMAGE* getMIA(int band=0);
    ///set memory from internal MIA representation for particular band
    CPLErr setMIA(int band=0);
    // ///set memory from MIA representation for particular band
    CPLErr setMIA(IMAGE* mia, int band=0);
    ///convert a GDAL data type to MIA data type
    /**
     *
     *
     * @param aGDALDataType
     *
     * @return MIA data type
     */
    int GDAL2MIADataType(GDALDataType aGDALDataType){
      //function exists, but introduced for naming consistency
      return(GDAL2MIALDataType(aGDALDataType));
    };
    ///convert a MIA data type to GDAL data type
    /**
     *
     *
     * @param aMIADataType the MIA data type to be converted
     *
     * @return GDAL data type (GDT_Byte, GDT_UInt16, GDT_Int16, GDT_UInt32, GDT_Int32, GDT_Float32, GDT_Float64)
     */
    GDALDataType MIA2GDALDataType(int aMIADataType)
    {
      switch (aMIADataType){
      case t_UCHAR:
        return GDT_Byte;
      case t_USHORT:
        return GDT_UInt16;
      case t_SHORT:
        return GDT_Int16;
      case t_UINT32:
        return GDT_UInt32;
      case t_INT32:
        return GDT_Int32;
      case t_FLOAT:
        return GDT_Float32;
      case t_DOUBLE:
        return GDT_Float64;
        // case t_UINT64:
        //   return GDT_UInt64;
        // case t_INT64:
        //   return GDT_Int64;
      case t_UNSUPPORTED:
        return GDT_Unknown;
      default:
        return GDT_Unknown;
      }
    };
    ///assignment operator
    Jim& operator=(Jim& imgSrc);
    /* ///relational == operator */
    /* bool operator==(Jim& refImg); */
    /* ///relational == operator */
    /* bool operator==(std::shared_ptr<Jim> refImg); */
    /* ///test for equality (relational == operator) */
    /* bool isEqual(Jim& refImg){return(*this==(refImg));}; */
    /* ///relational == operator */
    /* bool isEqual(std::shared_ptr<Jim> refImg){return(this->operator==(refImg));}; */
    /* ///relational != operator */
    /* bool operator!=(Jim& refImg){ return !(this->operator==(refImg)); }; */
    /* ///relational != operator */
    /* bool operator!=(std::shared_ptr<Jim> refImg){ return !(this->operator==(refImg)); }; */
    /* /// perform bitwise shift for a particular band */
    /* CPLErr shift(int value, int iband=0); */
    ///crop Jim image in memory returning Jim image
    /**
     * @param app application specific option arguments
     * @return output image
The utility pkcrop can subset and stack raster images. In the spatial domain it can crop a bounding box from a larger image. The output bounding box is selected by setting the new corner coordinates using the options -ulx -uly -lrx -lry. Alternatively you can set the new image center (-x -y) and size. This can be done either in projected coordinates (using the options -nx -ny) or in image coordinates (using the options -ns -nl). You can also use a vector file to set the new bounding box (option -e). In the spectral domain, pkcrop allows you to select individual bands from one or more input image(s). Bands are stored in the same order as provided on the command line, using the option -b. Band numbers start with index 0 (indicating the first band). The default is to select all input bands. If more input images are provided, the bands are stacked into a multi-band image. If the bounding boxes or spatial resolution are not identical for all input images, you should explicitly set them via the options. The pkcrop utility is not suitable to mosaic or composite images. Consider the utility pkcomposite instead.

\section pkcrop_options Options
 - use either `-short` or `--long` options (both `--long=value` and `--long value` are supported)
 - short option `-h` shows basic options only, long option `--help` shows all options
|short|long|type|default|description|
|-----|----|----|-------|-----------|
 | i      | input                | std::string |       |Input image file(s). If input contains multiple images, a multi-band output is created | 
 | o      | output               | std::string |       |Output image file | 
 | a_srs  | a_srs                | std::string |       |Override the projection for the output file (leave blank to copy from input file, use epsg:3035 to use European projection and force to European grid | 
 | ulx    | ulx                  | double | 0     |Upper left x value bounding box | 
 | uly    | uly                  | double | 0     |Upper left y value bounding box | 
 | lrx    | lrx                  | double | 0     |Lower right x value bounding box | 
 | lry    | lry                  | double | 0     |Lower right y value bounding box | 
 | b      | band                 | unsigned int |       |band index to crop (leave empty to retain all bands) | 
 | sband  | startband            | unsigned int |      |Start band sequence number | 
 | eband  | endband              | unsigned int |      |End band sequence number   | 
 | as     | autoscale            | double |       |scale output to min and max, e.g., --autoscale 0 --autoscale 255 | 
 | ot     | otype                | std::string |       |Data type for output image ({Byte/Int16/UInt16/UInt32/Int32/Float32/Float64/CInt16/CInt32/CFloat32/CFloat64}). Empty string: inherit type from input image | 
 | of     | oformat              | std::string | GTiff |Output image format (see also gdal_translate)| 
 | ct     | ct                   | std::string |       |color table (file with 5 columns: id R G B ALFA (0: transparent, 255: solid) | 
 | dx     | dx                   | double |       |Output resolution in x (in meter) (empty: keep original resolution) | 
 | dy     | dy                   | double |       |Output resolution in y (in meter) (empty: keep original resolution) | 
 | r      | resampling-method    | std::string | near  |Resampling method (near: nearest neighbor, bilinear: bi-linear interpolation). | 
 | e      | extent               | std::string |       |get boundary from extent from polygons in vector file | 
 | cut      | crop_to_cutline    | bool | false |Crop the extent of the target dataset to the extent of the cutline | 
 | eo       | eo                 | std::string |       |special extent options controlling rasterization: ATTRIBUTE|CHUNKYSIZE|ALL_TOUCHED|BURN_VALUE_FROM|MERGE_ALG, e.g., -eo ATTRIBUTE=fieldname |
 | m      | mask                 | std::string |       |Use the specified file as a validity mask (0 is nodata) | 
 | msknodata | msknodata            | float | 0     |Mask value not to consider for crop
 | mskband | mskband              | short | 0     |Mask band to read (0 indexed) | 
 | co     | co                   | std::string |       |Creation option for output file. Multiple options can be specified. | 
 | x      | x                    | double |       |x-coordinate of image center to crop (in meter) | 
 | y      | y                    | double |       |y-coordinate of image center to crop (in meter) | 
 | nx     | nx                   | double |       |image size in x to crop (in meter) | 
 | ny     | ny                   | double |       |image size in y to crop (in meter) | 
 | ns     | ns                   | int  |       |number of samples  to crop (in pixels) | 
 | nl     | nl                   | int  |       |number of lines to crop (in pixels) | 
 | scale  | scale                | double |       |output=scale*input+offset | 
 | off    | offset               | double |       |output=scale*input+offset | 
 | nodata | nodata               | float |       |Nodata value to put in image if out of bounds. | 
 | align  | align                | bool  |       |Align output bounding box to input image | 
 | mem    | mem                  | unsigned long int | 0 |Buffer size (in MB) to read image data blocks in memory | 
 | d      | description          | std::string |       |Set image description | 

Examples
========
Some examples how to use pkcrop can be found \ref examples_pkcrop "here"
**/
    std::shared_ptr<Jim> crop(app::AppFactory& app){
      std::shared_ptr<Jim> imgWriter=Jim::createImg();
      ImgRaster::crop(*imgWriter, app);
      return(imgWriter);
    }
    /* /// perform arithmetic operation for a particular band */
    CPLErr arith(Jim& imgRaster, int theOperation, int band=0);
    /* /\* CPLErr arith(std::shared_ptr<Jim> imgRaster, int theOperation, int band=0); *\/ */
    /* /// perform arithmetic operation for a particular band (non-destructive version) */
    /* std::shared_ptr<jiplib::Jim> getArith(Jim& imgRaster, int theOperation, int iband=0); */
    /* std::shared_ptr<jiplib::Jim> getArith(std::shared_ptr<Jim> imgRaster, int theOperation, int iband=0){return(getArith(*imgRaster,theOperation,iband));}; */
    /* /// perform arithmetic operation with a cst argument for a particular band */
    CPLErr arithcst(double dcst, int theOperation, int band=0);
    /* /// perform arithmetic operation with a cst argument for a particular band (non-destructive version) */
    /* std::shared_ptr<jiplib::Jim> getArithcst(double dcst, int theOperation, int iband=0); */
    /* /// perform a morphological reconstruction by dilation for a particular band */
    /* CPLErr rdil(std::shared_ptr<Jim> mask, int graph, int flag, int band=0); */
    /* /// perform a morphological reconstruction by dilation for a particular band (non-destructive version) */
    /* std::shared_ptr<jiplib::Jim> getRdil(std::shared_ptr<Jim> mask, int graph, int flag, int iband=0); */
    /* /// perform a morphological reconstruction by erosion for a particular band */
    /* CPLErr rero(std::shared_ptr<Jim> mask, int graph, int flag, int band=0); */
    /* /// perform a morphological reconstruction by erosion for a particular band (non-destructive version) */
    /* std::shared_ptr<jiplib::Jim> getRero(std::shared_ptr<Jim> mask, int graph, int flag, int iband=0); */
    ///get volume (from mialib)
    double getVolume(int iband=0) {IMAGE *mia=getMIA(iband);volume(mia);return(mia->vol);};
    /* ///read data from with reduced resolution */
    /* CPLErr GDALRead(std::string filename, int band, int nXOff, int nYOff, int nXSize, int nYSize, int nBufXSize=0, int nBufYSize=0); */

    //in memory functions from ImgRaster using AppFactory
    ///filter Jim image and return filtered image as shared pointer
    std::shared_ptr<Jim> filter(app::AppFactory& theApp){
      std::shared_ptr<Jim> imgWriter=createImg();
      ImgRaster::filter(*imgWriter,theApp);
      return(imgWriter);
    }
    ///stretch Jim image and return stretched image as shared pointer
    std::shared_ptr<Jim> stretch(app::AppFactory& app){
      std::shared_ptr<Jim> imgWriter=createImg();
      ImgRaster::stretch(*imgWriter, app);
      return(imgWriter);
    }
    ///create statistical profile from a collection
    std::shared_ptr<Jim> diff(app::AppFactory& app){
      std::shared_ptr<Jim> imgWriter=Jim::createImg();
      ImgRaster::diff(*imgWriter, app);
      return(imgWriter);
    }
    ///supervised classification using support vector machine (train with extractImg/extractOgr)
    std::shared_ptr<Jim> svm(app::AppFactory& app){
      std::shared_ptr<Jim> imgWriter=Jim::createImg();
      ImgRaster::svm(*imgWriter, app);
      return(imgWriter);
    }

  protected:
    ///number of planes in this dataset
    int m_nplane;
  private:
    virtual std::shared_ptr<ImgRaster> cloneImpl() {
      //test
      std::cout << "clone Jim object" << std::endl;
      /* return std::make_shared<Jim>(*this,false); */
      return(Jim::createImg());
    };
    IMAGE* m_mia;
  };

  class JimList : public ImgCollection{
  public:
    JimList(){ImgCollection();};
    ///constructor using vector of images
    JimList(const std::vector<std::shared_ptr<Jim> > &jimVector){
      for(int ijim=0;ijim<jimVector.size();++ijim){
        pushImage(jimVector[ijim]);
      }
    }
    ///push image to collection
    void pushImage(const std::shared_ptr<Jim> imgRaster){
      this->emplace_back(imgRaster);
    };
    ///get image from collection
    const std::shared_ptr<Jim> getImage(int index){
      return(std::dynamic_pointer_cast<Jim>(this->at(index)));
    }
    ///composite image only for in memory
    std::shared_ptr<Jim> composite(app::AppFactory& app){
      std::shared_ptr<Jim> imgWriter=Jim::createImg();
      ImgCollection::composite(*imgWriter, app);
      return(imgWriter);
    }
    /**
     * @param app application specific option arguments
     * @return output image
The utility pkcrop can subset and stack raster images. In the spatial domain it can crop a bounding box from a larger image. The output bounding box is selected by setting the new corner coordinates using the options -ulx -uly -lrx -lry. Alternatively you can set the new image center (-x -y) and size. This can be done either in projected coordinates (using the options -nx -ny) or in image coordinates (using the options -ns -nl). You can also use a vector file to set the new bounding box (option -e). In the spectral domain, pkcrop allows you to select individual bands from one or more input image(s). Bands are stored in the same order as provided on the command line, using the option -b. Band numbers start with index 0 (indicating the first band). The default is to select all input bands. If more input images are provided, the bands are stacked into a multi-band image. If the bounding boxes or spatial resolution are not identical for all input images, you should explicitly set them via the options. The pkcrop utility is not suitable to mosaic or composite images. Consider the utility pkcomposite instead.

\section pkcrop_options Options
 - use either `-short` or `--long` options (both `--long=value` and `--long value` are supported)
 - short option `-h` shows basic options only, long option `--help` shows all options
|short|long|type|default|description|
|-----|----|----|-------|-----------|
 | i      | input                | std::string |       |Input image file(s). If input contains multiple images, a multi-band output is created | 
 | o      | output               | std::string |       |Output image file | 
 | a_srs  | a_srs                | std::string |       |Override the projection for the output file (leave blank to copy from input file, use epsg:3035 to use European projection and force to European grid | 
 | ulx    | ulx                  | double | 0     |Upper left x value bounding box | 
 | uly    | uly                  | double | 0     |Upper left y value bounding box | 
 | lrx    | lrx                  | double | 0     |Lower right x value bounding box | 
 | lry    | lry                  | double | 0     |Lower right y value bounding box | 
 | b      | band                 | unsigned int |       |band index to crop (leave empty to retain all bands) | 
 | sband  | startband            | unsigned int |      |Start band sequence number | 
 | eband  | endband              | unsigned int |      |End band sequence number   | 
 | as     | autoscale            | double |       |scale output to min and max, e.g., --autoscale 0 --autoscale 255 | 
 | ot     | otype                | std::string |       |Data type for output image ({Byte/Int16/UInt16/UInt32/Int32/Float32/Float64/CInt16/CInt32/CFloat32/CFloat64}). Empty string: inherit type from input image | 
 | of     | oformat              | std::string | GTiff |Output image format (see also gdal_translate)| 
 | ct     | ct                   | std::string |       |color table (file with 5 columns: id R G B ALFA (0: transparent, 255: solid) | 
 | dx     | dx                   | double |       |Output resolution in x (in meter) (empty: keep original resolution) | 
 | dy     | dy                   | double |       |Output resolution in y (in meter) (empty: keep original resolution) | 
 | r      | resampling-method    | std::string | near  |Resampling method (near: nearest neighbor, bilinear: bi-linear interpolation). | 
 | e      | extent               | std::string |       |get boundary from extent from polygons in vector file | 
 | cut      | crop_to_cutline    | bool | false |Crop the extent of the target dataset to the extent of the cutline | 
 | eo       | eo                 | std::string |       |special extent options controlling rasterization: ATTRIBUTE|CHUNKYSIZE|ALL_TOUCHED|BURN_VALUE_FROM|MERGE_ALG, e.g., -eo ATTRIBUTE=fieldname |
 | m      | mask                 | std::string |       |Use the specified file as a validity mask (0 is nodata) | 
 | msknodata | msknodata            | float | 0     |Mask value not to consider for crop
 | mskband | mskband              | short | 0     |Mask band to read (0 indexed) | 
 | co     | co                   | std::string |       |Creation option for output file. Multiple options can be specified. | 
 | x      | x                    | double |       |x-coordinate of image center to crop (in meter) | 
 | y      | y                    | double |       |y-coordinate of image center to crop (in meter) | 
 | nx     | nx                   | double |       |image size in x to crop (in meter) | 
 | ny     | ny                   | double |       |image size in y to crop (in meter) | 
 | ns     | ns                   | int  |       |number of samples  to crop (in pixels) | 
 | nl     | nl                   | int  |       |number of lines to crop (in pixels) | 
 | scale  | scale                | double |       |output=scale*input+offset | 
 | off    | offset               | double |       |output=scale*input+offset | 
 | nodata | nodata               | float |       |Nodata value to put in image if out of bounds. | 
 | align  | align                | bool  |       |Align output bounding box to input image | 
 | mem    | mem                  | unsigned long int | 0 |Buffer size (in MB) to read image data blocks in memory | 
 | d      | description          | std::string |       |Set image description | 

Examples
========
Some examples how to use pkcrop can be found \ref examples_pkcrop "here"
**/
    std::shared_ptr<Jim> crop(app::AppFactory& app){
      std::shared_ptr<Jim> imgWriter=Jim::createImg();
      ImgCollection::crop(*imgWriter, app);
      return(imgWriter);
    }
    ///stack all images in collection to multiband image (alias for crop)
    std::shared_ptr<Jim> stack(app::AppFactory& app){return(crop(app));};
    ///stack all images in collection to multiband image (alias for crop)
    std::shared_ptr<Jim> stack(){app::AppFactory app;return(crop(app));};
    ///create statistical profile from a collection
    std::shared_ptr<Jim> statProfile(app::AppFactory& app){
      std::shared_ptr<Jim> imgWriter=Jim::createImg();
      ImgCollection::statProfile(*imgWriter, app);
      return(imgWriter);
    }
  };
}

#endif // _JIM_H_
