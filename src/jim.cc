/**********************************************************************
jim.cc: class to read raster files
History
2016/06/24 - Created by Pieter Kempeneers
Change log
***********************************************************************/
#include "config.h"
#include "json/json.h"
#include "jim.h"


using namespace jiplib;

///global function to create a Jim imageclasses
///Create new shared pointer to Jim object
/**
 * @param input (type: std::string) input filename
 * @param nodata (type: double) Nodata value to put in image if out of bounds.
 * @param band (type: int) Bands to open, index starts from 0
 * @param ulx (type: double) Upper left x value bounding box
 * @param uly (type: double) Upper left y value bounding box
 * @param lrx (type: double) Lower right x value bounding box
 * @param lry (type: double) Lower right y value bounding box
 * @param dx (type: double) Resolution in x
 * @param dy (type: double) Resolution in y
 * @param resample (type: std::string) (default: GRIORA_NearestNeighbour) resample: GRIORA_NearestNeighbour|GRIORA_Bilinear|GRIORA_Cubic|GRIORA_CubicSpline|GRIORA_Lanczos|GRIORA_Average|GRIORA_Average|GRIORA_Gauss (check http://www.gdal.org/gdal_8h.html#a640ada511cbddeefac67c548e009d5a)
 * @param extent (type: std::string) get boundary from extent from polygons in vector file
 * @param mem (type: unsigned long) (default: 0) Buffer size (in MB) to read image data blocks in memory
 * @param ncol (type: int) Number of columns
 * @param nrow (type: int) Number of rows
 * @param nband (type: int) (default: 1) Number of bands
 * @param otype (type: std::string) (default: Byte) Data type for output image ({Byte/Int16/UInt16/UInt32/Int32/Float32/Float64/CInt16/CInt32/CFloat32/CFloat64})
 * @param seed (type: unsigned long) (default: 0) seed value for random generator
 * @param mean (type: double) (default: 0) Mean value for random generator
 * @param sigma (type: double) (default: 0) Sigma value for random generator
 * @param description (type: std::string) Set image description
 * @param a_srs (type: std::string) Assign the spatial reference for the output file, e.g., psg:3035 to use European projection and force to European grid
 * @return shared pointer to new Jim object
 **/
std::shared_ptr<Jim> Jim::createImg(app::AppFactory &theApp){
  std::shared_ptr<Jim> pJim=std::make_shared<Jim>(theApp);
  return(pJim);
}
/* ///Create new shared pointer to Jim object */
/* /\** */
/*  * @param input (type: std::string) input filename */
/*  * @param nodata (type: double) Nodata value to put in image if out of bounds. */
/*  * @param band (type: int) Bands to open, index starts from 0 */
/*  * @param ulx (type: double) Upper left x value bounding box */
/*  * @param uly (type: double) Upper left y value bounding box */
/*  * @param lrx (type: double) Lower right x value bounding box */
/*  * @param lry (type: double) Lower right y value bounding box */
/*  * @param dx (type: double) Resolution in x */
/*  * @param dy (type: double) Resolution in y */
/*  * @param resample (type: std::string) (default: GRIORA_NearestNeighbour) resample: GRIORA_NearestNeighbour|GRIORA_Bilinear|GRIORA_Cubic|GRIORA_CubicSpline|GRIORA_Lanczos|GRIORA_Average|GRIORA_Average|GRIORA_Gauss (check http://www.gdal.org/gdal_8h.html#a640ada511cbddeefac67c548e009d5a) */
/*  * @param extent (type: std::string) get boundary from extent from polygons in vector file */
/*  * @param mem (type: unsigned long) (default: 0) Buffer size (in MB) to read image data blocks in memory */
/*  * @param ncol (type: int) Number of columns */
/*  * @param nrow (type: int) Number of rows */
/*  * @param nband (type: int) (default: 1) Number of bands */
/*  * @param otype (type: std::string) (default: Byte) Data type for output image ({Byte/Int16/UInt16/UInt32/Int32/Float32/Float64/CInt16/CInt32/CFloat32/CFloat64}) */
/*  * @param seed (type: unsigned long) (default: 0) seed value for random generator */
/*  * @param mean (type: double) (default: 0) Mean value for random generator */
/*  * @param sigma (type: double) (default: 0) Sigma value for random generator */
/*  * @param description (type: std::string) Set image description */
/*  * @param a_srs (type: std::string) Assign the spatial reference for the output file, e.g., psg:3035 to use European projection and force to European grid */
/*  * @return shared pointer to new Jim object */
/*  **\/ */
std::shared_ptr<Jim> Jim::createImg() {
  return(std::make_shared<Jim>());
};
/* ///Create new shared pointer to Jim object */
/* /\** */
/*  * @param input (type: std::string) input filename */
/*  * @param nodata (type: double) Nodata value to put in image if out of bounds. */
/*  * @param band (type: int) Bands to open, index starts from 0 */
/*  * @param ulx (type: double) Upper left x value bounding box */
/*  * @param uly (type: double) Upper left y value bounding box */
/*  * @param lrx (type: double) Lower right x value bounding box */
/*  * @param lry (type: double) Lower right y value bounding box */
/*  * @param dx (type: double) Resolution in x */
/*  * @param dy (type: double) Resolution in y */
/*  * @param resample (type: std::string) (default: GRIORA_NearestNeighbour) resample: GRIORA_NearestNeighbour|GRIORA_Bilinear|GRIORA_Cubic|GRIORA_CubicSpline|GRIORA_Lanczos|GRIORA_Average|GRIORA_Average|GRIORA_Gauss (check http://www.gdal.org/gdal_8h.html#a640ada511cbddeefac67c548e009d5a) */
/*  * @param extent (type: std::string) get boundary from extent from polygons in vector file */
/*  * @param mem (type: unsigned long) (default: 0) Buffer size (in MB) to read image data blocks in memory */
/*  * @param ncol (type: int) Number of columns */
/*  * @param nrow (type: int) Number of rows */
/*  * @param nband (type: int) (default: 1) Number of bands */
/*  * @param otype (type: std::string) (default: Byte) Data type for output image ({Byte/Int16/UInt16/UInt32/Int32/Float32/Float64/CInt16/CInt32/CFloat32/CFloat64}) */
/*  * @param seed (type: unsigned long) (default: 0) seed value for random generator */
/*  * @param mean (type: double) (default: 0) Mean value for random generator */
/*  * @param sigma (type: double) (default: 0) Sigma value for random generator */
/*  * @param description (type: std::string) Set image description */
/*  * @param a_srs (type: std::string) Assign the spatial reference for the output file, e.g., psg:3035 to use European projection and force to European grid */
/*  * @return shared pointer to new Jim object */
/*  **\/ */
std::shared_ptr<Jim> Jim::createImg(const std::shared_ptr<Jim> pSrc, bool copyData){
  std::shared_ptr<Jim> pJim=std::make_shared<Jim>(*pSrc,copyData);
  return(pJim);
}

CPLErr Jim::open(void* dataPointer, int ncol, int nrow, int nplane, const GDALDataType& dataType){
  m_ncol=ncol;
  m_nrow=nrow;
  m_nplane=nplane;
  m_nband=1;
  m_dataType=dataType;
  m_data.resize(m_nband);
  m_begin.resize(m_nband);
  m_end.resize(m_nband);
  m_blockSize=nrow;//memory contains entire image and has been read already
  if(dataPointer){
    for(int iband=0;iband<m_nband;++iband){
      m_data[iband]=(char *)dataPointer+iband*ncol*nrow*nplane*getDataTypeSizeBytes();
      m_begin[iband]=0;
      m_end[iband]=m_begin[iband]+m_blockSize;
    }
    // m_externalData=true;
    return(CE_None);
  }
  else
    return(CE_Failure);
}

///open dataset, read data and close (keep data in memory)
// CPLErr Jim::open(app::AppFactory &app) {
//   ImgRaster::open(app);
//   if(m_gds)
//     GDALClose(m_gds);
// }

///write to file previously set (eg., with setFile) without reset (keep data in memory)
CPLErr Jim::write(){
  //write, but do not reset
  if(m_data.size()&&m_filename.size()){
    for(int iband=0;iband<nrOfBand();++iband)
      writeNewBlock(nrOfRow(),iband);
  }
  char **papszOptions=NULL;
  for(std::vector<std::string>::const_iterator optionIt=m_options.begin();optionIt!=m_options.end();++optionIt)
    papszOptions=CSLAddString(papszOptions,optionIt->c_str());
  if(papszOptions)
    CSLDestroy(papszOptions);
  if(m_gds)
    GDALClose(m_gds);
  // reset();
}

///write to file without reset (keep data in memory)
CPLErr Jim::write(app::AppFactory &app){
  setFile(app);
  write();
}

///Create a JSON string from a Jim image
std::string Jim::jim2json(){
  Json::Value custom;
  custom["size"]=static_cast<int>(1);
  int iimg=0;
  Json::Value image;
  image["path"]=getFileName();
  std::string wktString=getProjectionRef();
  std::string key("EPSG");
  std::size_t foundEPSG=wktString.rfind(key);
  std::string fromEPSG=wktString.substr(foundEPSG);//EPSG","32633"]]'
  std::size_t foundFirstDigit=fromEPSG.find_first_of("0123456789");
  std::size_t foundLastDigit=fromEPSG.find_last_of("0123456789");
  std::string epsgString=fromEPSG.substr(foundFirstDigit,foundLastDigit-foundFirstDigit+1);
  image["epsg"]=atoi(epsgString.c_str());
  std::ostringstream os;
  os << iimg++;
  custom["0"]=image;
  Json::FastWriter fastWriter;
  return(fastWriter.write(custom));
}

///Create a custom collection from a Jim image
// std::string Jim::jim2custom(){
//   //todo:set directory as tmpnam will return path in /tmp
//   std::string custName = std::tmpnam(0);
//   std::vector<std::string> co;
//   co.push_back("COMPRESS=LZW");
//   setFile(custName,"GTiff",0,co);
//   write();
//   return(jim2json());
// }
/**
 *
 *
 * @param band the band to get the MIA image representation for
 *
 * @return pointer to MIA image representation
 */
IMAGE* Jim::getMIA(int band){
  if(getBlockSize()!=nrOfRow()){
    std::ostringstream s;
    s << "Error: increase memory to support MIA library functions (now at " << 100.0*getBlockSize()/nrOfRow() << "%)";
    throw(s.str());
  }
  if(m_mia.size()<band+1)
    m_mia.resize(band+1);
  if(m_mia[band])
    delete(m_mia[band]);
  m_mia[band]=new(IMAGE);
  m_mia[band]->p_im=m_data[band];/* Pointer to image data */
  m_mia[band]->DataType=getMIADataType();
  m_mia[band]->nx=nrOfCol();
  m_mia[band]->ny=nrOfRow();
  m_mia[band]->nz=nrOfPlane();
  m_mia[band]->NByte=m_mia[band]->nx * m_mia[band]->ny * m_mia[band]->nz * getDataTypeSizeBytes();//assumes image data type is not of bit type!!!
  //todo: remove m_mia[band]->vol and only rely on the getVolume function
  m_mia[band]->vol=0;//use getVolume() function
  m_mia[band]->lut=0;
  //USHORT *lut;   /* Pointer to colour map */
  //mia->g=getgetDataType();//not used
  return m_mia[band];
}

/**
 * set jim data pointer to the m_mia data pointer
 *
 * @param band the band for which the MIA image pointer needs to be set
 *
 * @return C_None if successful
 */
CPLErr Jim::setMIA(int band){
  try{
    // if(m_mia->nz>1){
    //   std::string errorString="Error: MIA image with nz>1 not supported";
    //   throw(errorString);
    // }
    if(m_mia.size()<band+1){
      std::ostringstream s;
      s << "Error: illegal band number when setting MIA in Jim";
      throw(s.str());
    }
    if(m_ncol!=m_mia[band]->nx){
      std::ostringstream s;
      s << "Error: x dimension of image (" << m_ncol << ") does not match MIA (" << m_mia[band]->nx << ")";
      throw(s.str());
    }
    if(m_nrow!=m_mia[band]->ny){
      std::ostringstream s;
      s << "Error: y dimension of image (" << m_nrow << ") does not match MIA (" << m_mia[band]->ny << ")";
      throw(s.str());
    }
    if(m_nband<=band){
      std::ostringstream s;
      std::string errorString="Error: band exceeds number of bands in target image";
      throw(errorString);
    }
    // if(m_nband>1&&m_dataType!=MIA2GDALDataType(m_mia[band]->DataType)){
    if( (m_dataType!=MIA2JIPLIBDataType(m_mia[band]->DataType)) && nrOfBand() > 1){
      std::cout << "Warning: changing data type of multiband image, make sure to set all bands" << std::endl;
    }
    m_dataType=MIA2JIPLIBDataType(m_mia[band]->DataType);
    m_data[band]=(void *)m_mia[band]->p_im;
    // m_data[band]=(unsigned char *)m_mia[band]->p_im + band * nrOfRow() * nrOfCol() * (GDALGetDataTypeSize(getDataType())>>3);
    m_begin[band]=0;
    m_end[band]=m_begin[band]+getBlockSize();
  }
  catch(std::string errorString){
    std::cerr << errorString << std::endl;
    return(CE_Failure);
  }
  catch(...){
    return(CE_Failure);
  }
  return(CE_None);
}

/**
 * set Jim attributes from external MIA image
 *
 * @param mia the MIA image pointer to be set
 * @param band the band for which the MIA image pointer needs to be set
 *
 * @return C_None if successful
 */
CPLErr Jim::setMIA(IMAGE* mia, int band){
  try{
    if(nrOfBand()){
      if(m_ncol!=mia->nx){
        std::ostringstream s;
        s << "Error: x dimension of image (" << m_ncol << ") does not match MIA (" << mia->nx << ")";
        throw(s.str());
      }
      if(m_nrow!=mia->ny){
        std::ostringstream s;
        s << "Error: y dimension of image (" << m_nrow << ") does not match MIA (" << mia->ny << ")";
        throw(s.str());
      }
      if(m_nplane!=mia->nz){
        std::string errorString="Error: number of planes of images do not match";
        throw(errorString);
      }
      if(m_dataType!=MIA2JIPLIBDataType(m_mia[band]->DataType)){
        std::string errorString="Error: inconsistent data types for multiband image";
        throw(errorString);
      }
    }
    if(m_mia.size()<band+1){
      m_mia.resize(band+1);
      m_nband=band+1;
    }
    if(m_data.size()<band+1){
      m_data.resize(band+1);
      m_begin.resize(band+1);
      m_end.resize(band+1);
    }
    m_nplane=mia->nz;
    m_ncol=mia->nx;
    m_nrow=mia->ny;
    m_blockSize=m_nrow;
    m_mia[band]=mia;
    // setExternalData(true);//todo: need to fix memory leak when setMIA used for single band only! (either create vector<bool> m_externalData or only allow for setMIA all bands)
    this->setMIA(band);
  }
  catch(std::string errorString){
    std::cerr << errorString << std::endl;
    return(CE_Failure);
  }
  catch(...){
    return(CE_Failure);
  }
  return(CE_None);
}

//used as a template for destructive functions returning IMAGE*
// CPLErr Jim::d_arith(Jim& imRaster_im2, int op, int iband){
//   try{
//     if(nrOfBand()<=iband){
//       std::string errorString="Error: band number exceeds number of bands in input image";
//       throw(errorString);
//     }
//     if(imRaster_im2.nrOfBand()<=iband){
//       std::string errorString="Error: band number exceeds number of bands in input image";
//       throw(errorString);
//     }
//     IMAGE * im1;
//     IMAGE * im2=imRaster_im2.getMIA(iband);
//     im1=this->getMIA(iband);
//     if(::arith(im1, im2, op) == NO_ERROR){
//       this->setMIA(iband);
//       imRaster_im2.setMIA(iband);
//       return(CE_None);
//     }
//     else{
//       this->setMIA(iband);
//       imRaster_im2.setMIA(iband);
//       std::string errorString="Error: arith() function in MIA failed, returning NULL pointer";
//       throw(errorString);
//     }
//   }
//   catch(std::string errorString){
//     std::cerr << errorString << std::endl;
//     return(CE_Failure);
//   }
//   catch(...){
//     return(CE_Failure);
//   }
// }
//used as a template for non-destructive functions returning IMAGE*
// std::shared_ptr<Jim> Jim::arith(Jim& imRaster_im2, int op, int iband){
//   try{
//     if(nrOfBand()<=iband){
//       std::string errorString="Error: band number exceeds number of bands in input image";
//       throw(errorString);
//     }
//     if(imRaster_im2.nrOfBand()<=iband){
//       std::string errorString="Error: band number exceeds number of bands in input image";
//       throw(errorString);
//     }
//     IMAGE * im1;
//     IMAGE * im2=imRaster_im2.getMIA(iband);
//     //make a copy of this
//     std::shared_ptr<Jim> copyImg=this->clone();
//     im1=copyImg->getMIA(iband);
//     if(::arith(im1, im2, op) == NO_ERROR){
//       copyImg->setMIA(iband);
//       imRaster_im2.setMIA(iband);
//       return(copyImg);
//     }
//     else{
//       copyImg->setMIA(iband);
//       imRaster_im2.setMIA(iband);
//       std::string errorString="Error: arith() function in MIA failed, returning NULL pointer";
//       throw(errorString);
//      }
//   }
//   catch(std::string errorString){
//     std::cerr << errorString << std::endl;
//     return(0);
//   }
//   catch(...){
//     return(0);
//   }
// }

// CPLErr Jim::magnify(int value, int iband){
//   try{
//     if(nrOfBand()<=iband){
//       std::string errorString="Error: band number exceeds number of bands in input image";
//       throw(errorString);
//     }
//     IMAGE* mia1=this->getMIA(iband);
//     if(::magnify(mia1, value) == NO_ERROR){
//       this->setMIA(iband);
//       return(CE_None);
//     }
//     else{
//       this->setMIA(iband);
//       std::string errorString="Error: arith function in MIA failed";
//       throw(errorString);
//     }
//   }
//   catch(std::string errorString){
//     std::cerr << errorString << std::endl;
//     return(CE_Failure);
//   }
//   catch(...){
//     return(CE_Failure);
//   }
// }
// /**
//  *
//  *
//  * @param imgRaster is operand
//  * @param theOperation the operation to be performed
//  * @param iband is the band for which the function needs to be performed (default 0 is first band)
//  *
//  * @return CE_None if successful
//  */
// CPLErr Jim::shift(int value, int iband){
//   try{
//     if(nrOfBand()<=iband){
//       std::string errorString="Error: band number exceeds number of bands in input image";
//       throw(errorString);
//     }
//     IMAGE* mia1=this->getMIA(iband);
//     if(::shift(mia1, value) == NO_ERROR){
//       this->setMIA(iband);
//       return(CE_None);
//     }
//     else{
//       this->setMIA(iband);
//       std::string errorString="Error: arith function in MIA failed";
//       throw(errorString);
//     }
//   }
//   catch(std::string errorString){
//     std::cerr << errorString << std::endl;
//     return(CE_Failure);
//   }
//   catch(...){
//     return(CE_Failure);
//   }
// }

// CPLErr Jim::thresh(double d_gt1, double d_gt2, double d_gbg, double d_gfg, int iband){
//   try{
//     if(nrOfBand()<=iband){
//       std::string errorString="Error: band number exceeds number of bands in input image";
//       throw(errorString);
//     }
//    IMAGE * im=this->getMIA(iband);
//    G_TYPE gt1;
//    G_TYPE gt2;
//    G_TYPE gbg;
//    G_TYPE gfg;
//    switch(getDataType()){
//    case(GDT_Byte):
//      gt1.uc_val=static_cast<unsigned char>(d_gt1);
//      gt2.uc_val=static_cast<unsigned char>(d_gt2);
//      gbg.uc_val=static_cast<unsigned char>(d_gbg);
//      gfg.uc_val=static_cast<unsigned char>(d_gfg);
//      break;
//    case(GDT_Int16):
//      gt1.s_val=static_cast<short int>(d_gt1);
//      gt2.s_val=static_cast<short int>(d_gt2);
//      gbg.s_val=static_cast<short int>(d_gbg);
//      gfg.s_val=static_cast<short int>(d_gfg);
//      break;
//    case(GDT_UInt16):
//      gt1.us_val=static_cast<unsigned short int>(d_gt1);
//      gt2.us_val=static_cast<unsigned short int>(d_gt2);
//      gbg.us_val=static_cast<unsigned short int>(d_gbg);
//      gfg.us_val=static_cast<unsigned short int>(d_gfg);
//      break;
//    case(GDT_Int32):
//      gt1.i32_val=static_cast<int>(d_gt1);
//      gt2.i32_val=static_cast<int>(d_gt2);
//      gbg.i32_val=static_cast<int>(d_gbg);
//      gfg.i32_val=static_cast<int>(d_gfg);
//      break;
//    case(GDT_UInt32):
//      gt1.u32_val=static_cast<unsigned int>(d_gt1);
//      gt2.u32_val=static_cast<unsigned int>(d_gt2);
//      gbg.u32_val=static_cast<unsigned int>(d_gbg);
//      gfg.u32_val=static_cast<unsigned int>(d_gfg);
//      break;
//    case(GDT_Float32):
//      gt1.f_val=static_cast<float>(d_gt1);
//      gt2.f_val=static_cast<float>(d_gt2);
//      gbg.f_val=static_cast<float>(d_gbg);
//      gfg.f_val=static_cast<float>(d_gfg);
//      break;
//    case(GDT_Float64):
//      gt1.d_val=static_cast<double>(d_gt1);
//      gt2.d_val=static_cast<double>(d_gt2);
//      gbg.d_val=static_cast<double>(d_gbg);
//      gfg.d_val=static_cast<double>(d_gfg);
//      break;
//    default:
//      std::string errorString="Error: data type not supported";
//      throw(errorString);
//      break;
//    }
//    if(::thresh(im, gt1, gt2, gbg, gfg) == NO_ERROR){
//      this->setMIA(iband);
//      return(CE_None);
//    }
//    else{
//      this->setMIA(iband);
//      std::string errorString="Error: arith function in MIA failed";
//      throw(errorString);
//    }
//  }
//      catch(std::string errorString){
//        std::cerr << errorString << std::endl;
//        return(CE_Failure);
//      }
//      catch(...){
//        return(CE_Failure);
//      }
//     }


// CPLErr Jim::bitwise_op(Jim& imRaster_im2, int op, int iband){
//   try{
//     if(nrOfBand()<=iband){
//       std::string errorString="Error: band number exceeds number of bands in input image";
//       throw(errorString);
//     }
//     if(imRaster_im2.nrOfBand()<=iband){
//       std::string errorString="Error: band number exceeds number of bands in input image";
//       throw(errorString);
//     }
//     IMAGE * im1=this->getMIA(iband);
//     IMAGE * im2=imRaster_im2.getMIA(iband);
//     if(::bitwise_op(im1, im2, op) == NO_ERROR){
//       this->setMIA(iband);
//       imRaster_im2.setMIA(iband);
//       return(CE_None);
//     }
//     else{
//       this->setMIA(iband);
//       imRaster_im2.setMIA(iband);
//       std::string errorString="Error: arith function in MIA failed";
//       throw(errorString);
//     }
//   }
//   catch(std::string errorString){
//     std::cerr << errorString << std::endl;
//     return(CE_Failure);
//   }
//   catch(...){
//     return(CE_Failure);
//   }
// }
// /**
//  *
//  *
//  * @param imgRaster is operand
//  * @param theOperation the operation to be performed
//  * @param iband is the band for which the function needs to be performed (default 0 is first band)
//  *
//  * @return CE_None if successful
//  */
// // CPLErr Jim::arith(std::shared_ptr<Jim> imgRaster, int theOperation, int iband){
// CPLErr Jim::arith(Jim& imgRaster, int theOperation, int iband){
//   try{
//     if(imgRaster.nrOfBand()<=iband){
//       std::string errorString="Error: band number exceeds number of bands in input image";
//       throw(errorString);
//     }
//     if(nrOfBand()<=iband){
//       std::string errorString="Error: band number exceeds number of bands in input image";
//       throw(errorString);
//     }
//     IMAGE* mia1=this->getMIA(iband);
//     IMAGE* mia2=imgRaster.getMIA(iband);
//     if(::arith(mia1, mia2, theOperation) == NO_ERROR){
//       this->setMIA(iband);
//       imgRaster.setMIA(iband);
//       return(CE_None);
//     }
//     else{
//       this->setMIA(iband);
//       imgRaster.setMIA(iband);
//       std::string errorString="Error: arith function in MIA failed";
//       throw(errorString);
//     }
//   }
//   catch(std::string errorString){
//     std::cerr << errorString << std::endl;
//     return(CE_Failure);
//   }
//   catch(...){
//     return(CE_Failure);
//   }
// }

// /**
//  *
//  *
//  * @param imgRaster is operand
//  * @param theOperation the operation to be performed
//  * @param iband is the band for which the function needs to be performed (default 0 is first band)
//  *
//  * @return CE_None if successful
//  */
// CPLErr Jim::arithcst(double dcst, int theOperation, int iband){
//   try{
//     if(nrOfBand()<=iband){
//       std::string errorString="Error: band number exceeds number of bands in input image";
//       throw(errorString);
//     }
//     G_TYPE gt;
//     IMAGE* mia1=this->getMIA(iband);
//     switch(getDataType()){
//     case(GDT_Byte):
//       gt.uc_val=static_cast<unsigned char>(dcst);
//       break;
//     case(GDT_Int16):
//       gt.us_val=static_cast<short>(dcst);
//       break;
//     case(GDT_UInt16):
//       gt.s_val=static_cast<unsigned short>(dcst);
//       break;
//     case(GDT_Int32):
//       gt.i32_val=static_cast<int>(dcst);
//       break;
//     case(GDT_UInt32):
//       gt.u32_val=static_cast<unsigned int>(dcst);
//       break;
//     case(GDT_Float32):
//       gt.f_val=static_cast<float>(dcst);
//       break;
//     case(GDT_Float64):
//       gt.d_val=static_cast<double>(dcst);
//       break;
//     default:
//       std::string errorString="Error: data type not supported";
//       throw(errorString);
//       break;
//     }
//     if(::arithcst(mia1, gt, theOperation) == NO_ERROR){
//       this->setMIA(iband);
//       return(CE_None);
//     }
//     else{
//       std::string errorString="Error: arithcst function in MIA failed";
//       throw(errorString);
//     }
//   }
//   catch(std::string errorString){
//     std::cerr << errorString << std::endl;
//     this->setMIA(iband);
//     return(CE_Failure);
//   }
//   catch(...){
//     this->setMIA(iband);
//     return(CE_Failure);
//   }
// }

//shown as a template function here only (not implemented because imout is composed of images of different datatypes)
// std::shared_ptr<Jim> Jim::imrgb2hsx(int x){
//   std::shared_ptr<Jim> imgWriter=std::make_shared<Jim>();
//   try{
//     IMAGE ** imout;
//     app::AppFactory app;
//     if(nrOfBand()!=3){
//       std::string errorString="Error: image is not an RGB image";
//       throw(errorString);
//     }
//     imout=::imrgb2hsx(getMIA(0),getMIA(1),getMIA(2),x);
//     if(imout){
//       for(int iim=0;iim<nrOfBand();++iim)
//         imgWriter->setMIA(imout[iim],iim);
//       imgWriter->copyGeoTransform(*this);
//       imgWriter->setProjection(this->getProjectionRef());
//       return(imgWriter);
//     }
//     else{
//       std::string errorString="Error: imrgb2hsx() function in MIA failed, returning empty image";
//       throw(errorString);
//     }
//   }
//   catch(std::string errorString){
//     std::cerr << errorString << std::endl;
//     return(imgWriter);
//   }
//   catch(...){
//     return(imgWriter);
//   }
// }

// /**
//  *
//  *
//  * @param dcst is the constant for operation
//  * @param theOperation the operation to be performed
//  * @param iband is the band for which the function needs to be performed (default 0 is first band)
//  *
//  * @return shared pointer to resulting image
//  */
// std::shared_ptr<jiplib::Jim> Jim::getArithcst(double dcst, int theOperation, int iband){
//   std::shared_ptr<jiplib::Jim> pJim=std::make_shared<jiplib::Jim>(*this, true);
//   if(pJim->arithcst(dcst, theOperation, iband)==CE_None)
//     return(pJim);
//   else
//     return(0);
// }
// /**
//  *
//  *
//  * @param imgRaster is operand
//  * @param theOperation the operation to be performed
//  * @param iband is the band for which the function needs to be performed (default 0 is first band)
//  *
//  * @return shared pointer to resulting image
//  */
// std::shared_ptr<jiplib::Jim> Jim::getArith(Jim& imgRaster, int theOperation, int iband){
//   std::shared_ptr<jiplib::Jim> pJim=std::make_shared<jiplib::Jim>(*this, true);
//   if(pJim->arith(imgRaster, theOperation, iband)==CE_None)
//     return(pJim);
//   else
//     return(0);
// }

// /**
//  *
//  *
//  * @param mask
//  * @param graph
//  * @param flag
//  * @param iband is the band for which the function needs to be performed (default is 0: first band)
//  *
//  * @return CE_None if successful
//  */
// CPLErr Jim::rdil(std::shared_ptr<Jim> mask, int graph, int flag, int iband){
//   IMAGE* markMIA=this->getMIA(iband);
//   IMAGE* maskMIA=mask->getMIA(iband);
//   if(::rdil(markMIA,maskMIA,graph,flag) == NO_ERROR){
//     this->setMIA(iband);
//     mask->setMIA(iband);
//     return(CE_None);
//   }
//   this->setMIA(iband);
//   mask->setMIA(iband);
//   return(CE_Failure);
// }

// /**
//  *
//  *
//  * @param mask
//  * @param graph
//  * @param flag
//  * @param iband is the band for which the operation needs to be performed (default is 0: first band)
//  *
//  * @return shared pointer to resulting image
//  */
// std::shared_ptr<jiplib::Jim> Jim::getRdil(std::shared_ptr<Jim> mask, int graph, int flag, int iband){
//   std::shared_ptr<jiplib::Jim> pJim=std::make_shared<jiplib::Jim>(*this, true);
//   IMAGE* markMIA=pJim->getMIA(iband);
//   IMAGE* maskMIA=mask->getMIA(iband);
//   if (::rdil(markMIA,maskMIA,graph,flag) == NO_ERROR){
//     pJim->setMIA(iband);
//   }
//   else
//     pJim=NULL;
//   mask->setMIA(iband);
//   return(pJim);
// }
// /**
//  *
//  *
//  * @param mask
//  * @param graph
//  * @param flag
//  * @param iband is the band for which the operation needs to be performed (default is 0: first band)
//  *
//  * @return CE_None if successful
//  */
// CPLErr Jim::rero(std::shared_ptr<Jim> mask, int graph, int flag, int iband){
//   IMAGE* markMIA=this->getMIA(iband);
//   IMAGE* maskMIA=mask->getMIA(iband);
//   if (::rero(markMIA,maskMIA,graph,flag) == NO_ERROR){
//     this->setMIA(iband);
//     mask->setMIA(iband);
//     return(CE_None);
//   }
//   this->setMIA(iband);
//   mask->setMIA(iband);
//   return(CE_Failure);
// }

// /**
//  *
//  *
//  * @param mask
//  * @param graph
//  * @param flag
//  * @param iband is the band for which the operation needs to be performed (default is 0: first band)
//  *
//  * @return shared pointer to resulting image
//  */
// std::shared_ptr<jiplib::Jim> Jim::getRero(std::shared_ptr<Jim> mask, int graph, int flag, int iband){
//   std::shared_ptr<jiplib::Jim> pJim=std::make_shared<jiplib::Jim>(*this, true);
//   IMAGE* markMIA=pJim->getMIA(iband);
//   IMAGE* maskMIA=mask->getMIA(iband);
//   if (::rero(markMIA,maskMIA,graph,flag) == NO_ERROR){
//     pJim->setMIA(iband);
//   }
//   else
//     pJim=NULL;
//   mask->setMIA(iband);
//   return(pJim);
// }

// /**
//  * @param imgSrc Use this source image as a template to copy image attributes
//  **/
// Jim& Jim::operator=(Jim& imgSrc)
// {
//   bool copyData=true;
//   //check for assignment to self (of the form v=v)
//   if(this==&imgSrc)
//      return *this;
//   else{
//     ImgRaster::open(imgSrc,copyData);
//     return *this;
//   }
// }


/**
 *
 * @param refImg Use this as the reference image
 *
 * @return true if image is equal to reference image
 */
bool Jim::operator==(std::shared_ptr<Jim> refImg)
{
  bool isEqual=true;
  if(nrOfBand()!=refImg->nrOfBand())
    return(false);
  if(nrOfRow()!=refImg->nrOfRow())
    return(false);
  if(nrOfCol()!=refImg->nrOfCol())
    return(false);

  for(int iband=0;iband<nrOfBand();++iband){
    if(getDataType(iband)!=refImg->getDataType(iband)){
      isEqual=false;
      break;
    }
    IMAGE* refMIA=refImg->getMIA(iband);
    IMAGE* thisMIA=this->getMIA(iband);
    if(::imequalp(thisMIA,refMIA)){
      isEqual=false;
      break;
    }
  }
  return(isEqual);
}
// /**
//  *
//  * @param refImg Use this as the reference image
//  *
//  * @return true if image is equal to reference image
//  */
// bool Jim::operator==(Jim& refImg)
// {
//   bool isEqual=true;
//   if(nrOfBand()!=refImg.nrOfBand())
//     return(false);
//   if(nrOfRow()!=refImg.nrOfRow())
//     return(false);
//   if(nrOfCol()!=refImg.nrOfCol())
//     return(false);

//   for(int iband=0;iband<nrOfBand();++iband){
//     if(getDataType(iband)!=refImg.getDataType(iband)){
//       isEqual=false;
//       break;
//     }
//     IMAGE* refMIA=refImg.getMIA(iband);
//     IMAGE* thisMIA=this->getMIA(iband);
//     if(::imequalp(thisMIA,refMIA)){
//       isEqual=false;
//       break;
//     }
//   }
//   return(isEqual);
// }

// double Jim::getVolume(int iband){
//   double theVolume=0;
//   std::vector<double> lineBuffer(nrOfCol());
//   for(int irow=0;irow<nrOfRow();++irow){
//     readData(lineBuffer,irow,iband);
//     for(int icol=0;icol<nrOfCol();++icol)
//       theVolume+=lineBuffer[icol];
//   }
//   return(theVolume);
// }

// std::shared_ptr<Jim> Jim::mean2d(int width, int iband){
//   try{
//     if(nrOfBand()<=iband){
//       std::string errorString="Error: band number exceeds number of bands in input image";
//       throw(errorString);
//     }
//     IMAGE * imout;
//     imout=::mean2d(this->getMIA(iband),width);
//     this->setMIA(iband);
//     if(imout){
//       std::shared_ptr<Jim> imgWriter=std::make_shared<Jim>(imout);
//       imgWriter->copyGeoTransform(*this);


// /**
//  * @param gt pointer to the six geotransform parameters:
//  * @param adfGeoTransform[0] top left x
//  * @param GeoTransform[1] w-e pixel resolution
//  * @param GeoTransform[2] rotation, 0 if image is "north up"
//  * @param GeoTransform[3] top left y
//  * @param GeoTransform[4] rotation, 0 if image is "north up"
//  * @param GeoTransform[5] n-s pixel resolution
//  **/
//       //if (this->isGeoRef()){
//       //double gt[6]={...};
//       //imgWriter->setGeoTransform(gt);

//       imgWriter->setProjection(getProjectionRef());
//       return(imgWriter);
//     }
//     else{
//       std::string errorString="Error: mean2d() function in MIA failed, returning NULL pointer";
//       throw(errorString);
//     }
//   }
//   catch(std::string errorString){
//     std::cerr << errorString << std::endl;
//     return(0);
//   }
//   catch(...){
//     return(0);
//   }
// }


// std::shared_ptr<Jim> Jim::copy_image(int iband){
//   try{
//     if(nrOfBand()<=iband){
//       std::string errorString="Error: band number exceeds number of bands in input image";
//       throw(errorString);
//     }
//     IMAGE * imout;
//     imout=::copy_image(this->getMIA(iband));
//     this->setMIA(iband);
//     if(imout){
//       std::shared_ptr<Jim> imgWriter=std::make_shared<Jim>(imout);
//       imgWriter->copyGeoTransform(*this);
//       imgWriter->setProjection(getProjectionRef());
//       return(imgWriter);
//     }
//     else{
//       std::string errorString="Error: copy_image() function in MIA failed, returning NULL pointer";
//       throw(errorString);
//     }
//   }
//   catch(std::string errorString){
//     std::cerr << errorString << std::endl;
//     return(0);
//   }
//   catch(...){
//     return(0);
//   }
// }

std::shared_ptr<Jim> Jim::setMask(app::AppFactory& app){
  std::shared_ptr<Jim> imgWriter=std::make_shared<Jim>();
  ImgRaster::setMask(*imgWriter, app);
  return(imgWriter);
}

std::shared_ptr<Jim> Jim::setMask(JimList& maskList, app::AppFactory& app){
  std::shared_ptr<Jim> imgWriter=std::make_shared<Jim>();
  ImgCollection maskReader;
  for(int imask=0;imask<maskList.size();++imask){
    maskReader.pushImage(maskList.getImage(imask));
    // std::shared_ptr<Jim> pJim=maskList.getImage(imask);
  }
  // std::list<std::shared_ptr<jiplib::Jim> >::iterator lit=maskList.begin();
  // for(lit=maskList.begin();lit!=maskList.end();++lit)
  //   maskReader.pushImage(*lit);
  ImgRaster::setMask(maskReader,*imgWriter, app);
  return(imgWriter);
}

// JimList Jim::rotatecoor(double theta, int iband){
//   JimList listout;
//   try{
//     int noutput=2;//depends on mialib function
//     if(nrOfBand()<=iband){
//       std::string errorString="Error: band number exceeds number of bands in input image";
//       throw(errorString);
//     }
//     IMAGE ** imout;
//     imout=::rotatecoor(this->getMIA(iband),theta);
//     this->setMIA(iband);
//     if(imout){
//       for(int iim=0;iim<noutput;++iim){
//         std::shared_ptr<Jim> imgWriter=std::make_shared<Jim>(imout[iim]);
//         imgWriter->copyGeoTransform(*this);
//         imgWriter->setProjection(getProjectionRef());
//         listout.pushImage(imgWriter);
//       }
//       return(listout);
//     }
//     else{
//       std::string errorString="Error: rotatecoor() function in MIA failed, returning empty list";
//       throw(errorString);
//     }
//   }
//   catch(std::string errorString){
//     std::cerr << errorString << std::endl;
//     return(listout);
//   }
//   catch(...){
//     return(listout);
//   }
// }

// JimList Jim::imgc(int iband){
//   JimList listout;
//   try{
//     int noutput=2;//depends on mialib function
//     if(nrOfBand()<=iband){
//       std::string errorString="Error: band number exceeds number of bands in input image";
//       throw(errorString);
//     }
//     IMAGE ** imout;
//     imout=::imgc(this->getMIA(iband));
//     this->setMIA(iband);
//     if(imout){
//       for(int iim=0;iim<noutput;++iim){
//         std::shared_ptr<Jim> imgWriter=std::make_shared<Jim>(imout[iim]);
//         imgWriter->copyGeoTransform(*this);
//         imgWriter->setProjection(getProjectionRef());
//         listout.pushImage(imgWriter);
//       }
//       return(listout);
//     }
//     else{
//       std::string errorString="Error: imgc() function in MIA failed, returning empty list";
//       throw(errorString);
//     }
//   }
//   catch(std::string errorString){
//     std::cerr << errorString << std::endl;
//     return(listout);
//   }
//   catch(...){
//     return(listout);
//   }
// }
